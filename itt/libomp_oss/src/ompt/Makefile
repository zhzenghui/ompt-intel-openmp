SRCS = ompt-initialize.c ompt-event-callbacks.c ompt-init.c ompt-fini.c
OBJS = $(patsubst %.c, %.o, $(SRCS))

all: $(OBJS)

clean:
	/bin/rm -f $(OBJS)

%.o: %.c
	gcc -c $<
