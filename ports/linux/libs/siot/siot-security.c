#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "siot.h"

mbedtls_pk_context pkpriv;
mbedtls_pk_context pkpub;

static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;

int siot_security_init(const char *privkey, const int privkey_len, const char *pubkey, const int pubkey_len)
{
	int status;

	debug("    initializing mbedtls\n");

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	status = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
	if (status) {
		printf(" failed to initialize crypto lib\n");
		return(-1);
	}

	mbedtls_pk_init(&pkpriv);
	status = mbedtls_pk_parse_key(&pkpriv, privkey, privkey_len, NULL, 0);
	if (status) {
		printf(" failed to load private key\n");
		return(-1);
	}
	
	debug("    parsing public key\n");
	mbedtls_pk_init(&pkpub);
	status = mbedtls_pk_parse_public_key(&pkpub, pubkey, pubkey_len);
	if (status) {
		printf(" failed to load public key\n");
		return(-1);
	}

	return(0);
}

int siot_security_cleanup(void)
{
	mbedtls_pk_free(&pkpriv);
	mbedtls_pk_free(&pkpub);
}

int siot_encrypt_blob(mbedtls_pk_context *pk, char *buf, int buf_len, char** buf_enc, size_t* buf_enc_len)
{
	int status;

	*buf_enc = malloc(buf_len+512);

	status = mbedtls_pk_encrypt(pk, buf, buf_len, *buf_enc, buf_enc_len, buf_len+512,
			mbedtls_ctr_drbg_random, &ctr_drbg);
	if (status) {
		perror("mbedtls_pk_encrypt");
		return(-1);
	}

	*buf_enc = realloc(*buf_enc, *buf_enc_len);

	return(0);
}

int siot_decrypt_blob(mbedtls_pk_context *pk, char *buf, int buf_len, char** buf_dec, size_t* buf_dec_len)
{
        int status;

        *buf_dec = malloc(2*buf_len);
        status = mbedtls_pk_decrypt(pk, buf, buf_len, *buf_dec, buf_dec_len, 2*buf_len,
                        mbedtls_ctr_drbg_random, &ctr_drbg);
        if (status) {
                perror("mbedtls_pk_decrypt");
                return(-1);
        }

	*buf_dec = realloc(*buf_dec, *buf_dec_len);

	return(0);
}

int siot_create_signature(mbedtls_pk_context* pk, char* buf, int buf_len, char** sig, size_t* sig_len)
{
	int status;
	unsigned char hash[32];

	mbedtls_sha256(buf, buf_len, hash, 0);

	status = mbedtls_pk_sign(pk, MBEDTLS_MD_SHA256, hash, 32, *sig, sig_len,
                         mbedtls_ctr_drbg_random, &ctr_drbg);
	if (status) {
		perror("mbedtls_pk_sign");
		return(-1);
	}

	return(0);
}

int siot_verify_signature(mbedtls_pk_context *pk, char *buf, int buf_len, char *sig, int sig_len)
{
	int status;
	unsigned char hash[32];

	mbedtls_sha256(buf, buf_len, hash, 0);

	status = mbedtls_pk_verify(pk, MBEDTLS_MD_SHA256, hash, 32, sig, sig_len);
	if (status) {
		printf(" error mbedtls_pk_verify()\n");
		return(-1);
	}

	return(0);
}
