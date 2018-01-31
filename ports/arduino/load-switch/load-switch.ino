#include <ESP8266WiFi.h>

#include <Siot.h>

char ssid[] = "ssid";
char pass[] = "password";

char mc_ip[4] = {239,0,0,1};
int mc_port = 2183;
Siot siot;

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
                }
        }

        if (level) {
                /* enable mosfet */
                digitalWrite(LED_BUILTIN, LOW);
        } else {
                /* disable mosfet */
                digitalWrite(LED_BUILTIN, HIGH);
        }

        /* publish status */
        status=sprintf(outmsg, "/load/switch/status channel=%d level=%d", ch, level);
        siot.Tx(outmsg, status+1);
}

void setup() {
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

void loop() {
  char buf[512];
  int len;
  
  // check for siot messages
  siot.Rx(sizeof(buf), buf, &len);
  if (len) {
    if (strstr(buf, "/load/switch/set")) {
      parse_set(buf);
    }
    len=0;
    memset(buf, 512, 0);
  }
}




