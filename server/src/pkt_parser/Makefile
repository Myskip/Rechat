SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))

.PHONY:all clean

all:$(OBJS)

$(OBJS):
	$(CC) -o $@ -c $(patsubst %.o,%.c,$@) $(CFLAG)
clean:
	$(RM) $(OBJS) 