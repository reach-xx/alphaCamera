#ifndef COMMON__H
#define COMMON__H
#include "demo.h"

#define SUPPORT_TRACK		//是否支持跟踪

#define ENABLE_DEUBG
/*关闭打开调试信息宏*/
#ifdef SDCARD
#define ENABLE_DEUBG
#endif
#ifdef ENABLE_DEUBG
#include <stdio.h>
typedef enum {
    DL_NONE, //=0
    DL_ERROR,  //=1
    DL_WARNING,  //=2
    DL_FLOW,	//=3
    DL_DEBUG,	//=4
    DL_ALL, 	//all
} EDebugLevle;

#define DEBUG_LEVEL   	(DL_ALL)	// // TODO: modify DEBUG Level
extern int 	g_nDebugLevel;

#if 1		//日志文件
#define DEBUG(x,fmt,arg...) \
	do {\
		if((x)<=g_nDebugLevel) \
		{ \
			pthread_mutex_lock(&sendlog_m);\		
			snprintf(logbuf,1023, fmt, ##arg);\
			sendto(g_Logserver, logbuf, 1024, 0, (struct sockaddr*)&g_serveraddr, sizeof(struct sockaddr_in));\
			pthread_mutex_unlock(&sendlog_m);\	
		}\
}while (0) 
#else
#include <syslog.h>
#define DEBUG(x,fmt,arg...) \
	do {\
		if ((x)<DEBUG_LEVEL){ \
			syslog(LOG_INFO, "%s",__func__);\
			syslog(LOG_INFO,fmt, ##arg);\
		}\
	}while (0)
#endif
#else
#define DEBUG(x,fmt,arg...) do {}while(0)
#endif
#define GPIO_0			0
#define GPIO_1			1
#define GPIO_2			2
#define GPIO_3			3
#define GPIO_4			4
#define GPIO_5			5
#define GPIO_6			6
#define GPIO_7			7
#define GPIO_8			8
#define GPIO_9			9

#define GPIO_25			25
#define GPIO_26			26


#define GPIO_INPUT		0
#define GPIO_OUTPUT		1
#define GPIO_LEVEL		0
#define GPIO_HIGH		1

#define KERNELFILENAME		"uImage"
#define FPGAFILENAME		"fpga.bin"
#define AIC32_REG_RIGHT_ADDR 26
#define AIC32_REG_LEFT_ADDR 29
/*offset*/
#define MAX_STDNUM_OFFSET        (8)
/* AD9880 idx*/
#define AD9880_INDEX			 (0)
/* ADV7401 idx*/
#define ADV7401_INDEX			 (AD9880_INDEX+MAX_STDNUM_OFFSET)
/* SDI idx*/
#define SDI_INDEX				 (ADV7401_INDEX+MAX_STDNUM_OFFSET)
/* AUTO index*/
#define AUTO_INDEX				(-1)

/*h264_pixel*/
/*h264_pixel*/
#define RUN_LED_GPIO				3//状态灯
#define STATUS_LED_GPIO				2//运行灯

#define LED_FREQ_TIMES				2

#define WIDTH 640
#define HEIGHT 360


typedef struct _DATE_TIME_INFO {
	int year;
	int month;
	int mday;
	int hours;
	int min;
	int sec;
} DATE_TIME_INFO;

#define VIDEO_IDR_FRAME		3
#define VIDEO_I_FRAME				1
#define VIDEO_NO_I_FRAME			0

typedef struct gblCommonData {
	//Capture_Handle  hcapture;
	char Kverison[20];
	int  frames;		//frames data
	int  Iframe;    	//I frame flag
	int  mode;			//video mode
	int  encodeIdx;		// encode index
	int	 h264_pixel; 	//encode lib sizes
	int	 hdmi;			//HDMI interface
	int	 samplerate;	//Sample Rate
	int  samplebit;		//samplebit  //16
	int  channels;		//channel number
	int  width;			//实际视频的尺寸
	int  height;		//
	pthread_mutex_t     hcapture_mutex;
	pthread_mutex_t     frames_mutex;
	pthread_mutex_t     Iframe_mutex;
	pthread_mutex_t     mode_mutex;
	pthread_mutex_t     encodeIdx_mutex;
	pthread_mutex_t     h264_pixel_mutex;
	pthread_mutex_t     hdmi_mutex;
	pthread_mutex_t     samplerate_mutex;
	pthread_mutex_t     samplebit_mutex;
	pthread_mutex_t     channels_mutex;
	pthread_mutex_t     width_mutex;
	pthread_mutex_t     height_mutex;

} gblCommonData;
extern int recv_long_tcp_data(int socket,void* buffer,const int len);
extern int send_long_tcp_data(int socket,void* buffer,const int len);

extern void getSysTime(DATE_TIME_INFO *dtinfo);
extern void setRtcTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
void signal_set(void);
//extern Capture_Handle	gblgetcapture(void);
extern int	gblgetframes(void);
extern int	gblgetIframe(void);
extern int	gblgetmode(void);
extern int	gblgetencodeIdx(void);
extern int	gblgeth264_pixel(void);
extern int	gblgethdmi(void);
extern int	gblgetsamplerate(void);
extern int	gblgetsamplebit(void);
extern int	gblgetchannels(void);


//extern void	gblsetcapture(Capture_Handle	 hcapture);
extern void	gblsetframes(int	 frames);
extern void	gblsetIframe(int	 Iframe);
extern void	gblsetmode(int	 mode);
extern void	gblsetencodeIdx(int	 encodeIdx, int writefile);
extern void	gblseth264_pixel(int	 h264_pixel);
extern void	gblsethdmi(int	 hdmi);
extern void	gblsetsamplerate(int	 samplerate);
extern void	gblsetsamplebit(int	 samplebit);
extern void	gblsetchannels(int	 channels);

extern void destorygblcommonmutex(void);
extern void initgblcommonmutex(void);
extern void gblcapturelock(void);
extern void gblcaptureunlock(void);


extern int writeWatchDog(void);
extern int initWatchDog(void);
extern int closeWatchDog(void);
extern void gblloadencodeidx(void);
/*get time */
extern unsigned long long getCurrentTime(void);

#endif


