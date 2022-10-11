http-server : main.c
	gcc main.c -o http-server

clean :
	rm http-server
