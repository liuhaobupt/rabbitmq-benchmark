all:producer consumer

producer:producer.c
	gcc -o producer -Wall producer.c utils.c unix/platform_utils.c -I. -lrabbitmq

consumer:consumer.c
	gcc -o consumer -Wall consumer.c utils.c unix/platform_utils.c -I. -lrabbitmq

clean:
	rm producer consumer
