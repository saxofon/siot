#include <ESP8266WiFi.h>

#include <Siot.h>

char ssid[] = "ssid";
char pass[] = "password";

char mc_ip[4] = { 239, 0, 0, 1 };

int mc_port = 2183;
Siot siot;

#define MAX_CHANNELS 16

struct sCh {
	int load_state;
} Ch[MAX_CHANNELS];

static void parse_set(char *message)
{
	char *str;
	char *strp;
	int status;
	char outmsg[64];
	int ch;
	int level;

	for (str = strtok_r(message, " ", &strp); str; str = strtok_r(NULL, " ", &strp)) {
		if (strstr(str, "channel")) {
			status = sscanf(str, "channel=%d", &ch);
			if (ch >= MAX_CHANNELS) {
				return;
			}
			continue;
		}
		if (strstr(str, "level")) {
			sscanf(str, "level=%d", &level);
			Ch[ch].load_state = level;
			continue;
		} else if (strstr(str, "toggle")) {
			Ch[ch].load_state = !(Ch[ch].load_state);
			level = Ch[ch].load_state;
			continue;
		}
	}

	if (level) {
		/* enable mosfet, simulate via onboard led */
		digitalWrite(LED_BUILTIN, LOW);
	} else {
		/* disable mosfet, simulate via onboard led */
		digitalWrite(LED_BUILTIN, HIGH);
	}

	/* publish status */
	status = sprintf(outmsg, "/load/switch/status channel=%d level=%d", ch, level);
	siot.Tx(outmsg, status + 1);
}

void setup()
{
	int status;

	Serial.begin(115200);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, pass);

	while ((status = WiFi.status()) != WL_CONNECTED) {
		delay(1000);
	}

	String APs = WiFi.BSSIDstr();
	char AP[80];

	APs.toCharArray(AP, sizeof(AP));

	Serial.printf("connected to AP %s, signal strength %d\n", AP, WiFi.RSSI());

	siot.Init(mc_ip, mc_port);

	pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
	char buf[512];
	int len;

	// check for siot messages
	siot.Rx(sizeof(buf), buf, &len);
	if (len) {
		if (strstr(buf, "/load/switch/set")) {
			parse_set(buf);
		}
		len = 0;
		memset(buf, 512, 0);
	}
}
