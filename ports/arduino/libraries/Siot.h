#ifndef __SIOT_H__
#define __SIOT_H__

#include <Arduino.h>

//#define SECURE

#ifdef ESP8266
#include <ESP8266WiFi.h>

extern "C" {
#ifdef SECURE
#include <mbedtls/config.h>

#define mbedtls_snprintf   Serial.snprintf
#define mbedtls_printf     Serial.printf

#include <mbedtls/error.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/sha256.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#endif

}
#endif
#include <WiFiUdp.h>
class Siot {
      public:
	Siot(void);
	void Init(const char *mcip, const int mcport);
	void Tx(const char *buf, const int len);
	void Rx(const int maxlen, char *buf, int *len);
#ifdef SECURE
	void InitSecurity(const char *privkey, const int privkey_len, const char *pubkey, const int pubkey_len);
	int simple_encrypt_blob(mbedtls_pk_context * pk, char *buf, int buf_len, char **buf_enc, size_t * buf_enc_len);
	int simple_decrypt_blob(mbedtls_pk_context * pk, char *buf, int buf_len, char **buf_dec, size_t * buf_dec_len);
#endif
      private:
	WiFiUDP _udp;
	IPAddress _ourIP;
	IPAddress _mcIP;
	int _mcPort;
#ifdef SECURE
	mbedtls_pk_context pkpriv;
	mbedtls_pk_context pkpub;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
#endif
};

#endif
