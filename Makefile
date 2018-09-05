betserver: BETserver.c SOCKETwrapper.c
	gcc -o betserver BETserver.c SOCKETwrapper.c -lpthread

clean:
	rm betserver
