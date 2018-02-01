APPS += load-switch
APPS += load-control

KEYS += target_priv.pem
KEYS += target_pub.pem

DBG := -g -ggdb

GENKEY=/usr/libexec/mbedtls/gen_key

SIOT_ROOT := ../libs/siot

CFLAGS += $(DBG) -I$(SIOT_ROOT)
LDFLAGS += $(DBG) -lpthread
LDFLAGS += -L$(SIOT_ROOT) -lsiot
LDFLAGS += -lmbedcrypto
LDFLAGS += $(KEYS:.pem=.o)

SRCS += $(shell ls *.c)

all: $(KEYS:.pem=.o) $(APPS)
	@echo "Run as :"
	@echo "LD_LIBRARY_PATH=$(SIOT_ROOT) ./load-switch"
	@echo "LD_LIBRARY_PATH=$(SIOT_ROOT) ./load-control"

target_priv.pem:
	$(GENKEY) type=rsa rsa_keysize=4096 filename=$@

target_pub.pem: target_priv.pem
	openssl rsa -in $< -outform PEM -pubout -out $@

%.o:%.pem
	dd if=/dev/zero bs=1 count=1 | cat >> $<
	$(LD) -r -b binary $< -o $@
	nm $@ | grep -v size | awk '{ printf "extern const char %s;\n", $$3}' > $(@:.o=.h)
	nm $@ | grep size | awk '{ printf "extern const int %s;\n", $$3}' >> $(@:.o=.h)

%.d:%.c
	$(CC) $(CFLAGS) -MM -MG $< >$@

-include $(SRCS:.c=.d)

clean:
	$(RM) $(APPS) $(SRCS:.c=.d) $(SRCS:.c=.o) $(KEYS:.pem=.o) $(KEYS:.pem=.h) $(KEYS)
