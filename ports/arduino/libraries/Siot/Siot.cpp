#include "Siot.h"

Siot::Siot(void)
{
}

void Siot::Init(const IPAddress mcip, const int mcport)
{
	int status;

	_ourIP = WiFi.localIP();
	//Serial.printf("ourIP=");
	//Serial.println(_ourIP);

	_mcIP = mcip;
	_mcPort = mcport;
	//Serial.printf("mcIP=");
	//Serial.println(_mcIP);

	status = _udp.beginMulticast(_ourIP, _mcIP, _mcPort);
	//Serial.printf("beginMulticast : %d\n", status);
}

void Siot::Tx(const char *buf, const int len)
{
	int status;

#ifdef SECURE
	char *tmpbuf;
	size_t tmplen;
	tmpbuf = malloc(1024);
	status = simple_encrypt_blob(&pkpub, buf, len, &tmpbuf, &tmplen);
#endif

	status = _udp.beginPacketMulticast(_mcIP, _mcPort, _ourIP, 10);
	//Serial.printf("beginPacketMulticast : %d\n", status);
	status = _udp.write(buf, len);
	//Serial.printf("  write : %d\n", status);
	status = _udp.endPacket();
	//Serial.printf("  endPacket : %d\n", status);

	return;
}

void Siot::Rx(const int maxlen, char *buf, int *len)
{
	int status;

	status = _udp.parsePacket();
	if (status) {
		// not interested in non-siot packets
		if (_udp.destinationIP() != _mcIP) {
			*len = 0;
			return;
		}

		*len = _udp.read(buf, maxlen);
	} else {
		*len = 0;
	}

#ifdef SECURE
	int cnt;
	char *tmpbuf;
	size_t tmplen;
	status = simple_decrypt_blob(&pkpriv, tmpbuf, status, buf, &len);
#endif

	return;
}
