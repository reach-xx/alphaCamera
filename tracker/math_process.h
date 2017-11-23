#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>  


#include "sample_comm.h"
#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_comm_vgs.h"

#include "auto_track.h"
#include "rh_interface.h"
#include "rh_motion_detect.h"
#include "strategy.h"
#include "rh_broadcast.h"

#define TRACK_WIDTH	(1280)
#define TRACK_HEIGHT	(720)

int Reach_Track_start();

int Reach_Track_run(HI_U8* pVirAddr ,rect_t* target_rect);

void Reach_Track_stop();

