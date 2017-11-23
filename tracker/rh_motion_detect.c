#include "rh_motion_detect.h"
#include "auto_track.h"
CeilTrackHand g_motion_detect_input_param = {0};
CeilITRACK_OutArgs g_motion_detect_output_param = {0};

int g_modify_param_flag = 0;

int g_display_ppt_flag = 0;	//是否显示的是ppt

int run_ppt_flag = 2;		//0表示释放，1表示运行，2表示停止运行

#define MOTION_DETECT_SCALE		(1)			//检测缩放倍数

#define CAPUTURE_VIDEO_WIDTH	(1920)		//采集视频宽
#define CAPUTURE_VIDEO_HEIGHT	(1080)		//采集视频高

#define MOTION_VIDEO_WIDTH		(CAPUTURE_VIDEO_WIDTH / MOTION_DETECT_SCALE)		//用于检测的图像宽
#define MOTION_VIDEO_HEIGHT		(CAPUTURE_VIDEO_HEIGHT / MOTION_DETECT_SCALE)		//用于检测的图像高
 
#define VIDEO_CUT_WIDTH			(576)		//在原始图像上截取的视频宽度
#define VIDEO_CUT_HEIGHT		(1080)		//在原始图像上截取的视频高度

#define VIDEO_DISPLAY_WIDTH		(384)		//视频在合成画面上显示的宽度
#define VIDEO_DISPLAY_HEIGHT	(720)		//视频在合成画面上显示的高度

#define DISPLAY_WIDTH			(1280)		//输出显示画面的宽度
#define DISPLAY_HEIGHT			(720)		//输出显示画面的宽度

#define VGA_DISPLAY_WIDTH		(DISPLAY_WIDTH - VIDEO_DISPLAY_WIDTH)
#define VGA_DISPLAY_HEIGHT		(DISPLAY_HEIGHT)


/* p, q is on the same of line l */
static int line_is_same(const motion_vertex_t *l_start, const motion_vertex_t *l_end,
	const motion_vertex_t *p,
	const motion_vertex_t *q)

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
static int is_intersect(const motion_vertex_t *s1_start, const motion_vertex_t *s1_end,
	const motion_vertex_t *s2_start, const motion_vertex_t *s2_end)
{
	int i, j;
	i = line_is_same(s1_start, s1_end, s2_start, s2_end);
	j = line_is_same(s2_start, s2_end, s1_start, s1_end);
	if ((i&&j) == 0)
		return 0;
	else
		return 1;

}

/**
* @ 判断点是否落在外包矩阵内，如果落在外包矩阵内，则进行下一步计算
*/
static void vertices_get_extent(const motion_vertex_t *vl, int np, /* in vertices */
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
static int32_t pt_in_poly(const motion_vertex_t *vl, int np,  /* polygon vl with np vertices  */
	const motion_vertex_t *v)
{
	int i, j, k1, k2, c;
	rect_t rc;
	motion_vertex_t w;
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
* @ 函数名称: ceil_track_area_check()
* @	函数功能: 设定跟踪区域
* @ 输入参数: input_param -- 应用层设置的参数
* @ 输出参数: 无
* @ 返回值:   无
*/
void area_check(CeilTrackHand *inst)
{
	CeilITRACK_Params *input_param;
	uint8_t *Area_vertex;
	int32_t	 	k = 0;
	int16_t		video_width;
	int16_t		video_height;

	int32_t		m = 0;
	int32_t		n = 0;
	motion_vertex_t	point_y_value = { 0 };
	if (NULL == inst)
	{
		return;
	}
	input_param = &(inst->input_param);
	Area_vertex = inst->t_global_obj.Area_vertex;
	video_width = input_param->static_param.video_width;
	video_height = input_param->static_param.video_height;

	for (k = 0; k < video_width * video_height; k++)
	{
		m = k%video_width;     //列
		n = k / video_width;    //行

		point_y_value.x = m;
		point_y_value.y = n;

		if (pt_in_poly((const motion_vertex_t *)input_param->dynamic_param.track_point, input_param->dynamic_param.track_point_num,
			&point_y_value) == 1)
		{
			*((uint8_t *)Area_vertex + n * video_width + m) = 255;
		}
		else
		{
			*((uint8_t *)Area_vertex + n * video_width + m) = 0;
		}
	}
}


static int32_t move_check_data_copy(int8_t *output_buf, CeilTrackHand *inst)
{
	int32_t			i = 0;
	uint8_t *src_rgb_buf = inst->t_global_obj.tmp_src_rgb_buf; //存放要进行运算的源rgb数据	


	int32_t nSrcBufHeight = inst->t_global_obj.nSrcBufHeight;
	int32_t nSrcBufWidth = inst->t_global_obj.nSrcBufWidth;

	int32_t 		left = inst->t_global_obj.tInRect.nLeft;
	int32_t 		top = inst->t_global_obj.tInRect.nTop;
	int32_t 		right = inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom = inst->t_global_obj.tInRect.nBottom;
	int32_t         len = (right - left + 1) * 3;
#if 0
	for (i = top; i <= bottom; i++)
	{
		memcpy(src_rgb_buf +( i*nSrcBufWidth + left)* 3, output_buf +( i*nSrcBufWidth + left)* 3, len);
	}
#else

	memcpy(src_rgb_buf, output_buf, nSrcBufHeight * nSrcBufWidth *3 /2);

#endif
	return 0;
}

static int32_t check_track_point(int8_t *output_buf, CeilTrackHand *inst)
{
	CeilITRACK_Params 	*input_param = &(inst->input_param);
	uint8_t 		*gray_buf = inst->t_global_obj.gray_buf;
	int32_t	m = 0;
	int32_t	n = 0;

	int32_t sens = input_param->dynamic_param.sens;
	int16_t	video_width = input_param->static_param.video_width;

	uint8_t *dst_rgb_buf = inst->t_global_obj.dst_rgb_buf; //存放每次跟踪后的第一帧ybuf数据

	uint8_t *Area_vertex = inst->t_global_obj.Area_vertex;
	int32_t nSrcBufWidth = inst->t_global_obj.nSrcBufWidth;
	int32_t nSrcBufHeight = inst->t_global_obj.nSrcBufHeight;
	yuv_value_info_t		yuv_value = { 0 };

	int32_t 		left = inst->t_global_obj.tInRect.nLeft;
	int32_t 		top = inst->t_global_obj.tInRect.nTop;
	int32_t 		right = inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom = inst->t_global_obj.tInRect.nBottom;
	//	int32_t			index           = 0;
	int32_t         len = right - left + 1;

	int32_t a1 = 0;
	int32_t	b1 = 0;
	int32_t a2 = 0;
	int32_t	b2 = 0;
	int32_t a3 = 0;
	int32_t	b3 = 0;
	int32_t	c1 = 0;
	int32_t	c2 = 0;
	int32_t	c3 = 0;
	int32_t offset = nSrcBufWidth*nSrcBufHeight;

	//UInt32 time0= Utils_getCurTimeInMsec();

	int SrcRowOffset = 0;
	int DstRowOffset = 0;

	int src_offset = 0;
	int dst_offset = 0;
	
	/*printf("sens = %d\n", sens);
	printf("video_width = %d\n", video_width);
	printf("nSrcBufWidth = %d\n", nSrcBufWidth);
	printf("nSrcBufHeight = %d\n", nSrcBufHeight);
	printf("left = %d\n", left);
	printf("top = %d\n", top);
	printf("right = %d\n", right);
	printf("bottom = %d\n", bottom);
	//printf("offset = %d\n", offset);*/
	for (n = top; n <= bottom; n++)
	{
		for (m = left; m <= right; m++)
		{

			if (*((uint8_t *)Area_vertex + n*video_width + m) == 0)
			{
				continue;
			}

#if 0
			//检测y是否有变化
			a1 = *((uint8_t *)output_buf + (video_width * n + m) * 3);
			b1 = *((uint8_t *)dst_rgb_buf + (video_width * n + m) * 3);
			//printf("a1= %d,b1 = %d\n",a1,b1);
			//printf("sens = %d\n",sens);
			c1 = abs(a1 - b1);
			//printf("a1= %d,b1 = %d,c1 = %d\n",a1,b1,c1);
			if (c1 > sens)
			{

			*((uint8_t *)output_buf + (video_width * n + m)*3) = 0;
			*((uint8_t *)output_buf + (video_width * n + m) * 3 + 1) = 0;
			*((uint8_t *)output_buf + (video_width * n + m) * 3 + 2) = 0;
				*(gray_buf + video_width * n + m) = 1;
				continue;
			}



			//检查cb是否有变化
			a2 = *((uint8_t *)output_buf + (video_width * n + m) * 3 +1);
			b2 = *((uint8_t *)dst_rgb_buf + (video_width * n + m) * 3 + 1);
			c2 = abs(a2 - b2);
			if (c2 > sens)
			{

				*((uint8_t *)output_buf + (video_width * n + m) * 3) = 0;
				*((uint8_t *)output_buf + (video_width * n + m) * 3 + 1) = 0;
				*((uint8_t *)output_buf + (video_width * n + m) * 3 + 2) = 0;
				*(gray_buf + video_width * n + m) = 1;
				continue;
			}

			a3 = *((uint8_t *)output_buf + (video_width * n + m) * 3 + 2);
			b3 = *((uint8_t *)dst_rgb_buf + (video_width * n + m) * 3 + 2);
			c3 = abs(a3 - b3);
			if (c3 > sens)
			{

				*((uint8_t *)output_buf + (video_width * n + m) * 3) = 0;
				*((uint8_t *)output_buf + (video_width * n + m) * 3 + 1) = 0;
				*((uint8_t *)output_buf + (video_width * n + m) * 3 + 2) = 0;
				*(gray_buf + video_width * n + m) = 1;
				continue;
			}
#else
			//检测y是否有变化
			a1 = *((uint8_t *)output_buf + (video_width * n + m) );
			b1 = *((uint8_t *)dst_rgb_buf + (video_width * n + m));
			c1 = abs(a1 - b1);

			if (c1 > sens)
			{

				*((uint8_t *)output_buf + (video_width * n + m)) = 0;
				*(gray_buf + video_width * n + m) = 1;
				continue;
			}
#endif
			*(gray_buf + video_width * n + m) = 0;

		}
	}
	return 1;
}

static int32_t set_check_max_rect(CeilTrackHand *inst)
{
	CeilITRACK_Params 	*input_param	= &(inst->input_param);
	int32_t		index	= 0;
	int32_t		min_x	= inst->t_global_obj.nSrcBufWidth;
	int32_t		min_y	= inst->t_global_obj.nSrcBufHeight;
	int32_t		max_x	= 0;
	int32_t		max_y	= 0;

	printf("input_param->dynamic_param.track_point_num = %d\n",input_param->dynamic_param.track_point_num);
	for(index = 0; index < input_param->dynamic_param.track_point_num; index++)
	{
		printf("input_param->dynamic_param.track_point[index].x  = %d\n",input_param->dynamic_param.track_point[index].x );
		printf("input_param->dynamic_param.track_point[index].y  = %d\n",input_param->dynamic_param.track_point[index].y );
		if(input_param->dynamic_param.track_point[index].x < min_x)
		{
			min_x = input_param->dynamic_param.track_point[index].x;
		}

		if(input_param->dynamic_param.track_point[index].x > max_x)
		{
			max_x = input_param->dynamic_param.track_point[index].x;
		}

		if(input_param->dynamic_param.track_point[index].y < min_y)
		{
			min_y = input_param->dynamic_param.track_point[index].y;
		}

		
		if(input_param->dynamic_param.track_point[index].y > max_y)
		{
			max_y = input_param->dynamic_param.track_point[index].y;
		}
	}

	if(max_x > input_param->static_param.video_width)
	{
		max_x = input_param->static_param.video_width;
	}

	if(max_y > input_param->static_param.video_height)
	{
		max_y = input_param->static_param.video_height;
	}

	inst->t_global_obj.tInRect.nLeft 	= min_x;
	inst->t_global_obj.tInRect.nRight	= max_x;
	inst->t_global_obj.tInRect.nTop   	= min_y;
	inst->t_global_obj.tInRect.nBottom  = max_y;


	//Vps_printf("%d %d %d %d\n",min_x,max_x,min_y,max_y);
	return 0;
}


//void GrayClose(uint8_t *pGray,uint8_t *pMidBuf,int32_t nWidth,T_Rect tInRect)
static int32_t gray_close(CeilTrackHand *inst)
{
	CeilITRACK_Params 	*input_param = &(inst->input_param);
	uint8_t *Area_vertex = inst->t_global_obj.Area_vertex;
	int32_t 		nWidth = input_param->static_param.video_width;
	uint8_t 		*pGray = inst->t_global_obj.gray_buf;
	uint8_t 		*pMidBuf = inst->t_global_obj.mid_buf;
	int 			i = 0;
	int 			j = 0;
	int 			index = 0;
	uint8_t 		pixel = 0;
	uint8_t 		temp[9];
	int32_t 		left = inst->t_global_obj.tInRect.nLeft;
	int32_t 		top = inst->t_global_obj.tInRect.nTop;
	int32_t 		right = inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom = inst->t_global_obj.tInRect.nBottom;

	memset(pMidBuf, 0, (right - left + 1)*(bottom - top + 1));

	//膨胀
	for (i = top + 1; i<bottom; i++)
	{
		for (j = left + 1; j<right; j++)
		{
			index = i*nWidth + j;

			if (*((uint8_t *)Area_vertex + i*nWidth + j) == 0)
			{
				continue;
			}
			temp[0] = pGray[index - nWidth - 1];
			temp[1] = pGray[index - nWidth];
			temp[2] = pGray[index - nWidth + 1];
			temp[3] = pGray[index - 1];
			temp[4] = pGray[index];
			temp[5] = pGray[index + 1];
			temp[6] = pGray[index + nWidth - 1];
			temp[7] = pGray[index + nWidth];
			temp[8] = pGray[index + nWidth + 1];
			pixel = temp[0] | temp[1] | temp[2] | temp[3] | temp[4] | temp[5] | temp[6] | temp[7] | temp[8];
			pMidBuf[index] = pixel;
		}
	}
#if 0
	//膨胀
	for (i = top + 1; i<bottom; i++)
	{
		for (j = left + 1; j<right; j++)
		{
			index = i*nWidth + j;
			if (*((uint8_t *)Area_vertex + i*nWidth + j) == 0)
			{
				continue;
			}

			temp[0] = pMidBuf[index - nWidth - 1];
			temp[1] = pMidBuf[index - nWidth];
			temp[2] = pMidBuf[index - nWidth + 1];
			temp[3] = pMidBuf[index - 1];
			temp[4] = pMidBuf[index];
			temp[5] = pMidBuf[index + 1];
			temp[6] = pMidBuf[index + nWidth - 1];
			temp[7] = pMidBuf[index + nWidth];
			temp[8] = pMidBuf[index + nWidth + 1];
			pixel = temp[0] | temp[1] | temp[2] | temp[3] | temp[4] | temp[5] | temp[6] | temp[7] | temp[8];
			pGray[index] = pixel;
		}
	}

	//腐蚀
	for (i = top + 1; i<bottom; i++)
	{
		for (j = left + 1; j<right; j++)
		{
			index = i*nWidth + j;
			if (*((uint8_t *)Area_vertex + i*nWidth + j) == 0)
			{
				continue;
			}

			temp[0] = pGray[index - nWidth - 1];
			temp[1] = pGray[index - nWidth];
			temp[2] = pGray[index - nWidth + 1];
			temp[3] = pGray[index - 1];
			temp[4] = pGray[index];
			temp[5] = pGray[index + 1];
			temp[6] = pGray[index + nWidth - 1];
			temp[7] = pGray[index + nWidth];
			temp[8] = pGray[index + nWidth + 1];
			pixel = temp[0] & temp[1] & temp[2] & temp[3] & temp[4] & temp[5] & temp[6] & temp[7] & temp[8];
			pMidBuf[index] = pixel;
		}
	}
#endif
	//腐蚀
	for (i = top + 1; i<bottom; i++)
	{
		for (j = left + 1; j<right; j++)
		{
			index = i*nWidth + j;
			if (*((uint8_t *)Area_vertex + i*nWidth + j) == 0)
			{
				continue;
			}

			temp[0] = pMidBuf[index - nWidth - 1];
			temp[1] = pMidBuf[index - nWidth];
			temp[2] = pMidBuf[index - nWidth + 1];
			temp[3] = pMidBuf[index - 1];
			temp[4] = pMidBuf[index];
			temp[5] = pMidBuf[index + 1];
			temp[6] = pMidBuf[index + nWidth - 1];
			temp[7] = pMidBuf[index + nWidth];
			temp[8] = pMidBuf[index + nWidth + 1];
			pixel = temp[0] & temp[1] & temp[2] & temp[3] & temp[4] & temp[5] & temp[6] & temp[7] & temp[8];
			pGray[index] = pixel;
		}
	}
	return 0;
}
static void connect(int tab[], int i, int j)
{
	int mid;
	while (1)
	{
		if (i == j)
		{
			return;
		}
		if (tab[i] == i)
		{
			tab[i] = j;
		}
		if (tab[i]>j)
		{
			i = tab[i];
		}
		else if (tab[i]<j)
		{
			mid = i;
			i = j;
			j = tab[mid];
		}
		else
		{
			return;
		}
	}
}

static int Mark_value(int tab[], int i)
{
	while (1)
	{
		if (tab[i] == i)
		{
			return i;
		}
		else
		{
			i = tab[i];
		}
	}
}

static void MergerTable(int tab[], int mark_num)
{
	int j = 0, temp = 0;
	for (j = mark_num; j>0; j--)
	{
		temp = Mark_value(tab, j);
		tab[j] = temp;

	}
}

static int32_t find_block_kernel(CeilTrackHand *inst)
{
	CeilITRACK_Params 	*input_param = &(inst->input_param);
	uint8_t 		*pGray = inst->t_global_obj.gray_buf;
	uint32_t 		*pMidBuf = (uint32_t *)inst->t_global_obj.mid_buf;
	uint16_t 		*pMarkedBuf = inst->t_global_obj.marked_buf;
	int32_t 		nWidth = input_param->static_param.video_width;
	T_Rect 			*tOutRects = (inst->t_global_obj.g_out_rect);

	int32_t 		nObjectCount = 0;
	T_Rect 		rect = { SHORT_MAX, SHORT_MAX, 0, 0 };
	int32_t 		left = inst->t_global_obj.tInRect.nLeft;
	int32_t 		top = inst->t_global_obj.tInRect.nTop;
	int32_t 		right = inst->t_global_obj.tInRect.nRight;
	int32_t 		bottom = inst->t_global_obj.tInRect.nBottom;
	int32_t 		nIndex = 0;
	T_Rect 			*tObjectRects;
	int32_t 		*tab, *marks;

	int32_t 		mark_min = 0;
	int32_t 		mark_num = 0;
	int32_t 		mark_count = 0;
	int32_t 		rect_count = 0;
	int32_t			bFound = 0;
	int32_t 		mark_new = -1;
	int32_t 		mark_old = -1;
	int32_t 		nRectWidth = 0;
	int32_t 		nRectHeight = 0;

	int32_t 		x1 = 0, x2 = 0;
	int32_t 		xL = 0, xR = 0;
	int32_t 		i = 0, j = 0, k = 0, n = 0;

	if (pGray == NULL)
	{
		return -1;
	}

	tab = (int32_t*)pMidBuf;
	marks = tab + 4096 * 16;
	tObjectRects = (T_Rect*)(marks + 1024 * 16);

	memset(tab, 0, 4096 * 4 * 16);
	memset(marks, 0, 1024 * 4 * 16);

	nIndex = top*nWidth;

	for (i = top; i <= bottom; i++)
	{
		n = 0;
		for (j = left; j <= right; j++)
		{
			pMarkedBuf[nIndex + j] = 0;
			if (pGray[nIndex + j] != 0)
			{
				if (0 == n)
				{
					x1 = j;
					x2 = j;
				}
				else
				{
					x2 = j;
				}
				n++;
			}
			if ((pGray[nIndex + j] == 0) || (j == right))
			{
				if (n>0)
				{

					bFound = 0;
					mark_min = SHORT_MAX;
					if (i>top)
					{
						mark_new = -1;
						mark_old = -1;
						//xL=(x1-1);
						xL = (x1);
						xR = (x2 + 1);
						if (xL<left)
						{
							xL = left;
						}

						if (xR>right)
						{
							xR = right;
						}
						mark_count = 0;
						for (k = xL; k <= xR; k++)
						{
							if (pGray[nIndex - nWidth + k] != 0)
							{
								bFound = 1;
								mark_new = pMarkedBuf[nIndex - nWidth + k];
								if (mark_new != mark_old)
								{
									mark_old = mark_new;
									marks[mark_count] = mark_new;
									mark_count++;
								}
								if (mark_new<mark_min)
								{
									mark_min = mark_new;
								}
							}
						}
						for (k = 0; k<mark_count; k++)
						{
							connect(tab, marks[k], mark_min);
						}
					}

					if (!bFound)
					{
						mark_num++;
						tab[mark_num] = mark_num;
						for (k = x1; k <= x2; k++)
						{
							pMarkedBuf[nIndex + k] = mark_num;
						}
					}
					else
					{
						for (k = x1; k <= x2; k++)
						{
							pMarkedBuf[nIndex + k] = mark_min;
						}
					}
				}
				n = 0;
			}
		}
		nIndex += nWidth;
	}

	MergerTable(tab, mark_num);
	for (i = 0; i<MAXRECTNUM; i++)
	{
		tObjectRects[i].nLeft = SHORT_MAX;
		tObjectRects[i].nTop = SHORT_MAX;
		tObjectRects[i].nRight = 0;
		tObjectRects[i].nBottom = 0;
		tObjectRects[i].nFillSize = 0;
	}

	nIndex = top*nWidth;
	for (i = top; i <= bottom; i++)
	{
		//nIndex=i*nWidth+left;
		for (j = left; j <= right; j++)
		{
			k = tab[pMarkedBuf[nIndex + j]];

			if ((0 == k) || (k >= MAXRECTNUM))
				//if((0==k))
			{
				//nIndex++;
				continue;
			}
			//rect.nLeft=SHORT_MAX;
			//rect.nTop=SHORT_MAX;
			//rect.nRight=0;
			//rect.nBottom=0;
			pMarkedBuf[nIndex + j] = k;
			if (k>rect_count)
			{
				rect_count = k;
			}
			rect = tObjectRects[k];
			rect.nFillSize++;
			if (i<rect.nTop)
			{
				rect.nTop = i;
			}
			if (i>rect.nBottom)
			{
				rect.nBottom = i;
			}
			if (j<rect.nLeft)
			{
				rect.nLeft = j;
			}
			if (j>rect.nRight)
			{
				rect.nRight = j;
			}
			tObjectRects[k] = rect;
			//nIndex++;
		}

		nIndex += nWidth;
	}

	nObjectCount = 0;

	for (i = 0; i <= rect_count; i++)
	{
		rect = tObjectRects[i];
		rect.nMarkVal = i;
		//printf("rect = %d,%d,%d,%d,%d\n",rect.nLeft,rect.nRight,rect.nTop,rect.nBottom,rect.nFillSize);
		//排除非矩形区域
		if (SHORT_MAX == rect.nLeft)
		{
			continue;
		}
#if 1
		//排除不满足条件的矩形
		nRectWidth = rect.nRight - rect.nLeft + 1;
		nRectHeight = rect.nBottom - rect.nTop + 1;

		if ((rect.nFillSize<input_param->dynamic_param.trigger_sum)
			|| nRectWidth<MINRECTWIDTH || nRectHeight<MINRECTHEIGHT)
		{
			for (k = rect.nTop; k <= rect.nBottom; k++)
			{
				for (j = rect.nLeft; j <= rect.nRight; j++)
				{
					if (pMarkedBuf[k*nWidth + j] == i)
					{
						pGray[k*nWidth + j] = 0;
						pMarkedBuf[k*nWidth + j] = 0;
					}
				}

			}
			continue;
		}
#endif
		if (nObjectCount >= MAX_TARGET_NUM)
		{
			nObjectCount++;
			break;
		}
		tOutRects[nObjectCount] = rect;
		nObjectCount++;
	}

	return nObjectCount;
}

#if 0
static int min_algorithm(int a, int b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}

}
static int max_algorithm(int a, int b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}

}

static int ClearRect(CeilTrackHand *inst, int nTriggerWidth, CeilITRACK_OutArgs *output_param, int8_t *output_buf)
{

	//OutputDebugStringA("fddfgfd\n");
	int nRectNum = output_param->trigger_num;
	int nOutNum = 0, nRectNo = 0;
	T_Rect rect[2];
	T_Rect *ptInRect = inst->t_global_obj.g_out_rect;
	T_Rect *ptMidRect = ptInRect + MAX_TARGET_NUM / 2;//10个目标内处理时用的临时空间
	yuv_value_info_t		yuv_value = { 0 };
	line_info_t 			line_info = { 0 };
	int i, j;

	int32_t					min_x = SHORT_MAX;
	int32_t					max_x = 0;
	int32_t					avg_width = SHORT_MAX;
	int32_t					width0 = 0;
	int32_t maxindex, minindex, space_value;
	int ifflag = 0;
	int clearflag = 0;
	int  width = inst->input_param.static_param.video_width;

	if (output_param->trigger_num == 0)
	{
		output_param->cmd_type = 0;
		return 0;
	}
	avg_width = 0;
	for (i = 0; i<nRectNum; i++)
	{
		width0 = ptInRect[i].nRight - ptInRect[i].nLeft;
		avg_width += width0;
		if (ptInRect[i].nLeft < min_x)
		{
			min_x = ptInRect[i].nLeft;
		}
		if (ptInRect[i].nRight> max_x)
		{
			max_x = ptInRect[i].nRight;
		}
	}
	avg_width = avg_width / nRectNum;
#if 1
	if ((5 * (max_x - min_x)) <(2* width))
	{
		//宽度小于于1/3，认为是单个目标
		output_param->cmd_type = 1;
		output_param->trigger[0].x = (min_x + max_x)/2;
		output_param->trigger[0].y = (inst->t_global_obj.g_out_rect[0].nTop + inst->t_global_obj.g_out_rect[0].nBottom) / 2;

		output_param->trigger[1].x = inst->t_global_obj.g_out_rect[0].nLeft;
		output_param->trigger[1].y = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger[2].x = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger[2].y = inst->t_global_obj.g_out_rect[0].nBottom;

		output_param->trigger_point[0].x0 = inst->t_global_obj.g_out_rect[0].nLeft;
		output_param->trigger_point[0].y0 = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger_point[0].x1 = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger_point[0].y1 = inst->t_global_obj.g_out_rect[0].nBottom;
		output_param->cmd_type = 1;
		return 0;
}
#endif
	if (output_param->trigger_num == 1)
	{
		output_param->trigger[0].x = (inst->t_global_obj.g_out_rect[0].nLeft + inst->t_global_obj.g_out_rect[0].nRight) / 2;
		output_param->trigger[0].y = (inst->t_global_obj.g_out_rect[0].nTop + inst->t_global_obj.g_out_rect[0].nBottom) / 2;
		#if 1
		output_param->trigger[1].x = inst->t_global_obj.g_out_rect[0].nLeft ;
		output_param->trigger[1].y = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger[2].x = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger[2].y = inst->t_global_obj.g_out_rect[0].nBottom;
		#endif

		output_param->trigger_point[0].x0 = inst->t_global_obj.g_out_rect[0].nLeft;
		output_param->trigger_point[0].y0 = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger_point[0].x1 = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger_point[0].y1 = inst->t_global_obj.g_out_rect[0].nBottom;
		output_param->cmd_type = 1;
	}

	else if ((output_param->trigger_num > 1) && (output_param->trigger_num <= 10))
	{
		memcpy(ptMidRect, ptInRect, sizeof(T_Rect)*output_param->trigger_num);
		do
		{
			clearflag = 0;
			for (i = 0; i<nRectNum; i++)
			{
				rect[0] = ptMidRect[i];
				if (rect[0].nLeft == SHORT_MAX)
				{
					continue;
				}
				width0 = rect[0].nRight - rect[0].nLeft;
				for (j = i + 1; j<nRectNum; j++)
				{
					rect[1] = ptMidRect[j];
					if (rect[1].nLeft == SHORT_MAX)
					{
						continue;
					}
					if (rect[1].nLeft >= rect[0].nLeft)
					{
						maxindex = 1;
						minindex = 0;
					}
					else
					{
						maxindex = 0;
						minindex = 1;
					}
					space_value = abs(rect[maxindex].nLeft - rect[minindex].nRight);

					ifflag = 0;
					if (CLASS_VIEW == inst->input_param.dynamic_param.track_mode)
					{//课中取景
						if ((rect[maxindex].nLeft <= rect[minindex].nRight) || (space_value <= avg_width * 2)
							|| ((5 * space_value) < nTriggerWidth))
						{
							ifflag = 1;
						}
					}
					else
					{//课前取景
						if ((rect[maxindex].nLeft <= rect[minindex].nRight) || (space_value <= avg_width))
						{
							ifflag = 1;
						}
					}

					if (ifflag == 1)
					{
						clearflag = 1;
						ptMidRect[j].nLeft = SHORT_MAX;
						ptMidRect[i].nLeft = min_algorithm(rect[0].nLeft, rect[1].nLeft);
						ptMidRect[i].nRight = max_algorithm(rect[0].nRight, rect[1].nRight);
						ptMidRect[i].nTop = min_algorithm(rect[0].nTop, rect[1].nTop);
						ptMidRect[i].nBottom = max_algorithm(rect[0].nBottom, rect[1].nBottom);
						rect[0] = ptMidRect[i];
						width0 = rect[0].nRight - rect[0].nLeft;
					}
				}
			}
		} while (clearflag == 1);
		nOutNum = 0;
		for (i = 0; i<nRectNum; i++)
		{
			rect[0] = ptMidRect[i];
			if (rect[0].nLeft != SHORT_MAX)
			{
				nRectNo = i;
				nOutNum++;

			}
		}

		if (nOutNum == 1)
		{

			output_param->trigger[0].x = (ptMidRect[nRectNo].nLeft + ptMidRect[nRectNo].nRight) / 2;
			output_param->trigger[0].y = (ptMidRect[nRectNo].nTop + ptMidRect[nRectNo].nBottom) / 2;

			output_param->trigger_point[0].x0 = ptMidRect[nRectNo].nLeft;
			output_param->trigger_point[0].y0 = ptMidRect[nRectNo].nTop;
			output_param->trigger_point[0].x1 = ptMidRect[nRectNo].nRight;
			output_param->trigger_point[0].y1 = ptMidRect[nRectNo].nBottom;

			output_param->cmd_type = 1;
		}
		else if (nOutNum > 1)
		{
			output_param->cmd_type = nOutNum;
			for (int index = 0; index < nOutNum; index++)
			{

				if (index == 0)
				{
					output_param->trigger[0].x = (ptMidRect[nRectNo].nLeft + ptMidRect[nRectNo].nRight) / 2;
					output_param->trigger[0].y = (ptMidRect[nRectNo].nTop + ptMidRect[nRectNo].nBottom) / 2;
				}
				output_param->trigger_point[index].x0 = ptMidRect[index].nLeft;
				output_param->trigger_point[index].y0 = ptMidRect[index].nTop;
				output_param->trigger_point[index].x1 = ptMidRect[index].nRight;
				output_param->trigger_point[index].y1 = ptMidRect[index].nBottom;
			}
			output_param->cmd_type = nOutNum;
		}

		else
		{
			output_param->cmd_type = 2;
		}

	}
	else if (output_param->trigger_num > 10)
	{
		output_param->cmd_type = 10;
	}

	return 0;
}
#endif

int enter_blackboard_eara(CeilTrackHand *inst, int x)
{
	CeilITRACK_Params 	*input_param = &(inst->input_param);
	int32_t nSrcBufWidth = inst->t_global_obj.nSrcBufWidth;

	int i = 0, j = 0;
	int top = input_param->dynamic_param.trigger[0].trigger_y0;
	int bottom = input_param->dynamic_param.trigger[0].trigger_y1;
	int left = input_param->dynamic_param.trigger[0].trigger_x0;
	int right = input_param->dynamic_param.trigger[0].trigger_x1;
	int a = 0, b = 0, c = 0, num = 0;
	yuv_value_info_t		yuv_value = { 0 };


	//printf("left = %d,right = %d\n",left,right);
	//printf("output_param->trigger[0].x = %d\n", output_param->trigger[0].x);
#if 0
	//if ((x >= (left + (right - left)/3)) && (x <= (right - (right - left)/4)))
	if ((x >= (left + (right - left) / 2)) && (x <= right))
	{
		//g_display_ppt_flag = 1;
		return 1;
	}
	else
	{
		return 0;
	}
#endif

	if (g_display_ppt_flag)
	{
		if ((x <= (left - (right - left) *3/ 4)) || (x >= (right + (right - left) *3/ 4)))
		{
			g_display_ppt_flag = 0;
		}
	}
	else
	{
		if ((x >= (left + (right - left) / 4)) && (x <= (right - (right - left) / 4)))
		{

			g_display_ppt_flag = 1;
		}
	}
	return g_display_ppt_flag;
}

extern ITRACK_STUDENTS_RIGHT_SIDE_Params 	g_track_param;
//初始化
int32_t motion_detect_init(CeilTrackHand *inst,int width,int height,int sens)
{
	int i = 0;
	inst->input_param.static_param.video_width = width;
	inst->input_param.static_param.video_height = height;
	inst->input_param.dynamic_param.track_mode = CLASS_VIEW;
	inst->input_param.dynamic_param.sens = sens;
	inst->input_param.dynamic_param.control_mode = 0;
	inst->input_param.dynamic_param.trigger_sum = g_track_param.dynamic_param.trigger_sum;//70;
/*
	inst->input_param.dynamic_param.track_point_num = 4;


	inst->input_param.dynamic_param.track_point[0].x = 0;
	inst->input_param.dynamic_param.track_point[0].y = 0;
	inst->input_param.dynamic_param.track_point[1].x = 0;
	inst->input_param.dynamic_param.track_point[1].y = height / 2 - 10;// height - 1;
	inst->input_param.dynamic_param.track_point[2].x = width - 1;
	inst->input_param.dynamic_param.track_point[2].y = height / 2 - 10;// height - 1;
	inst->input_param.dynamic_param.track_point[3].x = width - 1;
	inst->input_param.dynamic_param.track_point[3].y = 0;
*/
	inst->input_param.dynamic_param.track_point_num = g_track_param.dynamic_param.track_point_num;
	if(inst->input_param.dynamic_param.track_point_num > TRACK_STUDENTS_CEILTRACK_AREA_POINT_MAX)
	{
		inst->input_param.dynamic_param.track_point_num = TRACK_STUDENTS_CEILTRACK_AREA_POINT_MAX;
	}
	for(i = 0; i< inst->input_param.dynamic_param.track_point_num;i++)
	{
		inst->input_param.dynamic_param.track_point[i].x = g_track_param.dynamic_param.track_point[i].x;
		inst->input_param.dynamic_param.track_point[i].y = g_track_param.dynamic_param.track_point[i].y;
	}

	for(i =0;i<TRACK_TRIGGER_MAX_NUM;i++)
	{
		inst->input_param.dynamic_param.trigger[i].trigger_x0 =  g_track_param.dynamic_param.trigger[i].trigger_x0;
		inst->input_param.dynamic_param.trigger[i].trigger_y0 =  g_track_param.dynamic_param.trigger[i].trigger_y0;
		inst->input_param.dynamic_param.trigger[i].trigger_x1 =  g_track_param.dynamic_param.trigger[i].trigger_x1;
		inst->input_param.dynamic_param.trigger[i].trigger_y1 =  g_track_param.dynamic_param.trigger[i].trigger_y1;
	}

	T_CEIL_TRACK_GLOBAL_OBJ *pObj;

	pObj = &inst->t_global_obj;
	pObj->src_rgb_buf = (uint8_t *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height * 3/2);
	pObj->dst_rgb_buf = (uint8_t *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height * 3/2);
	pObj->tmp_src_rgb_buf = (uint8_t *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height * 3/2);

	pObj->gray_buf = (uint8_t *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height);
	pObj->Area_vertex = (uint8_t *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height);

	pObj->mid_buf = (unsigned char *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height * 3);
	pObj->marked_buf = (uint8_t *)malloc(inst->input_param.static_param.video_width * inst->input_param.static_param.video_height * 2);

	pObj->nSrcBufWidth = width;
	pObj->nSrcBufHeight = height;

	pObj->first_time = 10;
	pObj->frame_num = 0;
	pObj->last_move_flag = 0;
	pObj->last_track_status = 0;

	memset(pObj->gray_buf ,0, inst->input_param.static_param.video_width * inst->input_param.static_param.video_height);
	memset(pObj->marked_buf,0,inst->input_param.static_param.video_width * inst->input_param.static_param.video_height * 2);
	

	area_check(inst);
	set_check_max_rect(inst);

	return 0;
}


static int min_algorithm(int a, int b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}

}
static int max_algorithm(int a, int b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}

}
static int ClearRect(CeilTrackHand *inst, int nTriggerWidth, CeilITRACK_OutArgs *output_param, int8_t *output_buf)
{

	//OutputDebugStringA("fddfgfd\n");
	int nRectNum = output_param->trigger_num;
	int nOutNum = 0, nRectNo = 0;
	T_Rect rect[2];
	T_Rect *ptInRect = inst->t_global_obj.g_out_rect;
	T_Rect *ptMidRect = ptInRect + MAX_TARGET_NUM / 2;//10个目标内处理时用的临时空间
	yuv_value_info_t		yuv_value = { 0 };
	line_info_t 			line_info = { 0 };
	int i, j;

	int32_t					min_x = SHORT_MAX;
	int32_t					max_x = 0;
	int32_t					avg_width = SHORT_MAX;
	int32_t					width0 = 0;
	int32_t maxindex, minindex, space_value;
	int ifflag = 0;
	int clearflag = 0;
	int  width = inst->input_param.static_param.video_width;
	int index = 0;

	if (output_param->trigger_num == 0)
	{
		output_param->cmd_type = 0;
		return 0;
	}
	avg_width = 0;
	for (i = 0; i<nRectNum; i++)
	{
		width0 = ptInRect[i].nRight - ptInRect[i].nLeft;
		avg_width += width0;
		if (ptInRect[i].nLeft < min_x)
		{
			min_x = ptInRect[i].nLeft;
		}
		if (ptInRect[i].nRight> max_x)
		{
			max_x = ptInRect[i].nRight;
		}
	}
	avg_width = avg_width / nRectNum;
#if 1
	if ((5 * (max_x - min_x)) <(2* width))
	{
		//宽度小于于1/3，认为是单个目标
		output_param->cmd_type = 1;
		output_param->trigger[0].x = (min_x + max_x)/2;
		output_param->trigger[0].y = (inst->t_global_obj.g_out_rect[0].nTop + inst->t_global_obj.g_out_rect[0].nBottom) / 2;

		output_param->trigger[1].x = inst->t_global_obj.g_out_rect[0].nLeft;
		output_param->trigger[1].y = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger[2].x = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger[2].y = inst->t_global_obj.g_out_rect[0].nBottom;

		output_param->trigger_point[0].x0 = min_x;
		output_param->trigger_point[0].y0 = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger_point[0].x1 = max_x;
		output_param->trigger_point[0].y1 = inst->t_global_obj.g_out_rect[0].nBottom;
		output_param->cmd_type = 1;
		return 0;
}
#endif
	if (output_param->trigger_num == 1)
	{
		output_param->trigger[0].x = (inst->t_global_obj.g_out_rect[0].nLeft + inst->t_global_obj.g_out_rect[0].nRight) / 2;
		output_param->trigger[0].y = (inst->t_global_obj.g_out_rect[0].nTop + inst->t_global_obj.g_out_rect[0].nBottom) / 2;
		#if 1
		output_param->trigger[1].x = inst->t_global_obj.g_out_rect[0].nLeft ;
		output_param->trigger[1].y = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger[2].x = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger[2].y = inst->t_global_obj.g_out_rect[0].nBottom;
		#endif

		output_param->trigger_point[0].x0 = inst->t_global_obj.g_out_rect[0].nLeft;
		output_param->trigger_point[0].y0 = inst->t_global_obj.g_out_rect[0].nTop;
		output_param->trigger_point[0].x1 = inst->t_global_obj.g_out_rect[0].nRight;
		output_param->trigger_point[0].y1 = inst->t_global_obj.g_out_rect[0].nBottom;
		output_param->cmd_type = 1;
	}

	else if ((output_param->trigger_num > 1) && (output_param->trigger_num <= 10))
	{
		memcpy(ptMidRect, ptInRect, sizeof(T_Rect)*output_param->trigger_num);
		do
		{
			clearflag = 0;
			for (i = 0; i<nRectNum; i++)
			{
				rect[0] = ptMidRect[i];
				if (rect[0].nLeft == SHORT_MAX)
				{
					continue;
				}
				width0 = rect[0].nRight - rect[0].nLeft;
				for (j = i + 1; j<nRectNum; j++)
				{
					rect[1] = ptMidRect[j];
					if (rect[1].nLeft == SHORT_MAX)
					{
						continue;
					}
					if (rect[1].nLeft >= rect[0].nLeft)
					{
						maxindex = 1;
						minindex = 0;
					}
					else
					{
						maxindex = 0;
						minindex = 1;
					}
					space_value = abs(rect[maxindex].nLeft - rect[minindex].nRight);

					ifflag = 0;
					if (CLASS_VIEW == inst->input_param.dynamic_param.track_mode)
					{//课中取景
						if ((rect[maxindex].nLeft <= rect[minindex].nRight) || (space_value <= avg_width * 2)
							|| ((5 * space_value) < nTriggerWidth))
						{
							ifflag = 1;
						}
					}
					else
					{//课前取景
						if ((rect[maxindex].nLeft <= rect[minindex].nRight) || (space_value <= avg_width))
						{
							ifflag = 1;
						}
					}

					if (ifflag == 1)
					{
						clearflag = 1;
						ptMidRect[j].nLeft = SHORT_MAX;
						ptMidRect[i].nLeft = min_algorithm(rect[0].nLeft, rect[1].nLeft);
						ptMidRect[i].nRight = max_algorithm(rect[0].nRight, rect[1].nRight);
						ptMidRect[i].nTop = min_algorithm(rect[0].nTop, rect[1].nTop);
						ptMidRect[i].nBottom = max_algorithm(rect[0].nBottom, rect[1].nBottom);
						rect[0] = ptMidRect[i];
						width0 = rect[0].nRight - rect[0].nLeft;
					}
				}
			}
		} while (clearflag == 1);
		nOutNum = 0;
		for (i = 0; i<nRectNum; i++)
		{
			rect[0] = ptMidRect[i];
			if (rect[0].nLeft != SHORT_MAX)
			{
				nRectNo = i;
				nOutNum++;

			}
		}

		if (nOutNum == 1)
		{

			output_param->trigger[0].x = (ptMidRect[nRectNo].nLeft + ptMidRect[nRectNo].nRight) / 2;
			output_param->trigger[0].y = (ptMidRect[nRectNo].nTop + ptMidRect[nRectNo].nBottom) / 2;

			output_param->trigger_point[0].x0 = ptMidRect[nRectNo].nLeft;
			output_param->trigger_point[0].y0 = ptMidRect[nRectNo].nTop;
			output_param->trigger_point[0].x1 = ptMidRect[nRectNo].nRight;
			output_param->trigger_point[0].y1 = ptMidRect[nRectNo].nBottom;

			output_param->cmd_type = 1;
		}
		else if (nOutNum > 1)
		{
			output_param->cmd_type = nOutNum;
			for (index = 0; index < nOutNum; index++)
			{

				if (index == 0)
				{
					output_param->trigger[0].x = (ptMidRect[nRectNo].nLeft + ptMidRect[nRectNo].nRight) / 2;
					output_param->trigger[0].y = (ptMidRect[nRectNo].nTop + ptMidRect[nRectNo].nBottom) / 2;
				}
				output_param->trigger_point[index].x0 = ptMidRect[index].nLeft;
				output_param->trigger_point[index].y0 = ptMidRect[index].nTop;
				output_param->trigger_point[index].x1 = ptMidRect[index].nRight;
				output_param->trigger_point[index].y1 = ptMidRect[index].nBottom;
			}
			output_param->cmd_type = nOutNum;
		}

		else
		{
			output_param->cmd_type = 2;
		}

	}
	else if (output_param->trigger_num > 10)
	{
		output_param->cmd_type = 10;
	}

	return 0;
}


static int InterframeDetection(CeilTrackHand *inst,uint8_t *input_buf)
{
	CeilITRACK_Params 	*input_param 	= &(inst->input_param);
	int32_t nSrcBufWidth  = inst->t_global_obj.nSrcBufWidth;
	uint8_t *preframe = inst->t_global_obj.dst_rgb_buf;
	//uint8_t *preframe = inst->t_global_obj.mid_buf;
	int i,j;
	int top = input_param->dynamic_param.trigger[0].trigger_y0;
	int bottom = input_param->dynamic_param.trigger[0].trigger_y1;
	int left = input_param->dynamic_param.trigger[0].trigger_x0;
	int right = input_param->dynamic_param.trigger[0].trigger_x1;
	int a,b,c,num = 0;
//	yuv_value_info_t yuv_value;

	for(i= top; i < bottom;i++)
	{
		for(j = left; j < right; j++)
		{

			a = *((uint8_t *)preframe +  1280 * i + j);
			b = *((uint8_t *)input_buf +  nSrcBufWidth * i + j);
			c = abs(a-b);

			*((uint8_t *)preframe +  1280 * i + j) = *((uint8_t *)input_buf +  nSrcBufWidth * i + j);

			//if(c > 20)
			if(c > input_param->dynamic_param.sens)
			{
				num++;
				*((uint8_t *)input_buf +  nSrcBufWidth * i + j) = 0;
			}
			
			//*((uint8_t *)input_buf +  nSrcBufWidth * i + j) = 0;
		}
	}

	if(num > input_param->dynamic_param.trigger_sum)
	{
		return 1;
	}
	else
	{
		top = input_param->dynamic_param.trigger[1].trigger_y0;
		bottom = input_param->dynamic_param.trigger[1].trigger_y1;
		left = input_param->dynamic_param.trigger[1].trigger_x0;
		right = input_param->dynamic_param.trigger[1].trigger_x1;
		num = 0;
		for(i= top; i < bottom;i++)
		{
			for(j = left; j < right; j++)
			{

				a = *((uint8_t *)preframe +  1280 * i + j);
				b = *((uint8_t *)input_buf +  nSrcBufWidth * i + j);
				c = abs(a-b);

				*((uint8_t *)preframe +  1280 * i + j) = *((uint8_t *)input_buf +  nSrcBufWidth * i + j);

				//if(c > 20)
				if(c > input_param->dynamic_param.sens)
				{
					num++;
					*((uint8_t *)input_buf +  nSrcBufWidth * i + j) = 0;
				}
				
				//*((uint8_t *)input_buf +  nSrcBufWidth * i + j) = 0;
			}
		}

		if(num > input_param->dynamic_param.trigger_sum)
		{
			return 1;
		}
	}

	return 0;
}

int32_t	motion_detect_process(CeilTrackHand *inst, int8_t *output_buf, CeilITRACK_OutArgs *output_param)
{


	int32_t					index = 0;

	uint8_t                *tmpbuffer = NULL;

	rect_t rBroad, rObject;

	if ((NULL == inst) || (NULL == output_buf) || (NULL == output_param))
	{
		return -1;
	}

	memset(output_param,0,sizeof(CeilITRACK_OutArgs));
	memset(inst->t_global_obj.g_out_rect, 0, sizeof(T_Rect)*MAX_TARGET_NUM);


	

	inst->t_global_obj.dst_rgb_buf = inst->t_global_obj.src_rgb_buf;


	move_check_data_copy(output_buf, inst);
	

	check_track_point(output_buf, inst);

	output_param->IsTrigger = InterframeDetection(inst,output_buf);

	gray_close(inst);

	output_param->trigger_num = find_block_kernel(inst);

	//printf("output_param->trigger_num:%d\n", output_param->trigger_num);

	for (index = 0; index < output_param->trigger_num; index++)
	{

		if (index < TRIGGER_NUM_MAX)
		{
			output_param->trigger_point[index].x0 = inst->t_global_obj.g_out_rect[index].nLeft;
			output_param->trigger_point[index].y0 = inst->t_global_obj.g_out_rect[index].nTop;
			output_param->trigger_point[index].x1 = inst->t_global_obj.g_out_rect[index].nRight;
			output_param->trigger_point[index].y1 = inst->t_global_obj.g_out_rect[index].nBottom;

		}
	}
	output_param->cmd_type = output_param->trigger_num;
	if (output_param->cmd_type > 10)
	{
		output_param->cmd_type = 10;
	}
	
	ClearRect(inst, inst->t_global_obj.tInRect.nRight - inst->t_global_obj.tInRect.nLeft, output_param, output_buf);

	tmpbuffer = inst->t_global_obj.src_rgb_buf;
	inst->t_global_obj.src_rgb_buf = inst->t_global_obj.tmp_src_rgb_buf;
	inst->t_global_obj.tmp_src_rgb_buf = tmpbuffer;

	return 0;
}



int32_t motion_detect_release(CeilTrackHand *inst)
{
	T_CEIL_TRACK_GLOBAL_OBJ *pObj;

	pObj = &inst->t_global_obj;
	
	if (pObj->gray_buf)
	{
		free(pObj->gray_buf);
	}
	if (pObj->InBuffer)
	{
		free(pObj->InBuffer);
	}
	if (pObj->marked_buf)
	{
		free(pObj->marked_buf);
	}
	if (pObj->mid_buf)
	{
		free(pObj->mid_buf);
	}
	if (pObj->src_rgb_buf)
	{
		free(pObj->src_rgb_buf);
	}
	if (pObj->tmp_src_rgb_buf)
	{
		free(pObj->tmp_src_rgb_buf);
	}
	if (pObj->Area_vertex)
	{
		free(pObj->Area_vertex);
	}
	return 0;
}



