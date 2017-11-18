#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include <linux/watchdog.h>
#include <string.h>
#include <errno.h>

//#include "netcom.h"

#define WATCH_DOG_DEV		"/dev/watchdog"

#define INPUTTYPEFILE  "inputtype.cfg"
#define WATCH_DOG_TIMOUT		1
static  int			watchdogFD  = 0;

#define RTC_DEV_NAME "/dev/rtc"
/*gbl  common */

//#define GBL_COMMON_DATA  {NULL,"1.0.1",0,0,-1,AUTO_INDEX,0,0,0,0,2,720,480,}
#define GBL_COMMON_DATA  {"1.0.1",0,0,-1,AUTO_INDEX,0,0,0,0,2,720,480,}

gblCommonData gblData = GBL_COMMON_DATA;
//extern int capture_height;
//extern int capture_width;
int capture_height;
int capture_width;

int 	g_nDebugLevel=4;
int recv_long_tcp_data(int socket,void* buffer,const int len)
{
	int total_recv = 0;
	int recv_len = 0;
	while(total_recv < len)
	{
		recv_len = recv(socket, buffer + total_recv, len - total_recv, 0);
		if(recv_len < 1)
		{
			DEBUG(DL_ERROR, "recv long tcp data failed!\n");
			return -1;
		}
		total_recv += recv_len;
		if(total_recv == len)
		{
			break;
		}
	}
	return total_recv;
}

int send_long_tcp_data(int socket,void* buffer,const int len)
{
	int total_send = 0;
	int send_len = 0;
	while(total_send < len)
	{
		send_len = send(socket, buffer + total_send, len - total_send, 0);
		if(send_len < 1)
		{
			DEBUG(DL_ERROR, "send long tcp data failed!\n");
			return -1;
		}
		total_send += send_len;
		if(len == total_send)
		{
			break;
		}
	}
	return total_send;
}

void initgblcommonmutex(void)
{
	pthread_mutex_init(&gblData.hcapture_mutex, NULL);
	pthread_mutex_init(&gblData.frames_mutex, NULL);
	pthread_mutex_init(&gblData.Iframe_mutex, NULL);
	pthread_mutex_init(&gblData.mode_mutex, NULL);
	pthread_mutex_init(&gblData.encodeIdx_mutex, NULL);
	pthread_mutex_init(&gblData.h264_pixel_mutex, NULL);
	pthread_mutex_init(&gblData.hdmi_mutex, NULL);
	pthread_mutex_init(&gblData.samplerate_mutex, NULL);
	pthread_mutex_init(&gblData.samplebit_mutex, NULL);
	pthread_mutex_init(&gblData.channels_mutex, NULL);
}
void destorygblcommonmutex(void)
{
	pthread_mutex_destroy(&gblData.hcapture_mutex);
	pthread_mutex_destroy(&gblData.frames_mutex);
	pthread_mutex_destroy(&gblData.Iframe_mutex);
	pthread_mutex_destroy(&gblData.mode_mutex);
	pthread_mutex_destroy(&gblData.encodeIdx_mutex);
	pthread_mutex_destroy(&gblData.h264_pixel_mutex);
	pthread_mutex_destroy(&gblData.hdmi_mutex);
	pthread_mutex_destroy(&gblData.samplerate_mutex);
	pthread_mutex_destroy(&gblData.samplebit_mutex);
	pthread_mutex_destroy(&gblData.channels_mutex);

}
/*
Capture_Handle	gblgetcapture(void)
{
	Capture_Handle	 hcapture;
	hcapture = gblData.hcapture;
	return hcapture;
}*/
int	gblgetheight(void)
{
	int	 height;
	pthread_mutex_lock(&gblData.mode_mutex);
	height = gblData.height;
	pthread_mutex_unlock(&gblData.mode_mutex);
	return height;
}
int	gblgetwidth(void)
{
	int	 width;
	pthread_mutex_lock(&gblData.mode_mutex);
	width = gblData.width;
	pthread_mutex_unlock(&gblData.mode_mutex);
	return width;
}

/* */
int	gblgetframes(void)
{
	int	 frames;
	pthread_mutex_lock(&gblData.frames_mutex);
	frames = gblData.frames;
	pthread_mutex_unlock(&gblData.frames_mutex);
	return frames;
}

int	gblgetIframe(void)
{
	int	 Iframe;
	pthread_mutex_lock(&gblData.Iframe_mutex);
	Iframe = gblData.Iframe;
	pthread_mutex_unlock(&gblData.Iframe_mutex);
	return Iframe;
}

int	gblgetmode(void)
{
	int	 mode;
	pthread_mutex_lock(&gblData.mode_mutex);
	mode = gblData.mode;
	pthread_mutex_unlock(&gblData.mode_mutex);
	return mode;
}
void gblsetmode(int	 mode)
{
	pthread_mutex_lock(&gblData.mode_mutex);
	gblData.mode = mode;

	if(29 == mode) {
		capture_height = 480;
		gblData.height = 448;
		capture_width = 720;
		gblData.width = 688;
		//每次侦测到模式后赋值到全局视频宽高参数

	} else if(30 == mode) {
		capture_height = 576;
		gblData.height = 528;
		capture_width = 704;
		gblData.width = 672;
	}

	pthread_mutex_unlock(&gblData.mode_mutex);
}
void gblloadencodeidx(void)
{

	pthread_mutex_lock(&gblData.encodeIdx_mutex);
	gblData.encodeIdx = AD9880_INDEX;
	pthread_mutex_unlock(&gblData.encodeIdx_mutex);
	return ;
}

int	gblgetencodeIdx(void)
{
	int	 encodeIdx;
	pthread_mutex_lock(&gblData.encodeIdx_mutex);
	encodeIdx = gblData.encodeIdx;
	pthread_mutex_unlock(&gblData.encodeIdx_mutex);
	return encodeIdx;
}
/* */
int	gblgeth264_pixel(void)
{
	int	 h264_pixel;
	pthread_mutex_lock(&gblData.h264_pixel_mutex);
	h264_pixel = gblData.h264_pixel;
	pthread_mutex_unlock(&gblData.h264_pixel_mutex);
	return h264_pixel;
}

int	gblgethdmi(void)
{
	int	 hdmi;
	pthread_mutex_lock(&gblData.hdmi_mutex);
	hdmi = gblData.hdmi;
	pthread_mutex_unlock(&gblData.hdmi_mutex);
	return hdmi;
}

int	gblgetsamplerate(void)
{
	int	 samplerate;
	pthread_mutex_lock(&gblData.samplerate_mutex);
	samplerate = gblData.samplerate;
	pthread_mutex_unlock(&gblData.samplerate_mutex);
	return samplerate;
}


int	gblgetsamplebit(void)
{
	int	 samplebit;
	pthread_mutex_lock(&gblData.samplebit_mutex);
	samplebit = gblData.samplebit;
	pthread_mutex_unlock(&gblData.samplebit_mutex);
	return samplebit;
}

int	gblgetchannels(void)
{
	int	 channels;
	pthread_mutex_lock(&gblData.channels_mutex);
	channels = gblData.channels;
	pthread_mutex_unlock(&gblData.channels_mutex);
	return channels;
}

void gblcapturelock(void)
{
	pthread_mutex_lock(&gblData.hcapture_mutex);
}

void gblcaptureunlock(void)
{

	pthread_mutex_unlock(&gblData.hcapture_mutex);
}

/* */
/*void gblsetcapture(Capture_Handle	 hcapture)
{

	gblData.hcapture = hcapture;

}*/
/* */
void gblsetframes(int	 frames)
{
	pthread_mutex_lock(&gblData.frames_mutex);
	gblData.frames = frames;
	pthread_mutex_unlock(&gblData.frames_mutex);
}

void gblsetIframe(int	 Iframe)
{
	pthread_mutex_lock(&gblData.Iframe_mutex);
	gblData.Iframe = Iframe;
	pthread_mutex_unlock(&gblData.Iframe_mutex);
}

void gblsetencodeIdx(int encodeIdx, int writefile)
{
	int ret = 0 ;
	char buf[10];

	pthread_mutex_lock(&gblData.encodeIdx_mutex);

	encodeIdx = AD9880_INDEX;
	gblData.encodeIdx = AD9880_INDEX;
	pthread_mutex_unlock(&gblData.encodeIdx_mutex);
}

/* */
void	gblseth264_pixel(int	 h264_pixel)
{
	pthread_mutex_lock(&gblData.h264_pixel_mutex);
	gblData.h264_pixel = h264_pixel;
	pthread_mutex_unlock(&gblData.h264_pixel_mutex);
}

void	gblsethdmi(int	 hdmi)
{
	pthread_mutex_lock(&gblData.hdmi_mutex);
	gblData.hdmi = hdmi;
	pthread_mutex_unlock(&gblData.hdmi_mutex);
}

void	gblsetsamplerate(int	 samplerate)
{
	pthread_mutex_lock(&gblData.samplerate_mutex);
	gblData.samplerate = samplerate;
	pthread_mutex_unlock(&gblData.samplerate_mutex);
}


void	gblsetsamplebit(int	 samplebit)
{
	pthread_mutex_lock(&gblData.samplebit_mutex);
	gblData.samplebit = samplebit;
	pthread_mutex_unlock(&gblData.samplebit_mutex);
}

void	gblsetchannels(int	 channels)
{
	pthread_mutex_lock(&gblData.channels_mutex);
	gblData.channels = channels;
	pthread_mutex_unlock(&gblData.channels_mutex);
}

/*设置RTC的时间*/
void setRtcTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond)
{
	if(nSecond < 1) {
		nSecond  = 10;
	}

	int fd = 0;
	struct rtc_time rtc_tm;

	if(fd == 0)		{
		//DEBUG(DL_ERROR,"YYH:Enter into setRtcTime open \n");
		fd = open(RTC_DEV_NAME, O_RDWR);
		//DEBUG(DL_ERROR,"YYH:Leave out setRtcTime open \n");
	}

	if(fd == -1) 	{
		DEBUG(DL_ERROR, "Cannot open RTC device due to following reason.\n");
		DEBUG(DL_ERROR,"/dev/rtc");
		fd = 0;
		return;
	}

	rtc_tm.tm_mday = nDay;
	rtc_tm.tm_mon = nMonth - 1;
	rtc_tm.tm_year = nYear - 1900;
	rtc_tm.tm_hour = nHour;
	rtc_tm.tm_min = nMinute;
	rtc_tm.tm_sec = nSecond;
	//更新操作系统时间
	//普通服务器上可以使用   date   -s   '2005-12-12   22:22:22'
	//arm用date   -s   121222222005.22   MMDDhhmmYYYY.ss
	char setStr[256] = {0};
	sprintf(setStr, "date -s \"%02d/%02d/%04d %02d:%02d:%02d\"", nMonth, nDay, nYear, nHour, nMinute, nSecond);
	system(setStr);
	//取得星期几
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	rtc_tm.tm_wday = p->tm_wday;
	int retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);

	if(retval == -1) {
		fprintf(stderr, "Cannot do ioctl() due to following reason.\n");
		close(fd);
		return;
	}

	close(fd);

}

/*得到系统时间*/
void getSysTime(DATE_TIME_INFO *dtinfo)
{
	long ts;
	struct tm *ptm = NULL;

	ts = time(NULL);
	ptm = localtime((const time_t *)&ts);
	dtinfo->year = ptm->tm_year + 1900;
	dtinfo->month = ptm->tm_mon + 1;
	dtinfo->mday = ptm->tm_mday;
	dtinfo->hours = ptm->tm_hour;
	dtinfo->min = ptm->tm_min;
	dtinfo->sec = ptm->tm_sec;

}



/*向看门狗发消息*/
int writeWatchDog(void)
{
#if 1
	int dummy;

	if(ioctl(watchdogFD, WDIOC_KEEPALIVE, &dummy) != 0) {
		DEBUG(DL_ERROR, "writeWatchDog failed strerror(errno) = %s\n", strerror(errno));
		return -1;
	}

#endif
	return 0;
}

/*初始化看门狗*/
int initWatchDog(void)
{
#if 1
	watchdogFD = open(WATCH_DOG_DEV, O_WRONLY);

	if(watchdogFD == -1) {
		DEBUG(DL_ERROR, "open watch dog failed strerror(errno) = %s\n", strerror(errno));
		return -1;
	}

#endif
	return 0;
}

/*关闭看门狗*/
/*初始化看门狗*/
int closeWatchDog(void)
{
#if 1

	if(watchdogFD) {
		close(watchdogFD);
		watchdogFD = 0;
	}

#endif
	return 0;
}

/*get time */
unsigned long long getCurrentTime(void)
{
	struct timeval tv;
	struct timezone tz;
	unsigned long long ultime;

	gettimeofday(&tv , &tz);
	ultime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (ultime);
}





