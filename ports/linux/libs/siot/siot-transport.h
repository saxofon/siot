#ifndef __SIOT_TRANSPORT_H__
#define __SIOT_TRANSPORT_H__

extern int siot_transport_init(char *ip, short port);
extern int siot_transport_send(char* buf, int len);
extern int siot_transport_recv(char** buf);

#endif
