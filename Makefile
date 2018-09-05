all: betserver betclient

betserver: BETserver.c SOCKETwrapper.c
	gcc -o betserver BETserver.c SOCKETwrapper.c -lpthread

betclient: BETclient.c SOCKETwrapper.c
	gcc -o betclient BETclient.c SOCKETwrapper.c

clean:
	rm betserver betclient
