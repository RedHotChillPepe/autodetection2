/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef HEIF_FORMAT_H
#define HEIF_FORMAT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void *HeifHandle;

/* general error code */
typedef enum {
    // Everything ok, no error occurred.
    HEIF_OK = 0,
    HEIF_ERROR_NULL_PTR,
    HEIF_ERROR_INVALID_HANDLE,
    HEIF_ERROR_INVALIDARG,

    HEIF_ERROR_OPEN_FILE,
    HEIF_ERROR_CLOSE_FILE,
    HEIF_ERROR_READ_FILE,
    HEIF_ERROR_WRITE_FILE,
    HEIF_ERROR_SEEK_FILE,

    HEIF_ERROR_CREATE,
    HEIF_ERROR_DESTROY,
    HEIF_ERROR_CREATE_STREAM,
    HEIF_ERROR_DESTROY_STREAM,

    HEIF_ERROR_WRITE_ATOM,
} HeifErrorCode;

#define HEIF_MAX_HDLR_NAME 256  /* handlr name max length 255,one for '/0' */
#define HEIF_TRACK_NUM_MAX 6
#define HEIF_MAX_BOX_PATH  64  /* atom path  max length */
#define HEIF_MAX_FILE_NAME 512 /* file name path max */
#define HEIF_TYPE_SIZE     4    /* box type length */

typedef enum {
    HEIF_CONFIG_MUXER = 1, /* muxer type */
    HEIF_CONFIG_DEMUXER,   /* demuxer type */
    HEIF_CONFIG_BUTT       /* invalid config type */
} HeifConfigType;

typedef enum {
    HEIF_FORMAT_HEIC = 0,  /* heic */
    HEIF_FORMAT_BUTT
} HeifFormatProfile;

typedef enum {
    HEIF_TRACK_VIDEO = 1,
    HEIF_TRACK_DATA,
    HEIF_TRACK_BUTT
} HeifTrackType;

typedef enum {
    HEIF_CODEC_AVC = 1,
    HEIF_CODEC_HEVC,
    HEIF_CODEC_AV1,
    HEIF_CODEC_JPEG,
    HEIF_CODEC_ID_BUTT
} HeifCodecId;

typedef enum {
    HEIF_ROTATION_0 = 0,
    HEIF_ROTATION_90 = 90,
    HEIF_ROTATION_180 = 180,
    HEIF_ROTATION_270 = 270,
    HEIF_ROTATION_BUTT = 0xFFFFFFFF,
} HeifRotationType;

typedef enum {
    HEIF_MIRROR_TYPE_NONE = 0,
    HEIF_MIRROR_TYPE_H,  /* horizontal mirroring */
    HEIF_MIRROR_TYPE_V,  /* vertical mirroring */
} HeifMirrorType;

typedef enum {
    HEIF_IMAGE_MASTER = 0,
    HEIF_IMAGE_THUMBNAIL,
    HEIF_IMAGE_DEPTH,
} HeifImageType;

typedef struct {
    int32_t posX;
    int32_t posY;
    int32_t width;
    int32_t height;
} HeifCropInfo;

typedef struct {
    HeifImageType imageType;   /* iamge type */
    uint32_t width;            /* image width */
    uint32_t height;           /* image height */
    uint32_t rowImageNum;      /* the number of rows of images, only valid for grid mode */
    uint32_t columnImageNum;   /* the number of images per row, only valid for grid mode */
    HeifCodecId codecId;       /* codec type, only support HEIF_CODEC_HEVC now */
    HeifMirrorType mirrorType; /* the type of mirroring operation, only support HEIF_MIRROR_TYPE_NONE now */
    HeifRotationType rotation; /* the angle of rotation, only support HEIF_ROTATION_0 now */
    HeifCropInfo cropInfo;     /* the crop info for image, not support and ignored */
} HeifImageInfo;

typedef struct {
    HeifTrackType trackType;      /* the type of track, only support HEIF_TRACK_VIDEO currently */
    char hdlrName[HEIF_MAX_HDLR_NAME]; /* manufacturer  name */
    HeifImageInfo imageInfo;      /* imageInfo info */
} HeifTrackConfig;

typedef struct {
    int32_t  itemId;     /* the index of image item */
    int64_t  timeStamp;  /* frame timestamp, unit us */
    uint8_t  *data;      /* frame data buffer */
    uint32_t length;     /* frame data len */
    bool keyFrame;       /* key frame flag */
} HeifImageItem;

typedef struct {
    bool    isGrid;    /* true:grid, false:normal. not support grid mode currently */
    uint32_t rowImageNum;      /* the number of rows of images, only valid for grid mode */
    uint32_t columnImageNum;   /* the number of images per row, only valid for grid mode */
    HeifFormatProfile  formatProfile;  /* heic */
} HeifMuxerConfig;

typedef enum {
    HEIF_FILE_TYPE_URL = 0,
    HEIF_FILE_TYPE_FD,
} HeifFileType;

typedef struct {
    HeifFileType fileType; /* file type, uri or fd */
    union {
        char    fileName[HEIF_MAX_FILE_NAME]; /* file uri */
        int32_t fileFd;  /* file descriptions id */
    } input;
} HeifFileDesc;

typedef struct {
    HeifFileDesc fileInfo;
    HeifConfigType  configType;  /* the type of config, muxer or demuxer */
    HeifMuxerConfig muxerConfig; /* only used for HEIF_CONFIG_MUXER  */
} HeifConfig;

typedef enum {
    HEIF_EXTEND_TYPE_MIRROR = 0,
    HEIF_EXTEND_TYPE_CROP,
    HEIF_EXTEND_TYPE_ROTATION,
    HEIF_EXTEND_TYPE_THUMBNAIL,
    HEIF_EXTEND_TYPE_HEVC_EXT,
    HEIF_EXTEND_TYPE_BUTT,
} HeifExtendType;

typedef struct {
    HeifExtendType type; /* the type of extend info */
    void *data;          /* the address of extend info */
    uint32_t dataLen;    /* the length of extend info */
} HeifExtendInfo;

typedef struct {
    uint32_t trackId;                  /* the index of track */
    HeifTrackType trackType;           /* the type of track */
    char hdlrName[HEIF_MAX_HDLR_NAME]; /* manufacturer name */
    uint32_t imageCnt;                 /* the count of iamges */
    HeifImageInfo *imageInfo;          /* image meta info, it may be the infomation of master image and thumbnail */
} HeifTrackInfo;

typedef struct {
    int64_t duration;        /* unit us */
    uint32_t trackCount;      /* the count of trak in file, it should be 1, if single picture scence */
    HeifTrackInfo *trackInfo; /* trackInfo info */
} HeifFileInfo;

/**
 * @brief create heif instance.
 * @param[in/out] handle, Indicates the handle of heif
 * @param[in] heifCfg, Indicates the config informaation that needed by creating operation
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifCreate(HeifHandle *handle, const HeifConfig *config);

/**
 * @brief destroy heif instance.
 * @param[in] handle, Indicates the handle of heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifDestroy(HeifHandle handle);

/**
 * @brief create heif track. only used for video sequence scence.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] trackConfig, Indicates the track information of a new stream
 * @param[out] trackId, Indicates the index of track
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifCreateTrack(HeifHandle handle, const HeifTrackConfig *trackConfig, uint32_t *trackId);

/**
 * @brief set extend info for heif; such as mirror, rotation, crop, thumnail.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] trackId, Indicates the index of track. it should be get after  call  HeifCreateTrack.
 * @param[in] info, Indicates the extend info that will be setted to heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifSetExtendInfo(HeifHandle handle, uint32_t trackId, const HeifExtendInfo *info);

/**
 * @brief destroy all track.
 * @param[in] handle, Indicates the handle of heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifDestroyAllTracks(HeifHandle handle);

/**
 * @brief write image data
 * @param[in] handle, Indicates the handle of heif
 * @param[in] trackId, Indicates the index of track. it should be get after calling HeifCreateTrack for video sequeues
 * scence.
 * using <b>-1<b> if single picture scence
 * @param[in] item, Indicates the array of iamge items info;
 * @param[in] itemCnt, Indicates the counts of image items
 * @return   0 success
 * @return  err num  failure
 */
int32_t HeifWriteMasterImage(HeifHandle handle, uint32_t trackId, const HeifImageItem *item, uint32_t itemCnt);

/**
 * @brief get file information.
 * @param[in] handle, Indicates the handle of heif
 * @param[in/out] fileInfo, Indicates the information the input file.
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifGetFileInfo(HeifHandle handle, HeifFileInfo *fileInfo);

/**
 * @brief get extend info for heif; such as mirror, rotation, crop, thumnail.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] trackId, Indicates the track index that getted from calling HeifGetFileInfo
 * @param[in/out] info, Indicates the extend info that will be setted to heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifGetExtendInfo(HeifHandle handle,  uint32_t trackId,  uint32_t imageIndex, HeifExtendInfo *info);

/**
 * @brief get master iamage.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] trackId, Indicates the track index that getted from calling HeifGetFileInfo
 * @param[in/out] item, Indicates the array of iamge items info;
 * @param[in/out] itemCnt uint32_t* :the array count of item, and output the actually used count. and itemCnt should
 * not small than the num calcurate by file info
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t HeifGetMasterImage(HeifHandle handle, uint32_t trackId, HeifImageItem *item, uint32_t *itemCnt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
