#ifndef _REACH_LIVE555_PORT_H
#define _REACH_LIVE555_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

extern int RtspLiveServerInit(int port, char *usrname, char*password);
extern void* RtspLiveServerCreateSeesion(void **handle,char *streamName);
extern int  RtspLiveServerDelteSeesion(void *handle);
extern int RtspLiveServerSendVideoFrame(void *handle, char *ptr, int len);
extern int RtspLiveServerSendAudioFrame(void *handle, char *ptr, int len);
extern int RtspLiveServerStart();
extern int StopRtspLiveServer(void *handle);
extern int  RtspLiveServerGetSeesionLiveStatus(void *handle);
#ifdef __cplusplus
} // extern "C"
#endif

#endif

