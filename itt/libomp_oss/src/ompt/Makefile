SRCS = ompt-initialize.c ompt-event-callbacks.c
OBJS = $(patsubst %.c, %.o, $(SRCS))

all: $(OBJS)

clean:
	/bin/rm -f $(OBJS)

%.o: %.c
	gcc -c $<
