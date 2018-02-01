#include <ESP8266WiFi.h>

#define WIFIMGR
#ifdef WIFIMGR
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#endif

#include <Siot.h>

char ssid[] = "load-switch";
char pass[] = "password";

IPAddress mc_ip(239, 0, 0, 1);

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
		} else if (strstr(str, "level")) {
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
 
#ifdef WIFIMGR
  WiFiManager wifiManager;
  wifiManager.autoConnect(ssid, pass);
#else
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, pass);

	while ((status = WiFi.status()) != WL_CONNECTED) {
		delay(1000);
	}
#endif

	String APs = WiFi.BSSIDstr();
	char AP[80];

	APs.toCharArray(AP, sizeof(AP));

	Serial.printf("connected to AP %s, signal strength %d\n", AP, WiFi.RSSI());

	siot.Init(mc_ip, mc_port);

	pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, INPUT);
}

void loop()
{
	char buf[512];
	int len;
	int ch = 0;

	// check for siot messages
	siot.Rx(sizeof(buf), buf, &len);
	if (len) {
    Serial.printf("incoming msg\n");
		if (strstr(buf, "/load/switch/status")) {
			parse_status(buf);
		}
		len = 0;
		memset(buf, 0, sizeof(buf));
	}
#if 1
	// check button
	Ch[ch].button_currentState = digitalRead(2);
  Serial.printf("button momentary read says %d\n", Ch[ch].button_currentState);
	if (Ch[ch].button_currentState != Ch[ch].button_lastState) {
		Ch[ch].button_lastDebounceTime = millis();
	}
	if ((millis() - Ch[0].button_lastDebounceTime) > debounceDelay) {
		if (Ch[ch].button_currentState != Ch[ch].button_debouncedState) {
			Ch[ch].button_debouncedState = Ch[ch].button_currentState;

			// only toggle load if the new button debounced state is HIGH
			if (Ch[ch].button_debouncedState == HIGH) {
        Serial.printf("button pressed and debounced\n");
				len = sprintf(buf, "/load/switch/status channel=%d toggle", ch);
				siot.Tx(buf, len + 1);
			}
		}
	}
	Ch[ch].button_lastState = Ch[ch].button_currentState;
#endif
}
