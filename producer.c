#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <amqp.h>
#include <amqp_framing.h>

#include "utils.h"

int main(int argc, const char *argv[]) {

	const char *hostName;
	int port;
	const char *queueName;
	int msgSize;
	int durable = 0;
	int msgCount;

	if (argc < 7){
		fprintf(stderr,"Usage: producer host port queuename msgsize durable msgcount\n");
		exit(1);
	}

	hostName = argv[1];
	port = atoi(argv[2]);
	queueName = argv[3];
	msgSize = atoi(argv[4]);
	if(strcmp(argv[5],"true") == 0) durable = 1;
	msgCount = atoi(argv[6]);

	char *msgBody = malloc(msgSize);
	if(msgBody == NULL) {
		fprintf(stderr,"malloc error, try to reduce the msgsize parameter\n");
		exit(1);
	}
	memset(msgBody,'x',msgSize);

	int sockfd;
	int channelId = 1;
	amqp_connection_state_t conn;
	conn = amqp_new_connection();

	die_on_error(sockfd = amqp_open_socket(hostName, port), "Opening socket");
	amqp_set_sockfd(conn, sockfd);
	die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),"Logging in");
	amqp_channel_open(conn, channelId);
	die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

	amqp_basic_properties_t props;
	if(durable) {
		props._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
		props.delivery_mode = 2;
	}

	int i,j;
	long long start = timeInMilliseconds();
	for(i = 0; i < msgCount/10000; i++) {
		long long innerStart = timeInMilliseconds();
		for (j = 0; j < 10000; j++) {
			die_on_error(amqp_basic_publish(conn,channelId,amqp_cstring_bytes(""),amqp_cstring_bytes(queueName),0,
											0,&props,amqp_cstring_bytes(msgBody)),"Publishing");
		}
		long long innerEnd = timeInMilliseconds();
		printf("round %d takes %lld millseconds(10000 messages published every round)\n",i,innerEnd-innerStart);
	}

	long long end = timeInMilliseconds();
	printf("It takes %lld millseconds to send %d messages to queue\n",end-start,msgCount);

	die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
	die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection");
	die_on_error(amqp_destroy_connection(conn), "Ending connection");

	return 0;
}
