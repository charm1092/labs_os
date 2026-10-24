CC = cc
CFLAGS = -Wall -Wextra

all: parent child

parent: parent.c utils.c utils.h
	$(CC) $(CFLAGS) -o parent parent.c utils.c

child: child.c utils.c utils.h
	$(CC) $(CFLAGS) -o child child.c utils.c

clean:
	rm -f parent child
