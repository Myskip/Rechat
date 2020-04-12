SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CFLAGS+=-I../include 
ifeq ($(DBG), 1)
override CFLAG+=-g
endif

.PHONY:all clean

all:$(OBJS)

$(OBJS):
	$(CC) -o $@ -c $(patsubst %.o,%.c,$@) $(CFLAGS) $(CFLAG)

clean:
	$(RM) $(OBJS) 