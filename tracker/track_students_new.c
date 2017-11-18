/**
*
*/

//#include <stdlib.h>
//#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "rh_interface.h"


//#include <csl_tsc.h>

#include "track_students.h"


#define WIDTH 640
#define HEIGHT 360
#define X_OFFSET 1
#define Y_OFFSET 1
/**
* @ 控制画线的类型
*/
typedef enum _stu_draw_line_type
{
	DRAW_NO = 0,			//不画线
	DRAW_TRIGGER_TRACK,		//表示画跟踪框和触发框,限高线,和跟踪区域框
	DRAW_SLANT_LINE,		//画斜线表示画45度,135度,90度,180度线
	DRAW_MODEL,				//画模版
	DRAW_SKIN_COLOR,		//画检测区域的头像的肤色部分
	DRAW_TRACK_TRAJECTORY,	//画跟踪轨迹线
	DRAW_TRACK_DIRECT,		//画跟踪方向线
	DRAW_TRACK_SPEED,		//画跟踪速度线
	MAX_NUM_DRAW
}stu_draw_line_type_e;

typedef struct stRectIndex{
	unsigned char ucxMin;
	unsigned char ucxMax;
	unsigned char ucyMin;
	unsigned char ucyMax;
}STR_RECT_INDEX;

#define	INTER_PROG_RATIO	(1)		//1//1表示逐行,2表示隔行
#define YUV422_OR_YUV420	(2)		//1//1表示422,2表示420


#define min(a,b) (((a)>(b))?(b):(a))
#define max(a,b) (((a)<(b))?(b):(a))
//#define MAXVIDEOWIDTH 1280//1920
//#define MAXVIDEOHEIGHT 720//1088
#define STU_FIND_MV_DELAY 60//检测口检测到移动后60帧内不再检测
#define STU_MVDET_DELAY 600//学生起立后600帧内启动移动侦测

static int64_t time1,time2;

/* p, q is on the same of line l */
static int is_same(const vertex_t *l_start, const vertex_t *l_end,
	const vertex_t *p,
	const vertex_t *q)

{

	float_t dx = l_end->x - l_start->x;  	//多边蔚谋?
	float_t dy = l_end->y - l_start->y;

	float_t dx1 = p->x - l_start->x;    //p(待检测点)
	float_t dy1 = p->y - l_start->y;

	float_t dx2 = q->x - l_end->x;      //q(待检测点射线终点)
	float_t dy2 = q->y - l_end->y;

	if ((dx * dy1 - dy * dx1) * (dx * dy2 - dy * dx2) > 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* 2 line segments (s1, s2) are intersect? */
static int is_intersect(const vertex_t *s1_start, const vertex_t *s1_end,
	const vertex_t *s2_start, const vertex_t *s2_end)
{
	int i, j;
	i = is_same(s1_start, s1_end, s2_start, s2_end);
	j = is_same(s2_start, s2_end, s1_start, s1_end);
	if ((i&&j) == 0)
		return 0;
	else
		return 1;

}

/**
* @ 判断点是否落在外包矩阵内，如果落在外包矩阵内，则进行下一步计算
*/
static void vertices_get_extent(const vertex_t *vl, int np, /* in vertices */
	rect_t *rc /* out extent*/)
{
	short int i;
	i = vl[0].x;
	if (np > 0)
	{
		rc->min_x = rc->max_x = vl[0].x;
		rc->min_y = rc->max_y = vl[0].y;
	}

	else
	{
		rc->min_x = rc->min_y = rc->max_x = rc->max_y = 0; // =0 ? no vertices at all 
	}

	for (i = 1; i<np; i++)
	{
		if (vl[i].x < rc->min_x)
			rc->min_x = vl[i].x;
		if (vl[i].y < rc->min_y)
			rc->min_y = vl[i].y;
		if (vl[i].x > rc->max_x)
			rc->max_x = vl[i].x;
		if (vl[i].y > rc->max_y)
			rc->max_y = vl[i].y;
	}
}


/**
* @	检测点是0还是1，是0表示在检测区域外，是1表示在检测区域内
*/
static int32_t pt_in_poly(const  vertex_t *vl, int np,  /* polygon vl with np vertices  */
	const vertex_t *v)
{
	int i, j, k1, k2, c;
	rect_t rc;
	vertex_t w;

	
	if (np < 3)
	{
		return 0;
	}

	vertices_get_extent(vl, np, &rc);

	if (v->x < rc.min_x || v->x > rc.max_x || v->y < rc.min_y || v->y > rc.max_y)
	{
		return 0;
	}

	/* Set a horizontal beam l(*v, w) from v to the ultra right */
	w.x = rc.max_x + 1;
	w.y = v->y;
	c = 0;        /* Intersection points counter */

	for (i = 0; i<np; i++)
	{
		j = (i + 1) % np;

		if (vl[i].y != vl[j].y)
		{
			if (is_intersect(vl + i, vl + j, v, &w) == 1)
			{
				c++;
			}
		}

		if (vl[i].y == w.y)
		{
			if (vl[i].x >= v->x)
			{
				k1 = (np + i - 1) % np;
				k2 = (np + i + 1) % np;
				if (vl[k1].y > w.y)
				{
					if (vl[k2].y < w.y)
					{
						c++;
					}
				}

				if (vl[k2].y > w.y)
				{
					if (vl[k1].y < w.y)
					{
						c++;
					}
				}


			}
		}
	}

	return   c % 2;
}
/**
* @ 函数名称: students_area_check()
* @	函数功能: 设定跟踪区域
* @ 输入参数: input_param -- 应用层设置的参数
* @ 输出参数: 无
* @ 返回值:   无
*/
static vertex_t track_point[TRACK_AREA_POINT_MAX];

static int32_t students_area_check(StuTrackHand *inst)
{
	StuITRACK_Params *input_param=&(inst->input_param);
    int32_t	 	k				= 0;	
	int16_t		video_width		= input_param->static_param.video_width;
	int16_t		video_height	= input_param->static_param.video_height;
	int16_t     i;
	int32_t		m				= 0;
	int32_t		n				= 0;
	vertex_t	point_y_value	= {0};
	uint8_t *Students_Area_vertex=inst->t_stu_global_obj.Students_Area_vertex; 
	
	
	for(i=0;i<TRACK_AREA_POINT_MAX;i++)
		{

		track_point[i].x = input_param->dynamic_param.track_point[i].x>>X_OFFSET;
		track_point[i].y = input_param->dynamic_param.track_point[i].y>>Y_OFFSET;
		}


	for (k = 0; k < video_width * video_height; k++)  
	{
		m = k%video_width;     //列
		n = k/video_width;    //行

		point_y_value.x = m;
		point_y_value.y = n;

		if (pt_in_poly ((const vertex_t *)track_point, input_param->dynamic_param.track_point_num,  
			&point_y_value)==1)
		{
		#if 0
			if (n%2 == 0)
			{
				*((uint8_t *)Students_Area_vertex + n/2 * video_width + m) = 255;
			}
			else
			{
				*((uint8_t *)Students_Area_vertex + (n/2 + video_height/2)  * video_width + m) = 255;
			}
		#endif	
			*((uint8_t *)Students_Area_vertex + n * video_width + m) = 255;
		}
		else
		{
		#if 0
			if (n%2 == 0)
			{
				*((uint8_t *)Students_Area_vertex + n/2 * video_width + m) = 0;
			}
			else
			{
				*((uint8_t *)Students_Area_vertex + (n/2 + video_height/2) * video_width + m) = 0;
			}
		#endif	
			*((uint8_t *)Students_Area_vertex + n * video_width + m) = 0;
		}
	}
	return 0;
}



/**
* @	函数名称: 	data_copy()
* @ 函数功能: 	对图像进行缩放，并去隔行
* @	输入参数: 	input_param -- 外部设置的输入参数
* @	输出参数: 	无
* @ 返回值:	  	无
*/
static int32_t data_copy(StuTrackHand *inst, int8_t *output_buf)
{

	StuITRACK_Params *input_param = &(inst->input_param);

	uint8_t **students_dst_y_buf = inst->t_stu_global_obj.students_dst_y_buf;

memcpy(students_dst_y_buf[inst->t_stu_global_obj.nCopyFramNum],output_buf,\
	sizeof(uint8_t)*input_param->static_param.video_width*input_param->static_param.video_height);

/*
	StuITRACK_Params *input_param = &(inst->input_param);
	int16_t			pic_width = 0;
	int16_t			pic_width_4 = 0;
	int16_t			pic_height = 0;
	int16_t			video_width = input_param->static_param.video_width;
	int16_t			video_height = input_param->static_param.video_height;
	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;
	int32_t			i = 0;
	int32_t			j = 0;
	int32_t			nSrcShift = 0;
	int32_t			nDstShift = 0;
	uint32_t *			 pDst;
	uint8_t *			 pSrc;
	uint8_t *			 pDst_other;

	uint32_t			nVal0, nVal1;
	uint32_t			nVal2, nVal3;

	uint32_t			nDstVal;
	int k = 0;
	uint8_t **students_dst_y_buf = inst->t_stu_global_obj.students_dst_y_buf;
	int nSrcBufWidth = inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight = inst->t_stu_global_obj.nSrcBufHeight;

	pic_width = video_width / x_offset;
	pic_height = video_height / y_offset;

	pic_width_4 = pic_width >> 2;
#if 1
	for (i = 0; i < pic_height; i++)
	{
		for (j = 0; j < pic_width; j++)
		{

			* (students_dst_y_buf[inst->t_stu_global_obj.nCopyFramNum] + k) = *(output_buf + i*y_offset*nSrcBufWidth + j*x_offset);
			k++;
		}
	}

#else
	if (x_offset == 4)
	{
		for (i = 0; i < pic_height; i++)
		{
			pSrc = output_buf + i*y_offset*nSrcBufWidth;
			pDst = (uint32_t *)(students_dst_y_buf[nCopyFramNum] + i*pic_width);
			nSrcShift = 0;
			nDstShift = 0;

			for (j = 0; j < pic_width_4; j++)
			{
				nVal0 = _lo(_memd8(pSrc + nSrcShift));
				nVal1 = _hi(_memd8(pSrc + nSrcShift));
				nVal2 = _lo(_memd8(pSrc + nSrcShift + 8));
				nVal3 = _hi(_memd8(pSrc + nSrcShift + 8));

				nDstVal = _packl4(_packl4(nVal3, nVal2), _packl4(nVal1, nVal0));
				//_mem4(pDst+ nDstShift) = nDstVal;
				*(pDst + nDstShift) = nDstVal;

				nSrcShift += 16;
				nDstShift += 1;

			}
		}
	}
	else	if (x_offset == 2)
	{
		for (i = 0; i < pic_height; i++)
		{
			pSrc = output_buf + i*y_offset*nSrcBufWidth;
			pDst = (uint32_t *)(students_dst_y_buf[nCopyFramNum] + i*pic_width);
			nSrcShift = 0;
			nDstShift = 0;

			for (j = 0; j < pic_width_4; j++)
			{
				nVal0 = _lo(_memd8(pSrc + nSrcShift));
				nVal1 = _hi(_memd8(pSrc + nSrcShift));
				nDstVal = _packl4(nVal1, nVal0);
				//_mem4(pDst+ nDstShift) = nDstVal;
				*(pDst + nDstShift) = nDstVal;
				nSrcShift += 8;
				nDstShift += 1;
			}
		}
	}
	else	if (x_offset == 1)
	{
		for (i = 0; i < pic_height; i++)
		{
			pSrc = output_buf + i*y_offset*nSrcBufWidth;
			pDst = (uint32_t *)(students_dst_y_buf[nCopyFramNum] + i*pic_width);
			nSrcShift = 0;
			nDstShift = 0;

			for (j = 0; j < pic_width_4; j++)
			{
				nVal0 = _mem4(pSrc + nSrcShift);

				//_memd8(pDst+ nDstShift) = _itod(nVal1,nVal0);
				*(pDst + nDstShift) = nVal0;
				nSrcShift += 4;
				nDstShift += 4;

			}
		}
	}
	else
	{
		for (i = 0; i < pic_height; i++)
		{
			pSrc = output_buf + i*y_offset*nSrcBufWidth;
			pDst_other = students_dst_y_buf[nCopyFramNum] + i*pic_width;
			nSrcShift = 0;
			nDstShift = 0;

			for (j = 0; j < pic_width; j++)
			{
				*(pDst_other + nDstShift) = *(pSrc + nSrcShift);
				nSrcShift += x_offset;
				nDstShift++;

			}

		}
	}
#endif
*/
	inst->t_stu_global_obj.nCopyFramNum++;
	if (inst->t_stu_global_obj.nCopyFramNum == 3)
	{
		inst->t_stu_global_obj.nCopyFramNum = 0;
	}
	return 0;
}
//三帧差

#define OFFSET 1
static int32_t img_compare_y(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	int32_t k = 0, T1, T2, T3;

	int16_t			pic_width = 0;
	int16_t			pic_height = 0;
	int16_t			video_width = input_param->static_param.video_width;
	int16_t			video_height = input_param->static_param.video_height;
	int16_t         i;
	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;

	int16_t			sens = input_param->dynamic_param.sens;
	int32_t 		pic_all_pixel = 0;
	int16_t			m = 0;
	int16_t			n = 0;
	uint8_t *pTemp,*pTemp0,*pTemp1,*pTemp2,*pTemp9;
	uint8_t **students_dst_count = inst->t_stu_global_obj.students_dst_count;//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	const uint8_t **students_dst_y_buf = inst->t_stu_global_obj.students_dst_y_buf;		 //存放上一帧Ybuffer数据
	const uint8_t *Students_Area_vertex = inst->t_stu_global_obj.Students_Area_vertex;
	uint8_t * students_active_at9count = inst->t_stu_global_obj.students_active_at9count;
	uint8_t* student_active_count = inst->t_stu_global_obj.students_active_count[0];
#if OFFSET
	int16_t			m_shift = 0;
	int16_t			n_shift = 0;
	pic_width = video_width ;
	pic_height = video_height ;
#else
	int16_t			m_shift = 0 ;
	int16_t			n_shift = 0 ;
	pic_width = video_width ;
	pic_height = video_height ;
#endif
/*
	for (i = 0; i<9; i++)
	{
		memset((uint8_t*)inst->t_stu_global_obj.students_active_count[i], 0, sizeof(pic_width * pic_height));
	}
	*/

	pTemp = students_dst_count[8];
	students_dst_count[8] = students_dst_count[7];
	students_dst_count[7] = students_dst_count[6];
	students_dst_count[6] = students_dst_count[5];
	students_dst_count[5] = students_dst_count[4];
	students_dst_count[4] = students_dst_count[3];
	students_dst_count[3] = students_dst_count[2];
	students_dst_count[2] = students_dst_count[1];
	students_dst_count[1] = students_dst_count[0];
	students_dst_count[0] = pTemp;
	memset((uint8_t*)student_active_count,0,sizeof(uint8_t)*pic_width*pic_height);
memset((uint8_t*)students_dst_count[0],0,sizeof(uint8_t)*pic_width*pic_height);
	//printf("pic_height = %d,pic_width=%d\n",pic_width,pic_height);

	for (n = 0; n < pic_height ; n++)
	{
		for (m = 0; m < pic_width  ; m+=2)
		{
			k = n*pic_width + m;
			
			if(*((uint8_t *)Students_Area_vertex + k) != 0)
			{
				pTemp0 = students_dst_y_buf[0] + k;
				pTemp1 = students_dst_y_buf[1] + k;
				pTemp2 = students_dst_y_buf[2] + k;
				T1 = abs(*pTemp0 - *pTemp1);
				T2 = abs(*pTemp1 - *pTemp2);
				T3 = abs(*pTemp0 - *pTemp2);
				pTemp9 = students_active_at9count + k;
				if (T1>sens || T2>sens || T3>sens)                                    
				{
					*((uint8_t *)students_dst_count[0] + k) = 1;
					*(pTemp9) = (*(pTemp9)+1)> 6?6 : *(pTemp9)+1;
					*((uint8_t *)output_buf + k) = 255;
					
				}
				else
				{
					*(pTemp9) = (*(pTemp9) - 1)<0?0 : *(pTemp9) - 1;
					//m++;  //跳跃
				}
			}


			/**(student_active_count + k) = 0;
			
			if (*((uint8_t *)Students_Area_vertex + (n  * video_width) + m ) == 0)
			{
				*(students_active_at9count + k) = 0;
				*((uint8_t *)students_dst_count[0] + k) = 0;
			}
			else
			{
			pTemp0 = students_dst_y_buf[0] + k;
			pTemp1 = students_dst_y_buf[1] + k;
			pTemp2 = students_dst_y_buf[2] + k;
			T1 = abs(*pTemp0 - *pTemp1);
			T2 = abs(*pTemp1 - *pTemp2);
			T3 = abs(*pTemp0 - *pTemp2);
				if (T1>sens || T2>sens || T3>sens)
					//if (T1>sens||T3>sens)                                           
				{
					*((uint8_t *)students_dst_count[0] + k) = 1;
					*(students_active_at9count + k) = (*(students_active_at9count + k)+1)> 6?6 : *(students_active_at9count + k)+1;
				}
				else
				{
					*((uint8_t *)students_dst_count[0] + k) = 0;
					*(students_active_at9count + k) = (*(students_active_at9count + k) - 1)<0?0 : *(students_active_at9count + k) - 1;
				}
			}*/
		}
	}

	return 0;
}
//删除单个点
static int64_t _memd8(uint8_t* buf,int dis_n)//存放到寄存器
{
	int64_t value = buf[dis_n - 1];
	int i;
	for (i = dis_n - 2; i >= 0; i--)
	{
		value << 8;
		value += buf[i];
	}

	return value;
}
static int _lo(int64_t value)
{
	value = value & 0x0000000011111111;
	return value;
}
static int _hi(int64_t value)
{
	value = value & 0x1111111100000000;
	return value;
}

static int32_t img_compare_y0(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param = &(inst->input_param);

	int32_t 		i, j, k;
	int16_t			pic_width = 0;
	int16_t			pic_height = 0;
	int16_t			video_width = input_param->static_param.video_width;
	int16_t			video_height = input_param->static_param.video_height;

	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;

	int32_t			pic_all_pixel = 0;

	int16_t			m = 0;
	int16_t			n = 0;
	int64_t Data0,Data1,Data2;
	uint8_t* pTemp,*pTemp1,*pTemp2;

	uint8_t *students_dst_count = inst->t_stu_global_obj.students_dst_count[0];//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	int nSrcBufWidth = inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight = inst->t_stu_global_obj.nSrcBufHeight;

	pic_width = video_width ;
	pic_height = video_height ; 
/*
	pic_all_pixel = pic_width * pic_height;

	for (k = 0; k < pic_all_pixel; k++)
	{
		n = k / pic_width; 
		m = k % pic_width;
	*/	
	for (n = 0; n < pic_height ; n++)
	{
		for (m = 0; m < pic_width ; m++)
		{
			//k = n*pic_width + m;
			pTemp = students_dst_count + n*pic_width +m;
		if (*(pTemp) == 1)
		{
/*
			Data0 = _memd8(students_dst_count + (n - 1)*pic_width + m - 1,3);
			Data1 = _memd8(students_dst_count + n*pic_width + m - 1,3);
			Data2 = _memd8(students_dst_count + (n + 1)*pic_width + m - 1,3);
			if ((Data0 & 0xFFFFFF) == 0 && (Data1 & 0xFF00FF) == 0 &&(Data2 & 0xFFFFFF) == 0)
			{
				*((uint8_t *)students_dst_count + k) = 0;
			}
			*/
			//孤立的点表示为0，不做统计
			pTemp1 = students_dst_count + (n - 1)*pic_width + m - 1;
			
			if ((*(pTemp1) == 0)
				&& (*(pTemp1 + 1) == 0)
				&& (*(pTemp1 + 2) == 0)
				&& (*(pTemp1 + pic_width) == 0)
				&& (*(pTemp1 + pic_width + 2) == 0)
				&& (*(pTemp1 + 2*pic_width ) == 0)
				&& (*(pTemp1 + 2*pic_width + 1) == 0)
				&& (*(pTemp1 + 2*pic_width + 2) == 0))
			{
				*(pTemp) = 0;
			}
			else
			{
				//前面已经有保护，不需要再加保护
				for (i = -2; i< 0; i++)
				{
				
					
					for (j = -2; j <= 2; j++)
					{
						if (*((uint8_t *)students_dst_count + (n + i)*pic_width + m + j) != 0)
						{
							*(pTemp) = 2;
								i=0;break;
						}

					}
					
				}

			}

		}
			}
	}
	return 0;
}

uint64_t DisWidthValue(int flag, int dis_width)
{
	switch (dis_width)
	{
	
	case 0: {if (flag == 0) return 0x01; else if (flag == 1) return 0x11; }
	case 1: {if (flag == 0) return 0x01; else if (flag == 1) return 0x11; }
	case 2: {if (flag == 0) return 0x0101; else if (flag == 1) return 0x1111; }
	case 3: {if (flag == 0) return 0x010101; else if (flag == 1) return 0x111111; }
	case 4: {if (flag == 0) return 0x01010101; else if (flag == 1) return 0x1111111111; }
	case 5: {if (flag == 0) return 0x0101010101; else if (flag == 1) return 0x11111111; }
	case 6: {if (flag == 0) return 0x010101010101; else if (flag == 1) return 0x111111111111; }
	case 7: {if (flag == 0) return 0x01010101010101; else if (flag == 1) return 0x0000000011111111; }
	case 8: {if (flag == 0) return 0x0101010101010101; else if (flag == 1) return 0x0000000011111111; }
	default:{if (flag == 0) return 0x0101010101010101; else if (flag == 1) return 0x0000000011111111; }
	}
}
static int64_t memd8(uint8_t* buf,int dis_width,int64_t* Data0,int64_t* Data1,uint8_t* active)//存放到寄存器，像素点2*dis_width
{
	int64_t value0,value1;
	int i0,i2,dis_width2;
	*Data0 = 0;
	*Data1 = 0;
	*active = 0;
	if (dis_width > 8) dis_width = 8;
	else if (dis_width < 1) dis_width = 1;
	dis_width2 = dis_width<<1;
	//if (buf[dis_width2 - 1] == NULL) return -1;
	value0 = buf[dis_width-1];
	value1 = buf[dis_width2 - 1];
	if ((value0 & 0x01) == 1) *active += 1;
	if ((value1 & 0x01) == 1) *active += 1;
	for ( i0= dis_width-2,i2= dis_width2 -2; i0 >= 0 && i2>= dis_width ; i0--,i2--)
	{
	//printf("value0=%d,value1=%d,buf1=%d,buf2=%d\n",value0,value1,buf[i0],buf[i2]);
		value0 << 8;
		value0 += buf[i0];
		value1 << 8;
		value1 += buf[i2];
		if ((value0 & 0x01) == 1) *active += 1;
		if ((value1 & 0x01) == 1) *active += 1;
		
	}
	*Data0 = value0;
	*Data1 = value1;
	return 0;
}
/*
static int64_t lo(int64_t value, int dis_width)
{
	value = value &DisWidthValue(1,dis_width);
	return value;
}
static int hi(int64_t value,int dis_width)
{
	int move2 ;
	if (dis_width > 4) dis_width = 4;
	if (dis_width < 1) dis_width = 1;
	//move2= 8 * dis_width;
	move2 = dis_width<<3;
	value = value>> move2;
	return value;
}
*/
static int32_t up_getC(StuITRACK_Params *input_param, const uint8_t* inBuf, int trigger_height,int16_t m, int16_t n, int16_t inPreC, int16_t *outC)
{

	int32_t 	i;
	uint8_t*	pTemp;
	int16_t			pic_width = 0;
	int16_t			video_width = input_param->static_param.video_width;
	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;
	uint64_t	nData0, nData1, nData2, nData3;
	int64_t     ValueMem8;
	int16_t    	ThreadLo,ThreadHi;
	int32_t KeyValue;
	uint8_t active = 0;
	int32_t set_value = 0;
	int16_t max_dis_width = ((input_param->dynamic_param.track_point[1].x - input_param->dynamic_param.track_point[0].x)>>1) / (input_param->dynamic_param.track_num_row*input_param->dynamic_param.x_offset);
	int dis_width = (trigger_height*5) / (max_dis_width);
	pic_width = video_width;
	KeyValue = dis_width*2/3;
	
	
	ThreadLo = 0;
	ThreadHi = 100;
	set_value = dis_width - ThreadLo;
	for (i = 1; i <= 4; i++)
	{
		pTemp = inBuf + (n + inPreC - i)*pic_width + m - dis_width + 1;
		ValueMem8 = memd8(pTemp, dis_width,&nData0,&nData1,&active);
		//nData0 = lo(ValueMem8, dis_width);
		//nData1 = hi(ValueMem8, dis_width);

		if (((nData0 & DisWidthValue(0, KeyValue)) != 0) || ((nData1 & DisWidthValue(0, KeyValue)) != 0))
		{
			return -1;//goto C_Up_End_Pro;
		}
	}
	
	//printf("dis_width=%d,m=%d,n=%d\n",dis_width,m,n);
	for (i = ThreadLo; i<ThreadHi; i++)
	{
		pTemp = inBuf + (n + inPreC + i)*pic_width + m - dis_width + 1;
		ValueMem8 = memd8(pTemp, dis_width,&nData0,&nData1,&active);
		//if(ValueMem8 == -1)//越界
		//	{
		//	return -1;
		//	}
		//printf("%d,%ld,%ld\n",active,nData0,nData1);
		//nData0 = lo(ValueMem8, dis_width);
		//nData1 = hi(ValueMem8, dis_width);
		nData0 = nData0 & DisWidthValue(0, dis_width);
		nData1 = nData1 & DisWidthValue(0, dis_width);
		
		if ((nData0 != 0) || (nData1 != 0) )
		{
		//if(active >= KeyValue)
			//{
			//printf("activeheight = %d,activewidth = %d\n",n + inPreC + i,m );
			//printf("n=%d,m=%d,h=%d,i=%d,inPreC=%d,",n,m,n + inPreC,i,inPreC);
			*outC = i;
			return 1;//goto C2_Pro_Up;
			//}
		
		}
	}
	
	return 0;
}

//static int16_t C[9] = { 0, -1, -1, -1, -1, -1, -1, -1, -1 };
static int32_t check_up(StuTrackHand *inst, int16_t m, int16_t n, int16_t trigger_width, int16_t trigger_height)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	int32_t 	loop, loopmin = 8, loopmax = 1, looplast = 0;
	int16_t 	C_H = 0;
	 int16_t i;
	int16_t	C_COUNT = 0;
	int Thread = (trigger_height)/10;
	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;
	int32_t			index = 0;
	int32_t	nRet, nPreCAll = 0;
	//int32_t nSrcMultNo = 2, nDstMultNo = 3;
	const uint8_t **students_dst_count = inst->t_stu_global_obj.students_dst_count;//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	int16_t C[9] = { 0, -1, -1, -1, -1, -1, -1, -1, -1 };
	//C[0] = 0;
	//printf("active start\n");
	for (loop = 1; loop<9; loop++)
	{
		//C[loop] = -1;
		nRet = up_getC(input_param, (const uint8_t*)students_dst_count[loop], trigger_height, m, n, nPreCAll, &C[loop]);
		if (nRet<0)
		{
			break;//goto C_Up_End_Pro;
		}
		if ((C[loop] == -1))// && (C[loop - 1] == -1))
		{
			break;//goto C_Up_End_Pro;
		}
		else
		{
			if (C[loop]>0)
			{
				
				nPreCAll += C[loop];
				C_COUNT++;
				C_H = C_H + C[loop];
				if (C_H > (Thread) && C_COUNT>2)
				{  //有向上的动作
				
				for(i=0;i<=loop;i++)
					{
					printf("activeheight = %d,activewidth = %d,loop = %d,c= %d,C_h = %d\n",n,m ,loop,C[i],C_H);
					}
				//printf("CH=%d ,trigger_height=%d, C_COUNT=%d\n",C_H,trigger_height,C_COUNT);
				//printf("active end\n");
					return 1;
				}
			}
		}
	}
	//printf("active end\n");
	//C_Up_End_Pro:
	/*
	for (loop = 1; loop<9; loop++)
	{
		if (C[loop]>0)
		{
			C_COUNT++;
			C_H = C_H + C[loop];
			//loopmin = min(loopmin, loop);
			//loopmax = max(loopmax, loop);
			//looplast = loop;
		}
	}
	//if ((loopmax - loopmin + 1)>C_COUNT + 2)
	//{
		//return 0;
	//}
	*/
	
	if (C_H > (Thread) && C_COUNT>2)
	{  //有向上的动作
	
		return 1;
	}
	
		return 0;
}

static int32_t CheckHeadPoint(StuTrackHand *inst, int8_t *output_buf, int16_t m, int16_t n,int16_t trigger_width,int16_t trigger_height,int16_t* npHeadShift)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	i,j,k;
	int16_t 	T_0_Count= 0,T_1_Count= 0,T_2_Count= 0,T_3_Count= 0;

	int16_t 		pic_width		= 0;
	int16_t 		video_width 	= input_param->static_param.video_width;
	int16_t 		video_height	= input_param->static_param.video_height;
	int16_t 		x_offset		= input_param->dynamic_param.x_offset;
	int16_t 		y_offset		= input_param->dynamic_param.y_offset;
	int32_t	min_x,min_y,max_x,max_y,max_width,nValWidth,max_headwidth;
	int32_t	nSrcMultNo=1,nDstMultNo=2;
	int32_t	nHeadFlag=0,nHeadShift=0;
	int32_t head_width;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t	yuv_value	= {0};
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;
	
	int16_t trigger_width_cur=trigger_width;
	int16_t	trigger_width_step=trigger_width>>2;
	int16_t	trigger_width_max=trigger_width;

	int16_t loop=0;
	int16_t nFindHead=0,nFindHeadInUpArea=0;

	pic_width = video_width;

	if(trigger_width_step==0)
	{
		trigger_width_step=2;
	}
	trigger_width_max=trigger_width*2;
	trigger_width_max = trigger_width+30;
	while(trigger_width_cur<=trigger_width_max)
	{
		head_width=trigger_width_cur*2/3;
		
		nHeadFlag=0;
		 for(k= -1;k<=1;k++)
		 {
			 T_0_Count=0;
			 min_x=video_width;
			 max_x=0;
			 min_y=video_height;
			 max_y=0;
			 max_width=0;
			 nValWidth=0;
			nHeadShift=k*head_width/2;
			 //head   k=1时检测的头顶点在实际头顶最左边，k=0时在中间，k=-1时检测的头顶点在实际头顶最右边
			for (i= 0;i<=head_width;i++) 	  
			{
				min_x=video_width;
				max_x=0;

				for (j= -head_width*3/4+nHeadShift;j<= head_width*3/4+nHeadShift;j++)
				{
					if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
					{
						T_0_Count++; 
						min_x=min(min_x,j);
						max_x=max(max_x,j);
						min_y=min(min_y,i);
						max_y=max(max_y,i);
						nValWidth=max_x-min_x;
					}
				}  
				max_width=max(max_width,nValWidth);
			}  
			max_headwidth=max_width;
			if ((T_0_Count*nDstMultNo > head_width*head_width*nSrcMultNo )
				&&(max_width*5>head_width*3)&&((max_y-min_y)*5>head_width*3)
				)
			{
				nHeadFlag=1;
				break;
			}
		}
		 if(nHeadFlag==0)
		 {
			 trigger_width_cur+=trigger_width_step;
			 continue;
		 }
		//		 头向下的第二 个框内变化点个数 ，身体部分也有变化
		T_1_Count=0;
		min_x=video_width;
		max_x=0;
		min_y=video_height;
		max_y=0;
		max_width=0;
		nValWidth=0;
		for (i= head_width;i<=head_width*2;i++)
		{
			min_x=video_width;
			max_x=0;
			for (j= -head_width+nHeadShift;j<= head_width+nHeadShift;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
				{
						T_1_Count++; 
						min_x=min(min_x,j);
						max_x=max(max_x,j);
						min_y=min(min_y,i);
						max_y=max(max_y,i);
						nValWidth=max_x-min_x;
				}
			}  
			max_width=max(max_width,nValWidth);
		}  
		if ((T_1_Count*nDstMultNo< head_width*head_width*nSrcMultNo )
			||(max_width<max_headwidth)||((max_y-min_y)*2<head_width*1)
			)
		{
			trigger_width_cur+=trigger_width_step;
			continue;
		}
		
#if 1
		T_2_Count=0;
		T_3_Count=0;
		for (i= 0;i<=head_width;i++)
		{
			for (j= -head_width*3/2+nHeadShift;j<= -head_width/2+nHeadShift;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
				{
						T_2_Count++; 
				}
			}  
			for (j= head_width/2+nHeadShift;j<= head_width*3/2+nHeadShift;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j) != 0)
				{
						T_3_Count++; 
				}
			}  
		}  
		if ((T_2_Count*5>= T_0_Count*4)||(T_2_Count*5>= T_1_Count*4)||(T_3_Count*5>= T_0_Count*4)||(T_3_Count*5>= T_1_Count*4))
		{
			trigger_width_cur+=trigger_width_step;
			continue;
		}
#endif
		*npHeadShift=nHeadShift;
		for (i= -trigger_width_cur;i< 0;i++)		//					   
		{
			for (j=  -trigger_width_cur;j <= trigger_width_cur;j++)
			{
				if (*((uint8_t*)students_dst_count + (n+i)*pic_width + m + j)== 1)
				{
					nFindHeadInUpArea=1;
				}
			} 
		}  
		nFindHead=1;
		break;
	}
	if(nFindHeadInUpArea==1)
	{
		nFindHead=0;
	}
	if(nFindHead==0)
	{
		*((uint8_t*)students_dst_count + n*pic_width + m ) = 2;
	}
	return nFindHead;
}

//寻找区域
void find_area(StuTrackHand* inst,int16_t x,int16_t y,int32_t* min_x,int32_t* min_y,int32_t* max_x,int32_t* max_y,int16_t width_step,int16_t height_step,int16_t* find_x)
{
int i,j;
int16_t pic_width = inst->input_param.static_param.pic_width;
int16_t pic_height = inst->input_param.static_param.pic_height;
uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];
	 uint8_t *students_active_count = inst->t_stu_global_obj.students_active_count[0];

	const uint8_t *Students_Area_vertex = inst->t_stu_global_obj.Students_Area_vertex;
	uint8_t* student_active_9count = inst->t_stu_global_obj.students_active_at9count;
int16_t left,top,right,bottom;
int k = 0;
left = (x - width_step) > 0 ?(x - width_step):0;
top = (y - height_step) > 0 ?(y - height_step):0;	
right = (x + width_step) < (pic_width-1) ?(x + width_step):(pic_width - 1); 
bottom = (y + height_step) < (pic_height - 1) ?(y + height_step):(pic_height - 1);	

	for( i = top;i < bottom;i++)
		{
		for(j = left;j<right;j++)
			{
	k = i*pic_width +j;
			if (*((uint8_t *)Students_Area_vertex + k) == 0)
			{
				continue;
			}
			if (*(student_active_9count + k) > 0  && *(students_active_count + k) != 1)
				{
					*(students_active_count + k )  = 1;
					if (*min_x > j || *min_y > i || *max_x <j || *max_y < i)
					{
						*min_x=min(*min_x,j);
						*max_x=max(*max_x,j);
						//*min_y=min(*min_y,i);
						*max_y=max(*max_y,i);
						if (*min_y > i)
						{
							*min_y = i;
							*find_x = j;
							
						}
						find_area(inst,j,i,min_x,min_y,max_x,max_y,width_step,height_step,find_x);
					}

				}
				
			}
		}
	
}

static int32_t FindHeadPoint(StuTrackHand *inst, int8_t *output_buf, int16_t m, int16_t n,int16_t trigger_width,int16_t trigger_height,int16_t* npHeadShift,int16_t* Head_m,int16_t* Head_n,rect_t* rect)
{
	StuITRACK_Params *input_param=&(inst->input_param);
	int32_t 	i,j,k;
	int16_t 	T_0_Count= 0,T_1_Count= 0,T_2_Count= 0,T_3_Count= 0;

	int16_t 		pic_width		= 0;
	int16_t 		video_width 	= input_param->static_param.video_width;
	int16_t 		video_height	= input_param->static_param.video_height;
	int16_t 		x_offset		= input_param->dynamic_param.x_offset;
	int16_t 		y_offset		= input_param->dynamic_param.y_offset;
	int32_t	min_x,min_y,max_x,max_y,max_width,nValWidth,max_headwidth;
	int32_t	nSrcMultNo=1,nDstMultNo=2;
	int32_t	nHeadFlag=0,nHeadShift=0;
	int32_t head_width;
	line_info_t 		line_info	= {0};
	students_yuv_value_info_t	yuv_value	= {0};
  	uint8_t *students_dst_count=inst->t_stu_global_obj.students_dst_count[0];//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	int nSrcBufWidth=inst->t_stu_global_obj.nSrcBufWidth;
	int nSrcBufHeight=inst->t_stu_global_obj.nSrcBufHeight;
	
	int16_t trigger_width_cur=trigger_width;
	
	int16_t	trigger_width_step=10;
	int16_t	trigger_height_step=7;
	
	int16_t	trigger_width_max=trigger_width;
	int16_t nFindHead=0;
	int16_t find_x = m;
	int16_t value_x = m,value_y = n;
	pic_width = video_width;

	trigger_width_max=trigger_width*2;
	min_x = max_x = m;
	min_y = max_y = n;

	find_area(inst, value_x, value_y, &min_x, &min_y, &max_x, &max_y, trigger_width_step,trigger_height_step, &find_x);
	*Head_m = find_x;
	*Head_n = min_y;
	uint8_t *students_active_count = inst->t_stu_global_obj.students_active_count[0];
	
	if(max_x - min_x < trigger_width/6 || max_y - min_y < trigger_width/4 )
		{

			for(i=min_y;i<max_y;i++)
			{
				for (j = min_x; j<max_x; j++)
				{
					k = i*pic_width + j;
					*(students_active_count + k) = 1;
					
					*(students_dst_count + k) = 0;
				}
			}
		
		}


	else
		{
		int set_y = min_y + 50 > max_y?min_y+50:max_y;
	for(i=min_y;i<set_y;i++)
		{
		for (j = min_x; j<max_x; j++)
		{
			k = i*pic_width + j;
			*(students_active_count + k) = 1;
			if (i>min_y+15 )
			{
				*(students_dst_count + k) = 0;

			}
		}
	}
		}
	/*
	for(j=min_x;j<max_x;j++)
		{
		k = min_y*pic_width + j;
		*(output_buf+k) = 255;
		k = max_y*pic_width + j;
		*(output_buf+k) = 255;
		
		}
	for(i=min_y;i<max_y;i++)
		{
		k = i*pic_width + min_x;
		*(output_buf+k) = 255;
		k = i*pic_width + max_x;
		*(output_buf+k) = 255;
		}*/
		rect->min_x = min_x;
	rect->min_y = min_y;
	rect->max_x = max_x;
	rect->max_y = max_y ;
		
//	printf("last  x= %d,y=%d,w=%d,h=%d\n", min_x, min_y, max_x - min_x, max_y-min_y);
	return nFindHead;
}



inline float Get_b(const vertex_t v1, const vertex_t v2,float a)
{
	return ((v2.y>>1) - a*(v2.x>>1));
}
inline float Get_a(const vertex_t v1, const vertex_t v2)
{
	if(v2.x == v1.x)
	{
		return 0;
	}
	return ((float)(v1.y - v2.y)) / (v1.x - v2.x);
}
void JudgeLine(StuTrackHand *inst)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	
	input_param->dynamic_param.line_left.a = Get_a( input_param->dynamic_param.track_point[3], input_param->dynamic_param.track_point[0]);
	input_param->dynamic_param.line_left.b = Get_b(input_param->dynamic_param.track_point[3], input_param->dynamic_param.track_point[0],input_param->dynamic_param.line_left.a);
	input_param->dynamic_param.line_right.a = Get_a( input_param->dynamic_param.track_point[2], input_param->dynamic_param.track_point[1]);
	input_param->dynamic_param.line_right.b = Get_b(input_param->dynamic_param.track_point[2], input_param->dynamic_param.track_point[1],input_param->dynamic_param.line_right.a);
	
	
}
#if 1
float ChickW(Line_ab line, int n)
{
	float w = 0;
	if (line.a == 0)
	{
		return w;
	}
	else
	{
		
		w = (n - line.b) / line.a;
	}
	
	return w;
}
#else
float ChickW(const vertex_t v1,const vertex_t v2, int n)
{
	float w = 0;
	if (v1.x == v2.x)
	{
		return v1.x;
	}
	else
	{
		float a = ((float)(v1.y - v2.y)) / (v1.x - v2.x);
		float b = v2.y - a*v2.x;
		//w = (n - line.b) / line.a;
		w = (n - b)/a;
	}
	
	return w;
}
#endif
int16_t GetAreaWH(StuITRACK_Params *input_param, int m, int n)
{
	int16_t         People_width;
	float wleft, wright,dis;
	
	if(input_param->dynamic_param.track_point[0].x == input_param->dynamic_param.track_point[3].x)
		wleft = input_param->dynamic_param.track_point[3].x>>1;
	else
		wleft = ChickW(input_param->dynamic_param.line_left, n);//左线的x坐标
	if(input_param->dynamic_param.track_point[1].x == input_param->dynamic_param.track_point[2].x)
		wright = input_param->dynamic_param.track_point[2].x>>1;
	else
		wright = ChickW(input_param->dynamic_param.line_right, n);//右线的x坐标
		
	
	/*
	if(input_param->dynamic_param.track_point[0].y == input_param->dynamic_param.track_point[1].y) //顺时针
		{
		wleft = ChickW(input_param->dynamic_param.track_point[0],input_param->dynamic_param.track_point[3],n);
		wright = ChickW(input_param->dynamic_param.track_point[1],input_param->dynamic_param.track_point[2],n);
		}
	else
		{
		wleft = ChickW(input_param->dynamic_param.track_point[0],input_param->dynamic_param.track_point[1],n);
		wright = ChickW(input_param->dynamic_param.track_point[2],input_param->dynamic_param.track_point[3],n);
		}
		*/
		
	dis = wright - wleft;   //在高度为n的情况下，区域内的宽度
	People_width = dis / (input_param->dynamic_param.track_num_row);  //在高度为n的情况下，每个学生应该有的宽度
	
	return People_width;
}
static int32_t stu_up_getC(Rect_Head* rect_head, int16_t trigger_height,int16_t head_m, int16_t head_n,rect_t rect,int* Pre_x,int* Pre_y,rect_t* Pre_rect, int16_t *outC)
{

	int32_t 	i;
	uint8_t*	pTemp;
	
	uint64_t	nData0, nData1, nData2, nData3;
	int64_t     ValueMem8;
	int16_t    	ThreadLo,ThreadHi;
	int32_t KeyValue;
	int32_t   dis_head;
	int32_t   set_Thread = trigger_height/2;
	int min_dis_head = trigger_height*2;
	int find_x,find_y,head_dis_y,head_dis_x;
	rect_t intersect;//交集区域
	int rect_area1,rect_area2,rect_intersect;
	int min_rect_area;//两区域中最小的面积
	for( i = 0;i<rect_head->rectNum;i++)
		{
		
		head_dis_y = head_n - rect_head->head[i].head_y;
		if (head_dis_y > 0) //向下找，head_dis_y必须小于或等于0
			{
				continue;
			}
			head_dis_x = abs(head_m - rect_head->head[i].head_x) ;
			dis_head = -(head_dis_y) + head_dis_x;
			//printf("dis_head=%d,min_dis_head=%d,set_Thread=%d,dis_width=%d\n",dis_head,min_dis_head,set_Thread,dis_width);
			if (dis_head < min_dis_head && dis_head < set_Thread && head_dis_x < set_Thread/2 )//&& head_dis_y > -set_Thread)
			{
			//计算交集
				intersect.min_x = rect_head->rect[i].min_x > rect.min_x ? rect_head->rect[i].min_x:rect.min_x;
				intersect.min_y = rect_head->rect[i].min_y > rect.min_y ? rect_head->rect[i].min_y:rect.min_y;
				intersect.max_x = rect_head->rect[i].max_x < rect.max_x ? rect_head->rect[i].max_x:rect.max_x;
				intersect.max_y = rect_head->rect[i].max_y < rect.max_y ? rect_head->rect[i].max_y:rect.max_y;

				if ((intersect.max_x - intersect.min_x) <= 0 || (intersect.max_y - intersect.min_y) <= 0) //没有交集
				{
					continue;
				}
				rect_intersect = (intersect.max_x - intersect.min_x)*(intersect.max_y - intersect.min_y);
				//计算两区域面积
				rect_area1 = (rect_head->rect[i].max_x - rect_head->rect[i].min_x)*(rect_head->rect[i].max_y - rect_head->rect[i].min_y);
				rect_area2 = (rect.max_x - rect.min_x)*(rect.max_y - rect.min_y);
				//两区域最小的一个
				min_rect_area = rect_area1 < rect_area2 ? rect_area1 : rect_area2;
				//printf("rect1=%d,rect2=%d,interset=%d\n",rect_area1,rect_area2,rect_intersect);
				if(rect_intersect > min_rect_area/2)
				{
					min_dis_head = dis_head;
					*Pre_x = rect_head->head[i].head_x;
					*Pre_y = rect_head->head[i].head_y;
					*outC =  -head_dis_y;
					Pre_rect->min_x = rect_head->rect[i].min_x;
					Pre_rect->max_x = rect_head->rect[i].max_x;
					Pre_rect->min_y = rect_head->rect[i].min_y;
					Pre_rect->max_y = rect_head->rect[i].max_y;
					//printf("dis_head=%d,min_dis_head=%d,set_Thread=%d\n",dis_head,min_dis_head,set_Thread);
					//printf("Pre_x=%d,Pre_y=%d,outC=%d\n",*Pre_x,*Pre_y,*outC);
				}
				
			}
			
		}
	
	return 0;
}

int stu_check_up(StuTrackHand* inst,int16_t Head_m,int16_t Head_n,rect_t rect ,int16_t trigger_width)
{
	int32_t 	loop, loopmin = 8, loopmax = 1, looplast = 0;
	int16_t 	C_H = 0;
	 int16_t i,nRet;
	int16_t	C_COUNT = 0;
	int Thread = (trigger_width)/4;
	int min_head_m[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int min_head_n[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	rect_t min_rect[9] = {0};
	int Pre_x = 0;
	int Pre_y = 0;
	rect_t Pre_rect;
	int16_t C[9] = { 0, -1, -1, -1, -1, -1, -1, -1, -1 };
	min_head_m[0] = Head_m;
	min_head_n[0] = Head_n;
	min_rect[0] = rect;
	
	for (loop = 1; loop<9; loop++)
	{
	
	//printf("loop=%d,head_num = %d\n",loop,inst->t_stu_global_obj.student_Head[loop]->rectNum);
		nRet = stu_up_getC( inst->t_stu_global_obj.student_Head[loop], trigger_width, min_head_m[loop-1], min_head_n[loop-1],min_rect[loop-1],&Pre_x,&Pre_y, &Pre_rect,&C[loop]);
		if (nRet<0)
		{
			break;//goto C_Up_End_Pro;
		}
		if(C[loop] < 0)
		{
			break;
		}
		if(abs(Pre_x - Head_m) > trigger_width/4)
		{
		//printf("absdis = %d,trigger_width=%d\n",min_head_m - Head_m,trigger_width);
			break;
		}
		else
		{
		
			if (C[loop]>=0)
			{
		
				//printf("Pre_x = %d,Pre_y%d,loop=%d\n",Pre_x,Pre_y,loop);
				min_head_m[loop] = Pre_x;
				min_head_n[loop] = Pre_y;
				min_rect[loop] = Pre_rect;
				C_H = C_H + C[loop];
				if (C_H >= (Thread) )
				{  //有向上的动作
				for(i=0;i<=loop;i++)
					{
					//printf("Pre_x = %d,Pre_y%d,loop=%d,trigger_width =%d\n",min_head_m[i],min_head_n[i],i,trigger_width);
					}
				//	printf("C_H=%d\n",C_H);
					return 1;
				}
			}
		}
	}
	/*if (C_H > (Thread) )
	{  //有向上的动作
	
		return 1;
	}*/
	
		return 0;
}

static int32_t img_compare_y1(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	//Rect_Head *student_head = &(inst->t_stu_global_obj.student_Head);
	int32_t 	i, k,w,h;

	int16_t			m = 0;
	int16_t			n = 0;

	int16_t			pic_width = 0;
	int16_t			pic_height = 0;
	int16_t			video_width = input_param->static_param.video_width;
	int16_t			video_height = input_param->static_param.video_height;

	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;

	int32_t			pic_all_pixel = 0;
	rect_t rect;
	int color[9] = {0,80,160,240,255,255,255,255,255};

	int16_t			trigger_width = 0;
	int16_t			trigger_height = 0;
	uint8_t*      pTemp;
	const uint8_t *students_dst_count = inst->t_stu_global_obj.students_dst_count[0];//实际操作内存指针，与students_time_count配合使用，为了减少内存拷贝
	uint8_t *students_active_count = inst->t_stu_global_obj.students_active_count[0];
	const uint8_t *Students_Area_vertex = inst->t_stu_global_obj.Students_Area_vertex;
	TStuCheckRect *t_check_rect = inst->t_stu_global_obj.t_check_point;
	Rect_Head *student_head;
	int KeyNum  = 0;
	int32_t	nRet;
	int16_t nHeadShift = 0;

	int16_t Head_m,Head_n;
	pic_width = video_width ;
	pic_height = video_height ;

	//copy before head information
	//inst->t_stu_global_obj.student_Head[0] = pTemp;
	inst->t_stu_global_obj.student_Head[8]->rectNum = inst->t_stu_global_obj.student_Head[7]->rectNum;
	inst->t_stu_global_obj.student_Head[7]->rectNum = inst->t_stu_global_obj.student_Head[6]->rectNum;
	inst->t_stu_global_obj.student_Head[6]->rectNum = inst->t_stu_global_obj.student_Head[5]->rectNum;
	inst->t_stu_global_obj.student_Head[5]->rectNum = inst->t_stu_global_obj.student_Head[4]->rectNum;
	inst->t_stu_global_obj.student_Head[4]->rectNum = inst->t_stu_global_obj.student_Head[3]->rectNum;
	inst->t_stu_global_obj.student_Head[3]->rectNum = inst->t_stu_global_obj.student_Head[2]->rectNum;
	inst->t_stu_global_obj.student_Head[2]->rectNum = inst->t_stu_global_obj.student_Head[1]->rectNum;
	inst->t_stu_global_obj.student_Head[1]->rectNum = inst->t_stu_global_obj.student_Head[0]->rectNum;
	
	memcpy(inst->t_stu_global_obj.student_Head[8]->head,inst->t_stu_global_obj.student_Head[7]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[7]->head,inst->t_stu_global_obj.student_Head[6]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[6]->head,inst->t_stu_global_obj.student_Head[5]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[5]->head,inst->t_stu_global_obj.student_Head[4]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[4]->head,inst->t_stu_global_obj.student_Head[3]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[3]->head,inst->t_stu_global_obj.student_Head[2]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[2]->head,inst->t_stu_global_obj.student_Head[1]->head,sizeof(Head_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[1]->head,inst->t_stu_global_obj.student_Head[0]->head,sizeof(Head_t)*1000);

	memcpy(inst->t_stu_global_obj.student_Head[8]->rect,inst->t_stu_global_obj.student_Head[7]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[7]->rect,inst->t_stu_global_obj.student_Head[6]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[6]->rect,inst->t_stu_global_obj.student_Head[5]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[5]->rect,inst->t_stu_global_obj.student_Head[4]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[4]->rect,inst->t_stu_global_obj.student_Head[3]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[3]->rect,inst->t_stu_global_obj.student_Head[2]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[2]->rect,inst->t_stu_global_obj.student_Head[1]->rect,sizeof(rect_t)*1000);
	memcpy(inst->t_stu_global_obj.student_Head[1]->rect,inst->t_stu_global_obj.student_Head[0]->rect,sizeof(rect_t)*1000);

	//printf("pic_width=%d,pic_height=%d\n",pic_width,pic_height);

	student_head = inst->t_stu_global_obj.student_Head[0];
	student_head->rectNum = 0;

	for (n = 0; n < pic_height ; n++)
	{
		for (m = 0; m < pic_width ; m++)
		{
			pTemp = students_dst_count + n*pic_width +m;
		if (*((uint8_t *)Students_Area_vertex + n*video_width + m) == 0)
		{
			continue;
		}
		if (*(pTemp) == 1 && *(students_active_count + n*pic_width+m) != 1)
		{
			KeyNum++;
			*(students_active_count + n*pic_width+m) = 1;
			trigger_width = GetAreaWH(input_param, m, n);
			//printf("trigger_width=%d\n",trigger_width);
			trigger_height = trigger_width;
			//GetArea
			/*
			nRet=CheckHeadPoint(inst, output_buf,  m, n, trigger_width,trigger_height,&nHeadShift);
			if(nRet==0)
			{
				continue;
			}*/
			nRet=FindHeadPoint(inst, output_buf,  m, n, trigger_width,trigger_height,&nHeadShift,&Head_m,&Head_n,&rect);
			for(h=Head_n-2;h<Head_n +2;h++)
					{
					for(w=Head_m-3;w<Head_m+3;w++)

						{
							k=h*pic_width + w;
							*(output_buf+k) = color[0];
						}
					}
			//save head rect
			//printf("Head_m=%d,Head_n=%d\n",Head_m,Head_n);
			student_head->rect[student_head->rectNum].min_x = rect.min_x;
			student_head->rect[student_head->rectNum].min_y = rect.min_y;
			student_head->rect[student_head->rectNum].max_x = rect.max_x;
			student_head->rect[student_head->rectNum].max_y = rect.max_y;
			student_head->head[student_head->rectNum].head_x = Head_m;
			student_head->head[student_head->rectNum].head_y = Head_n;
			student_head->rectNum++;
			
			nRet = stu_check_up(inst,Head_m,Head_n,rect,trigger_width);
			if (nRet == 1)
			{

			//printf("trigger_width=%d,Head_m=%d,Head_n=%d\n",trigger_width,Head_m,Head_n);
				//有向上的动作
				//判断一定范围内是不是已经有触发点，如果有则更新位置
				int BeginCheck = -1;
				for (i = 0; i<STU_MAX_CHECKNUM; i++)
				{
					if (t_check_rect[i].nCheckFlag == 1)
					{
						if (Head_m > t_check_rect[i].nCheckX[t_check_rect[i].nCheckNum] && Head_m < t_check_rect[i].nCheckX[t_check_rect[i].nCheckNum] + t_check_rect[i].nCheckW
							&& Head_n > t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum] && Head_n < t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum] + t_check_rect[i].nCheckH)
						{
							int checkNum = t_check_rect[i].nCheckNum+1;
							t_check_rect[i].nCheckNum++;
							t_check_rect[i].nCheckX[t_check_rect[i].nCheckNum] = Head_m - (trigger_width >>1 );
							t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum] = Head_n - (trigger_height >>1) ;
							t_check_rect[i].nCheckActiveTime = 4;
							
							for(w=rect.min_x;w<rect.max_x;w++)
							{
							k=rect.min_y*pic_width + w;
							*(output_buf+k) = color[checkNum];
							k = rect.max_y*pic_width + w;
							*(output_buf+k) = color[checkNum];
							}
						for(h=rect.min_y;h<rect.max_y;h++)
							{
							k = h*pic_width + rect.min_x;
							*(output_buf+k) =color[checkNum];
							k = h*pic_width + rect.max_x;
							*(output_buf+k) = color[checkNum];
							}
							break;
						}
					}
					/*
					else
						{
						if (BeginCheck == -1)
							{
							BeginCheck = i;
							}
						}
						*/
				}
				//是一个新的触发点
				if (i == STU_MAX_CHECKNUM)
				{

					for (i = 0; i < STU_MAX_CHECKNUM; i++)
					{
						if (t_check_rect[i].nCheckFlag != 1)
						{
							t_check_rect[i].nCheckActiveTime = 4;
							t_check_rect[i].nCheckFlag = 1;
							t_check_rect[i].nCheckTime = 12;
							t_check_rect[i].nCheckNum = 0;
							//t_check_rect[i].nCheckMoveH[0] = n;
							//t_check_rect[i].nCheckMoveW[0] = m;
							t_check_rect[i].nCheckX[0] = Head_m - (trigger_width >> 1);
							t_check_rect[i].nCheckY[0] = Head_n - (trigger_height >> 1);
							t_check_rect[i].nCheckW = trigger_width ;
							t_check_rect[i].nCheckH = trigger_height ;

							for(w=rect.min_x;w<rect.max_x;w++)
							{
							k=rect.min_y*pic_width + w;
							*(output_buf+k) = 0;
							k = rect.max_y*pic_width + w;
							*(output_buf+k) = 0;
							
							}
						for(h=rect.min_y;h<rect.max_y;h++)
							{
							k = h*pic_width + rect.min_x;
							*(output_buf+k) =0;
							k = h*pic_width + rect.max_x;
							*(output_buf+k) = 0;
							}
						
							break;
						}
					}
					
					
				}
				//m += (trigger_width>>3);  //存在则跳跃一段距离
			}

		}
	}

	}
	//printf("rectNum =%d\n",student_head->rectNum);
	
	return 0;
}

static int32_t img_process_y(StuTrackHand *inst, int8_t *output_buf, StuITRACK_OutArgs *output_param)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	line_info_t 		line_info = { 0 };
	students_yuv_value_info_t 	yuv_value = { 0 };
	int32_t Points_dis;
	int32_t i,n;
	int32_t nPos = 0;
	int16_t center_x,center_y;
	TStuCheckRect *t_check_rect = inst->t_stu_global_obj.t_check_point;
	
	//int16_t *k_Nearest = inst->t_stu_global_obj.k_Nearest;		//最近的点
	int16_t ZeroFrame;
	//t_mvdet_info *tMoveInfo = &(inst->t_stu_global_obj.tMoveInfo);
	int64_t time_now = getCurrentTime();
	for (i = 0; i<STU_MAX_CHECKNUM; i++)
	{
		if (t_check_rect[i].nCheckFlag == 1)
		{
			t_check_rect[i].nCheckTime--;
			t_check_rect[i].nCheckActiveTime--;
			if (t_check_rect[i].nCheckTime == 0)
			{
				t_check_rect[i].nCheckFlag = 0;
				t_check_rect[i].nCheckNum = 0;
			}
			if (t_check_rect[i].nCheckActiveTime == 0)
			{
				t_check_rect[i].nCheckFlag = 0;
				t_check_rect[i].nCheckNum = 0;
			}
			if (t_check_rect[i].nCheckNum > 3 && t_check_rect[i].nCheckTime < 9 )
			{
				//printf("i=%d,beginy = %d,endy=%d,nCheckNum=%d,trigger_width =%d\n",i,t_check_rect[i].nCheckY[0],
				//	t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum],t_check_rect[i].nCheckNum,t_check_rect[i].nCheckH);
				//printf("w=%d,Thread=%d\n",t_check_rect[i].nCheckY[0] - t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum],(t_check_rect[i].nCheckH/2));
									
				
				if( t_check_rect[i].nCheckY[0] - t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum] < (t_check_rect[i].nCheckH/6))

				{
					printf("i=%d,beginy = %d,endy=%d,nCheckNum=%d,trigger_width =%d\n",i,t_check_rect[i].nCheckY[0],
					t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum],t_check_rect[i].nCheckNum,t_check_rect[i].nCheckH);
					continue;
				}
				center_x = t_check_rect[i].nCheckX[t_check_rect[i].nCheckNum] + (t_check_rect[i].nCheckW>>1);
				center_y = t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum] + (t_check_rect[i].nCheckH>>1);
				for(n = 0;n <output_param->up_num;n++)
				{
					if(time_now - output_param->time[n] < 6000)//在一定帧数范围内，和已经存在的点进行判断
					{
						Points_dis = (center_x-output_param->Up_List[n].x)
*(int32_t)(center_x-output_param->Up_List[n].x)
						 + (center_y - output_param->Up_List[n].y)*(center_y - output_param->Up_List[n].y) ; 
						
						if (Points_dis < ((t_check_rect[i].nCheckW*t_check_rect[i].nCheckW)<<3))
						{

							break;
						}
					}
				}
				if (n == output_param->up_num)
				{
					output_param->Up_List[n].x = center_x;
					output_param->Up_List[n].y = center_y;
					output_param->time[n] = getCurrentTime();
					
output_param->up_num++;
				}
				//默认x_offset == 1 和y_offsett==1 

				t_check_rect[i].nCheckX[0] = t_check_rect[i].nCheckX[t_check_rect[i].nCheckNum];
				t_check_rect[i].nCheckY[0] = t_check_rect[i].nCheckY[t_check_rect[i].nCheckNum];
				t_check_rect[i].nCheckNum = 0;
				
			}


		}
		
	}
	return 0;
}

static int32_t	delay_dec(int32_t frame_Now,StuITRACK_OutArgs *output_param,int reset_time)
{
	int32_t n = 0;
	int64_t time_now = getCurrentTime();
	while(n<output_param->up_num)
	{
		
		if (time_now - output_param->time[n] > reset_time)
		{
			if(output_param->up_num - 1 >n)
			{
				output_param->Up_List[n].x = output_param->Up_List[output_param->up_num - 1].x;
				output_param->Up_List[n].y = output_param->Up_List[output_param->up_num - 1].y;
				output_param->time[n] = output_param->time[output_param->up_num - 1];
			}
			output_param->up_num--;
		}
		else
			n++;
	}
	return 0;
}
int32_t Chick_is_inShield(StuTrackHand* inst,int x,int y)
{
	StuITRACK_Params* input_param = &(inst->input_param);
	int index;
	student_trigger_info_t* shield_info = input_param->dynamic_param.shield_info;

	
	for(index = 0; index < STUDENTS_SHIELD_NUM; index++)
	{
		if(x > shield_info[index].x && y > shield_info[index].y && x < shield_info[index].x + shield_info[index].width
			&& y < shield_info[index].y + shield_info[index].height)
		{
			return 0;
		}
	}
	return 1;
	
}
int32_t track_students_init(StuTrackHand *inst)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	if (RE_START == input_param->dynamic_param.reset_level)
	{
		students_area_check(inst);
	}
	memset(&inst->output_param, 0, sizeof(StuITRACK_OutArgs));
	memset(&(inst->t_stu_global_obj.t_check_point), 0, sizeof(TStuCheckRect)*STU_MAX_CHECKNUM);

	inst->t_stu_global_obj.students_dst_count[0] = inst->t_stu_global_obj.students_time_count[0];
	inst->t_stu_global_obj.students_dst_count[1] = inst->t_stu_global_obj.students_time_count[1];
	inst->t_stu_global_obj.students_dst_count[2] = inst->t_stu_global_obj.students_time_count[2];
	inst->t_stu_global_obj.students_dst_count[3] = inst->t_stu_global_obj.students_time_count[3];
	inst->t_stu_global_obj.students_dst_count[4] = inst->t_stu_global_obj.students_time_count[4];
	inst->t_stu_global_obj.students_dst_count[5] = inst->t_stu_global_obj.students_time_count[5];
	inst->t_stu_global_obj.students_dst_count[6] = inst->t_stu_global_obj.students_time_count[6];
	inst->t_stu_global_obj.students_dst_count[7] = inst->t_stu_global_obj.students_time_count[7];
	inst->t_stu_global_obj.students_dst_count[8] = inst->t_stu_global_obj.students_time_count[8];

	inst->t_stu_global_obj.nSrcBufWidth = inst->input_param.static_param.video_width;//MAXVIDEOWIDTH;
	inst->t_stu_global_obj.nSrcBufHeight = inst->input_param.static_param.video_height; //MAXVIDEOHEIGHT;
	//inst->t_stu_global_obj.video_switch_stop = 0;
	inst->t_stu_global_obj.nFrameNum = 0;
	inst->t_stu_global_obj.nCopyPos = 0;
	inst->t_stu_global_obj.nCopy10FrameNum = 0;
	inst->t_stu_global_obj.nCopyFramNum = 0;
	JudgeLine(inst);//计算直线的参数

	return 0;
}
static int32_t data_copy10(StuTrackHand *inst, int8_t *output_buf)
{
	StuITRACK_Params *input_param = &(inst->input_param);
	int16_t			pic_width = 0;
	int16_t			pic_height = 0;
	int16_t			video_width = input_param->static_param.video_width;
	int16_t			video_height = input_param->static_param.video_height;
	int16_t			x_offset = input_param->dynamic_param.x_offset;
	int16_t			y_offset = input_param->dynamic_param.y_offset;
	int32_t			i = 0;
	int32_t			j = 0;
	int16_t			sens = input_param->dynamic_param.sens;

	int k = 0;
	uint8_t **students_src_y_buf = inst->t_stu_global_obj.students_src_y_buf;   //存放缩放后Ybuffer数据,每10帧存放一次原始数据 
	int nSrcBufWidth = inst->t_stu_global_obj.nSrcBufWidth;
	pic_width = video_width ;
	pic_height = video_height ;
	if (inst->t_stu_global_obj.nCopy10FrameNum == 0)
	{
	/*
		for (i = 0; i < pic_height; i++)
		{
			for (j = 0; j < pic_width; j++)
			{

				*(students_src_y_buf[inst->t_stu_global_obj.nCopyPos] + k) = *((uint8_t *)output_buf + i*y_offset*nSrcBufWidth + j*x_offset);

				k++;
			}
		}*/
		memcpy(students_src_y_buf[inst->t_stu_global_obj.nCopyPos],output_buf,sizeof(uint8_t)*input_param->static_param.video_width*input_param->static_param.video_height);
		inst->t_stu_global_obj.nCopyPos = !inst->t_stu_global_obj.nCopyPos;
	}
	inst->t_stu_global_obj.nCopy10FrameNum++;
	if (inst->t_stu_global_obj.nCopy10FrameNum == 10)
	{
		inst->t_stu_global_obj.nCopy10FrameNum = 0;
	}

	return 0;
}

int FindMaxIndex(int* val, int valNum)
{
	int i, ret = 0, num = 0;
	for(i = 0; i < valNum; i++)
	{
		if(*(val+i) > ret)
		{
			ret = *(val+i);
			num = i;
		}
	}
	return num;
}

int FindMax(int* val, int valNum)
{
	int i, ret = 0, num = 0;
	for(i = 0; i < valNum; i++)
	{
		if(*(val+i) > ret)
		{
			ret = *(val+i);
			num = i;
		}
	}
	return ret;
}

int FindMinIndex(int* val, int valNum)
{
    int i, ret = 9999, num = 0;
	for(i = 0; i < valNum; i++)
	{
		if(*(val+i) < ret)
		{
			ret = *(val+i);
			num = i;
		}
	}
	return num;
}

int FindMin(int* val, int valNum)
{
    int i, ret = 9999, num = 0;
	for(i = 0; i < valNum; i++)
	{
		if(*(val+i) < ret)
		{
			ret = *(val+i);
			num = i;
		}
	}
	return ret;
}

int PosExchange(StuTrackHand *inst, int xIn , int yIn, unsigned char* paucPos)
{
	STR_RECT_INDEX stIndex;
	int x[10],y[10], i, j,iMax, xMax, yMax, xMin, yMin, iLenx, iLenxIn, iLeny, iLenyIn, iZoomLen, iZoomIn, iZoomPos, tmp, iDistanceCam1, iDistanceCam2;
	unsigned int iDistanceCam, iDistanceCamIn, iDistancex, iCamPos, iDistancey, iComparePos;
	StuITRACK_DynamicParams* pStuParam ;
	unsigned char* pacOutPos = paucPos;
	pStuParam = &inst->input_param.dynamic_param;
	iMax = pStuParam->track_point_num <= 4 ? pStuParam->track_point_num : 4;
	
	for(i = 0; i < iMax; i++)
	{
		x[i] = pStuParam->track_point[i].x;
		y[i] = pStuParam->track_point[i].y;
	}
	/*
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			
			printf("%d,%d,%d\n",pStuParam->CamPrePos[j].x[i],pStuParam->CamPrePos[j].y[i],pStuParam->CamPrePos[j].z[i]);
		}
	}*/
	stIndex.ucxMin = FindMinIndex(x, iMax);
	stIndex.ucxMax = FindMaxIndex(x, iMax);
	stIndex.ucyMin = FindMinIndex(y, iMax);
	stIndex.ucyMax = FindMaxIndex(y, iMax);
	xMin = FindMin(x, iMax);
	xMax = FindMax(x, iMax);
	yMin = FindMin(y, iMax);
	yMax = FindMax(y, iMax);
	//printf("ucxmin=%d,ucymax=%d,ucymin=%d,ucymax=%d,xmin=%d,xmax=%d,ymin=%d,ymax=%d\n",stIndex.ucxMin,stIndex.ucxMax,
	//	stIndex.ucyMin,stIndex.ucyMax,xMin,xMax,yMin,yMax);
	if(xIn < xMin || xIn > xMax || yIn < yMin || yIn > yMax)
	{
	//printf("here0\n");
		return 0;
	}
	iLenx = xMax - xMin;
	iLenxIn = xIn - xMin;
	if(iLenxIn < 1)
	{iLenxIn = 1;}
	iLeny = yMax - yMin;
	iLenyIn = yIn - yMin;
	if(iLenyIn < 1)
	{iLenyIn = 1;}
	
	//XXXXXXXXXXXXXXX
	iComparePos = 0;
	for(i = 0; i < 4; i++)
	{
	  if(pStuParam->CamPrePos[stIndex.ucxMin].x[i] != pStuParam->CamPrePos[stIndex.ucxMax].x[i])
	  {
	     iComparePos = i;
		 break;
	  }
	}
	if((pStuParam->CamPrePos[stIndex.ucxMin].x[3] == pStuParam->CamPrePos[stIndex.ucxMax].x[3])
		&& (iComparePos == 3))
	{
	printf("ERR| xmin %d %d %d %d = xmax\n", pStuParam->CamPrePos[stIndex.ucxMin].x[0],
		pStuParam->CamPrePos[stIndex.ucxMin].x[1], pStuParam->CamPrePos[stIndex.ucxMin].x[2],
		pStuParam->CamPrePos[stIndex.ucxMin].x[3]);
	    return 0;
	}
		
	/*if(pStuParam->CamPrePos[stIndex.ucxMin].x[0] == pStuParam->CamPrePos[stIndex.ucxMax].x[0])
	{


		 iComparePos = 1;
	}
	else if(pStuParam->CamPrePos[stIndex.ucxMin].x[1] == pStuParam->CamPrePos[stIndex.ucxMax].x[1])
	{
		 iComparePos = 2;
	}
	else if(pStuParam->CamPrePos[stIndex.ucxMin].x[2] == pStuParam->CamPrePos[stIndex.ucxMax].x[2])
	{
		 iComparePos = 3;
	}
	else if(pStuParam->CamPrePos[stIndex.ucxMin].x[3] == pStuParam->CamPrePos[stIndex.ucxMax].x[3])//ERR
	{
	printf("here1\n");
		return 0;
	}*/

	if(pStuParam->CamPrePos[stIndex.ucxMin].x[0] > 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] > 8)
	{
		iDistanceCam = abs(pStuParam->CamPrePos[stIndex.ucxMin].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMin].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMin].x[2]*16
			  + pStuParam->CamPrePos[stIndex.ucxMin].x[3] - (pStuParam->CamPrePos[stIndex.ucxMax].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMax].x[1]*256 
			  + pStuParam->CamPrePos[stIndex.ucxMax].x[2]*16 + pStuParam->CamPrePos[stIndex.ucxMax].x[3]));
		iDistanceCamIn = (iDistanceCam * iLenxIn)/iLenx;
	}
	else if(pStuParam->CamPrePos[stIndex.ucxMin].x[0] < 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] < 8)
	{
		iDistanceCam = abs(pStuParam->CamPrePos[stIndex.ucxMin].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMin].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMin].x[2]*16
				  + pStuParam->CamPrePos[stIndex.ucxMin].x[3] - (pStuParam->CamPrePos[stIndex.ucxMax].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMax].x[1]*256
				  + pStuParam->CamPrePos[stIndex.ucxMax].x[2]*16 + pStuParam->CamPrePos[stIndex.ucxMax].x[3]));
		iDistanceCamIn = (iDistanceCam * iLenxIn)/iLenx;
	}
	else
	{
		if(pStuParam->CamPrePos[stIndex.ucxMin].
x[0] < 8)
		{
			 iDistanceCam1 = pStuParam->CamPrePos[stIndex.ucxMin].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMin].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMin].x[2]*16
					  + pStuParam->CamPrePos[stIndex.ucxMin].x[3];
			 iDistanceCam2 = 0xffff - (pStuParam->CamPrePos[stIndex.ucxMax].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMax].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMax].x[2]*16
					  + pStuParam->CamPrePos[stIndex.ucxMax].x[3]) + 1;
			 iDistanceCam = iDistanceCam1 + iDistanceCam2;
			 iDistanceCamIn = (iDistanceCam * iLenxIn)/iLenx;
		}
		else
		{
			 iDistanceCam1 = pStuParam->CamPrePos[stIndex.ucxMax].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMax].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMax].x[2]*16
					  + pStuParam->CamPrePos[stIndex.ucxMax].x[3];
			 iDistanceCam2 = 0xffff - (pStuParam->CamPrePos[stIndex.ucxMin].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMin].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMin].x[2]*16
					  + pStuParam->CamPrePos[stIndex.ucxMin].x[3]) + 1;
			 iDistanceCam = iDistanceCam1 + iDistanceCam2;
			 iDistanceCamIn = (iDistanceCam * iLenxIn)/iLenx;
		}
	}
	
	if((pStuParam->CamPrePos[stIndex.ucxMin].x[0] < 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] > 8)
			|| (pStuParam->CamPrePos[stIndex.ucxMin].x[0] < 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] < 8 
						&& pStuParam->CamPrePos[stIndex.ucxMin].x[iComparePos] > pStuParam->CamPrePos[stIndex.ucxMax].x[iComparePos])
			|| (pStuParam->CamPrePos[stIndex.ucxMin].x[0] > 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] > 8 
						&& pStuParam->CamPrePos[stIndex.ucxMin].x[iComparePos] > pStuParam->CamPrePos[stIndex.ucxMax].x[iComparePos]))
	{
		tmp = (pStuParam->CamPrePos[stIndex.ucxMin].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMin].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMin].x[2]*16
					  + pStuParam->CamPrePos[stIndex.ucxMin].x[3]) - iDistanceCamIn;
		if(tmp >= 0)
		{
			 iCamPos = tmp;
		}
		else
		{
			 iCamPos = 0xffff + tmp + 1;
		}
	}
	else if((pStuParam->CamPrePos[stIndex.ucxMin].x[0] > 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] < 8)
			|| (pStuParam->CamPrePos[stIndex.ucxMin].x[0] < 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] < 8 
						&& pStuParam->CamPrePos[stIndex.ucxMin].x[iComparePos] < pStuParam->CamPrePos[stIndex.ucxMax].x[iComparePos])
			|| (pStuParam->CamPrePos[stIndex.ucxMin].x[0] > 8 && pStuParam->CamPrePos[stIndex.ucxMax].x[0] > 8
						&& pStuParam->CamPrePos[stIndex.ucxMin].x[iComparePos] < pStuParam->CamPrePos[stIndex.ucxMax].x[iComparePos]))
	{
		tmp = (pStuParam->CamPrePos[stIndex.ucxMin].x[0]*4096 + pStuParam->CamPrePos[stIndex.ucxMin].x[1]*256 + pStuParam->CamPrePos[stIndex.ucxMin].x[2]*16
			  + pStuParam->CamPrePos[stIndex.ucxMin].x[3]) + iDistanceCamIn;
		if(tmp > 0xffff)
		{
			 iCamPos = tmp - 0xffff - 1;
		}
		else
		{
			 iCamPos = tmp;
		}
	}
	else         //eqaul
	{
	printf("xmin=%d,xmax=%d,xmincom=%d,xmaxcm=%d,here2\n",pStuParam->CamPrePos[stIndex.ucxMin].x[0],pStuParam->CamPrePos[stIndex.ucxMax].x[0],
		pStuParam->CamPrePos[stIndex.ucxMin].x[iComparePos],pStuParam->CamPrePos[stIndex.ucxMax].x[iComparePos]);
		return 0;
	}
	
	 pacOutPos[0] =  iCamPos / 4096;
	 pacOutPos[1] =  (iCamPos % 4096)/256;
	 pacOutPos[2] =  (iCamPos % 256)/16;
	 pacOutPos[3] =  iCamPos % 16;

	 //YYYYYYYYYYYYYYYYYYYYY	
	iComparePos = 0;
	for(i = 0; i < 4; i++)
	{
	  if(pStuParam->CamPrePos[stIndex.ucxMin].x[i] != pStuParam->CamPrePos[stIndex.ucxMax].x[i])
	  {
	     iComparePos = i;
		 break;
	  }
	}
	if((pStuParam->CamPrePos[stIndex.ucxMin].x[3] == pStuParam->CamPrePos[stIndex.ucxMax].x[3])
		&& (iComparePos == 3))
	{
	printf("ERR| xmin %d %d %d %d = xmax\n", pStuParam->CamPrePos[stIndex.ucxMin].x[0],
		pStuParam->CamPrePos[stIndex.ucxMin].x[1], pStuParam->CamPrePos[stIndex.ucxMin].x[2],
		pStuParam->CamPrePos[stIndex.ucxMin].x[3]);
	    return 0;
	}
	/*if(pStuParam->CamPrePos[stIndex.ucyMin].y[0] == pStuParam->CamPrePos[stIndex.ucyMax].y[0])
	{
		 iComparePos = 1;
	}
	else if(pStuParam->CamPrePos[stIndex.ucyMin].y[1] == pStuParam->CamPrePos[stIndex.ucyMax].y[1])
	{
		 iComparePos = 2;
	}
	else if(pStuParam->CamPrePos[stIndex.ucyMin].y[2] == pStuParam->CamPrePos[stIndex.ucyMax].y[2])
	{
		 iComparePos = 3;
	}
	else if(pStuParam->CamPrePos[stIndex.ucyMin].y[3] == pStuParam->CamPrePos[stIndex.ucyMax].y[3])//ERR
	{
	printf("ucxmin=%d,ucxmax=%d,ucymin=%d,ucymax=%d,xmin=%d,xmax=%d,ymin=%d,ymax=%d\n",stIndex.ucxMin,stIndex.ucxMax,
		stIndex.ucyMin,stIndex.ucyMax,xMin,xMax,yMin,yMax);
	printf("pStuParam ymin0=%d,ymax0=%d,ymin1=%d,ymax1=%d,ymin2=%d,ymax2=%d,ymin3=%d,ymax3=%d\n",
		pStuParam->CamPrePos[stIndex.ucyMin].y[0],pStuParam->CamPrePos[stIndex.ucyMax].y[0],
		pStuParam->CamPrePos[stIndex.ucyMin].y[1],pStuParam->CamPrePos[stIndex.ucyMax].y[1],
		pStuParam->CamPrePos[stIndex.ucyMin].y[2],pStuParam->CamPrePos[stIndex.ucyMax].y[2],
		pStuParam->CamPrePos[stIndex.ucyMin].y[3],pStuParam->CamPrePos[stIndex.ucyMax].y[3]);
		return 0;
	}*/

	if(pStuParam->CamPrePos[stIndex.ucyMin].y[0] > 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] > 8)
	{
		iDistanceCam = abs(pStuParam->CamPrePos[stIndex.ucyMin].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].y[2]*16
			  + pStuParam->CamPrePos[stIndex.ucyMin].y[3] - (pStuParam->CamPrePos[stIndex.ucyMax].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMax].y[1]*256 
			  + pStuParam->CamPrePos[stIndex.ucyMax].y[2]*16 + pStuParam->CamPrePos[stIndex.ucyMax].y[3]));
		iDistanceCamIn = (iDistanceCam * iLenyIn)/iLeny;
	}
	else if(pStuParam->CamPrePos[stIndex.ucyMin].y[0] < 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] < 8)
	{
		iDistanceCam = abs(pStuParam->CamPrePos[stIndex.ucyMin].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].y[2]*16
				  + pStuParam->CamPrePos[stIndex.ucyMin].y[3] - (pStuParam->CamPrePos[stIndex.ucyMax].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMax].y[1]*256
				  + pStuParam->CamPrePos[stIndex.ucyMax].y[2]*16 + pStuParam->CamPrePos[stIndex.ucyMax].y[3]));
		iDistanceCamIn = (iDistanceCam * iLenyIn)/iLeny;
	}
	else
	{
		if(pStuParam->CamPrePos[stIndex.ucyMin].y[0] < 8)
		{
			 iDistanceCam1 = pStuParam->CamPrePos[stIndex.ucyMin].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].y[2]*16
					  + pStuParam->CamPrePos[stIndex.ucyMin].y[3];
			 iDistanceCam2 = 0xffff - (pStuParam->CamPrePos[stIndex.ucyMax].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMax].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMax].y[2]*16
					  + pStuParam->CamPrePos[stIndex.ucyMax].y[3]) + 1;
			 iDistanceCam = iDistanceCam1 + iDistanceCam2;
			 iDistanceCamIn = (iDistanceCam * iLenyIn)/iLeny;
		}
		else
		{
			 iDistanceCam1 = pStuParam->CamPrePos[stIndex.ucyMax].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMax].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMax].y[2]*16
					  + pStuParam->CamPrePos[stIndex.ucyMax].y[3];
			 iDistanceCam2 = 0xffff - (pStuParam->CamPrePos[stIndex.ucyMin].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].y[2]*16
					  + pStuParam->CamPrePos[stIndex.ucyMin].y[3]) + 1;
			 iDistanceCam = iDistanceCam1 + iDistanceCam2;
			 iDistanceCamIn = (iDistanceCam * iLenyIn)/iLeny;
		}
	}

	if((pStuParam->CamPrePos[stIndex.ucyMin].y[0] < 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] > 8)
			|| (pStuParam->CamPrePos[stIndex.ucyMin].y[0] < 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] < 8 
						&& pStuParam->CamPrePos[stIndex.ucyMin].y[iComparePos] > pStuParam->CamPrePos[stIndex.ucyMax].y[iComparePos])
			|| (pStuParam->CamPrePos[stIndex.ucyMin].y[0] > 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] > 8 
						&& pStuParam->CamPrePos[stIndex.ucyMin].y[iComparePos] > pStuParam->CamPrePos[stIndex.ucyMax].y[iComparePos]))
	{
		tmp = (pStuParam->CamPrePos[stIndex.ucyMin].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].y[2]*16
					  + pStuParam->CamPrePos[stIndex.ucyMin].y[3]) - iDistanceCamIn;
		if(tmp >= 0)
		{
			 iCamPos = tmp;
		}
		else
		{
			 iCamPos = 0xffff + tmp + 1;
		}
	}
	else if((pStuParam->CamPrePos[stIndex.ucyMin].y[0] > 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] < 8)
			|| (pStuParam->CamPrePos[stIndex.ucyMin].y[0] < 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] < 8 
						&& pStuParam->CamPrePos[stIndex.ucyMin].y[iComparePos] < pStuParam->CamPrePos[stIndex.ucyMax].y[iComparePos])
			|| (pStuParam->CamPrePos[stIndex.ucyMin].y[0] > 8 && pStuParam->CamPrePos[stIndex.ucyMax].y[0] > 8
						&& pStuParam->CamPrePos[stIndex.ucyMin].y[iComparePos] < pStuParam->CamPrePos[stIndex.ucyMax].y[iComparePos]))
	{
		tmp = (pStuParam->CamPrePos[stIndex.ucyMin].y[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].y[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].y[2]*16
			  + pStuParam->CamPrePos[stIndex.ucyMin].y[3]) + iDistanceCamIn;
		if(tmp > 0xffff)
		{
			 iCamPos = tmp - 0xffff - 1;
		}
		else
		{
			 iCamPos = tmp;
		}
	}
	else         //eqaul
	{
	//printf("here4\n");
		return 0;
	}
	 pacOutPos[4] =  iCamPos / 4096;
	 pacOutPos[5] =  (iCamPos % 4096)/256;
	 pacOutPos[6] =  (iCamPos % 256)/16;
	 pacOutPos[7] =  iCamPos % 16;

	 //ZZZZZZZZZZZZZ
	 iZoomLen = abs(pStuParam->CamPrePos[stIndex.ucyMin].z[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].z[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].z[2]*16
		 + pStuParam->CamPrePos[stIndex.ucyMin].z[3] -(pStuParam->CamPrePos[stIndex.ucyMax].z[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMax].z[1]*256 
		 + pStuParam->CamPrePos[stIndex.ucyMax].z[2]*16 + pStuParam->CamPrePos[stIndex.ucyMax].z[3]));
	 if(iZoomLen != 0)
	 {
	 	iZoomIn = (iZoomLen * iLenyIn)/iLeny;
	    iZoomPos = pStuParam->CamPrePos[stIndex.ucyMin].z[0]*4096 + pStuParam->CamPrePos[stIndex.ucyMin].z[1]*256 + pStuParam->CamPrePos[stIndex.ucyMin].z[2]*16
		 + pStuParam->CamPrePos[stIndex.ucyMin].z[3] - iZoomIn;
	    pacOutPos[8] =  iZoomPos/4096;
	    pacOutPos[9] =  (iZoomPos % 4096)/256;
	    pacOutPos[10] =  (iZoomPos % 256)/16;
	    pacOutPos[11] =  iZoomPos % 16;
	 }
	 else
	 {
	    pacOutPos[8] =  pStuParam->CamPrePos[stIndex.ucyMin].z[0];
	    pacOutPos[9] =  pStuParam->CamPrePos[stIndex.ucyMin].z[1];
	    pacOutPos[10] = pStuParam->CamPrePos[stIndex.ucyMin].z[2];
	    pacOutPos[11] = pStuParam->CamPrePos[stIndex.ucyMin].z[3];	    
	 }
     //printf("%d %d %d %d\n",pacOutPos[8],pacOutPos[9],pacOutPos[10],pacOutPos[11]);
	// printf("zoomlen=%d,zoomin=%d,zoompos=%d,z0=%d,z1=%d,z2=%d,z3=%d\n",iZoomLen,iZoomIn,iZoomPos,pStuParam->CamPrePos[stIndex.ucyMin].z[0],
	 //	pStuParam->CamPrePos[stIndex.ucyMin].z[1],pStuParam->CamPrePos[stIndex.ucyMin].z[2],pStuParam->CamPrePos[stIndex.ucyMin].z[3]);
	 return 1;
}
FILE* file ;

int32_t	track_students_process(StuTrackHand *inst, int8_t *output_buf, StuITRACK_OutArgs *output_param)
{
	int32_t i,iMax;
	int x,y,z,j;
	int64_t t1,t2,t3,t4,t5,t6,t7;
	unsigned char posCmdOutput[12];
	StuITRACK_Params *input_param;
	if ((NULL == inst) || (NULL == output_buf) || (NULL == output_param))
	{
		printf("process NULL\n");
		return -1;
	}
	
	//if inst->t_stu_global_obj.nFrameNum = 0;
	input_param = &(inst->input_param);	
	inst->t_stu_global_obj.nFrameNum++;
	//printf("pic_width=%d,pic_height=%d\n",inst->input_param.static_param.pic_width,inst->input_param.static_param.pic_height);
	//printf("video_width=%d,pic_height=%d\n",inst->input_param.static_param.video_width,inst->input_param.static_param.video_height);
	
	//memset(output_param, 0, sizeof(StuITRACK_OutArgs));
	//t1 = getCurrentTime();
	data_copy(inst, output_buf);
	//t2 = getCurrentTime();
	//printf("data_copy used time = %lld ms\n",t2 - t1);
	data_copy10(inst, output_buf);
	//t3 = getCurrentTime();
	//printf("data_copy10 used time = %lld ms\n",t3 - t2);
	img_compare_y(inst, output_buf);
	//t4 = getCurrentTime();
	//printf("img_compare_y used time = %lld ms\n",t4 - t3);
	img_compare_y0(inst, output_buf);
	//t5 = getCurrentTime();
	//printf("img_compare_y0 used time = %lld ms\n",t5 - t4);
	img_compare_y1(inst, output_buf);
	//t6 = getCurrentTime();
	//printf("img_compare_y1 used time = %lld ms\n",t6 - t5);
	img_process_y(inst, output_buf, &inst->output_param);
	//t7 = getCurrentTime();

	//printf("img_process_y used time = %lld ms\n",t7 - t6);
	delay_dec(inst->t_stu_global_obj.nFrameNum, &inst->output_param,input_param->dynamic_param.reset_time);
    /*
	if(fwrite(output_buf,sizeof(int8_t),inst->input_param.static_param.pic_height*inst->input_param.static_param.pic_width,file)< 1)
		{
		printf("write error\n");
		}
		*/
	//结果输出
	
		output_param->up_num = 0;
		//active_rect.activeNum = 0;
		for( i=0;i<inst->output_param.up_num;i++)
		{
			//if(inst->t_stu_global_obj.nFrameNum == inst->output_param.frame_num[i])
			//{
				output_param->Up_List[output_param->up_num].x = inst->output_param.Up_List[i].x;
				output_param->Up_List[output_param->up_num].y = inst->output_param.Up_List[i].y;
				output_param->time[output_param->up_num] = inst->output_param.time[i];
				//判断是否在屏蔽区域内
				if (Chick_is_inShield(inst,output_param->Up_List[output_param->up_num].x,output_param->Up_List[output_param->up_num].y) == 0)
				{
					continue;
				}
				
				//画线区域
				/*active_rect.rect[active_rect.activeNum].min_x = output_param->Up_List[active_rect.activeNum].x - 80 < 0?0: output_param->Up_List[active_rect.activeNum].x - 80;
				active_rect.rect[active_rect.activeNum].min_y = output_param->Up_List[active_rect.activeNum].y - 50 < 0?0: output_param->Up_List[active_rect.activeNum].y - 50;
				active_rect.rect[active_rect.activeNum].max_x = output_param->Up_List[active_rect.activeNum].x + 80 >= 1280 ?1279 : output_param->Up_List[active_rect.activeNum].x + 80;
				active_rect.rect[active_rect.activeNum].max_y = output_param->Up_List[active_rect.activeNum].y + 100 >= 720 ?719: output_param->Up_List[active_rect.activeNum].y + 100;
				active_rect.activeNum++;
				printf("x =%d,y=%d\n",output_param->Up_List[output_param->up_num].x,output_param->Up_List[output_param->up_num].y);
				if(PosExchange(inst, output_param->Up_List[output_param->up_num].x, output_param->Up_List[output_param->up_num].y, posCmdOutput) == 0)
				{
					//err
					printf("Exchange error\n");
					return 0;
				}*/
				output_param->up_num++;
			//}
		}
		
	
	return 0;
}
int Re_Track_student_init(StuTrackHand* inst,StuITRACK_Params *params)
{
	if (params == NULL && params->static_param.video_width != WIDTH && params->static_param.video_height != HEIGHT ) {
			return -1;
		}
	else {
		if (params->dynamic_param.track_num_row < 1)
		{
			inst->input_param.dynamic_param.track_num_row = 1;
		}
		else
		{
			inst->input_param.dynamic_param.track_num_row = params->dynamic_param.track_num_row;

		}

		
		inst->input_param.dynamic_param.reset_time = params->dynamic_param.reset_time;
		inst->input_param.dynamic_param.message = params->dynamic_param.message;
		inst->input_param.dynamic_param.reset_level = params->dynamic_param.reset_level;
		if (params->dynamic_param.sens < 0)
		{
			inst->input_param.dynamic_param.sens = 0;
		}
		else
		{
			inst->input_param.dynamic_param.sens = params->dynamic_param.sens;
		}
		
		inst->input_param.dynamic_param.track_point_num = params->dynamic_param.track_point_num;
		//防止跟踪区域越界
		if(params->dynamic_param.track_point[0].x <10 || params->dynamic_param.track_point[0].x >=params->static_param.pic_width*2  - 10)
			params->dynamic_param.track_point[0].x = 10;
		if(params->dynamic_param.track_point[0].y <10 || params->dynamic_param.track_point[0].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[0].y = params->static_param.pic_height*2 - 10;
		if(params->dynamic_param.track_point[1].x <10 || params->dynamic_param.track_point[1].x >=params->static_param.pic_width*2 - 10)
			params->dynamic_param.track_point[1].x = params->static_param.pic_width*2 - 10;
		if(params->dynamic_param.track_point[1].y <10 || params->dynamic_param.track_point[1].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[1].y = params->static_param.pic_height*2 - 10;
		if(params->dynamic_param.track_point[2].x <10 || params->dynamic_param.track_point[2].x >=params->static_param.pic_width*2 - 10)
			params->dynamic_param.track_point[2].x = params->static_param.pic_width*2 - 10;
		if(params->dynamic_param.track_point[2].y <10 || params->dynamic_param.track_point[2].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[2].y = 10;
		if(params->dynamic_param.track_point[3].x <10 || params->dynamic_param.track_point[3].x >=params->static_param.pic_width*2 - 10)
			params->dynamic_param.track_point[3].x = 10;
		if(params->dynamic_param.track_point[3].y <10 || params->dynamic_param.track_point[3].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[3].y = 10;

		memcpy(inst->input_param.dynamic_param.track_point,
					params->dynamic_param.track_point, sizeof(vertex_t)*TRACK_AREA_POINT_MAX);
		memcpy(inst->input_param.dynamic_param.CamPrePos,params->dynamic_param.CamPrePos,sizeof(stCamPos)*4);
		//拷贝屏蔽框
		memcpy(inst->input_param.dynamic_param.shield_info , params->dynamic_param.shield_info,sizeof(student_trigger_info_t)*STUDENTS_SHIELD_NUM);
		

		track_students_init(inst);
		}
}


int TRACK_STUDENTS_initObj(void* handle,StuITRACK_Params *params)
{
int i,j;
int16_t pic_width ,pic_height;
	short m;
	int value = 0;
	StuTrackHand *inst = (StuTrackHand*)handle;
	if (params == NULL && params->static_param.video_width != WIDTH && params->static_param.video_height != HEIGHT ) {
		return -1;
	}
	else {
		if (params->dynamic_param.track_num_row < 1)
		{
			inst->input_param.dynamic_param.track_num_row = 1;
		}
		else
		{
			inst->input_param.dynamic_param.track_num_row = params->dynamic_param.track_num_row;

		}
		
		printf("track_row=%d\n",inst->input_param.dynamic_param.track_num_row);
		inst->input_param.static_param.video_width = params->static_param.video_width;
		inst->input_param.static_param.video_height = params->static_param.video_height;
		inst->input_param.static_param.pic_width = params->static_param.video_width; //缩放为1
		inst->input_param.static_param.pic_height = params->static_param.video_height;
		
		
		pic_width  = inst->input_param.static_param.pic_width;
		pic_height = inst->input_param.static_param.pic_height;

		
		inst->input_param.dynamic_param.reset_time = params->dynamic_param.reset_time;
		inst->input_param.dynamic_param.message = params->dynamic_param.message;
		//inst->input_param.dynamic_param.process_type = params->dynamic_param.process_type;
		inst->input_param.dynamic_param.reset_level = params->dynamic_param.reset_level;
		if (params->dynamic_param.sens < 0)
		{
			inst->input_param.dynamic_param.sens = 0;
		}
		else
		{
			inst->input_param.dynamic_param.sens = params->dynamic_param.sens;
		}
		
		inst->input_param.dynamic_param.track_point_num = params->dynamic_param.track_point_num;
		//inst->input_param.dynamic_param.trigger_num = params->dynamic_param.trigger_num;
		inst->input_param.dynamic_param.x_offset = 1;//params->dynamic_param.x_offset;
		inst->input_param.dynamic_param.y_offset = 1;//params->dynamic_param.y_offset;
		//防止跟踪区域越界
		if(params->dynamic_param.track_point[0].x <10 || params->dynamic_param.track_point[0].x >=params->static_param.pic_width*2  - 10)
			params->dynamic_param.track_point[0].x = 10;
		if(params->dynamic_param.track_point[0].y <10 || params->dynamic_param.track_point[0].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[0].y = params->static_param.pic_height*2 - 10;
		if(params->dynamic_param.track_point[1].x <10 || params->dynamic_param.track_point[1].x >=params->static_param.pic_width*2 - 10)
			params->dynamic_param.track_point[1].x = params->static_param.pic_width*2 - 10;
		if(params->dynamic_param.track_point[1].y <10 || params->dynamic_param.track_point[1].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[1].y = params->static_param.pic_height*2 - 10;
		if(params->dynamic_param.track_point[2].x <10 || params->dynamic_param.track_point[2].x >=params->static_param.pic_width*2 - 10)
			params->dynamic_param.track_point[2].x = params->static_param.pic_width*2 - 10;
		if(params->dynamic_param.track_point[2].y <10 || params->dynamic_param.track_point[2].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[2].y = 10;
		if(params->dynamic_param.track_point[3].x <10 || params->dynamic_param.track_point[3].x >=params->static_param.pic_width*2 - 10)
			params->dynamic_param.track_point[3].x = 10;
		if(params->dynamic_param.track_point[3].y <10 || params->dynamic_param.track_point[3].y >=params->static_param.pic_height*2 - 10)
			params->dynamic_param.track_point[3].y = 10;
	//	printf("copy 0 \n");
	 	inst->input_param.dynamic_param.shield_info = (student_trigger_info_t*)malloc(sizeof(student_trigger_info_t)*STUDENTS_SHIELD_NUM);
		memcpy(inst->input_param.dynamic_param.track_point,
					params->dynamic_param.track_point, sizeof(vertex_t)*TRACK_AREA_POINT_MAX);
		memcpy(inst->input_param.dynamic_param.CamPrePos,params->dynamic_param.CamPrePos,sizeof(stCamPos)*4);
		//拷贝屏蔽框
		//printf("shield=%d,%d\n",params->dynamic_param.shield_info[STUDENTS_SHIELD_NUM-1].x,params->dynamic_param.shield_info[STUDENTS_SHIELD_NUM-1].y);
		memcpy(inst->input_param.dynamic_param.shield_info , params->dynamic_param.shield_info,sizeof(student_trigger_info_t)*STUDENTS_SHIELD_NUM);
		for(i=0;i<4;i++)
			{
			//printf("shield x= %d,y=%d,w=%d,h=%d\n",inst->input_param.dynamic_param.shield_info[i].x,inst->input_param.dynamic_param.shield_info[i].y,
			//	inst->input_param.dynamic_param.shield_info[i].width,inst->input_param.dynamic_param.shield_info[i].height);
			printf("track_point x=%d,y=%d\n",inst->input_param.dynamic_param.track_point[i].x,inst->input_param.dynamic_param.track_point[i].y);
		}
		 
		inst->t_stu_global_obj.students_src_y_buf[0] =  (uint8_t*)malloc(pic_width * pic_height);//memTab[2].base;
		inst->t_stu_global_obj.students_src_y_buf[1] =  (uint8_t*)malloc(pic_width * pic_height);//memTab[3].base;
		inst->t_stu_global_obj.students_dst_y_buf[0] =  (uint8_t*)malloc(pic_width * pic_height);//memTab[4].base;
		inst->t_stu_global_obj.students_dst_y_buf[1] =  (uint8_t*)malloc(pic_width * pic_height);// memTab[5].base;
		inst->t_stu_global_obj.students_dst_y_buf[2] =  (uint8_t*)malloc(pic_width * pic_height);//memTab[6].base;
		inst->t_stu_global_obj.students_time_count[0] = (uint8_t*)malloc(pic_width * pic_height);//memTab[7].base;
		inst->t_stu_global_obj.students_time_count[1] = (uint8_t*)malloc(pic_width * pic_height);// memTab[8].base;
		inst->t_stu_global_obj.students_time_count[2] = (uint8_t*)malloc(pic_width * pic_height);// memTab[9].base;
		inst->t_stu_global_obj.students_time_count[3] = (uint8_t*)malloc(pic_width * pic_height);// memTab[10].base;
		inst->t_stu_global_obj.students_time_count[4] = (uint8_t*)malloc(pic_width * pic_height);//memTab[11].base;
		inst->t_stu_global_obj.students_time_count[5] = (uint8_t*)malloc(pic_width * pic_height);//memTab[12].base;
		inst->t_stu_global_obj.students_time_count[6] = (uint8_t*)malloc(pic_width * pic_height);//memTab[13].base;
		inst->t_stu_global_obj.students_time_count[7] = (uint8_t*)malloc(pic_width * pic_height);//memTab[14].base;
		inst->t_stu_global_obj.students_time_count[8] = (uint8_t*)malloc(pic_width * pic_height);//memTab[15].base;

		inst->t_stu_global_obj.Students_Area_vertex = (uint8_t*)malloc(pic_width * pic_height);//memTab[16].base;

		inst->t_stu_global_obj.students_active_count[0] = (uint8_t*)malloc(pic_width * pic_height);//memTab[7].base;
		inst->t_stu_global_obj.students_active_count[1] = (uint8_t*)malloc(pic_width * pic_height);// memTab[8].base;
		inst->t_stu_global_obj.students_active_count[2] = (uint8_t*)malloc(pic_width * pic_height);// memTab[9].base;
		inst->t_stu_global_obj.students_active_count[3] = (uint8_t*)malloc(pic_width * pic_height);// memTab[10].base;
		inst->t_stu_global_obj.students_active_count[4] = (uint8_t*)malloc(pic_width * pic_height);//memTab[11].base;
		inst->t_stu_global_obj.students_active_count[5] = (uint8_t*)malloc(pic_width * pic_height);//memTab[12].base;
		inst->t_stu_global_obj.students_active_count[6] = (uint8_t*)malloc(pic_width * pic_height);//memTab[13].base;
		inst->t_stu_global_obj.students_active_count[7] = (uint8_t*)malloc(pic_width * pic_height);//memTab[14].base;
		inst->t_stu_global_obj.students_active_count[8] = (uint8_t*)malloc(pic_width * pic_height);//memTab[15].base;


		for(i=0;i<9;i++)
		{
			memset(inst->t_stu_global_obj.students_active_count[i],0,sizeof(pic_width * pic_height));
		}
		
		inst->t_stu_global_obj.students_active_at9count = (uint8_t*)malloc(pic_width * pic_height);//memTab[15].base;
		memset(inst->t_stu_global_obj.students_active_at9count, 0, sizeof(pic_width * pic_height));
		//初始化学生跟踪需要的参数
		
	for(i=0;i<9;i++)
	{
		inst->t_stu_global_obj.student_Head[i] = (Rect_Head*)malloc(sizeof(Rect_Head));
		inst->t_stu_global_obj.student_Head[i]->rect = (rect_t*)malloc(sizeof(rect_t)*1000);
		inst->t_stu_global_obj.student_Head[i]->head = (Head_t*)malloc(sizeof(Head_t)*1000);
		inst->t_stu_global_obj.student_Head[i]->rectNum = 0;
	}
	for(i=0;i<TRACK_AREA_POINT_MAX;i++)

	{

		printf("point %d=%d,%d\n",i,inst->input_param.dynamic_param.track_point[i].x,inst->input_param.dynamic_param.track_point[i].y);
	}
		
		track_students_init(inst);
		
/*
		file = fopen("datastudent.txt","w+");
		if(file == NULL)
		{


			printf("open file fail\n");
		}
			*/
		return 0;
	}


}
int Track_Students_process(void* handle,
	int8_t *outBuf, StuITRACK_OutArgs *outArgs)
{
	uint32_t i;
	StuTrackHand *inst = (StuTrackHand *)handle;
	inst->input_param.dynamic_param.process_type = 0;
	track_students_process(inst, outBuf, outArgs);
	return (ITRACK_EOK);
}
