#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
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

#define MAX_CHANNELS 16

struct sCh {
  int load_state;
} Ch[MAX_CHANNELS];

static int parse_set(char *message)
{
        char *str;
        char *strp;
	int status;
	char outmsg[64];
	int ch;
        int level;

        for (str=strtok_r(message," ", &strp); str; str=strtok_r(NULL, " ", &strp)) {
		if (strstr(str, "channel")) {
			status=sscanf(str, "channel=%d", &ch);
			continue;
		}
                if (strstr(str, "level")) {
                        sscanf(str, "level=%d", &level);
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
		/* enable mosfet */
		printf("mosfet %d on\n", ch);
	} else {
		/* disable mosfet */
		printf("mosfet %d off\n", ch);
	}

	/* publish status */
	status=sprintf(outmsg, "/load/switch/status channel=%d level=%d", ch, level);
	siot_transport_send(outmsg, status+1);
}

static void *thread_rx(void *arg)
{
	int status;
	char *message;

	while (1) {
		status = siot_transport_recv(&message);

		if (strstr(message, "/load/switch/set")) {
			parse_set(message);
			continue;
		}

		free(message);
	}
}

int main(int argc, char* argv[])
{
	int status;
	size_t privkey_size = (size_t)(&_binary_target_priv_pem_end-&_binary_target_priv_pem_start);
	size_t pubkey_size = (size_t)(&_binary_target_pub_pem_end-&_binary_target_pub_pem_start);

	siot_transport_init(MC_IP, MC_PORT);
	siot_security_init(&_binary_target_priv_pem_start, privkey_size, &_binary_target_pub_pem_start, pubkey_size);

	status = pthread_create(&tid_rx, NULL, thread_rx, NULL);

	while(1) {
		sleep(10);
	}
}
