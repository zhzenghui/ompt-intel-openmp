SRCS = \
  ompt-control.c \
  ompt-enumerate.c \
  ompt-event-callbacks.c \
  ompt-fini.c \
  ompt-init.c \
  ompt-initialize.c \
  ompt-parallel.c \
  ompt-runtime-version.c \
  ompt-state.c \
  ompt-thread-data.c \
  ompt-version.c

OBJS = $(patsubst %.c, %.o, $(SRCS))

all: $(OBJS)

clean:
	/bin/rm -f $(OBJS)

%.o: %.c
	gcc -c $<
