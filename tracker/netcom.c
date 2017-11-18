/*
## FileName:   netcom.c
##
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/videodev2.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <net/if.h>
#include <ctype.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "netcom.h"
#include "common.h"

#ifdef HAVE_RTSP_MODULE

#endif

#define	MAX_DUP_CHK	(8 * 128)
#define	DEFDATALEN	(64 - 8)	/* default data length */
#define	MAXPACKET	(200)/* max packet size */



UINT32 g_uiStreamOffset = 0;
int frame_number = 0;





const unsigned int FPGA_HV_TALBE[MAX_MODE_VALUE][4] = {
	//ad9880-H-A, ad9880-V-A,ad9880-H-D, ad9880-V-D
	{124, 28, 48, 27}, //640*480*60-----0
	{148, 24, 150, 31}, //640*480*72-----1
	{164, 12, 120, 13}, //640*480*75-----2
	{116, 20, 80, 21}, //640*480*85-----3			//{640,480,0,0,116,20,80,21,81,26},//640*480*85-----3
	{196, 19, 88, 19}, //800*600*60-----4
	{164, 21, 64, 21}, //800*600*72-----5
	{220, 17, 160, 16}, //800*600*75-----6
	{196, 22, 152, 22}, //800*600*85-----7
	{274, 27, 160, 28}, //1024*768*60-----8
	{258, 27, 144, 28}, //1024*768*70-----9
	{252, 24, 206, 24}, //1024*768*75-----10
	{284, 31, 208, 32}, //1024*768*85-----11
	{304, 19, 192, 19}, //1280*768*60-----12
	{318, 20, 240, 19}, //1280*800*60-----13
	{404, 31, 312, 31}, //1280*960*60-----14
	{364, 23, 232, 23}, //1440*900*60-----15
	{364, 29, 230, 31}, //1400*1050*60-----16
	{340, 33, 246, 28}, //1280*1024*60-----17
	{372, 34, 248, 33}, //1280*1024*75-----18
	{476, 40, 380, 41}, //1600*1200*60-----19
	{278, 17, 120, 19}, //1280X720X60VGA-----20		//	{1280,720,0,0,276,17,220,19,260,20},//1280X720X60VGA-----20
	{276, 17, 260, 25}, //1280X720X50-----21
	{276, 17, 260, 25}, //1280X720X60-----22		//{1280,720,0,0,276,17,260,25,260,20},//1280X720X60-----22
	{212, 13, 192, 22}, //1920X1080IX50-----23
	{212, 13, 192, 20}, //1920X1080IX60-----24
	{166, 29, 192, 41}, //1920*1Q2080*25P-----25
	{198, 34, 170, 39}, //1920*1080*30P-----26
	{216, 34, 192, 41}, //1920*1080*50P-----27
	{216, 34, 192, 41}, //1920*1080*60P-----28
	{110, 12, 110, 11}, //480i 60-----29
	{110, 16, 110, 16}, //576i 50----30		//{720,576,0,0,110,16,270,21,7,1},//576i 50----30
};
/*h264 header length*/
extern unsigned char g_h264Header[H264_HEADER_LEN];
/*system/audio/video param table*/
//extern SavePTable  gSysParaT;
SavePTable  gSysParaT;
SavePTable2  gSysParaT2;
/*resize param table*/
extern ResizeParam gResizeP;
/*pthread mutex lock*/
PLock gSetP_m;
/*send data count*/
static unsigned int gnSentCount = 0;
/*send audio count*/
static unsigned int gnAudioCount = 0;
/*send data packet*/
unsigned char gszSendBuf[MAX_PACKET];
/*send audio data max packets*/
unsigned char gszAudioBuf[MAX_PACKET];
/*client param infomation*/
DSPClientParam gDSPCliPara[DSP_NUM];
/*max packet*/
u_char outpack[MAXPACKET];
char rcvd_tbl[MAX_DUP_CHK / 8];
static int g_sockfd = 0;

struct sockaddr_in 	g_UdpAddr ;
SOCKET      		g_UdpSocket = 0;

/*ts multi*/
int g_UdpTSSocket[MAX_PROTOCOL];
struct sockaddr_in g_UdpTSAddr[MAX_PROTOCOL];
extern int g_UdpaudioSocket;
extern struct sockaddr_in g_UdpaudioAddr;

int g_far_ctrl_index;

extern HVTable gHVTable;
int g_Logserver=-1;
struct sockaddr_in               g_serveraddr;
pthread_mutex_t sendlog_m;
char logbuf[1024];

/*Initial pthread mutex*/
int initMutexPthread()
{
	pthread_mutex_init(&gSetP_m.sys_m, NULL);
	pthread_mutex_init(&gSetP_m.save_flush_m, NULL);
	pthread_mutex_init(&gSetP_m.send_m, NULL);
	pthread_mutex_init(&gSetP_m.send_audio_m, NULL);
	pthread_mutex_init(&gSetP_m.audio_video_m, NULL);
	pthread_mutex_init(&gSetP_m.net_send_m, NULL);
	pthread_mutex_init(&gSetP_m.save_sys_m, NULL);
	pthread_mutex_init(&gSetP_m.rtmp_send_m, NULL);
	pthread_mutex_init(&gSetP_m.rtmp_audio_video, NULL);
	pthread_mutex_init(&gSetP_m.rtp_send_m, NULL);
	pthread_mutex_init(&gSetP_m.rtsp_send_m, NULL);
	pthread_mutex_init(&gSetP_m.ts_send_m, NULL);

	return 0;
}

#if 0

/*Create append Param File*/
void CreateAppendParam(const char *config_file)
{
	int ret = 0 ;
	ret = ConfigSetKey(config_file, "protocol", "RTMP", "0");
	ret = ConfigSetKey(config_file, "protocol", "RTSP", "0");
	ret = ConfigSetKey(config_file, "protocol", "RTP", "0");
	ret = ConfigSetKey(config_file, "protocol", "TS", "0");
	ret = ConfigSetKey(config_file, "protocol", "status", "0");

	return ;
}


void CreateAppendParam(const char *config_file)
{
	int ret = 0 ;
	ret = ConfigSetKey(config_file, "remote", "index", "3");
	return 0;
}


#endif


/*Initial protocol param*/
int InitProtocolParam(Protocol *pp)
{
	memset(pp, 0, sizeof(Protocol));
	return 0;
}

/*print protocol*/
int PrintProtocol(Protocol *pp)
{
	DEBUG(DL_DEBUG, "status = %d\n", pp->status);
	DEBUG(DL_DEBUG, "TS = %d\n", pp->TS);
	DEBUG(DL_DEBUG, "RTP = %d\n", pp->RTP);
	DEBUG(DL_DEBUG, "RTSP = %d\n", pp->RTSP);
	DEBUG(DL_DEBUG, "RTMP = %d\n", pp->RTMP);
	DEBUG(DL_DEBUG, "count = %d\n", pp->count);
	return 0;
}



/*Read Remote Protocol*/
int ReadRemoteCtrlIndex(const char *config_file, int *index)
{
	char 			temp[512] = {0};
	int 			ret  = 0 ;
	int 			enable = 0;
	*index = 1;
	pthread_mutex_lock(&gSetP_m.save_sys_m);
	/*remote control index*/
	ret =  ConfigGetKey(config_file, "remote", "index", temp);
	pthread_mutex_unlock(&gSetP_m.save_sys_m);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Failed to Get remote index\n");
		return 0;
	}

	*index = atoi(temp);
	return 1;
	/*
	ret =  ConfigGetKey(config_file,"remote","enable",temp);
	if(ret != 0 ) {
		DEBUG(DL_ERROR,"Failed to Get remote index\n");
		goto EXIT;
	}
	enable = atoi(temp);
	*/

}

/*save remote control index*/
int SaveRemoteCtrlIndex(void)
{
	char temp[10];
	char filename[15];
	int ret ;

	strcpy(filename, REMOTE_NAME);
	pthread_mutex_lock(&gSetP_m.save_sys_m);
	/*index*/
	sprintf(temp, "%d", g_far_ctrl_index);
	ret =  ConfigSetKey(filename, "remote", "index", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "set remote index failed\n");
		goto EXIT;
	}

EXIT:
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;

}

/*protocol ctrl*/
int ReadProtocolIni(const char *config_file, Protocol *pp)
{
	char 			temp[512];
	int 			ret  = -1 ;

	pthread_mutex_lock(&gSetP_m.save_sys_m);
	pp->count = 0;
	/*status*/
	ret =  ConfigGetKey(config_file, "protocol", "status", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Failed to Get Protocol status\n");
		goto EXIT;
	}

	pp->status = atoi(temp);
	/*TS stream*/
	ret =  ConfigGetKey(config_file, "protocol", "TS", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Failed to Get Protocol TS stream \n");
		goto EXIT;
	}

	pp->TS = atoi(temp);

	if(pp->TS && pp->status) {
		pp->count++;
	}

	/*RTP protocol*/
	ret =  ConfigGetKey(config_file, "protocol", "RTP", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Failed to Get Protocol RTP protocol \n");
		goto EXIT;
	}

	pp->RTP = atoi(temp);

	if(pp->RTP && pp->status) {
		pp->count++;
	}

	/*RTSP protocol*/
	ret =  ConfigGetKey(config_file, "protocol", "RTSP", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Failed to Get Protocol RTSP protocol\n");
		goto EXIT;
	}

	pp->RTSP = atoi(temp);

	if(pp->RTSP && pp->status) {
		pp->count++;
	}

	/*RTMP protocol*/
	ret =  ConfigGetKey(config_file, "protocol", "RTMP", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Failed to Get Protocol RTMP protocol\n");
		goto EXIT;
	}

	pp->RTMP = atoi(temp);

	if(pp->RTMP && pp->status) {
		pp->count++;
	}

EXIT:
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;
}

#if 0
/*create sys Param File*/
void CreateSysFile(const char *config_file)
{
	int ret = 0 ;
	ret = ConfigSetKey(config_file, "sysParam", "type", "6");
	ret = ConfigSetKey(config_file, "sysParam", "channels", "1");
	ret = ConfigSetKey(config_file, "sysParam", "version", "1.0.1");
	ret = ConfigSetKey(config_file, "sysParam", "name", "ENC1200");
	ret = ConfigSetKey(config_file, "sysParam", "netmask", "255.255.0.0");
	ret = ConfigSetKey(config_file, "sysParam", "gateway", "192.168.0.1");
	ret = ConfigSetKey(config_file, "sysParam", "ipaddr", "192.168.202.48");
	ret = ConfigSetKey(config_file, "sysParam", "macaddr", "00:09:08:22:30:43");

	/*Video Param*/
	ret = ConfigSetKey(config_file, "videoParam", "bitrate", "2048");
	ret = ConfigSetKey(config_file, "videoParam", "cbr", "0");
	ret = ConfigSetKey(config_file, "videoParam", "quality", "90");
	ret = ConfigSetKey(config_file, "videoParam", "colors", "24");
	ret = ConfigSetKey(config_file, "videoParam", "height", "1080");
	ret = ConfigSetKey(config_file, "videoParam", "width", "1920");
	ret = ConfigSetKey(config_file, "videoParam", "framerate", "30");
	ret = ConfigSetKey(config_file, "videoParam", "codec", "34363248");

	/*Audio param*/
	ret = ConfigSetKey(config_file, "audioParam", "inputMode", "0");
	ret = ConfigSetKey(config_file, "audioParam", "Rvolume", "26");
	ret = ConfigSetKey(config_file, "audioParam", "Lvolume", "26");
	ret = ConfigSetKey(config_file, "audioParam", "samplebit", "16");
	ret = ConfigSetKey(config_file, "audioParam", "channel", "2");
	ret = ConfigSetKey(config_file, "audioParam", "bitrate", "64000");
	ret = ConfigSetKey(config_file, "audioParam", "samplerate", "2");
	ret = ConfigSetKey(config_file, "audioParam", "codec", "53544441");

	if(ret < 0) {
		DEBUG(DL_ERROR, "error \n");
	}
}

#endif

static int SplitMacAddr(char *src, unsigned char *dst, int num)
{
	char *p;
	char *q = src;
	int val = 0  , i = 0;

	for(i = 0 ; i < num ; i++) {
		p = strstr(q, ":");

		if(!p) {
			val = strtol(q, 0, 16);
			dst[i]  = val ;

			if(i == num - 1) {
				continue;
			} else {
				return -1;
			}
		}

		*(p++) = '\0';
		val = strtol(q, 0, 16);
		dst[i]  = val ;
		q = p;
	}

	return 0;
}

static int JoinMacAddr(char *dst, unsigned char *src, int num)
{
	sprintf(dst, "%02x:%02x:%02x:%02x:%02x:%02x", src[0],
	        src[1], src[2], src[3], src[4], src[5]);
	DEBUG(DL_DEBUG, "mac addr  = %s \n", dst);

	return 0;
}
/*check IP addr */
int CheckIP(int ipaddr, int netmask)
{
	int mask, ip, gateip;
	mask = netmask;
	mask = htonl(mask);
	ip = ipaddr;
	ip = htonl(ip);
	

	if((((ip & 0xFF000000) >> 24) > 223) || ((((ip & 0xFF000000) >> 24 == 127)))) {
		return 0;
	}

	if(((ip & 0xFF000000) >> 24) < 1) {
		return 0;
	}

	if((ip & mask) == 0) {
		return 0;
	}

	if((ip & (~mask)) == 0) {
		return 0;
	}

	if((~(ip | mask)) == 0) {
		return 0;
	}

	while(mask != 0) {
		if(mask > 0) {
			return 0;
		}

		mask <<= 1;
	}

	return 1;
}


/*check IP addr and Netmask */
int CheckIPNetmask(int ipaddr, int netmask, int gw)
{
	int mask, ip, gateip;
	mask = netmask;
	mask = htonl(mask);
	ip = ipaddr;
	ip = htonl(ip);
	gateip = gw;
	gateip = htonl(gateip);

	if((((ip & 0xFF000000) >> 24) > 223) || ((((ip & 0xFF000000) >> 24 == 127)))) {
		return 0;
	}

	if(((ip & 0xFF000000) >> 24) < 1) {
		return 0;
	}

	if((((gateip & 0xFF000000) >> 24) > 223) || (((gateip & 0xFF000000) >> 24 == 127))) {
		return 0;
	}

	if(((gateip & 0xFF000000) >> 24) < 1) {
		return 0;
	}

	if((ip & mask) == 0) {
		return 0;
	}

	if((ip & (~mask)) == 0) {
		return 0;
	}

	if((~(ip | mask)) == 0) {
		return 0;
	}

	while(mask != 0) {
		if(mask > 0) {
			return 0;
		}

		mask <<= 1;
	}

	return 1;
}
/*initial hporch and vporch value*/
int InitHVTable(HVTable *pp)
{
	int i = 0;

	for(i = 0; i <= 30 ; i++) {
		pp->analog[i].hporch = FPGA_HV_TALBE[i][0];
		pp->analog[i].vporch = FPGA_HV_TALBE[i][1];
		pp->digital[i].hporch = FPGA_HV_TALBE[i][2];
		pp->digital[i].vporch = FPGA_HV_TALBE[i][3];
	}

	return 0;
}
/*read Hportch Vportch Table*/
int ReadHVTable(const HVTable *pp, int digital)
{
	int ret = 0, val = 0;
	char temp[512], config_file[20];
	HV *pHV = NULL;

	if(digital) {
		pHV = pp->analog;
		strcpy(config_file, HV_TABLE_A_NAME);
	} else {
		pHV = pp->digital;
		strcpy(config_file, HV_TABLE_D_NAME);
	}

	pthread_mutex_lock(&gSetP_m.save_sys_m);
	ret = ConfigGetKey(config_file, "640x480x60", "hporch", temp); // 0

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[0].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[0].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x72", "hporch", temp);	// 1

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[1].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x72", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[1].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x75", "hporch", temp);	// 2

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[2].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x75", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[2].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x85", "hporch", temp);	// 3

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[3].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "640x480x85", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[3].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x60", "hporch", temp);	// 4

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[4].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[4].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x72", "hporch", temp);	// 5

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[5].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x72", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[5].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x75", "hporch", temp);	// 6

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[6].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x75", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[6].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x85", "hporch", temp);	// 7

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[7].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "800x600x85", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[7].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x60", "hporch", temp);	// 8

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[8].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[8].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x70", "hporch", temp);	 // 9

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[9].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x70", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[9].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x75", "hporch", temp);	// 10

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[10].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x75", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[10].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x85", "hporch", temp);	// 11

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[11].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1024x768x85", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[11].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x768x60", "hporch", temp);	// 12

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[12].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x768x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[12].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x800x60", "hporch", temp);	// 13

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[13].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x800x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[13].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x960x60", "hporch", temp);	// 14

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[14].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x960x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[14].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1440x900x60", "hporch", temp);	// 15

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[15].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1440x900x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[15].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1400x1050x60", "hporch", temp);	// 16

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[16].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1400x1050x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[16].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x1024x60", "hporch", temp); // 17

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[17].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x1024x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[17].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x1024x75", "hporch", temp); // 18

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[18].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x1024x75", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[18].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1600x1200x60", "hporch", temp); // 19

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[19].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1600x1200x60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[19].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x720V60", "hporch", temp);	 // 20

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[20].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x720V60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[20].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x720P50", "hporch", temp);	// 21

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[21].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x720P50", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[21].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x720P60", "hporch", temp);	// 22

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[22].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1280x720P60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[22].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080I50", "hporch", temp); // 23

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[23].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080I50", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[23].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080I60", "hporch", temp); // 24

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[24].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080I60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[24].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080P25", "hporch", temp); // 25

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[25].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080P25", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[25].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080P30", "hporch", temp); // 26

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[26].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080P30", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[26].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080Px60", "hporch", temp); // 27

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[27].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080Px60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[27].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080Px50", "hporch", temp); // 28

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[28].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "1920x1080Px50", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[28].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "720x480I60", "hporch", temp);	 // 29

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[29].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "720x480I60", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[29].vporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "720x576I50", "hporch", temp);	// 30

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[30].hporch = val;
		}
	}

	ret = ConfigGetKey(config_file, "720x576I50", "vporch", temp);

	if(ret == 0) {
		val = atoi(temp);

		if(val > 0) {
			pHV[30].vporch = val;
		}
	}

EXIT:
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;
}

/*save HV  Table*/
int SaveHVTable(HVTable *pOld, HVTable *pNew, int digital)
{
	char  temp[512], config_file[20];
	HV *pHV = NULL, *pNewHV = NULL;
	int ret;

	if(digital) {
		pHV = pOld->digital;
		pNewHV = pNew->digital;
		strcpy(config_file, HV_TABLE_D_NAME);
	} else {
		pHV = pOld->analog ;
		pNewHV = pNew->analog;
		strcpy(config_file, HV_TABLE_A_NAME);
	}

	pthread_mutex_lock(&gSetP_m.save_sys_m);

	if(pNewHV[0].hporch != pHV[0].hporch) {
		pHV[0].hporch = pNewHV[0].hporch;
		sprintf(temp, "%d", pHV[0].hporch);
		ret = ConfigSetKey(config_file, "640x480x60", "hporch", temp); // 0

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[0].vporch != pHV[0].vporch) {
		pHV[0].vporch = pNewHV[0].vporch;
		sprintf(temp, "%d", pHV[0].vporch);
		ret = ConfigSetKey(config_file, "640x480x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[1].hporch != pHV[1].hporch) {
		pHV[1].hporch = pNewHV[1].hporch;
		sprintf(temp, "%d", pHV[1].hporch);
		ret = ConfigSetKey(config_file, "640x480x72", "hporch", temp);	// 1

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x72 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[1].vporch != pHV[1].vporch) {
		pHV[1].vporch = pNewHV[1].vporch;
		sprintf(temp, "%d", pHV[1].vporch);
		ret = ConfigSetKey(config_file, "640x480x72", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x72 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[2].hporch != pHV[2].hporch) {
		pHV[2].hporch = pNewHV[2].hporch;
		sprintf(temp, "%d", pHV[2].hporch);
		ret = ConfigSetKey(config_file, "640x480x75", "hporch", temp);	// 2

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x75 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[2].vporch != pHV[2].vporch) {
		pHV[2].vporch = pNewHV[2].vporch;
		sprintf(temp, "%d", pHV[2].vporch);
		ret = ConfigSetKey(config_file, "640x480x75", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x75 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[3].hporch != pHV[3].hporch) {
		pHV[3].hporch = pNewHV[3].hporch;
		sprintf(temp, "%d", pHV[3].hporch);
		ret = ConfigSetKey(config_file, "640x480x85", "hporch", temp);	// 3

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x75 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[3].vporch != pHV[3].vporch) {
		pHV[3].vporch = pNewHV[3].vporch;
		sprintf(temp, "%d", pHV[3].vporch);
		ret = ConfigSetKey(config_file, "640x480x85", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "640x480x85 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[4].hporch != pHV[4].hporch) {
		pHV[4].hporch = pNewHV[4].hporch;
		sprintf(temp, "%d", pHV[4].hporch);
		ret = ConfigSetKey(config_file, "800x600x60", "hporch", temp);	// 4

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[4].vporch != pHV[4].vporch) {
		pHV[4].vporch = pNewHV[4].vporch;
		sprintf(temp, "%d", pHV[4].vporch);
		ret = ConfigSetKey(config_file, "800x600x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[5].hporch != pHV[5].hporch) {
		pHV[5].hporch = pNewHV[5].hporch;
		sprintf(temp, "%d", pHV[5].hporch);
		ret = ConfigSetKey(config_file, "800x600x72", "hporch", temp);	// 5

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x72 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[5].vporch != pHV[5].vporch) {
		pHV[5].vporch = pNewHV[5].vporch;
		sprintf(temp, "%d", pHV[5].vporch);
		ret = ConfigSetKey(config_file, "800x600x72", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x72 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[6].hporch != pHV[6].hporch) {
		pHV[6].hporch = pNewHV[6].hporch;
		sprintf(temp, "%d", pHV[6].hporch);
		ret = ConfigSetKey(config_file, "800x600x75", "hporch", temp);	// 6

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x75 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[6].vporch != pHV[6].vporch) {
		pHV[6].vporch = pNewHV[6].vporch;
		sprintf(temp, "%d", pHV[6].vporch);
		ret = ConfigSetKey(config_file, "800x600x75", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x75 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[7].hporch != pHV[7].hporch) {
		pHV[7].hporch = pNewHV[7].hporch;
		sprintf(temp, "%d", pHV[7].hporch);
		ret = ConfigSetKey(config_file, "800x600x85", "hporch", temp);	// 7

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x85 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[7].vporch != pHV[7].vporch) {
		pHV[7].vporch = pNewHV[7].vporch;
		sprintf(temp, "%d", pHV[7].vporch);
		ret = ConfigSetKey(config_file, "800x600x85", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "800x600x85 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[8].hporch != pHV[8].hporch) {
		pHV[8].hporch = pNewHV[8].hporch;
		sprintf(temp, "%d", pHV[8].hporch);
		ret = ConfigSetKey(config_file, "1024x768x60", "hporch", temp);	// 8

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x60 hporch!!\n");
			goto EXIT;
		}

		DEBUG(DL_DEBUG, "1024x768 change hporch!!! = %s\n", temp);
	}

	if(pNewHV[8].vporch != pHV[8].vporch) {
		pHV[8].vporch = pNewHV[8].vporch;
		sprintf(temp, "%d", pHV[8].vporch);
		ret = ConfigSetKey(config_file, "1024x768x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x60 hporch!!\n");
			goto EXIT;
		}

		DEBUG(DL_DEBUG, "1024x768 change vporch!!! = %s\n", temp);
	}

	if(pNewHV[9].hporch != pHV[9].hporch) {
		pHV[9].hporch = pNewHV[9].hporch;
		sprintf(temp, "%d", pHV[9].hporch);
		ret = ConfigSetKey(config_file, "1024x768x70", "hporch", temp);	 // 9

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x70 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[9].vporch != pHV[9].vporch) {
		pHV[9].vporch = pNewHV[9].vporch;
		sprintf(temp, "%d", pHV[9].vporch);
		ret = ConfigSetKey(config_file, "1024x768x70", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x70 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[10].hporch != pHV[10].hporch) {
		pHV[10].hporch = pNewHV[10].hporch;
		sprintf(temp, "%d", pHV[10].hporch);
		ret = ConfigSetKey(config_file, "1024x768x75", "hporch", temp);	// 10

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x70 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[10].vporch != pHV[10].vporch) {
		pHV[10].vporch = pNewHV[10].vporch;
		sprintf(temp, "%d", pHV[10].vporch);
		ret = ConfigSetKey(config_file, "1024x768x75", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x75 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[11].hporch != pHV[11].hporch) {
		pHV[11].hporch = pNewHV[11].hporch;
		sprintf(temp, "%d", pHV[11].hporch);
		ret = ConfigSetKey(config_file, "1024x768x85", "hporch", temp);	// 11

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x85 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[11].vporch != pHV[11].vporch) {
		pHV[11].vporch = pNewHV[11].vporch;
		sprintf(temp, "%d", pHV[11].vporch);
		ret = ConfigSetKey(config_file, "1024x768x85", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x85 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[12].hporch != pHV[12].hporch) {
		pHV[12].hporch = pNewHV[12].hporch;
		sprintf(temp, "%d", pHV[12].hporch);
		ret = ConfigSetKey(config_file, "1280x768x60", "hporch", temp);	// 12

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x85 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[12].vporch != pHV[12].vporch) {
		pHV[12].vporch = pNewHV[12].vporch;
		sprintf(temp, "%d", pHV[12].vporch);
		ret = ConfigSetKey(config_file, "1280x768x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1024x768x85 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[13].hporch != pHV[13].hporch) {
		pHV[13].hporch = pNewHV[13].hporch;
		sprintf(temp, "%d", pHV[13].hporch);
		ret = ConfigSetKey(config_file, "1280x800x60", "hporch", temp);	// 13

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x800x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[13].vporch != pHV[13].vporch) {
		pHV[13].vporch = pNewHV[13].vporch;
		sprintf(temp, "%d", pHV[13].vporch);
		ret = ConfigSetKey(config_file, "1280x800x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x800x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[14].hporch != pHV[14].hporch) {
		pHV[14].hporch = pNewHV[14].hporch;
		sprintf(temp, "%d", pHV[14].hporch);
		ret = ConfigSetKey(config_file, "1280x960x60", "hporch", temp);	// 14

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x960x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[14].vporch != pHV[14].vporch) {
		pHV[14].vporch = pNewHV[14].vporch;
		sprintf(temp, "%d", pHV[14].vporch);
		ret = ConfigSetKey(config_file, "1280x960x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x960x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[15].hporch != pHV[15].hporch) {
		pHV[15].hporch = pNewHV[15].hporch;
		sprintf(temp, "%d", pHV[15].hporch);
		ret = ConfigSetKey(config_file, "1440x900x60", "hporch", temp);	// 15

		if(ret != 0) {
			DEBUG(DL_ERROR, "1440x900x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[15].vporch != pHV[15].vporch) {
		pHV[15].vporch = pNewHV[15].vporch;
		sprintf(temp, "%d", pHV[15].vporch);
		ret = ConfigSetKey(config_file, "1440x900x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1440x900x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[16].hporch != pHV[16].hporch) {
		pHV[16].hporch = pNewHV[16].hporch;
		sprintf(temp, "%d", pHV[16].hporch);
		ret = ConfigSetKey(config_file, "1400x1050x60", "hporch", temp);	// 16

		if(ret != 0) {
			DEBUG(DL_ERROR, "1400x1050x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[16].vporch != pHV[16].vporch) {
		pHV[16].vporch = pNewHV[16].vporch;
		sprintf(temp, "%d", pHV[16].vporch);
		ret = ConfigSetKey(config_file, "1400x1050x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1400x1050x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[17].hporch != pHV[17].hporch) {
		pHV[17].hporch = pNewHV[17].hporch;
		sprintf(temp, "%d", pHV[17].hporch);
		ret = ConfigSetKey(config_file, "1280x1024x60", "hporch", temp); // 17

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x1024x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[17].vporch != pHV[17].vporch) {
		pHV[17].vporch = pNewHV[17].vporch;
		sprintf(temp, "%d", pHV[17].vporch);
		ret = ConfigSetKey(config_file, "1280x1024x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x1024x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[18].hporch != pHV[18].hporch) {
		pHV[18].hporch = pNewHV[18].hporch;
		sprintf(temp, "%d", pHV[18].hporch);
		ret = ConfigSetKey(config_file, "1280x1024x75", "hporch", temp); // 18

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x1024x75 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[18].vporch != pHV[18].vporch) {
		pHV[18].vporch = pNewHV[18].vporch;
		sprintf(temp, "%d", pHV[18].vporch);
		ret = ConfigSetKey(config_file, "1280x1024x75", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x1024x75 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[19].hporch != pHV[19].hporch) {
		pHV[19].hporch = pNewHV[19].hporch;
		sprintf(temp, "%d", pHV[19].hporch);
		ret = ConfigSetKey(config_file, "1600x1200x60", "hporch", temp); // 19

		if(ret != 0) {
			DEBUG(DL_ERROR, "1600x1200x60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[19].vporch != pHV[19].vporch) {
		pHV[19].vporch = pNewHV[19].vporch;
		sprintf(temp, "%d", pHV[19].vporch);
		ret = ConfigSetKey(config_file, "1600x1200x60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1600x1200x60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[20].hporch != pHV[20].hporch) {
		pHV[20].hporch = pNewHV[20].hporch;
		sprintf(temp, "%d", pHV[20].hporch);
		ret = ConfigSetKey(config_file, "1280x720V60", "hporch", temp);	 // 20

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x720V60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[20].vporch != pHV[20].vporch) {
		pHV[20].vporch = pNewHV[20].vporch;
		sprintf(temp, "%d", pHV[20].vporch);
		ret = ConfigSetKey(config_file, "1280x720V60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x720V60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[21].hporch != pHV[21].hporch) {
		pHV[21].hporch = pNewHV[21].hporch;
		sprintf(temp, "%d", pHV[21].hporch);
		ret = ConfigSetKey(config_file, "1280x720P50", "hporch", temp);	// 21

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x720P50 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[21].vporch != pHV[21].vporch) {
		pHV[21].vporch = pNewHV[21].vporch;
		sprintf(temp, "%d", pHV[21].vporch);
		ret = ConfigSetKey(config_file, "1280x720P50", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x720P50 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[22].hporch != pHV[22].hporch) {
		pHV[22].hporch = pNewHV[22].hporch;
		sprintf(temp, "%d", pHV[22].hporch);
		ret = ConfigSetKey(config_file, "1280x720P60", "hporch", temp);	// 22

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x720P60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[22].vporch != pHV[22].vporch) {
		pHV[22].vporch = pNewHV[22].vporch;
		sprintf(temp, "%d", pHV[22].vporch);
		ret = ConfigSetKey(config_file, "1280x720P60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1280x720P60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[23].hporch != pHV[23].hporch) {
		pHV[23].hporch = pNewHV[23].hporch;
		sprintf(temp, "%d", pHV[23].hporch);
		ret = ConfigSetKey(config_file, "1920x1080I50", "hporch", temp); // 23

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080I50 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[23].vporch != pHV[23].vporch) {
		pHV[23].vporch = pNewHV[23].vporch;
		sprintf(temp, "%d", pHV[23].vporch);
		ret = ConfigSetKey(config_file, "1920x1080I50", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080I50 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[24].hporch != pHV[24].hporch) {
		pHV[24].hporch = pNewHV[24].hporch;
		sprintf(temp, "%d", pHV[24].hporch);
		ret = ConfigSetKey(config_file, "1920x1080I60", "hporch", temp); // 24

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080I60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[24].vporch != pHV[24].vporch) {
		pHV[24].vporch = pNewHV[24].vporch;
		sprintf(temp, "%d", pHV[24].vporch);
		ret = ConfigSetKey(config_file, "1920x1080I60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080I60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[25].hporch != pHV[25].hporch) {
		pHV[25].hporch = pNewHV[25].hporch;
		sprintf(temp, "%d", pHV[25].hporch);
		ret = ConfigSetKey(config_file, "1920x1080P25", "hporch", temp); // 25

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080P25 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[25].vporch != pHV[25].vporch) {
		pHV[25].vporch = pNewHV[25].vporch;
		sprintf(temp, "%d", pHV[25].vporch);
		ret = ConfigSetKey(config_file, "1920x1080P25", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080P25 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[26].hporch != pHV[26].hporch) {
		pHV[26].hporch = pNewHV[26].hporch;
		sprintf(temp, "%d", pHV[26].hporch);
		ret = ConfigSetKey(config_file, "1920x1080P30", "hporch", temp); // 26

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080P30 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[26].vporch != pHV[26].vporch) {
		pHV[26].vporch = pNewHV[26].vporch;
		sprintf(temp, "%d", pHV[26].vporch);
		ret = ConfigSetKey(config_file, "1920x1080P30", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080P30 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[27].hporch != pHV[27].hporch) {
		pHV[27].hporch = pNewHV[27].hporch;
		sprintf(temp, "%d", pHV[27].hporch);
		ret = ConfigSetKey(config_file, "1920x1080Px60", "hporch", temp); // 27

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080Px60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[27].vporch != pHV[27].vporch) {
		pHV[27].vporch = pNewHV[27].vporch;
		sprintf(temp, "%d", pHV[27].vporch);
		ret = ConfigSetKey(config_file, "1920x1080Px60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080Px60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[28].hporch != pHV[28].hporch) {
		pHV[28].hporch = pNewHV[28].hporch;
		sprintf(temp, "%d", pHV[28].hporch);
		ret = ConfigSetKey(config_file, "1920x1080Px50", "hporch", temp); // 28

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080Px50 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[28].vporch != pHV[28].vporch) {
		pHV[28].vporch = pNewHV[28].vporch;
		sprintf(temp, "%d", pHV[28].vporch);
		ret = ConfigSetKey(config_file, "1920x1080Px50", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "1920x1080Px50 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[29].hporch != pHV[29].hporch) {
		pHV[29].hporch = pNewHV[29].hporch;
		sprintf(temp, "%d", pHV[29].hporch);
		ret = ConfigSetKey(config_file, "720x480I60", "hporch", temp);	 // 29

		if(ret != 0) {
			DEBUG(DL_ERROR, "720x480I60 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[29].vporch != pHV[29].vporch) {
		pHV[29].vporch = pNewHV[29].vporch;
		sprintf(temp, "%d", pHV[29].vporch);
		ret = ConfigSetKey(config_file, "720x480I60", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "720x480I60 vporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[30].hporch != pHV[30].hporch) {
		pHV[30].hporch = pNewHV[30].hporch;
		sprintf(temp, "%d", pHV[30].hporch);
		ret = ConfigSetKey(config_file, "720x576I50", "hporch", temp);	// 30

		if(ret != 0) {
			DEBUG(DL_ERROR, "720x576I50 hporch!!\n");
			goto EXIT;
		}
	}

	if(pNewHV[30].vporch != pHV[30].vporch) {
		pHV[30].vporch = pNewHV[30].vporch;
		sprintf(temp, "%d", pHV[30].vporch);
		ret = ConfigSetKey(config_file, "720x576I50", "vporch", temp);

		if(ret != 0) {
			DEBUG(DL_ERROR, "720x576I50 vporch!!\n");
			goto EXIT;
		}
	}

EXIT:
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	DEBUG(DL_DEBUG, "save HV table succeed!!\n");
	return 0;

}


/*compare beyond HV table*/
int BeyondHVTable(int mode, int digital, short hporch, short vporch, HVTable *pp)
{
	if(mode > 30) {
		return -1;
	}

	if(digital) {
		pp->digital[mode].hporch = hporch;
		pp->digital[mode].vporch = vporch;
	} else {
		pp->analog[mode].hporch = hporch;
		pp->analog[mode].vporch = vporch;
	}

	return 0;
}
unsigned int GetIPaddr(char *interface_name)
{
	int s;
	unsigned int ip;

	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		DEBUG(DL_ERROR,"Socket");
		return -1;
	}

	struct ifreq ifr;

	strcpy(ifr.ifr_name, interface_name);

	if(ioctl(s, SIOCGIFADDR, &ifr) < 0) {
		DEBUG(DL_ERROR,"ioctl");
		return -1;
	}

	struct sockaddr_in *ptr;

	ptr = (struct sockaddr_in *) &ifr.ifr_ifru.ifru_addr;

	DEBUG(DL_DEBUG,"        IP:%s\n", inet_ntoa(ptr->sin_addr));

	memcpy(&ip, &ptr->sin_addr, 4);

	return ip;
}

unsigned int GetNetmask(char *interface_name)
{
	int s;
	unsigned int ip;

	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		DEBUG(DL_ERROR,"Socket");
		return -1;
	}

	struct ifreq ifr;

	strcpy(ifr.ifr_name, interface_name);

	if(ioctl(s, SIOCGIFNETMASK, &ifr) < 0) {
		DEBUG(DL_ERROR,"ioctl");
		return -1;
	}

	struct sockaddr_in *ptr;

	ptr = (struct sockaddr_in *) &ifr.ifr_ifru.ifru_netmask;

	DEBUG(DL_DEBUG,"        Netmask:%s\n", inet_ntoa(ptr->sin_addr));

	memcpy(&ip, &ptr->sin_addr, 4);

	return ip;
}

unsigned int GetBroadcast(char *interface_name)
{
	int s;
	unsigned int ip;

	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		DEBUG(DL_ERROR,"Socket");
		return -1;
	}

	struct ifreq ifr;

	strcpy(ifr.ifr_name, interface_name);

	if(ioctl(s, SIOCGIFBRDADDR, &ifr) < 0) {
		DEBUG(DL_ERROR,"ioctl");
		return -1;
	}

	struct sockaddr_in *ptr;

	ptr = (struct sockaddr_in *) &ifr.ifr_ifru.ifru_broadaddr;

	DEBUG(DL_DEBUG,"        Broadcast:%s\n", inet_ntoa(ptr->sin_addr));

	memcpy(&ip, &ptr->sin_addr, 4);

	return ip;

}
int ReadEncodeParam(char *config_file, SavePTable *tblParam)
{
	int 			ret;
	unsigned int 	val = 0;
	char  			temp[512] = {0};
	int 			dwAddr, dwNetmask, dwGateWay;
	int 			netflag = 0;
	struct 			in_addr	addr ;
	unsigned int 		value;

	if(config_file == NULL) {
		DEBUG(DL_ERROR, "filename is null\n");
		return -1;
	}


	/**********************system config******************************/
	ret =  ConfigGetKey(config_file, "sysParam", "macaddr", temp);

	if(ret == 0) {
		SplitMacAddr(temp, tblParam->sysPara.szMacAddr, 6);
	} else {
		DEBUG(DL_ERROR, "\n");
	}



#if 0
	/*IP地址*/
	ret =  ConfigGetKey(config_file, "sysParam", "ipaddr", temp);

	if(ret == 0) {
		netflag++;
		inet_aton(temp, &addr);
		memcpy(&value, &addr, 4);
		dwAddr = value;
	} else {
		DEBUG(DL_ERROR, "\n");
	}



	/*网关*/
	ret =  ConfigGetKey(config_file, "sysParam", "gateway", temp);

	if(ret == 0) {
		netflag++;
		inet_aton(temp, &addr);
		memcpy(&value, &addr, 4);
		dwGateWay = value;
	} else {
		DEBUG(DL_ERROR, "\n");
	}

	/*子网掩码*/
	ret =  ConfigGetKey(config_file, "sysParam", "netmask", temp);

	if(ret == 0) {
		netflag++;
		inet_aton(temp, &addr);
		memcpy(&value, &addr, 4);
		dwNetmask = value;
	} else {
		DEBUG(DL_ERROR, "read netmask\n");
	}

#else

	/*IP地址*/
	ret =  ConfigGetKey("network.ini_1", "sysParam", "ipaddr", temp);

	if(ret == 0) {
		netflag++;
		inet_aton(temp, &addr);
		memcpy(&value, &addr, 4);
		dwAddr = value;
	} else {
		DEBUG(DL_ERROR, "\n");
	}



	/*网关*/
	ret =  ConfigGetKey("network.ini_1", "sysParam", "gateway", temp);

	if(ret == 0) {
		netflag++;
		inet_aton(temp, &addr);
		memcpy(&value, &addr, 4);
		dwGateWay = value;
	} else {
		DEBUG(DL_ERROR, "\n");
	}

	/*子网掩码*/
	ret =  ConfigGetKey("network.ini_1", "sysParam", "netmask", temp);

	if(ret == 0) {
		netflag++;
		inet_aton(temp, &addr);
		memcpy(&value, &addr, 4);
		dwNetmask = value;
	} else {
		DEBUG(DL_ERROR, "read netmask\n");
	}

#endif

	/*检测ip地址的合法性*/
	if(netflag == 3 && CheckIPNetmask(dwAddr, dwNetmask, dwGateWay)) {
		//拷贝网络相关信息
		tblParam->sysPara.dwAddr		= dwAddr;
		tblParam->sysPara.dwGateWay		= dwGateWay;
		tblParam->sysPara.dwNetMark 	= dwNetmask;
		DEBUG(DL_DEBUG, "read network config is OK\n");
	} else {
		DEBUG(DL_ERROR, "read network config is ERROR\n");
	}


	/*veision*/
	ret =  ConfigGetKey(config_file, "sysParam", "version", temp);

	if(ret == 0) {
		snprintf(tblParam->sysPara.strVer, sizeof(tblParam->sysPara.strVer), "%s", temp);
	} else {
		DEBUG(DL_ERROR, "ERROR!sysParam.version\n");
	}

	/*name*/
	ret =  ConfigGetKey(config_file, "sysParam", "name", temp);

	if(ret == 0) {
		snprintf(tblParam->sysPara.strName, sizeof(tblParam->sysPara.strName), "%s", temp);
	} else {
		DEBUG(DL_ERROR, "ERROR!sysParam.name\n");
	}

	/*bvmode*/
	ret =  ConfigGetKey(config_file, "sysParam", "bvmode", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 2) {
			tblParam->sysPara.bVmode =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/**********************video config*********************************/
	/*video codec*/
	ret =  ConfigGetKey(config_file, "videoParam", "codec", temp);

	if(ret == 0) {
		if(strtol(temp, 0, 16) > 0) {
			tblParam->videoPara[DSP1].nDataCodec  = strtol(temp, 0, 16);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*帧率*/
	ret =  ConfigGetKey(config_file, "videoParam", "framerate", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 30) {
			tblParam->videoPara[DSP1].nFrameRate =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*视频宽高*/
	ret =  ConfigGetKey(config_file, "videoParam", "width", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 1920) {
			tblParam->videoPara[DSP1].nWidth =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}


	ret =  ConfigGetKey(config_file, "videoParam", "height", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 1080) {
			tblParam->videoPara[DSP1].nHight =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*video color*/
	ret =  ConfigGetKey(config_file, "videoParam", "colors", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->videoPara[DSP1].nColors =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*video quality*/
	ret =  ConfigGetKey(config_file, "videoParam", "quality", temp);

	if(ret == 0) {
		if(atol(temp) > 5 && atol(temp) <= 90) {
			tblParam->videoPara[DSP1].nQuality =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*video cbr*/
	ret =  ConfigGetKey(config_file, "videoParam", "cbr", temp);

	if(ret == 0) {
		if(atol(temp) == 0 || atol(temp) == 1) {
			tblParam->videoPara[DSP1].sCbr =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*video bitrate*/
	ret =  ConfigGetKey(config_file, "videoParam", "bitrate", temp);

	if(ret == 0) {
		if(atol(temp) > 128) {
			tblParam->videoPara[DSP1].sBitrate =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

#ifdef LOW_VIDEO
	/******************low video config*********************************/
	/*low video codec*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "codec", temp);

	if(ret == 0) {
		if(strtol(temp, 0, 16) > 0) {
			tblParam->lowvideoPara[DSP1].nDataCodec  = strtol(temp, 0, 16);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*low 帧率*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "framerate", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 30) {
			tblParam->lowvideoPara[DSP1].nFrameRate =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*low 视频宽高*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "width", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 1920) {
			tblParam->lowvideoPara[DSP1].nWidth =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}


	ret =  ConfigGetKey(config_file, "lowvideoParam", "height", temp);

	if(ret == 0) {
		if(atol(temp) > 0 && atol(temp) <= 1080) {
			tblParam->lowvideoPara[DSP1].nHight =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*low video color*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "colors", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->lowvideoPara[DSP1].nColors =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*low video quality*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "quality", temp);

	if(ret == 0) {
		if(atol(temp) > 5 && atol(temp) <= 90) {
			tblParam->lowvideoPara[DSP1].nQuality =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*low video cbr*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "cbr", temp);

	if(ret == 0) {
		if(atol(temp) == 0 || atol(temp) == 1) {
			tblParam->lowvideoPara[DSP1].sCbr =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*low video bitrate*/
	ret =  ConfigGetKey(config_file, "lowvideoParam", "bitrate", temp);

	if(ret == 0) {
		if(atol(temp) > 128) {
			tblParam->lowvideoPara[DSP1].sBitrate =	atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

#endif

	/******************audio video config*********************************/
	/*audio codec*/
	ret =  ConfigGetKey(config_file, "audioParam", "codec", temp);

	if(ret == 0) {
		if(strtol(temp, 0, 16) > 0) {
			tblParam->audioPara[DSP1].Codec = strtol(temp, 0, 16);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio samplerate*/
	ret =  ConfigGetKey(config_file, "audioParam", "samplerate", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->audioPara[DSP1].SampleRate = atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio bitrate*/
	ret =  ConfigGetKey(config_file, "audioParam", "bitrate", temp);

	if(ret == 0) {
		if(atol(temp) > 16000) {
			tblParam->audioPara[DSP1].BitRate = atol(temp);
		}

	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio channel*/
	ret =  ConfigGetKey(config_file, "audioParam", "channel", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->audioPara[DSP1].Channel = atol(temp);
		}
	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio SampleBit*/
	ret =  ConfigGetKey(config_file, "audioParam", "samplebit", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->audioPara[DSP1].SampleBit = atol(temp);
		}

	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio Lvolume*/
	ret =  ConfigGetKey(config_file, "audioParam", "Lvolume", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->audioPara[DSP1].LVolume = atol(temp);
		}

	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio Rvolume*/
	ret =  ConfigGetKey(config_file, "audioParam", "Rvolume", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->audioPara[DSP1].RVolume = atol(temp);
		}

	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	/*audio inputMode*/
	ret =  ConfigGetKey(config_file, "audioParam", "inputMode", temp);

	if(ret == 0) {
		if(atol(temp) > 0) {
			tblParam->audioPara[DSP1].InputMode = atol(temp);
		}

	} else {
		DEBUG(DL_ERROR, "ERROR!");
	}

	//systemv2 版本 video相关参数保存在video里面
	tblParam->sysPara.nFrame = tblParam->videoPara[DSP1].nFrameRate;
	tblParam->sysPara.nColors = tblParam->videoPara[DSP1].nColors;
	tblParam->sysPara.nQuality = tblParam->videoPara[DSP1].nQuality;
	tblParam->sysPara.sCbr = tblParam->videoPara[DSP1].sCbr;
	tblParam->sysPara.sBitrate = tblParam->videoPara[DSP1].sBitrate;



	return 0;
}
/*读配置文件*/
int ReadEncodeParamTable(char *config_file, SavePTable *tblParam)
{
	SavePTable 		tmpTable;
	char 			temp[512];
	int 			ret  = -1 ;
	struct in_addr	addr ;
	unsigned int 	value;
	unsigned int 	dwAddr, dwNetmask, dwGateWay;
	FILE			*fp;
	pthread_mutex_lock(&gSetP_m.save_sys_m);
	fp = fopen(CONFIG_NAME, "r");
	fread(&tmpTable, sizeof(gSysParaT), 1, fp);
	fclose(fp);

	/*检测ip地址的合法性*/
	if(CheckIPNetmask(tmpTable.sysPara.dwAddr, tmpTable.sysPara.dwNetMark, tmpTable.sysPara.dwGateWay)) {
		memcpy(tblParam, &tmpTable, sizeof(gSysParaT));
	}

	//tblParam->sysPara.dwAddr = GetIPaddr("eth0");
	//tblParam->sysPara.dwNetMark = GetNetmask("eth0");


	if(gSysParaT.audioPara[DSP1].BitRate < 16000) {
		gSysParaT.audioPara[DSP1].BitRate = 16000;
	}

	if((gSysParaT.sysPara.nFrame > 30) || (gSysParaT.sysPara.nFrame < 1)) {
		gSysParaT.sysPara.nFrame = 30;
	}

	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;
	/*MAC 地址*/
	DEBUG(DL_DEBUG, "read config = %s \n", config_file);
	ret =  ConfigGetKey(config_file, "sysParam", "macaddr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key mac addr failed\n");
		goto EXIT;
	}

	ret = SplitMacAddr(temp, tmpTable.sysPara.szMacAddr, 6);

	if(ret < 0) {
		DEBUG(DL_ERROR, "[SplitMacAddr] mac addr error\n");
		goto EXIT;
	}

	/*IP地址*/
	ret =  ConfigGetKey(config_file, "sysParam", "ipaddr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key ipaddr failed\n");
		goto EXIT;
	}

	inet_aton(temp, &addr);
	memcpy(&value, &addr, 4);
	dwAddr = value;


	/*网关*/
	ret =  ConfigGetKey(config_file, "sysParam", "gateway", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key gateway failed\n");
		goto EXIT;
	}

	inet_aton(temp, &addr);
	memcpy(&value, &addr, 4);
	dwGateWay = value;


	/*子网掩码*/
	ret =  ConfigGetKey(config_file, "sysParam", "netmask", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key netmask failed\n");
		goto EXIT;
	}

	inet_aton(temp, &addr);
	memcpy(&value, &addr, 4);
	dwNetmask = value;

	/*检测ip地址的合法性*/
	if(CheckIPNetmask(dwAddr, dwNetmask, dwGateWay)) {
		tmpTable.sysPara.dwAddr = dwAddr;
		tmpTable.sysPara.dwGateWay = dwGateWay;
		tmpTable.sysPara.dwNetMark = dwNetmask;
	} else {
		goto EXIT;
	}

	/*设备序列号*/
	ret =  ConfigGetKey(config_file, "sysParam", "name", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key name failed\n");
		goto EXIT;
	}

	strcpy(tmpTable.sysPara.strName, temp);

	/*版本信息*/
	ret =  ConfigGetKey(config_file, "sysParam", "version", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key version failed\n");
		goto EXIT;
	}

	strcpy(tmpTable.sysPara.strVer, temp);

	/*类型标识*/
	ret =  ConfigGetKey(config_file, "sysParam", "type", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key type failed\n");
		goto EXIT;
	}

	tmpTable.sysPara.bType = 0x4;

	/*视频参数*/
	ret =  ConfigGetKey(config_file, "videoParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key codec failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].nDataCodec = strtol(temp, 0, 16);

	/*帧率*/
	ret =  ConfigGetKey(config_file, "videoParam", "framerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key framerate failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].nFrameRate = atol(temp);
	/*视频宽高*/
	ret =  ConfigGetKey(config_file, "videoParam", "width", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key width failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].nWidth = atol(temp);
	ret =  ConfigGetKey(config_file, "videoParam", "height", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key height failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].nHight = atol(temp);

	/*colors*/
	ret =  ConfigGetKey(config_file, "videoParam", "colors", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key colors failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].nColors = atol(temp);
	/*quality*/
	ret =  ConfigGetKey(config_file, "videoParam", "quality", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key quality failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].nQuality = atol(temp);

	/*CBR*/
	ret =  ConfigGetKey(config_file, "videoParam", "cbr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key cbr failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].sCbr = atoi(temp);
	/*bitrate*/
	ret =  ConfigGetKey(config_file, "videoParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key bitrate failed\n");
		goto EXIT;
	}

	tmpTable.videoPara[DSP1].sBitrate = atoi(temp);

	/*音频参数*/
	ret =  ConfigGetKey(config_file, "audioParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key codec failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].Codec = strtol(temp, 0, 16);

	/*samplerate*/
	ret =  ConfigGetKey(config_file, "audioParam", "samplerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key samplerate failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].SampleRate = atol(temp);

	/*bitrate*/
	ret =  ConfigGetKey(config_file, "audioParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key bitrate failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].BitRate = atol(temp);
	/*channel*/
	ret =  ConfigGetKey(config_file, "audioParam", "channel", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key channel failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].Channel = atol(temp);

	/*samplebit */
	ret =  ConfigGetKey(config_file, "audioParam", "samplebit", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key samplebit failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].SampleBit = atol(temp);

	/*Lvolume */
	ret =  ConfigGetKey(config_file, "audioParam", "Lvolume", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key Lvolume failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].LVolume = atoi(temp);

	/*Rvolume */
	ret =  ConfigGetKey(config_file, "audioParam", "Rvolume", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key Rvolume failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].RVolume = atoi(temp);
	/*inputMode */
	ret =  ConfigGetKey(config_file, "audioParam", "inputMode", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Get Key Rvolume failed\n");
		goto EXIT;
	}

	tmpTable.audioPara[DSP1].InputMode = atoi(temp);
	memcpy(tblParam, &tmpTable, sizeof(SavePTable));
EXIT:
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;
}
/*################################
## 保存参数表
##################################*/
int save_table_operate(char *config_file , SavePTable *pTbl)
{
	char  	temp[512];
	int 	ret = 0;
	struct 	in_addr	addr ;

	if(config_file == NULL) {
		DEBUG(DL_ERROR, "[SaveParamtable] filename is null\n");
		return -1;
	}


	//systemv2 版本 video相关参数保存在video里面
	pTbl->videoPara[DSP1].nFrameRate		= pTbl->sysPara.nFrame;
	pTbl->videoPara[DSP1].nColors			= pTbl->sysPara.nColors ;
	pTbl->videoPara[DSP1].nQuality		= pTbl->sysPara.nQuality ;
	pTbl->videoPara[DSP1].sCbr			= pTbl->sysPara.sCbr ;
	pTbl->videoPara[DSP1].sBitrate		= pTbl->sysPara.sBitrate ;


	/**********************system config******************************/
	/*MAC Address*/
	ret = JoinMacAddr(temp, pTbl->sysPara.szMacAddr, 6);

	if(ret < 0) {
		DEBUG(DL_ERROR, "[JoinMacAddr] mac addr error ret = %d\n", ret);
	}

	ret =  ConfigSetKey(config_file, "sysParam", "macaddr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key mac addr failed\n");
	}

#if 0
	/*IP address*/
	memcpy(&addr, &pTbl->sysPara.dwAddr, 4);
	strcpy(temp, inet_ntoa(addr));
	ret =  ConfigSetKey(config_file, "sysParam", "ipaddr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key ipaddr failed\n");
	}

	/*GateWay address*/
	memcpy(&addr, &pTbl->sysPara.dwGateWay, 4);
	strcpy(temp, inet_ntoa(addr));
	ret =  ConfigSetKey(config_file, "sysParam", "gateway", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key gateway failed\n");
	}

	/*NetMask*/
	memcpy(&addr, &pTbl->sysPara.dwNetMark, 4);
	strcpy(temp, inet_ntoa(addr));
	ret =  ConfigSetKey(config_file, "sysParam", "netmask", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key netmask failed\n");
	}

#endif
	strcpy(temp, pTbl->sysPara.strName);
	ret =  ConfigSetKey(config_file, "sysParam", "name", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key name failed\n");
	}

	/*Version*/
	strcpy(temp, pTbl->sysPara.strVer);
	ret =  ConfigSetKey(config_file, "sysParam", "version", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key version failed\n");
	}

	/*type*/
	sprintf(temp, "%d", 6);
	ret =  ConfigSetKey(config_file, "sysParam", "type", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key channels failed\n");
	}

	/*bvmode*/
	sprintf(temp, "%d", pTbl->sysPara.bVmode);
	ret =  ConfigSetKey(config_file, "sysParam", "bvmode", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key width failed\n");
	}


	/**********************video config******************************/
	/*data codec*/
	sprintf(temp, "%x", pTbl->videoPara[DSP1].nDataCodec);
	ret =  ConfigSetKey(config_file, "videoParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key codec failed\n");
	}

	/*frame rate */
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nFrameRate);
	ret =  ConfigSetKey(config_file, "videoParam", "framerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key framerate failed\n");
	}

	/*width*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nWidth);
	ret =  ConfigSetKey(config_file, "videoParam", "width", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key width failed\n");
	}

	/*heigth*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nHight);
	ret =  ConfigSetKey(config_file, "videoParam", "height", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key height failed\n");
	}

	/*colors*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nColors);
	ret =  ConfigSetKey(config_file, "videoParam", "colors", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key colors failed\n");
	}

	/*quality*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nQuality);
	ret =  ConfigSetKey(config_file, "videoParam", "quality", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key quality failed\n");
	}

	/*CBR*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].sCbr);
	ret =  ConfigSetKey(config_file, "videoParam", "cbr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key cbr failed\n");
	}

	/*bitrate*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].sBitrate);
	ret =  ConfigSetKey(config_file, "videoParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key bitrate failed\n");
	}

	/*******************low video config******************************/
#ifdef LOW_VIDEO
	/*data codec*/
	sprintf(temp, "%x", pTbl->lowvideoPara[DSP1].nDataCodec);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key codec failed\n");
	}

	//low video framerate 无作用
	/*frame rate */
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].nFrameRate);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "framerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key framerate failed\n");
	}

	/*width*/
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].nWidth);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "width", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key width failed\n");
	}

	/*heigth*/
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].nHight);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "height", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key height failed\n");
	}

	/*colors*/
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].nColors);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "colors", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key colors failed\n");
	}

	/*quality*/
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].nQuality);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "quality", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key quality failed\n");
	}

	/*CBR*/
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].sCbr);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "cbr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key cbr failed\n");
	}

	/*bitrate*/
	sprintf(temp, "%d", pTbl->lowvideoPara[DSP1].sBitrate);
	ret =  ConfigSetKey(config_file, "lowvideoParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key bitrate failed\n");
	}

#endif

	/**********************audio config******************************/
	sprintf(temp, "%x", pTbl->audioPara[DSP1].Codec);
	ret =  ConfigSetKey(config_file, "audioParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key codec failed\n");
	}

	/*samplerate*/
	sprintf(temp, "%x", pTbl->audioPara[DSP1].SampleRate);
	ret =  ConfigSetKey(config_file, "audioParam", "samplerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key samplerate failed\n");
	}

	/*bitrate*/
	sprintf(temp, "%d", pTbl->audioPara[DSP1].BitRate);
	ret =  ConfigSetKey(config_file, "audioParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key bitrate failed\n");
	}

	/*channel*/
	sprintf(temp, "%d", pTbl->audioPara[DSP1].Channel);
	ret =  ConfigSetKey(config_file, "audioParam", "channel", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key channel failed\n");
	}

	/*samplebit */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].SampleBit);
	ret =  ConfigSetKey(config_file, "audioParam", "samplebit", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key samplebit failed\n");
	}

	/*Lvolume */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].LVolume);
	ret =  ConfigSetKey(config_file, "audioParam", "Lvolume", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key Lvolume failed\n");
	}

	/*Rvolume */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].RVolume);
	ret =  ConfigSetKey(config_file, "audioParam", "Rvolume", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key Rvolume failed\n");
	}

	/*inputMode */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].InputMode);
	ret =  ConfigSetKey(config_file, "audioParam", "inputMode", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key Rvolume failed\n");
	}

Err:

	return 0;
}
/*save params to flash "config.dat"*/
int SaveParamsToFlash(int dsp)
{

	SavePTable 		*pTbl = &gSysParaT;
	char 			temp[512], config_file[20];
	int 			ret  = -1 ;
	FILE			*fp;
	struct in_addr	addr ;

	ret = CheckIPNetmask(pTbl->sysPara.dwAddr, pTbl->sysPara.dwNetMark, pTbl->sysPara.dwGateWay);

	if(ret == 0) {
		DEBUG(DL_ERROR, "IP addr no real!!!!\n");
		return 0;
	}

	pthread_mutex_lock(&gSetP_m.save_sys_m);
	fp = fopen(CONFIG_NAME, "w+");
	fwrite(pTbl, sizeof(gSysParaT), 1, fp);
	fclose(fp);
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;

	strcpy(config_file, CONFIG_NAME);
	/*MAC Address*/
	ret = JoinMacAddr(temp, pTbl->sysPara.szMacAddr, 6);

	if(ret < 0) {
		DEBUG(DL_ERROR, "[JoinMacAddr] mac addr error ret = %d\n", ret);
		goto EXIT;
	}

	DEBUG(DL_DEBUG, "write config = %s \n", config_file);
	ret =  ConfigSetKey(config_file, "sysParam", "macaddr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key mac addr failed\n");
		goto EXIT;
	}

	/*IP address*/
	memcpy(&addr, &pTbl->sysPara.dwAddr, 4);
	strcpy(temp, inet_ntoa(addr));
	ret =  ConfigSetKey(config_file, "sysParam", "ipaddr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key ipaddr failed\n");
		goto EXIT;
	}

	/*GateWay address*/
	memcpy(&addr, &pTbl->sysPara.dwGateWay, 4);
	strcpy(temp, inet_ntoa(addr));
	ret =  ConfigSetKey(config_file, "sysParam", "gateway", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key gateway failed\n");
		goto EXIT;
	}

	/*NetMask*/
	memcpy(&addr, &pTbl->sysPara.dwNetMark, 4);
	strcpy(temp, inet_ntoa(addr));
	ret =  ConfigSetKey(config_file, "sysParam", "netmask", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key netmask failed\n");
		goto EXIT;
	}

	/*Name*/
	strcpy(temp, pTbl->sysPara.strName);
	ret =  ConfigSetKey(config_file, "sysParam", "name", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key name failed\n");
		goto EXIT;
	}

	/*Version*/
	strcpy(temp, pTbl->sysPara.strVer);
	ret =  ConfigSetKey(config_file, "sysParam", "version", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key version failed\n");
		goto EXIT;
	}

	sprintf(temp, "%d", 6);
	/*type*/
	ret =  ConfigSetKey(config_file, "sysParam", "type", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key channels failed\n");
		goto EXIT;
	}

	/*video param */
	/*data codec*/
	sprintf(temp, "%x", pTbl->videoPara[DSP1].nDataCodec);
	ret =  ConfigSetKey(config_file, "videoParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key codec failed\n");
		goto EXIT;
	}

	/*frame rate */
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nFrameRate);
	ret =  ConfigSetKey(config_file, "videoParam", "framerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key framerate failed\n");
		goto EXIT;
	}

	/*width*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nWidth);
	ret =  ConfigSetKey(config_file, "videoParam", "width", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key width failed\n");
		goto EXIT;
	}

	/*heigth*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nHight);
	ret =  ConfigSetKey(config_file, "videoParam", "height", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key height failed\n");
		goto EXIT;
	}

	/*colors*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nColors);
	ret =  ConfigSetKey(config_file, "videoParam", "colors", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key colors failed\n");
		goto EXIT;
	}

	/*quality*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].nQuality);
	ret =  ConfigSetKey(config_file, "videoParam", "quality", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key quality failed\n");
		goto EXIT;
	}

	/*CBR*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].sCbr);
	ret =  ConfigSetKey(config_file, "videoParam", "cbr", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key cbr failed\n");
		goto EXIT;
	}

	/*bitrate*/
	sprintf(temp, "%d", pTbl->videoPara[DSP1].sBitrate);
	ret =  ConfigSetKey(config_file, "videoParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key bitrate failed\n");
		goto EXIT;
	}

	/*Audio Param*/
	sprintf(temp, "%x", pTbl->audioPara[DSP1].Codec);
	ret =  ConfigSetKey(config_file, "audioParam", "codec", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key codec failed\n");
		goto EXIT;
	}

	/*samplerate*/
	sprintf(temp, "%x", pTbl->audioPara[DSP1].SampleRate);
	ret =  ConfigSetKey(config_file, "audioParam", "samplerate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key samplerate failed\n");
		goto EXIT;
	}

	/*bitrate*/
	sprintf(temp, "%d", pTbl->audioPara[DSP1].BitRate);
	ret =  ConfigSetKey(config_file, "audioParam", "bitrate", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key bitrate failed\n");
		goto EXIT;
	}

	/*channel*/
	sprintf(temp, "%d", pTbl->audioPara[DSP1].Channel);
	ret =  ConfigSetKey(config_file, "audioParam", "channel", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key channel failed\n");
		goto EXIT;
	}

	/*samplebit */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].SampleBit);
	ret =  ConfigSetKey(config_file, "audioParam", "samplebit", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key samplebit failed\n");
		goto EXIT;
	}

	/*Lvolume */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].LVolume);
	ret =  ConfigSetKey(config_file, "audioParam", "Lvolume", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key Lvolume failed\n");
		goto EXIT;
	}

	/*Rvolume */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].RVolume);
	ret =  ConfigSetKey(config_file, "audioParam", "Rvolume", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key Rvolume failed\n");
		goto EXIT;
	}

	/*inputMode */
	sprintf(temp, "%d", pTbl->audioPara[DSP1].InputMode);
	ret =  ConfigSetKey(config_file, "audioParam", "inputMode", temp);

	if(ret != 0) {
		DEBUG(DL_ERROR, "Set Key Rvolume failed\n");
		goto EXIT;
	}

	DEBUG(DL_DEBUG, "SAVE 	param     ok!\n");

EXIT:
	pthread_mutex_unlock(&gSetP_m.save_sys_m);
	return 0;
}

#if 0
/*print*/
int PrintsysParamTable(SavePTable *tbl)
{
	int i;
	struct in_addr addr ;

	DEBUG(DL_DEBUG, "PrintsysParamTable()\n");

	for(i = 0; i < 6 ; i++) {
		DEBUG(DL_DEBUG, "%2X ", tbl->sysPara.szMacAddr[i]);
	}

	DEBUG(DL_DEBUG, "\n");
	memcpy(&addr, &tbl->sysPara.dwAddr, 4);
	DEBUG(DL_DEBUG, "IP: %s\n", inet_ntoa(addr));

	memcpy(&addr, &tbl->sysPara.dwGateWay, 4);
	DEBUG(DL_DEBUG, "GW: %s\n", inet_ntoa(addr));

	memcpy(&addr, &tbl->sysPara.dwNetMark, 4);
	DEBUG(DL_DEBUG, "NM: %s\n", inet_ntoa(addr));
	DEBUG(DL_DEBUG, "Name:%s\n", tbl->sysPara.strName);
	DEBUG(DL_DEBUG, "Ver:%s\n", tbl->sysPara.strVer);
	DEBUG(DL_DEBUG, "chl:%d\n", tbl->sysPara.unChannel);
	DEBUG(DL_DEBUG, "type:%d\n", tbl->sysPara.bType);

	DEBUG(DL_DEBUG, "\n\n");

	DEBUG(DL_DEBUG, "nDataCodec:%X\n", tbl->videoPara[DSP1].nDataCodec);
	DEBUG(DL_DEBUG, "nFrameRate:%d\n", tbl->videoPara[DSP1].nFrameRate);
	DEBUG(DL_DEBUG, "nWidth:%d\n", tbl->videoPara[DSP1].nWidth);
	DEBUG(DL_DEBUG, "nHight:%d\n", tbl->videoPara[DSP1].nHight);
	DEBUG(DL_DEBUG, "nColors:%d\n", tbl->videoPara[DSP1].nColors);
	DEBUG(DL_DEBUG, "nQuality:%d\n", tbl->videoPara[DSP1].nQuality);
	DEBUG(DL_DEBUG, "sCbr:%d\n", tbl->videoPara[DSP1].sCbr);
	DEBUG(DL_DEBUG, "sBitrate:%d\n", tbl->videoPara[DSP1].sBitrate);
	DEBUG(DL_DEBUG, "\n\n");
	DEBUG(DL_DEBUG, "Codec:%X\n", tbl->audioPara[DSP1].Codec);
	DEBUG(DL_DEBUG, "BitRate:%d\n", tbl->audioPara[DSP1].BitRate);
	DEBUG(DL_DEBUG, "Channel:%d\n", tbl->audioPara[DSP1].Channel);
	DEBUG(DL_DEBUG, "InputMode:%d\n", tbl->audioPara[DSP1].InputMode);
	DEBUG(DL_DEBUG, "LVolume:%d\n", tbl->audioPara[DSP1].LVolume);
	DEBUG(DL_DEBUG, "RVolume:%d\n", tbl->audioPara[DSP1].RVolume);
	DEBUG(DL_DEBUG, "SampleBit:%d\n", tbl->audioPara[DSP1].SampleBit);
	DEBUG(DL_DEBUG, "SampleRate:%d\n", tbl->audioPara[DSP1].SampleRate);

}
#endif

/*
 * in_cksum --
 *	Checksum routine for Internet Protocol family headers (C Version)
 */
int in_cksum(addr, len)
u_short *addr;
int len;
{
	register int nleft = len;
	register u_short *w = addr;
	register int sum = 0;
	u_short answer = 0;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while(nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if(nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return(answer);
}


/*Send ICMP message to router*/
void InitICMP(void)
{
	struct protoent *proto;
	int hold = 0;

	if(!(proto = getprotobyname("icmp"))) {
		DEBUG(DL_ERROR, "unknown protocol icmp.\n");
		return ;
	}

	if((g_sockfd = socket(AF_INET, SOCK_RAW, proto->p_proto)) < 0) {
		DEBUG(DL_ERROR,"ping: socket");
		return ;
	}

	hold = 48 * 1024;
	setsockopt(g_sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&hold, sizeof(hold));
	return ;
}

/*close*/
int cleanICMP(void)
{
	close(g_sockfd);
	return 0;
}

void SendICMPmessage(void)
{
	register struct icmp *icp;
	int ntransmitted = 0 , ident;
	int mx_dup_ck = MAX_DUP_CHK;
	int datalen = DEFDATALEN;
	register int cc;
	char ipaddr[20] = "192.168.22.22";
	struct sockaddr_in to;

	bzero((char *)&to, sizeof(struct sockaddr));
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = inet_addr(ipaddr);
	ident = getpid() & 0xFFFF;

	icp = (struct icmp *)outpack;
	icp->icmp_type = ICMP_ECHO;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = ntransmitted++;
	icp->icmp_id = ident;				/* ID */
	CLR(icp->icmp_seq % mx_dup_ck);
	gettimeofday((struct timeval *)&outpack[8], (struct timezone *)NULL);

	cc = datalen + 8;			/* skips ICMP portion */

	/* compute ICMP checksum here */
	icp->icmp_cksum = in_cksum((u_short *)icp, cc);
	sendto(g_sockfd, (char *)outpack, cc, 0, &to, sizeof(struct sockaddr_in));
	DEBUG(DL_DEBUG, "ICMP message!!!!!\n");
	return 0;
}


/*set ethernet ip address*/
void SetEthConfigIP(unsigned int ipaddr, unsigned netmask)
{
	int ret;
	struct in_addr addr1, addr2;
	char temp[200];

	memset(temp, 0, sizeof(temp));
	memcpy(&addr1, &ipaddr, 4);
	memcpy(&addr2, &netmask, 4);
	strcpy(temp, "ifconfig eth0 ");
	strcat(temp, inet_ntoa(addr1));
	strcat(temp, " netmask ");
	strcat(temp, inet_ntoa(addr2));
	DEBUG(DL_DEBUG,"command:%s\n", temp);
	ret = system(temp);

	if(ret < 0) {
		DEBUG(DL_ERROR, "ifconfig %s up ERROR\n", inet_ntoa(addr1));
	}
}

/*set ethernet gatewall*/
void SetEthConfigGW(unsigned int gw)
{
	char temp[200];
	struct in_addr addr1;
	int ret;

	memset(temp, 0, sizeof(temp));
	memcpy(&addr1, &gw, 4);
	strcpy(temp, "route add default gw ");
	strcat(temp, inet_ntoa(addr1));
	DEBUG(DL_DEBUG, "command:%s\n", temp);

	ret = system(temp);

	if(ret < 0) {
		DEBUG(DL_ERROR, "SetEthConfigGW() command:%s ERROR\n", temp);
	}

}

/*set Mac Address*/
int SetMacAddr(int fd)
{
	char mac_addr[20];

	//GetMACAddr(fd, mac_addr);
	DEBUG(DL_DEBUG, "MAC addr = %s\n", mac_addr);
	SplitMacAddr(mac_addr, gSysParaT.sysPara.szMacAddr, 6);
	return 0;
}




/*modify standard system param*/
static void InitStandardParam(SysDSPParamsV2 *std)
{
	SysDSPParamsV2 *v2p;

	memset(&gSysParaT.sysPara, 0, sizeof(gSysParaT.sysPara));
	//	p = &gSysParaT.sysPara;
	v2p = &gSysParaT.sysPara;
	strcpy(v2p->strCodeVer, BOX_VER);

	gSysParaT.sysPara.dwAddr = inet_addr("192.168.2.137");
	gSysParaT.sysPara.dwGateWay = inet_addr("192.168.0.1");
	gSysParaT.sysPara.dwNetMark = inet_addr("255.255.0.0");

	gSysParaT.sysPara.szMacAddr[0] = 0x00;
	gSysParaT.sysPara.szMacAddr[1] = 0x09;
	gSysParaT.sysPara.szMacAddr[2] = 0x30;
	gSysParaT.sysPara.szMacAddr[3] = 0x28;
	gSysParaT.sysPara.szMacAddr[4] = 0x22;
	gSysParaT.sysPara.szMacAddr[5] = 0x22;

	strcpy(gSysParaT.sysPara.strName, "KT-8");
	strcpy(gSysParaT.sysPara.strAdminPassword, "123");
	strcpy(gSysParaT.sysPara.strUserPassword, "123");
	strcpy(gSysParaT.sysPara.strVer, BOX_VER);

	gSysParaT.sysPara.nFrame = 30;
	gSysParaT.sysPara.nQuality = 45;
	gSysParaT.sysPara.nColors = 24;
	gSysParaT.sysPara.sCodeType = 0x0f; //0x0f ----H264    0--- RCH0
	gSysParaT.sysPara.sCbr = 0;
	gSysParaT.sysPara.sBitrate = 2048;
	gSysParaT.sysPara.bVmode = 0; //   0  ----- D1   1-----CIF  2----QCIF
	gSysParaT.sysPara.bType = 0x4; //    0x0 ----vgabox

}

/*Initial video param table*/
static void InitVideoParam(VideoParam *video)
{
	//全初始化为零
	bzero(video, sizeof(VideoParam));
	video->nDataCodec = 0x34363248;	//"H264"
	video->nWidth = 800;				//视频宽EncoderManage 0---720*480 1---CIF 2 ---QCIF
	video->nHight = 600;				//视频高
	video->nQuality = 45;
	video->sCbr = 0;
	video->nFrameRate = 30;			//控制帧率

}

/*Initial audio param table*/
static void InitAudioParam(AudioParam *audio)
{
	//全初始化为零
	bzero(audio, sizeof(AudioParam));
	audio->Codec = ADTS;		//"ADTS"
	///0---8KHz
	///2---44.1KHz 音频采样率 (默认为44100)
	///1---32KHz
	///3---48KHz
	///其他-----96KHz
	audio->SampleRate = 2;
	audio->Channel = 2;					//声道数 (默认为2)
	audio->SampleBit = 16;				//采样位 (默认为16)
	audio->BitRate = 64000;				//编码带宽 (默认为64000)
	audio->InputMode = LINE_INPUT;		//线性输入
	audio->RVolume = 23;					//左右声道音量
	audio->LVolume = 23;
}
/*PPT检测处理*/
void cameraprotocol(int socket, int nPos, unsigned char *data, int len)
{
	int ret = 0, length = 0;
	char index;
	unsigned char temp[6];
	index = g_far_ctrl_index;

	if(0 == len) {
		/*回复消息*/
		length = 3 + 1;
		temp[0] = 0;
		temp[1] = length;
		temp[2] = MSG_CAMERACTRL_PROTOCOL;
		temp[3] = index;
		ret = WriteData(socket, temp, length);
		return;
	}

	index = data[0];

	if(index >= 0 && index < MAX_FAR_CTRL_NUM) {
		g_far_ctrl_index = index;
	}

	length = 3 + 1;
	temp[0] = 0;
	temp[1] = length;
	temp[2] = MSG_CAMERACTRL_PROTOCOL;
	temp[3] = g_far_ctrl_index;
	ret = WriteData(socket, temp, length);
	SaveRemoteCtrlIndex();
	DEBUG(DL_DEBUG, "data[0]=%d,g_far_ctrl_index= %d\n", data[0], g_far_ctrl_index);
	sleep(1);
	system("reboot -f");
	return;
}
/*初始化默认系统参数*/
void InitSysParams()
{
	int dsp;

	/*标准参数*/
	InitStandardParam(&gSysParaT.sysPara);

	for(dsp = 0; dsp < DSP_NUM ; dsp++) {

		/*视频参数*/
		InitVideoParam(&gSysParaT.videoPara[dsp]);

		/*音频参数*/
		InitAudioParam(&gSysParaT.audioPara[dsp]);
	}

	return;
}

/*get control frame rate value*/
unsigned int GetCtrlFrame()
{
	return(gSysParaT.sysPara.nFrame);
}

/*get control frame rate value*/
unsigned int GetCtrlFrame2()
{
	return(gSysParaT2.lowvideoPara[DSP1].nFrameRate);
}
/*set h264 width*/
int SetVGAH264Width(int width)
{

	gSysParaT.videoPara[DSP1].nWidth = width;
	return (gSysParaT.videoPara[DSP1].nWidth);
}

/*set h264 height*/
int SetVGAH264Height(int height)
{
	gSysParaT.videoPara[DSP1].nHight = height;
	return (gSysParaT.videoPara[DSP1].nHight);
}

/*get encode width*/
unsigned int VGAH264Width(void)
{
	return (gSysParaT.videoPara[DSP1].nWidth);
}

unsigned int VGAH264Height(void)
{
	return (gSysParaT.videoPara[DSP1].nHight);
}



/*Save encode param table*/


/*set Recv  timeout*/
int SetRecvTimeOut(SOCKET sSocket, unsigned long time)
{
	struct timeval timeout ;
	int ret = 0;

	timeout.tv_sec = time ; //3
	timeout.tv_usec = 0;

	ret = setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	if(ret == -1) {
		DEBUG(DL_ERROR, "setsockopt() Set Send Time Failed\n");
	}

	return ret;
}



/*set send timeout*/
/*
int SetSendTimeOut(SOCKET sSocket, unsigned long time)
{
	struct timeval timeout ;
	int ret = 0;

	timeout.tv_sec = time ; //3
	timeout.tv_usec = 0;

	ret = setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	if(ret == -1) {
		DEBUG(DL_ERROR, "setsockopt() Set Send Time Failed\n");
	}

	return ret;
}
*/

static unsigned long writen(int fd, const void *vptr, size_t n)
{
	unsigned long nleft;
	unsigned long nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;

	while(nleft > 0) {
		if((nwritten = write(fd, ptr, nleft)) <= 0) {
			if(nwritten < 0 && errno == EINTR) {
				nwritten = 0;    
			} else {
				return(-1);    
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return(n);
}
/* end writen */

/*send data to tty com*/
int SendDataToCom(int fd, unsigned char *data, int len)
{
	unsigned long real_len = 0 ;
	//DEBUG(DL_DEBUG, "SendDataToCom data=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9]);

	if((real_len = writen(fd, data, len)) != len) {
		DEBUG(DL_ERROR, "SendDataToCom() write tty error\n");
		return -1;
	}

	usleep(20000);
	return (real_len);
}

#ifdef SUPPORT_TRACK
/*send data to tty com*/
int SendDataToComNoDelay(int fd, unsigned char *data, int len)
{
	unsigned long real_len = 0 ;

	if((real_len = writen(fd, data, len)) != len) {
		DEBUG(DL_ERROR, "SendDataToCom() write tty error\n");
		return -1;
	}

	return (real_len);
}
#endif


/*
##
## send date to socket runtime
##
##
*/
/*
int WriteData(int s, void *pBuf, int nSize)
{
	int nWriteLen = 0;
	int nRet = 0;
	int nCount = 0;
	pthread_mutex_lock(&gSetP_m.send_m);

	while(nWriteLen < nSize) {
		nRet = send(s, (char *)pBuf + nWriteLen, nSize - nWriteLen, 0);

		if(nRet < 0) {
			if((errno == ENOBUFS) && (nCount < 10)) {
				DEBUG(DL_ERROR, "network buffer have been full!\n");
				usleep(10000);
				nCount++;
				continue;
			}

			pthread_mutex_unlock(&gSetP_m.send_m);
			return nRet;
		} else if(nRet == 0) {
			DEBUG(DL_ERROR, "Send Net Data Error nRet= %d\n", nRet);
			continue;
		}

		nWriteLen += nRet;
		nCount = 0;
	}

	pthread_mutex_unlock(&gSetP_m.send_m);
	return nWriteLen;
}
*/
#if 0
/*set video header infomation*/
static void SetVideoHeader(FRAMEHEAD *pVideo, VideoParam *pSys)
{
	pVideo->nColors = pSys->nColors;
}
#endif

//----------------------创建组播SOCKET---------------------------
//**************ts*****************
int SendMultCastDataToVlc(char *pData, int length)
{
	if(sendto(g_UdpTSSocket[TS_STREAM], pData, length, 0, (struct sockaddr *) &g_UdpTSAddr[TS_STREAM], sizeof(struct sockaddr_in)) < 0) {
		DEBUG(DL_ERROR, "sendto error!\n");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------
//设置音频头
static void SetAudioHeader(DSPDataHeader *pAudio, AudioParam *pSys)
{
	pAudio->fccCompressedHandler = 0x53544441 ; //ADTS;
	pAudio->biWidth = pSys->SampleRate; 			//   1--44.1KHz 音频采样率 (默认为44100)
	pAudio->biBitCount = pSys->Channel;					//声道数 (默认为2)
	pAudio->fccHandler = pSys->SampleBit;				//采样位 (默认为16)
	pAudio->biHeight = pSys->BitRate;			//sample rate 1-----44.1KHz

}



void SendAudioToClient(int nLen, unsigned char *pData,
                       int nFlag, unsigned char index, unsigned int samplerate)
{
	int nRet, nSent, nSendLen, nPacketCount, nMaxDataLen;
	DSPDataHeader  DataFrame;

#ifdef HAVE_RTSP_MODULE
	DataHeader_110E DataFrame2;
#endif

	int cnt = 0;

	pthread_mutex_lock(&gSetP_m.send_audio_m);

	bzero(&DataFrame, sizeof(DSPDataHeader));
#ifdef HAVE_RTSP_MODULE
	memset(&DataFrame2, 0, sizeof(DataFrame2));
#endif
	DataFrame.dwPacketNumber = gnAudioCount++;

	nSent = 0;
	nSendLen = 0;
	nPacketCount = 0;
	nMaxDataLen = MAX_PACKET - sizeof(DSPDataHeader) - HEAD_LEN;


	if(nFlag == 1) {
		DataFrame.dwFlags = AVIIF_KEYFRAME;
	} else {
		DataFrame.dwFlags = 0;
	}

	while(nSent < nLen) {

		if(nLen - nSent > nMaxDataLen) {
			nSendLen = nMaxDataLen;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 2;
			} else {
				DataFrame.dwSegment = 0;
			}

			nPacketCount++;
		} else {
			nSendLen = nLen - nSent;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 3;
			} else {
				DataFrame.dwSegment = 1;
			}

			nPacketCount++;
		}

		SetAudioHeader(&DataFrame, &gSysParaT.audioPara[DSP1]);
		DataFrame.dwCompressedDataLength = nLen;
		memcpy(gszAudioBuf + HEAD_LEN, &DataFrame, sizeof(DSPDataHeader));
		memcpy(gszAudioBuf + sizeof(DSPDataHeader) + HEAD_LEN, pData + nSent, nSendLen);
		gszAudioBuf[0] = (sizeof(DSPDataHeader) + nSendLen + 3) >> 8;
		gszAudioBuf[1] = (sizeof(DSPDataHeader) + nSendLen + 3) & 0xff;
		gszAudioBuf[2] = MSG_AUDIODATA;
#ifdef HAVE_RTSP_MODULE
		DataFrame2.id = DataFrame.id;

		DataFrame2.dwCompressedDataLength = DataFrame.dwCompressedDataLength;

		DataFrame2.biBitCount = DataFrame.biBitCount;  				//颜色数

		DataFrame2.biWidth = DataFrame.biWidth;		//视频宽
		DataFrame2.biHeight = DataFrame.biHeight; 	//视频高

		DataFrame2.dwFlags = DataFrame.dwFlags;

		DataFrame2.fccCompressedHandler = DataFrame.fccCompressedHandler;//0x30484352; H264
		DataFrame2.fccHandler = DataFrame.fccHandler;//0x30484352;  H264
		DataFrame2.dwSegment  = DataFrame.dwSegment ;
		DataFrame2.dwPacketNumber = DataFrame.dwPacketNumber;

		if(GetMultStatus(RTSP_STREAM)) {
			rtsp_stream_rebuild_audio_frame_110e(gszAudioBuf + HEAD_LEN, nSendLen + sizeof(DSPDataHeader), 0, &DataFrame2);
		}

#endif

		for(cnt = 0; cnt < MAX_CLIENT; cnt++) {
			if(ISUSED(index, cnt) && ISLOGIN(index, cnt)) {
				pthread_mutex_lock(&gSetP_m.audio_video_m);
				nRet = WriteData(GETSOCK(index, cnt), gszAudioBuf, nSendLen + sizeof(DSPDataHeader) + HEAD_LEN);

				if(nRet < 0) {
					SETCLIUSED(index, cnt, FALSE);
					SETCLILOGIN(index, cnt, FALSE);
					SETLOWSTREAM(DSP1, cnt, FALSE);
					//SETFARCTRLLOCK(index, cnt, 0);
					writeWatchDog();
					fprintf(stderr, "Audio Send error index:%d, cnt:%d %d nSendLen:%d  ,nRet:%d\n", index, cnt, errno, nSendLen, nRet);
				}

				pthread_mutex_unlock(&gSetP_m.audio_video_m);
			}
		}

		nSent += nSendLen;
	}

	pthread_mutex_unlock(&gSetP_m.send_audio_m);
}
#if 0
void SendADTSAudio(unsigned char *pData, unsigned int dwSize, int nUseLen, int index)
{
	int nLen = 0;
	nLen |= ((pData[3] & 0x03) << 11);
	nLen |= (pData[4] << 3);
	nLen |= ((pData[5] & 0xE0) >> 5);
	SendAudioToClient(nLen, pData, 0, 0);

	if(nUseLen + nLen < dwSize) {
		SendADTSAudio(pData + nLen, dwSize, nUseLen + nLen, index);
	}
}
#endif




/*send encode data to every client*/
/*
void SendDataToClient(int nLen, unsigned char *pData,
                      int nFlag, int width, int height, unsigned char index)
{
	int nRet, nSent, nSendLen, nPacketCount, nMaxDataLen;
	DataHeader  DataFrame;
#ifdef HAVE_RTSP_MODULE
	DataHeader_110E DataFrame2;
#endif
	int cnt = 0;
	SOCKET  sendsocket = 0;

	pthread_mutex_lock(&gSetP_m.net_send_m);
	bzero(&DataFrame, sizeof(DataHeader));
#ifdef HAVE_RTSP_MODULE
	memset(&DataFrame2, 0, sizeof(DataFrame2));
#endif
	nSent = 0;
	nSendLen = 0;
	nPacketCount = 0;
	nMaxDataLen = MAX_PACKET - sizeof(DataHeader) - HEAD_LEN;
	DataFrame.id = 0x1001;

	DataFrame.dwCompressedDataLength = nLen;

	DataFrame.biBitCount = 24;  				//颜色数

	DataFrame.biWidth = width;		//视频宽
	DataFrame.biHeight = height; 	//视频高

	if(nFlag == 1)	{	//if I frame
		DataFrame.dwFlags = AVIIF_KEYFRAME;
	} else {
		DataFrame.dwFlags = 0;
	}

	DataFrame.fccCompressedHandler = 0x34363248;//0x30484352; H264
	DataFrame.fccHandler = 0x34363248;//0x30484352;  H264

#ifdef HAVE_RTSP_MODULE
	DataFrame2.id = DataFrame.id;

	DataFrame2.dwCompressedDataLength = DataFrame.dwCompressedDataLength;

	DataFrame2.biBitCount = DataFrame.biBitCount;  				//颜色数

	DataFrame2.biWidth = DataFrame.biWidth;		//视频宽
	DataFrame2.biHeight = DataFrame.biHeight; 	//视频高

	DataFrame2.dwFlags = DataFrame.dwFlags;

	DataFrame2.fccCompressedHandler = DataFrame.fccCompressedHandler;//0x30484352; H264
	DataFrame2.fccHandler = DataFrame.fccHandler;//0x30484352;  H264
#endif


	while(nSent < nLen) {
		if(nLen - nSent > nMaxDataLen) {
			nSendLen = nMaxDataLen;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 2;    //start frame
			} else {
				DataFrame.dwSegment = 0;    //middle frame
			}

			nPacketCount++;
		} else {
			nSendLen = nLen - nSent;

			if(nPacketCount == 0) {
				DataFrame.dwSegment = 3;    //first frame and last frame
			} else {
				DataFrame.dwSegment = 1;    //last frame
			}

			nPacketCount++;
		}

		DataFrame.dwPacketNumber = gnSentCount++;
		memcpy(gszSendBuf + HEAD_LEN, &DataFrame, sizeof(DataHeader));
		memcpy(gszSendBuf + sizeof(DataHeader) + HEAD_LEN, pData + nSent, nSendLen);
		gszSendBuf[0] = (nSendLen + sizeof(DataHeader) + HEAD_LEN) >> 8;
		gszSendBuf[1] = (nSendLen + sizeof(DataHeader) + HEAD_LEN) & 0xff;
		gszSendBuf[2] = MSG_SCREENDATA;
#ifdef HAVE_RTSP_MODULE
		DataFrame2.dwSegment  = DataFrame.dwSegment ;
		DataFrame2.dwPacketNumber = DataFrame.dwPacketNumber;

		//send rtsp client
		if(GetMultStatus(RTSP_STREAM)) {
			rtsp_stream_rebuild_video_frame_110e(gszSendBuf + HEAD_LEN, nSendLen + sizeof(DataHeader), 0, (void *)&DataFrame2);
		}

#endif

		//send multi client
		for(cnt = 0; cnt < MAX_CLIENT; cnt++) {
			if(ISUSED(index, cnt) && ISLOGIN(index, cnt) && !GETLOWSTREAM(index, cnt)) {
				pthread_mutex_lock(&gSetP_m.audio_video_m);
				sendsocket = GETSOCK(index, cnt);

				if(sendsocket > 0)  {
					nRet = WriteData(sendsocket, gszSendBuf, nSendLen + sizeof(DataHeader) + HEAD_LEN);

					if(nRet < 0) {
						SETCLIUSED(index, cnt, FALSE);
						SETCLILOGIN(index, cnt, FALSE);
						SETLOWSTREAM(DSP1, cnt, FALSE);
						//SETFARCTRLLOCK(index, cnt, 0);
						writeWatchDog();
						DEBUG(DL_ERROR, "Send to socket Error: SOCK = %d count = %d  errno = %d  ret = %d\n", sendsocket, cnt, errno, nRet);
					}
				}

				pthread_mutex_unlock(&gSetP_m.audio_video_m);
			}
		}

		nSent += nSendLen;
	}

	pthread_mutex_unlock(&gSetP_m.net_send_m);
}
*/
/*Print current avaliable users*/
void PrintClientNum()
{
	int i;

	for(i = 0 ; i < MAX_CLIENT; i++) {
		if(ISUSED(DSP1, i) && ISLOGIN(DSP1, i)) {
			DEBUG(DL_DEBUG, "====================\n");
			DEBUG(DL_DEBUG, "Client[%d]sock=%d\n", i, GETSOCK(DSP1, i));
			DEBUG(DL_DEBUG, "====================\n");
		}
	}
}
/*find encode box*/
void DSP1UDPTask(void *pParam)
{
	SOCKET sSocket;
	struct sockaddr_in SrvAddr ;
	WhoIsMsg theMsg;
	int nLen, size;

	struct timeval tv;
	pthread_detach(pthread_self());

	//signal_set();

	bzero(&SrvAddr, sizeof(struct sockaddr_in));
	SrvAddr.sin_family = AF_INET;
	SrvAddr.sin_port = htons(11115);
	SrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sSocket < 0) {
		DEBUG(DL_ERROR, "DSP1UDPTask create error:%d\n", errno);
		goto WhoIsExit;
	}

	if(bind(sSocket, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr)) < 0) {
		DEBUG(DL_ERROR, "bind terror:%d\n", errno);
		goto WhoIsExit;
	}

	nLen = 1;
	tv.tv_sec = 1;
	tv.tv_usec = 10000;
	setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if(setsockopt(sSocket, SOL_SOCKET, SO_BROADCAST, (void *)&nLen, sizeof(nLen))) {
		DEBUG(DL_ERROR, "setsockopt SO_BROADCAST error:%d\n", errno);
		goto WhoIsExit;
	}

	DEBUG(DL_DEBUG, "who is task is ready!\n");

	while(1) {
		memset(&theMsg, 0, sizeof(theMsg));
		size = sizeof(SrvAddr);
		nLen = recvfrom(sSocket, &theMsg, sizeof(theMsg), 0, (struct sockaddr *)&SrvAddr, (socklen_t *)&size);

		if(nLen < 0)  {
			//DEBUG(DL_DEBUG, "recvfrom() Error Len:%d  errno:%d\n ",nLen,errno);
			continue;
		}

		if((theMsg.szFlag[0] == 0x7e)
		   && (theMsg.szFlag[1] == 0x7e)
		   && (theMsg.szFlag[2] == 0x7e)
		   && (theMsg.szFlag[3] == 0x7e)) {
			memcpy(theMsg.szFlag + 4, &gSysParaT.sysPara, sizeof(theMsg) - 4);
			theMsg.nType = gSysParaT.sysPara.bType;  //6---ENC1200  8--ENC-1100
			theMsg.szFlag[0] = 0x7e;
			theMsg.szFlag[1] = 0x7e;
			theMsg.szFlag[2] = 0x7e;
			theMsg.szFlag[3] = 0x7e;
			strcpy(theMsg.strVer, gSysParaT.sysPara.strVer);
			strcpy(theMsg.strName, gSysParaT.sysPara.strName);
			sendto(sSocket, &theMsg, sizeof(theMsg), 0, (struct sockaddr *)&SrvAddr, sizeof(SrvAddr));
			DEBUG(DL_DEBUG, "sendto\n");
		}
	}

WhoIsExit:
	DEBUG(DL_DEBUG, "DSP1UDPTask \n");
	close(sSocket);
}


