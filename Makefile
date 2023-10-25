CC = gcc
CFLAGS = -Wall -Werror -std=gnu99 -pthread

Target: dbclient dbserver

dbclient: dbclient.c msg.h
	$(CC) $(CFLAGS) -o dbclient dbclient.c

dbserver: dbserver.c msg.h
	$(CC) $(CFLAGS) -o dbserver dbserver.c

clean:
	rm -f dbclient dbserver



