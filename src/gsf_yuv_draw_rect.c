/*
 * ref: https://blog.csdn.net/u010312436/article/details/101379177
 * 
 */
#include <stdio.h>
#include <string.h>
#include "gsf_yuv_draw_rect.h"
 
static stYuvColor s_color_table[YUV_COLOR_MAX] = {
	{0x00, 0x00, 0x00}, // green
	{0x00, 0x00, 0xff}, // red
	{0x00, 0xff, 0x00},	// blue
	{0xff, 0xff, 0xff}, // light purple
	{0xff, 0x00, 0x00}, // dark green
	{0xff, 0x00, 0xff}, // yellow
	{0xff, 0xff, 0x00}, // light blue
	{0x00, 0xff, 0xff},	// purple
	{0x00, 0x80, 0x80}, // dark black
	{0x80, 0x80, 0x80}, // gray
	{0xff, 0x80, 0x80}, // white
};
//yuv设置数据
void yuv_setdata(
	uInt8* YBuff,
	uInt8* UVBuff,
	enYuvType yuvType,
	uInt16 width,
	uInt16 height,
	stPoint draw_point,
	enYuvColorIdx clrIdx)
{
	switch(yuvType)
	{
		case TYPE_YUV422I_UYVY:
		case TYPE_YUV422I_YUYV:
		{
			/*
				UYVY UYVY UYVY UYVY
			*/
			uInt32 tmp = draw_point.y * width * 2;
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			if(yuvType == TYPE_YUV422I_UYVY) {
				u_offset = tmp + draw_point.x / 2 * 4;
				v_offset = u_offset + 2;
				y_offset = u_offset + 1;
			}
			else {
				y_offset = tmp + draw_point.x / 2 * 4;
				u_offset = y_offset + 1;
				v_offset = u_offset + 2;
			}
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			YBuff[y_offset + 2] = s_color_table[clrIdx].Y;
			YBuff[u_offset] = s_color_table[clrIdx].U;
			YBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV420SP_NV12:
		case TYPE_YUV420SP_NV21:
		{
			/*
				YY YY
				YY YY
				UV UV
			*/
			uInt32 y_offset = draw_point.y * width + draw_point.x;
			uInt32 u_offset = 0, v_offset = 0;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			#if 0
			Int32 x_flag = 1, y_flag = 1;
			if(draw_point.y % 2 == 0) {
				YBuff[y_offset + width] = s_color_table[clrIdx].Y;
				y_flag = 1;
			}
			else {
				YBuff[y_offset - width] = s_color_table[clrIdx].Y;
				y_flag = -1;
			}
 
			if(draw_point.x % 2 == 0) {
				YBuff[y_offset + 1] = s_color_table[clrIdx].Y;
				x_flag = 1;
			}
			else {
				YBuff[y_offset - 1] = s_color_table[clrIdx].Y;
				x_flag = -1;
			}
			YBuff[y_offset + width * y_flag + 1 * x_flag] = s_color_table[clrIdx].Y;
			#endif
			
			if(yuvType == TYPE_YUV420SP_NV12) {
				u_offset = (draw_point.y / 2) * width + draw_point.x / 2 * 2;
				v_offset = u_offset + 1;
			}
			else {
				v_offset = (draw_point.y / 2) * width + draw_point.x / 2 * 2;
				u_offset = v_offset + 1;
			}
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
			//printf("[%d, %d]: y_offset = %d, u_offset = %d, v_offset = %d\n",
			//	draw_point.x, draw_point.y, y_offset, u_offset, v_offset);
		}break;
		case TYPE_YUV444P:
		{
			/*
				YYYYYYYY
				UUUUUUUU
				VVVVVVVV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			uInt32 plane_size = width * height;
			y_offset = draw_point.y * width + draw_point.x;
			u_offset = y_offset;
			v_offset = plane_size + u_offset;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV444I:
		{
			/*
				YUV YUV YUV YUV YUV YUV YUV YUV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			y_offset = draw_point.y * width * 3 + draw_point.x * 3;
			u_offset = y_offset + 1;
			v_offset = u_offset + 1;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			YBuff[u_offset] = s_color_table[clrIdx].U;
			YBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV422P:
		{
			/*
				YYYYYYYY
				UUUU
				VVVV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			uInt32 plane_size = width * height / 2;
			y_offset = draw_point.y * width + draw_point.x;
			u_offset = y_offset / 2;
			v_offset = plane_size + y_offset / 2;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
        case TYPE_YUV420I:
		{
			/*
				YYYYYYYY
				UU
				VV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			uInt32 plane_size = width * height / 4;
			y_offset = draw_point.y * width + draw_point.x;
			u_offset = draw_point.y / 2 * width / 2 + draw_point.x/2;
			v_offset = u_offset + plane_size;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
	}
}
//yuv绘画线
void yuv_drawline(stYuvBuffInfo *pYuvBuffInfo, stDrawLineInfo *pDrawLineInfo)
{
	if(!pYuvBuffInfo || !pYuvBuffInfo->pYuvBuff)
		return;
 
	uInt8 *YBuff = NULL, *UVBuff = NULL;
	uInt16 x0 = pDrawLineInfo->startPoint.x, y0 = pDrawLineInfo->startPoint.y;
	uInt16 x1 = pDrawLineInfo->endPoint.x, y1 = pDrawLineInfo->endPoint.y;
 
	if(pDrawLineInfo->lineWidth == 0) pDrawLineInfo->lineWidth = 1;
	x0 = (x0 >= pYuvBuffInfo->width) ? (x0 - pDrawLineInfo->lineWidth) : x0;
	x1 = (x1 >= pYuvBuffInfo->width) ? (x1 - pDrawLineInfo->lineWidth) : x1;
	y0 = (y0 >= pYuvBuffInfo->height) ? (y0 - pDrawLineInfo->lineWidth) : y0;
	y1 = (y1 >= pYuvBuffInfo->height) ? (y1 - pDrawLineInfo->lineWidth) : y1;
 
	uInt16 dx = (x0 > x1) ? (x0 - x1) : (x1 - x0);	//结束点x坐标与固定点的差值
	uInt16 dy = (y0 > y1) ? (y0 - y1) : (y1 - y0);	//结束点y坐标与固定点的差值
 
	Int16 xstep = (x0 < x1) ? 1 : -1;
	Int16 ystep = (y0 < y1) ? 1 : -1;
	Int16 nstep = 0, eps = 0;
 
	stPoint draw_point;
	draw_point.x = x0;
	draw_point.y = y0;
 
	switch(pYuvBuffInfo->yuvType)
	{
		case TYPE_YUV422I_UYVY:
		case TYPE_YUV422I_YUYV:
		case TYPE_YUV444I:
		{
			YBuff = pYuvBuffInfo->pYuvBuff;
			UVBuff = NULL;
		}break;
		case TYPE_YUV420SP_NV12:
		case TYPE_YUV420SP_NV21:
		case TYPE_YUV444P:
		case TYPE_YUV422P:
        case TYPE_YUV420I:
		{
			YBuff = pYuvBuffInfo->pYuvBuff;
			UVBuff = pYuvBuffInfo->pYuvBuff + pYuvBuffInfo->width * pYuvBuffInfo->height;
		}break;
		default:
			return;
	}
 
	// 布雷森汉姆算法画线
	if(dx > dy){		//横线
		while(nstep <= dx) {
			yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, pDrawLineInfo->clrIdx); //设置该点数据为指定颜色线
			eps += dy;
			if( (eps << 1) >= dx ) {
				draw_point.y += ystep;
				eps -= dx;
			}
			draw_point.x += xstep;		//绘画点的x坐标移动  1 正向 如:0-1    -1 反向 如:2-3
			nstep++;
			//printf("[%s %d] eps(%hu) xstep(%hu) nstep(%hu) drawpoint(%hu, %hu)\n", __func__, __LINE__, 
			//	eps,xstep, nstep, draw_point.x, draw_point.y);
		}
	}else {				//竖线
		while(nstep <= dy){	
			yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, pDrawLineInfo->clrIdx); //设置该点数据为指定颜色线
			eps += dx;
			if( (eps << 1) >= dy ) {
				draw_point.x += xstep;
				eps -= dy;
			}
			draw_point.y += ystep;		//绘画点的y坐标移动  1 正向 如:1-2    -1 反向 如:3-0
			nstep++;
			//printf("[%s %d] xstep(%hu) nstep(%hu) drawpoint(%hu, %hu)\n", __func__, __LINE__, 
			//	xstep, nstep, draw_point.x, draw_point.y);
		}
	}
}
//yuv绘画矩形
void yuv_draw_rect(stYuvBuffInfo* yuvBuffInfo, stDrawRectInfo *pDrawRectInfo)
{
	int i = 0;
	stDrawLineInfo drawLineInfo = {0};
	if (yuvBuffInfo == NULL || yuvBuffInfo->pYuvBuff == NULL || 
		yuvBuffInfo->width < 2 || yuvBuffInfo->height < 2 ||
		pDrawRectInfo == NULL || pDrawRectInfo->pointNum < 2)
	{
		printf("[%s %d] invalid argument!\n", __func__, __LINE__);
		return;
	}
	
	for (i = 0; i < pDrawRectInfo->pointNum && i < POINT_MAX_NUM; i++)
	{
		drawLineInfo.lineWidth = pDrawRectInfo->lineWidth;
		drawLineInfo.clrIdx = pDrawRectInfo->clrIdx[i];
		drawLineInfo.startPoint.x = pDrawRectInfo->stPoint[i].x;
		drawLineInfo.startPoint.y = pDrawRectInfo->stPoint[i].y;
		drawLineInfo.endPoint.x = pDrawRectInfo->stPoint[(i+1)%pDrawRectInfo->pointNum].x; //结束点为当前点的下一点，0-1 1-2 2-3 3-0
		drawLineInfo.endPoint.y = pDrawRectInfo->stPoint[(i+1)%pDrawRectInfo->pointNum].y;
		yuv_drawline(yuvBuffInfo, &drawLineInfo);	//yuv绘画线
	}
	
}
