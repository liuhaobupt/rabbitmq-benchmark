all:producer

producer:producer.c
	gcc -o producer -Wall producer.c utils.c unix/platform_utils.c -I. -lrabbitmq

clean:
	rm producer
