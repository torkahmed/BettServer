betserver: BETserver.c SOCKETwrapper.c
	gcc -o betserver BETserver.c SOCKETwrapper.c

clean:
	rm betserver