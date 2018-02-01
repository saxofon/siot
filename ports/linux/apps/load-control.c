#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#include "siot.h"
#include "load-switch.h"

#include "target_priv.h"
#include "target_pub.h"

static pthread_t tid_rx;
static pthread_t tid_tx;

#define MAX_CHANNELS 16

struct sCh {
  int load_state;
} Ch[MAX_CHANNELS];

static int parse_status(char *message)
{
	char *str;
	char *strp;
	int ch;
	int level;

	for (str=strtok_r(message," ", &strp); str; str=strtok_r(NULL, " ", &strp)) {
		if (strstr(str, "channel")) {
			sscanf(str, "channel=%d", &ch);
			continue;
		}
		if (strstr(str, "level")) {
			sscanf(str, "level=%d", &level);
			Ch[ch].load_state = level;
			continue;
		} else if (strstr(str, "toggle")) {
			printf("Toggling %d from ", ch, Ch[ch].load_state);
			Ch[ch].load_state = !(Ch[ch].load_state);
			printf("to %d\n", Ch[ch].load_state);
			level = Ch[ch].load_state;
			continue;
		} 
	}

	if (level) {
		printf("led %d on\n", ch);
	} else {
		printf("led %d off\n", ch);
	}
}

static void *thread_rx(void *arg)
{
	int status;
	char *message;

	while (1) {
		status = siot_transport_recv(&message);

		if (strstr(message, "/load/switch/status")) {
			parse_status(message);
		}

		free(message);
	}
}

/* This thread simulates a user pushing switch 1 button every 2 seconds.. */
static void *thread_tx(void *arg)
{
	int status;
	char *reply;
	size_t reply_len;
	char *leftovers;
	char message[50];
	int ch=0;
	int newch;


	while (1) {
		printf("[ nr sets channel, t toggles it, l/h sets it low/high] : ");
		reply = NULL;
		reply_len = 0;
		status=getline(&reply, &reply_len, stdin);
		if (status>0 && status<79) {
			reply[status]=0;
		}
		newch=strtol(reply, &leftovers, 10);
		if (reply != leftovers) {
			ch == newch;
		} else if (reply[0] == 't') {
			printf("requesting channel %d toggling\n", ch);
			status=sprintf(message, "/load/switch/set channel=%d toggle", ch);
			siot_transport_send(message, status+1);
		} else if (reply[0] == 'l') {
			printf("requesting channel %d set to low\n", ch);
			status=sprintf(message, "/load/switch/set channel=%d level=0", ch);
			siot_transport_send(message, status+1);
		} else if (reply[0] == 'h') {
			printf("requesting channel %d set to high\n", ch);
			status=sprintf(message, "/load/switch/set channel=%d level=1", ch);
			siot_transport_send(message, status+1);
		}
		free(reply);
	}
}

int main(int argc, char* argv[])
{
	int status;
#ifdef SECURE
	size_t privkey_size = (size_t)(&_binary_target_priv_pem_end-&_binary_target_priv_pem_start);
	size_t pubkey_size = (size_t)(&_binary_target_pub_pem_end-&_binary_target_pub_pem_start);
#endif

	siot_transport_init(MC_IP, MC_PORT);
#ifdef SECURE
	siot_security_init(&_binary_target_priv_pem_start, privkey_size, &_binary_target_pub_pem_start, pubkey_size);
#endif

	status = pthread_create(&tid_rx, NULL, thread_rx, NULL);
	status = pthread_create(&tid_tx, NULL, thread_tx, NULL);

	while(1) {
		sleep(10);
	}
}
