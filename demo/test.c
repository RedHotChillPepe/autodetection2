#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include "svp_acl.h"
#include "svp_acl_rt.h"
#include "svp_acl_mdl.h"
#include "tracking.h"

#include "devsdk.h"

#define MODEL_INPUT_WIDTH   640
#define MODEL_INPUT_HEIGHT  640


static volatile int g_exit = 0;
static int g_device_id = 0;
static svp_acl_rt_context g_ctx = NULL;

static uint32_t g_model_id = 0;
static svp_acl_mdl_desc *g_model_desc = NULL;
static void *g_model_mem = NULL;
static size_t g_model_mem_size = 0;
static int g_input_num = 0;
static int g_output_num= 0;

static YuvHdl g_hdl = NULL; // handle из DEVSDK

typedef struct {
    unsigned char *host_buf;
    int width;
    int height;
    int size;
    int fresh;
    pthread_mutex_t mutex;
} FrameData;

static FrameData g_frame;

// Пример имён классов

static const char* g_class_names[] = {
    "person","bicycle","car","motorcycle","airplane","bus","train","truck","boat",
    "traffic light","fire hydrant","stop sign","parking meter","bench","bird","cat",
    "dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack",
    "umbrella","handbag","tie","suitcase","frisbee","skis","snowboard","sports ball",
    "kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket",
    "bottle","wine glass","cup","fork","knife","spoon","bowl","banana","apple",
    "sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair",
    "couch","potted plant","bed","dining table","toilet","tv","laptop","mouse",
    "remote","keyboard","cell phone","microwave","oven","toaster","sink",
    "refrigerator","book","clock","vase","scissors","teddy bear","hair drier","toothbrush"
};



// static const char* g_class_names[] = {
//     "car","comerc","truck","bus","trailer","bicycle","motorcycle","tractor","tram",
//     "pedestrian"
// };
static int g_class_count = sizeof(g_class_names)/sizeof(g_class_names[0]);

// ---------------------------------------------------------------------
// Пример структуры бокса (для внутреннего использования)
typedef struct {
    float lx, ly, rx, ry;
    float score;
    int   cid;
} DET_BOX;

static inline uint64_t get_time_us()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (ts.tv_nsec / 1000ULL);
}

static void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_exit = 1;
    }
}

static void* ReadFileAll(const char *fname, size_t *out_size)
{
    FILE *fp = fopen(fname, "rb");
    if(!fp){
        fprintf(stderr,"[Err] cannot open %s\n", fname);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    if(sz <= 0){
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    void *buf = malloc(sz);
    if(!buf){
        fclose(fp);
        return NULL;
    }
    size_t rr = fread(buf, 1, sz, fp);
    fclose(fp);
    if(rr < (size_t)sz){
        free(buf);
        return NULL;
    }
    if(out_size)*out_size=(size_t)sz;
    return buf;
}

// Глобальные device-буферы (для оптимизации)
static void *g_dev_in0 = NULL;  // для YUV
static size_t g_dev_in0_size = 0;

static void *g_dev_in1 = NULL;  // для rpn (4 float)
static size_t g_dev_in1_size = 0;

static void *g_dev_in2 = NULL;  
static size_t g_dev_in2_size = 0;

static void *g_dev_in3 = NULL;  
static size_t g_dev_in3_size = 0;

// Выходы
static void *g_dev_out0 = NULL;
static size_t g_dev_out0_size = 0;
static void *g_dev_out1 = NULL;
static size_t g_dev_out1_size = 0;


static int LoadModel(const char* om_path)
{
    // 1) читаем om в host
    size_t fsz = 0;
    void *host_om = ReadFileAll(om_path, &fsz);
    if(!host_om){
        fprintf(stderr,"[Err] ReadFileAll fail\n");
        return -1;
    }
    // 2) alloc device
    svp_acl_error ret = svp_acl_rt_malloc(&g_model_mem, fsz, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    if(ret != SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] rt_malloc fail\n");
        free(host_om);
        return -1;
    }
    memcpy(g_model_mem, host_om, fsz);
    free(host_om);
    g_model_mem_size = fsz;

    // 3) load
    ret = svp_acl_mdl_load_from_mem(g_model_mem, g_model_mem_size, &g_model_id);
    if(ret != SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] mdl_load_from_mem fail\n");
        svp_acl_rt_free(g_model_mem);
        g_model_mem=NULL;
        return -1;
    }

    // 4) desc
    g_model_desc = svp_acl_mdl_create_desc();
    if(!g_model_desc){
        fprintf(stderr,"[Err] mdl_create_desc fail\n");
        svp_acl_mdl_unload(g_model_id);
        svp_acl_rt_free(g_model_mem);
        g_model_mem=NULL;
        return -1;
    }
    ret = svp_acl_mdl_get_desc(g_model_desc, g_model_id);
    if(ret != SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] mdl_get_desc fail\n");
        svp_acl_mdl_destroy_desc(g_model_desc);
        g_model_desc=NULL;
        svp_acl_mdl_unload(g_model_id);
        svp_acl_rt_free(g_model_mem);
        g_model_mem=NULL;
        return -1;
    }

    g_input_num  = svp_acl_mdl_get_num_inputs(g_model_desc);
    g_output_num = svp_acl_mdl_get_num_outputs(g_model_desc);
    printf("[Info] Model loaded: input_num=%d, output_num=%d\n", g_input_num, g_output_num);

    // -- Теперь выделим device-буферы раз и навсегда ---
    // Определяем размеры
    if(g_input_num>=1){
        g_dev_in0_size = svp_acl_mdl_get_input_size_by_index(g_model_desc, 0);
        svp_acl_rt_malloc(&g_dev_in0, g_dev_in0_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    if(g_input_num>=2){
        g_dev_in1_size = svp_acl_mdl_get_input_size_by_index(g_model_desc, 1);
        svp_acl_rt_malloc(&g_dev_in1, g_dev_in1_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    if(g_input_num>=3){
        g_dev_in2_size = svp_acl_mdl_get_input_size_by_index(g_model_desc, 2);
        svp_acl_rt_malloc(&g_dev_in2, g_dev_in2_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    if(g_input_num>=4){
        g_dev_in3_size = svp_acl_mdl_get_input_size_by_index(g_model_desc, 3);
        svp_acl_rt_malloc(&g_dev_in3, g_dev_in3_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    // output
    if(g_output_num>=1){
        g_dev_out0_size = svp_acl_mdl_get_output_size_by_index(g_model_desc, 0);
        svp_acl_rt_malloc(&g_dev_out0, g_dev_out0_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    if(g_output_num>=2){
        g_dev_out1_size = svp_acl_mdl_get_output_size_by_index(g_model_desc, 1);
        svp_acl_rt_malloc(&g_dev_out1, g_dev_out1_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }

    return 0;
}

static void UnloadModel()
{
    // free global device bufs
    if(g_dev_in0){  svp_acl_rt_free(g_dev_in0);  g_dev_in0=NULL; }
    if(g_dev_in1){  svp_acl_rt_free(g_dev_in1);  g_dev_in1=NULL; }
    if(g_dev_in2){  svp_acl_rt_free(g_dev_in2);  g_dev_in2=NULL; }
    if(g_dev_in3){  svp_acl_rt_free(g_dev_in3);  g_dev_in3=NULL; }
    if(g_dev_out0){ svp_acl_rt_free(g_dev_out0); g_dev_out0=NULL;}
    if(g_dev_out1){ svp_acl_rt_free(g_dev_out1); g_dev_out1=NULL;}

    // desc/model
    if(g_model_desc){
        svp_acl_mdl_destroy_desc(g_model_desc);
        g_model_desc = NULL;
    }
    if(g_model_id > 0){
        svp_acl_mdl_unload(g_model_id);
        g_model_id = 0;
    }
    if(g_model_mem){
        svp_acl_rt_free(g_model_mem);
        g_model_mem=NULL;
    }
}

static int AddDataBuffer(svp_acl_mdl_dataset *dataset, void *dev_ptr, size_t size, size_t stride)
{
    svp_acl_data_buffer *db = svp_acl_create_data_buffer(dev_ptr, size, stride);
    if(!db){
        fprintf(stderr,"[Err] create_data_buffer fail\n");
        return -1;
    }
    svp_acl_error ret = svp_acl_mdl_add_dataset_buffer(dataset, db);
    if(ret != SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] add_dataset_buffer fail\n");
        svp_acl_destroy_data_buffer(db);
        return -1;
    }
    return 0;
}

int get_detections(BBox *detections, int max_detections) {
    // Ждём появления нового кадра (g_frame.fresh == 1)
    pthread_mutex_lock(&g_frame.mutex);
    while (g_frame.fresh == 0) {
        pthread_mutex_unlock(&g_frame.mutex);
        usleep(1000);  // ожидание 1 мс
        pthread_mutex_lock(&g_frame.mutex);
    }
    unsigned char *local_buf = g_frame.host_buf;
    int local_size = g_frame.size;
    int loc_w = g_frame.width;
    int loc_h = g_frame.height;
    g_frame.fresh = 0;  // Помечаем, что кадр забрали
    pthread_mutex_unlock(&g_frame.mutex);

    // Копируем BGR-данные в устройство (g_dev_in0)
    if (g_dev_in0 && local_size <= (int)g_dev_in0_size) {
        memcpy(g_dev_in0, local_buf, local_size);
    } else {
        return 0;
    }

    // Подготавливаем датасеты для инференса
    svp_acl_mdl_dataset *inDset = svp_acl_mdl_create_dataset();
    svp_acl_mdl_dataset *outDset = svp_acl_mdl_create_dataset();
    if (!inDset || !outDset) {
        if (inDset) svp_acl_mdl_destroy_dataset(inDset);
        if (outDset) svp_acl_mdl_destroy_dataset(outDset);
        return 0;
    }

    // Добавляем входные буферы (пример для первого входа)
    size_t stride0 = svp_acl_mdl_get_input_default_stride(g_model_desc, 0);
    AddDataBuffer(inDset, g_dev_in0, g_dev_in0_size, stride0);
    // Если модель имеет дополнительные входы, добавьте их аналогично:
    if (g_input_num >= 2) {
        size_t stride1 = svp_acl_mdl_get_input_default_stride(g_model_desc, 1);
        AddDataBuffer(inDset, g_dev_in1, g_dev_in1_size, stride1);
    }
    // Для in2 и in3 можно заполнить нулями, если не используются:
    if (g_input_num >= 3) {
        memset(g_dev_in2, 0, g_dev_in2_size);
        size_t stride2 = svp_acl_mdl_get_input_default_stride(g_model_desc, 2);
        AddDataBuffer(inDset, g_dev_in2, g_dev_in2_size, stride2);
    }
    if (g_input_num >= 4) {
        memset(g_dev_in3, 0, g_dev_in3_size);
        size_t stride3 = svp_acl_mdl_get_input_default_stride(g_model_desc, 3);
        AddDataBuffer(inDset, g_dev_in3, g_dev_in3_size, stride3);
    }

    // Добавляем выходные буферы
    if (g_output_num >= 1) {
        size_t ostride0 = svp_acl_mdl_get_output_default_stride(g_model_desc, 0);
        AddDataBuffer(outDset, g_dev_out0, g_dev_out0_size, ostride0);
    }
    if (g_output_num >= 2) {
        size_t ostride1 = svp_acl_mdl_get_output_default_stride(g_model_desc, 1);
        AddDataBuffer(outDset, g_dev_out1, g_dev_out1_size, ostride1);
    }

    // Выполняем инференс модели
    svp_acl_error ret = svp_acl_mdl_execute(g_model_id, inDset, outDset);
    if (ret != SVP_ACL_SUCCESS) {
        // Очистка датасетов (пропустите детекции)
        size_t nb = svp_acl_mdl_get_dataset_num_buffers(inDset);
        for (size_t i = 0; i < nb; i++) {
            svp_acl_data_buffer* db = svp_acl_mdl_get_dataset_buffer(inDset, i);
            if (db) svp_acl_destroy_data_buffer(db);
        }
        svp_acl_mdl_destroy_dataset(inDset);
        nb = svp_acl_mdl_get_dataset_num_buffers(outDset);
        for (size_t i = 0; i < nb; i++) {
            svp_acl_data_buffer* db = svp_acl_mdl_get_dataset_buffer(outDset, i);
            if (db) svp_acl_destroy_data_buffer(db);
        }
        svp_acl_mdl_destroy_dataset(outDset);
        return 0;
    }

    // Парсим выход модели: функция parse_output уже реализована в test.c
    AI_DATA_INFO_S arr[32];
    memset(arr, 0, sizeof(arr));
    int arr_count = 0;
    parse_output(outDset, loc_w, loc_h, arr, &arr_count, 32);

    // Очистка датасетов
    {
        size_t nb = svp_acl_mdl_get_dataset_num_buffers(inDset);
        for (size_t i = 0; i < nb; i++) {
            svp_acl_data_buffer* db = svp_acl_mdl_get_dataset_buffer(inDset, i);
            if (db) svp_acl_destroy_data_buffer(db);
        }
        svp_acl_mdl_destroy_dataset(inDset);
    }
    {
        size_t nb = svp_acl_mdl_get_dataset_num_buffers(outDset);
        for (size_t i = 0; i < nb; i++) {
            svp_acl_data_buffer* db = svp_acl_mdl_get_dataset_buffer(outDset, i);
            if (db) svp_acl_destroy_data_buffer(db);
        }
        svp_acl_mdl_destroy_dataset(outDset);
    }

    // Преобразуем распарсенные данные в массив BBox
    int count = (arr_count < max_detections) ? arr_count : max_detections;
    for (int i = 0; i < count; i++) {
        detections[i].x = (float)arr[i].rect.x;
        detections[i].y = (float)arr[i].rect.y;
        detections[i].w = (float)arr[i].rect.width;
        detections[i].h = (float)arr[i].rect.height;
    }
    return count;
}


// Возвращает 0 при успехе
static int parse_output(svp_acl_mdl_dataset *out,
                        int net_w, int net_h,
                        AI_DATA_INFO_S *arr, int *arr_count, int max_arr)
{
    if(g_output_num < 2){
        *arr_count=0;
        return 0;
    }
    svp_acl_data_buffer *db0 = svp_acl_mdl_get_dataset_buffer(out, 0);
    float *out0_ptr = (float*)svp_acl_get_data_buffer_addr(db0);
    svp_acl_mdl_io_dims dims0;
    svp_acl_mdl_get_output_dims(g_model_desc, 0, &dims0);

    int cls_count = 1;
    if(dims0.dim_count>0){
        cls_count = dims0.dims[dims0.dim_count - 1];
    }
    int totalValid=0;
    int *validBoxNum = (int*)malloc(cls_count*sizeof(int));
    if(!validBoxNum){
        *arr_count=0;
        return -1;
    }
    for(int i=0; i<cls_count; i++){
        validBoxNum[i] = (int) out0_ptr[i];
        totalValid += validBoxNum[i];
    }

    svp_acl_data_buffer *db1 = svp_acl_mdl_get_dataset_buffer(out, 1);
    float *out1_ptr = (float*)svp_acl_get_data_buffer_addr(db1);
    size_t stride1 = svp_acl_mdl_get_output_default_stride(g_model_desc, 1);
    size_t stride1_f = stride1 / sizeof(float);

    DET_BOX *boxes = (DET_BOX*)malloc(totalValid*sizeof(DET_BOX));
    if(!boxes){
        free(validBoxNum);
        *arr_count=0;
        return -1;
    }
    int offset=0;
    for(int c=0; c<cls_count; c++){
        int cnum = validBoxNum[c];
        for(int b=0; b<cnum; b++){
            DET_BOX *pb = &boxes[offset + b];
            // для удобства:
            size_t idx = (size_t)(offset + b);
            pb->lx    = out1_ptr[idx + 0*stride1_f];
            pb->ly    = out1_ptr[idx + 1*stride1_f];
            pb->rx    = out1_ptr[idx + 2*stride1_f];
            pb->ry    = out1_ptr[idx + 3*stride1_f];
            pb->score = out1_ptr[idx + 4*stride1_f];
            pb->cid   = (int) out1_ptr[idx + 5*stride1_f];
        }
        offset += cnum;
    }
    free(validBoxNum);

    //  printf("[INFO] input image width[%d]; height[%d]\n", net_w, net_h);
    // ///////////////////////ВЫВОДЫ ДЛЯ ДЕБАГА//////////////////////////////////////////////

    printf("[INFO]  current class valid box number is: %d\n", totalValid);
    for(int i=0; i<totalValid; i++){
        printf("[INFO]  lx: %.2f, ly: %.2f, rx: %.2f, ry: %.2f, score: %.5f; class id: %d\n",
               boxes[i].lx, boxes[i].ly, boxes[i].rx, boxes[i].ry, boxes[i].score, boxes[i].cid);
    }


    int n = (totalValid>max_arr)? max_arr: totalValid;
    for(int i=0; i<n; i++){
        arr[i].rect.x      = (int)boxes[i].lx;
        arr[i].rect.y      = (int)boxes[i].ly;
        arr[i].rect.width  = (int)(boxes[i].rx - boxes[i].lx);
        arr[i].rect.height = (int)(boxes[i].ry - boxes[i].ly);

        char tmp[64];
        if(boxes[i].cid>=0 && boxes[i].cid<g_class_count){
            snprintf(tmp,sizeof(tmp), "%s(%.2f)", g_class_names[boxes[i].cid], boxes[i].score);
        } else {
            snprintf(tmp,sizeof(tmp), "cls_%d(%.2f)", boxes[i].cid, boxes[i].score);
        }
        strncpy(arr[i].szText, tmp, sizeof(arr[i].szText)-1);
        arr[i].szText[ sizeof(arr[i].szText)-1 ]= '\0';
    }
    *arr_count = n;
    free(boxes);
    return 0;
}

// -------------------- callback --------------------
int my_yuv_callback(int nStatus, int nWidth, int nHeight, int nPixelFormat,
                    int nSize, unsigned char *pYUVData, void *pUser)
{
    if(nStatus!=0) return 0;
    if(nWidth!=MODEL_INPUT_WIDTH || nHeight!=MODEL_INPUT_HEIGHT){
        return 0; 
    }
    // trylock
    int lockRet = pthread_mutex_trylock(&g_frame.mutex);
    if(lockRet!=0){
        // Пропуск
        printf("[WARN] can't lock g_frame\n");
        return 0;
    }
    if(g_frame.fresh==1){
        // предыдущий кадр ещё не обработан
        pthread_mutex_unlock(&g_frame.mutex);
         printf("[WARN] no new frame\n");
        return 0;
    }
    if(!g_frame.host_buf){
        g_frame.host_buf=(unsigned char*)malloc(nSize);
        if(!g_frame.host_buf){
            pthread_mutex_unlock(&g_frame.mutex);
            return 0;
        }
    }
    memcpy(g_frame.host_buf, pYUVData, nSize);
    g_frame.width  = nWidth;
    g_frame.height = nHeight;
    g_frame.size   = nSize;
    g_frame.fresh  = 1;
    pthread_mutex_unlock(&g_frame.mutex);
    return 0;
}


// -------------------- Поток инференса --------------------
static void *InferenceThread(void *arg)
{
    svp_acl_error ret = svp_acl_rt_set_current_context(g_ctx);
    if(ret!=SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] set_current_context fail\n");
        return NULL;
    }

    uint64_t last_print_time=get_time_us();
    int frame_counter=0;

    while(!g_exit){
        int do_infer=0;
        int local_size=0;
        unsigned char* local_buf=NULL;
        int loc_w=0, loc_h=0;

        pthread_mutex_lock(&g_frame.mutex);
        if(g_frame.fresh==1 && g_frame.host_buf){
            local_buf= g_frame.host_buf;
            local_size=g_frame.size;
            loc_w=g_frame.width;
            loc_h=g_frame.height;
            g_frame.fresh=0;
            do_infer=1;
        }
        pthread_mutex_unlock(&g_frame.mutex);

        if(do_infer){
            frame_counter++;
            // memcpy вход на device
            // Поскольку мы уже зарезервировали g_dev_in0, просто копируем
            if(g_dev_in0 && local_size<=g_dev_in0_size){
                memcpy(g_dev_in0, local_buf, local_size);
            } else {
                // ошибка - пропустим
                continue;
            }
            // rpn
            float rpn[4] = {0.8f, 0.35f, 5.0f, 5.0f}; // nms=0.8 / score=0.15 / 1.0 / 1.0 - это по умолчанию
            if(g_dev_in1 && g_dev_in1_size >=sizeof(rpn)){
                memcpy(g_dev_in1, rpn, sizeof(rpn));
            }

            // in2,in3 — заполняем нулями
            if(g_dev_in2 && g_dev_in2_size>0){
                memset(g_dev_in2, 0, g_dev_in2_size);
            }
            if(g_dev_in3 && g_dev_in3_size>0){
                memset(g_dev_in3, 0, g_dev_in3_size);
            }

            // создаём input/output dataset
            svp_acl_mdl_dataset *inDset = svp_acl_mdl_create_dataset();
            svp_acl_mdl_dataset *outDset= svp_acl_mdl_create_dataset();
            if(!inDset || !outDset){
                if(inDset) svp_acl_mdl_destroy_dataset(inDset);
                if(outDset)svp_acl_mdl_destroy_dataset(outDset);
                continue;
            }
            // input0
            size_t stride0 = svp_acl_mdl_get_input_default_stride(g_model_desc,0);
            AddDataBuffer(inDset, g_dev_in0, g_dev_in0_size, stride0);

            // input1
            if(g_input_num>=2){
                size_t stride1= svp_acl_mdl_get_input_default_stride(g_model_desc,1);
                AddDataBuffer(inDset, g_dev_in1, g_dev_in1_size, stride1);
            }
            // input2,3
            if(g_input_num>=3){
                size_t s2= svp_acl_mdl_get_input_default_stride(g_model_desc,2);
                AddDataBuffer(inDset, g_dev_in2, g_dev_in2_size, s2);
            }
            if(g_input_num>=4){
                size_t s3= svp_acl_mdl_get_input_default_stride(g_model_desc,3);
                AddDataBuffer(inDset, g_dev_in3, g_dev_in3_size, s3);
            }
            // output0,1
            if(g_output_num>=1){
                size_t ostride0= svp_acl_mdl_get_output_default_stride(g_model_desc,0);
                AddDataBuffer(outDset, g_dev_out0, g_dev_out0_size, ostride0);
            }
            if(g_output_num>=2){
                size_t ostride1= svp_acl_mdl_get_output_default_stride(g_model_desc,1);
                AddDataBuffer(outDset, g_dev_out1, g_dev_out1_size, ostride1);
            }

            uint64_t t1= get_time_us();
            ret= svp_acl_mdl_execute(g_model_id, inDset, outDset);
            uint64_t t2= get_time_us();
            double inf_ms= (double)(t2 - t1)/1000.0; // считаем время инференса

            if(ret==SVP_ACL_SUCCESS){
                AI_DATA_INFO_S arr[32];
                memset(arr,0,sizeof(arr));
                int arr_count=0;
                uint64_t p1= get_time_us();
                parse_output(outDset, loc_w, loc_h, arr, &arr_count, 32);
                uint64_t p2= get_time_us(); // считаем время парсинга
                double parse_ms= (double)(p2 - p1)/1000.0;

                // Интеграция трекинга
                BBox detections[32];
                int num_detections = get_detections(arr, arr_count, detections, 32);

                update_tracks(detections, num_detections);

                int final_count = track_count;
                for (int i = 0; i < final_count; i++) {
                    arr[i].rect.x = (int)tracks[i].bbox.x;
                    arr[i].rect.y = (int)tracks[i].bbox.y;
                    arr[i].rect.width = (int)tracks[i].bbox.w;
                    arr[i].rect.height = (int)tracks[i].bbox.h;
                    snprintf(arr[i].szText, sizeof(arr[i].szText), "ID:%d", tracks[i].id);
                }
                arr_count = final_count;

                // Отрисовка
                DEVSDK_SetAiData(g_hdl, arr, arr_count);

                if((p2 - last_print_time)>1000000ULL){
                    double dur_s= (double)(p2 - last_print_time)/1000000.0;
                    double fps= (double)frame_counter/dur_s;
                    printf("[INFO] fps=%.2f, inf=%.2f ms, parse=%.2f ms\n", // выводим текущую скорость / занятое время раз в секунду
                           fps, inf_ms, parse_ms);
                    frame_counter=0;
                    last_print_time=p2;
                }
            }
            // чистим input
            {
                size_t nb = svp_acl_mdl_get_dataset_num_buffers(inDset);
                for(size_t i=0; i<nb; i++){
                    svp_acl_data_buffer* db= svp_acl_mdl_get_dataset_buffer(inDset,i);
                    if(db) svp_acl_destroy_data_buffer(db);
                }
                svp_acl_mdl_destroy_dataset(inDset);
            }
            // чистим output
            {
                size_t nb = svp_acl_mdl_get_dataset_num_buffers(outDset);
                for(size_t i=0; i<nb; i++){
                    svp_acl_data_buffer* db= svp_acl_mdl_get_dataset_buffer(outDset,i);
                    if(db) svp_acl_destroy_data_buffer(db);
                }
                svp_acl_mdl_destroy_dataset(outDset);
            }
        } else {
            // Нет свежего кадра — поспим чуть.
            
            usleep(2000);
        }
    }
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc<2){
        printf("Usage: %s yolov8.om [vpss_chn]\n", argv[0]);
        return 0;
    }
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    svp_acl_error ret = svp_acl_init(NULL);
    if(ret!=SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] acl_init fail\n");
        return -1;
    }
    ret= svp_acl_rt_set_device(g_device_id);
    if(ret!=SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] set_device fail\n");
        svp_acl_finalize();
        return -1;
    }
    ret= svp_acl_rt_create_context(&g_ctx, g_device_id);
    if(ret!=SVP_ACL_SUCCESS){
        fprintf(stderr,"[Err] create_context fail\n");
        svp_acl_rt_reset_device(g_device_id);
        svp_acl_finalize();
        return -1;
    }
    // Load model
    if(LoadModel(argv[1])!=0){
        svp_acl_rt_destroy_context(g_ctx);
        g_ctx=NULL;
        svp_acl_rt_reset_device(g_device_id);
        svp_acl_finalize();
        return -1;
    }

    // Инициализация devsdk
    DEVSDK_Init();
    int vpssNo = 3; // по умолчанию
    if(argc>=3) vpssNo = atoi(argv[2]);

    // Запуск YUV
    DEVSDK_YuvDataProcPrepare(vpssNo, MODEL_INPUT_WIDTH, MODEL_INPUT_HEIGHT, 3, 0);
    g_hdl= DEVSDK_GetYUVDataProcV2(0, vpssNo, MODEL_INPUT_WIDTH, MODEL_INPUT_HEIGHT,
                                 3, // создаем новый VPSS
                                 my_yuv_callback, (void*)0x101 );
    if(!g_hdl){
        fprintf(stderr,"[Err] devsdk getYUVDataProc fail\n");
    }
    memset(&g_frame,0,sizeof(g_frame));
    pthread_mutex_init(&g_frame.mutex,NULL);

    // Запускаем инференс-поток
    pthread_t tid;
    pthread_create(&tid, NULL, InferenceThread, NULL);

    while(!g_exit){
        sleep(1);
    }

    // Остановка
    if(g_hdl){
        DEVSDK_StopGetYUVDataProc(g_hdl);
        g_hdl=NULL;
    }
    DEVSDK_Uninit();

    pthread_join(tid,NULL);

    if(g_frame.host_buf){
        free(g_frame.host_buf);
        g_frame.host_buf=NULL;
    }
    UnloadModel();

    svp_acl_rt_destroy_context(g_ctx);
    g_ctx=NULL;
    svp_acl_rt_reset_device(g_device_id);
    svp_acl_finalize();

    printf("[Info] Exit success\n");
    return 0;
}

