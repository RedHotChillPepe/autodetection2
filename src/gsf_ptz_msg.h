#ifndef __GSF_PTZ_MSG__H
#define __GSF_PTZ_MSG__H

typedef struct _GSF_PTZ_NOTIFY_MSG_HDL
{
	int nExit;
	int nSock;
	void * User;
	PtzMsgCallbackFunc fCallback;
	pthread_t thid;
}GSF_PTZ_NOTIFY_MSG_HDL;

#define  GSF_PTZ_NOTIFY_MSG_PATH "/tmp/ptzmsg"




void *gsf_ptz_msg_task(void *);

int gsf_ptz_msg_stop(void *hdl);






#endif

