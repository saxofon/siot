#ifndef __SIOT_SECURITY_H__
#define __SIOT_SECURITY_H__

#include <mbedtls/config.h>

/* possibility to override mbedtls output */
#define mbedtls_snprintf   snprintf
#define mbedtls_printf     printf

#include <mbedtls/error.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/sha256.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

extern mbedtls_pk_context pkpriv;
extern mbedtls_pk_context pkpub;

extern int siot_security_init(const char *privkey, const int privkey_len, const char *pubkey, const int pubkey_len);
extern int siot_security_cleanup(void);
extern int siot_encrypt_blob(mbedtls_pk_context *pk, char *buf, int buf_len, char** buf_enc, size_t* buf_enc_len);
extern int siot_decrypt_blob(mbedtls_pk_context *pk, char *buf, int buf_len, char** buf_dec, size_t* buf_dec_len);
extern int siot_create_signature(mbedtls_pk_context* pk, char* buf, int buf_len, char** sig, size_t* sig_len);
extern int siot_verify_signature(mbedtls_pk_context *pk, char *buf, int buf_len, char *sig, int sig_len);

#endif
