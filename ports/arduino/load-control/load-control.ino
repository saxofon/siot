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
	int button_lastState;
	int button_currentState;
	int button_debouncedState;
	unsigned long button_lastDebounceTime;
} Ch[MAX_CHANNELS];

// the debounce time; increase if the state flickers
unsigned long debounceDelay = 50;

static void parse_status(char *message)
{
	char *str;
	char *strp;
	int ch;
	int level;

	for (str = strtok_r(message, " ", &strp); str; str = strtok_r(NULL, " ", &strp)) {
		if (strstr(str, "channel")) {
			sscanf(str, "channel=%d", &ch);
			if (ch >= MAX_CHANNELS) {
				return;
			}
			continue;
		}
		if (strstr(str, "level")) {
			sscanf(str, "level=%d", &level);
			continue;
		}
	}

	Ch[ch].load_state = level;

	if (level) {
		digitalWrite(LED_BUILTIN, LOW);
	} else {
		digitalWrite(LED_BUILTIN, HIGH);
	}
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
//  pinMode(2, INPUT);
}

void loop()
{
	char buf[512];
	int len;
	int ch = 0;

	// check for siot messages
	siot.Rx(sizeof(buf), buf, &len);
	if (len) {
		if (strstr(buf, "/load/switch/status")) {
			parse_status(buf);
		}
		len = 0;
		memset(buf, 512, 0);
	}
#if 0
	// check button
	Ch[ch].button_currentState = digitalRead(2);
	if (Ch[ch].button_currentState != Ch[ch].button_lastState) {
		Ch[ch].button_lastDebounceTime = millis();
	}
	if ((millis() - Ch[0].button_lastDebounceTime) > debounceDelay) {
		if (Ch[ch].button_currentState != Ch[ch].button_debouncedState) {
			Ch[ch].button_debouncedState = Ch[ch].button_currentState;

			// only toggle load if the new button debounced state is HIGH
			if (Ch[ch].button_debouncedState == HIGH) {
				len = sprintf(buf, "/load/switch/status channel=%d toggle", ch);
				siot.Tx(buf, len + 1);
			}
		}
	}
	Ch[ch].button_lastState = Ch[ch].button_currentState;
#endif
}
