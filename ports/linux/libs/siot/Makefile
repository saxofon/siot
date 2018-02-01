#DBG := -g -ggdb

CFLAGS += $(DBG) -fpic
LDFLAGS += $(DBG) -shared

LIB += libsiot.so

SRCS += $(shell ls *c)
OBJS += $(SRCS:.c=.o)

all: $(LIB)

$(LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

-include $(SRCS:.c=.d)

clean:
	$(RM) $(LIB) $(OBJS) $(SRCS:.c=.d)
