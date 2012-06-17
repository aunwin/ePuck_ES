/**
 * \file l2cap_client.c ändern in RFCOMM Socket
 *
 *
 * \author Nico Laum <nico.laum@uni-rostock.de>
 * \author Christian Lerche <christian.lerche@uni-rostock.de>
 *
 *
 *
 * modified for ePuck !!!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
//#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include "common.h"
#include <stdint.h>
#include <readline/readline.h>
#include <readline/history.h>

void usage(const char *argv) {
	printf("Usage:\n");
	printf("\t%s\n", argv);
}

int main(int argc, char **argv) {
	char* buf,input[32];

	const char *bt_destination_address = "10:00:E8:52:BC:49";	//Feste Serveradresse eintragen

	if (strlen(bt_destination_address) != BT_ADDRESS_LENGTH) {
		fprintf(stderr, "Wrong Bluetooth address format\n");
		exit(1);
	}

	// request socket
	int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (s < 0) {
		perror("socket()");
		exit(1);
	}

	// set the connection parameters (who to connect to)
	struct sockaddr_rc addr = { 0 };
//	addr.sockaddr_rc = AF_BLUETOOTH;
	addr.rc_family = AF_BLUETOOTH;
//	addr.l2_psm = htobs(psm);
	addr.rc_channel = 1;
	if (str2ba(bt_destination_address, &addr.rc_bdaddr) < 0) {
		perror("str2ba()");
		exit(1);
	}

	//const char *bt_destination_address = str2ba(dest,&addr.rc_bdaddr) ;

	// connect to server and send message
	int status;
	status = connect(s, (struct sockaddr *) &addr, sizeof(addr));
	if (status < 0) {
		perror("connect()");
	}

	printf("sending initial dummies...\n");
	status = send(s, "\n\n", sizeof("\n\n")-1, 0);
	if (status < 0) {
			perror("error:send()\n");
		} else {
			printf("status: %d\n",status);
		}


	// Command for Drive: 'd,-1000,1000' , where 1000 is maximal value of left and right wheel
	// Type 'exit' to reset ePuck, close Connection and leave

	send(s, "l,9,1\n", sizeof("l,9,1\n"), 0); // flash leds for 1 second
	sleep(1);
	send(s, "l,9,0\n", sizeof("l,9,0\n"), 0);

	send(s, "f,1\n", sizeof("f,1\n"), 0); // flash light, type 'f,0' to switch off

	sprintf(input,"command$>");
	buf=readline(input);
	while(strncmp(buf,"exit",4)){
		send(s, buf, strlen(buf), 0);
		send(s, "\n", 1, 0);
		buf=readline(input);
	}
	status = send(s, "d,0,0\n", sizeof("d,0,0\n"), 0);
		if (status < 0) {
			perror("error:send()\n");
		} else {
			printf("status code: %d\n",status);
		}
	printf("bye\n");
	free(buf);
	close(s);
	return 0;
}
