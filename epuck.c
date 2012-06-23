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
//#include <curses.h>
//#include <term.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>

static SDL_Joystick *js;
int l=0;
int r=0;
int light=1;
int leds=0;
int r2=0;
int s;
int f;
int t=0;
char* input;
char* buf[32];
static void Joystick_Info (void) {
  int num_js, i;
  num_js = SDL_NumJoysticks ();
  printf ("Es wurde(n) %d Joystick(s) auf dem System gefunden\n",
     num_js);
  if (num_js == 0) {
    printf ("Es wurde kein Joystick gefunden\n");
    return;
  }
  /* Informationen zum Joystick */
  for (i = 0; i < num_js; i++) {
    js = SDL_JoystickOpen (i);
    if (js == NULL) {
      printf ("Kann Joystick %d nicht öffnen\n", i);
    }
    else {
      printf ("Joystick %d\n", i);
      printf ("\tName:       %s\n", SDL_JoystickName(i));
      printf ("\tAxen:       %i\n", SDL_JoystickNumAxes(js));
      printf ("\tTrackballs: %i\n", SDL_JoystickNumBalls(js));
      printf ("\tButtons:   %i\n",SDL_JoystickNumButtons(js));
      SDL_JoystickClose (js);
    }
  }
}
static int eventloop_joystick (void) {
  SDL_Event event;
  while (SDL_PollEvent (&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        printf ("ESCAPE für Ende betätigt\n");
        return 0;
      }
      break;
    /*case SDL_JOYAXISMOTION:
      printf ("Joystick %d, Achse %d bewegt nach %d\n",
         event.jaxis.which, event.jaxis.axis, event.jaxis.value);

      break;*/
    case SDL_JOYBUTTONUP:
    	if(event.jbutton.state==1){
    	switch (event.jbutton.button){
      	  case 7:
      		  l=0;
      		  break;
      	  case 2:
      		  break;
      	  case 6:
      		  r=0;
      		  break;
      	  case 5:
      		  l=0;
      		  break;
      	  case 4:
      		  r=0;
    	}break;}break;
    case SDL_JOYBUTTONDOWN:
      printf ("Joystick %i Button %i: %i\n",
         event.jbutton.which, event.jbutton.button,
         event.jbutton.state);
      	  switch (event.jbutton.button){
      	  case 0:
      		  f=1000;
      		  break;
      	  case 8:if(event.jbutton.state==1){
      		  if(leds==1){
      			  leds=0;
      		  }else{
      			  leds=1;
      		  }}
      		  break;
      	  case 9:if(event.jbutton.state==1){
      		  if(light==1){
      			  light=0;
      		  }else{
      			  light=1;
      		  }}
      		  break;
      	  case 1:
      		  f=500;
      		  break;
      	  case 2:
      		break;
      	  case 3:
      		  break;
      	  case 10:
      		break;
      	  case 11:
      		return 0;
      		break;
      	  case 7:
      		  if(event.jbutton.state==1){
      			  l=f;
      		  }else{ l=0;}
      		  break;
      	  case 6:
      		if(event.jbutton.state==1){
      			r=f;
      		}else{ r=0;}
      		break;
      	  case 5:
      		if(event.jbutton.state==1){
      			l=-1*f;
      		}else{ l=0;}
      		break;
      	  case 4:
      		if(event.jbutton.state==1){
      			r=-1*f;
      		}else{ r=0;
      	  }break;}
      break;
    case SDL_QUIT:
      return 0;
      	  }
    //l2=(int)l*a;
    //r2=(int)r*a;
    //printf("accel.-factor:%d",a);
    //printf("He would send\n");
      sprintf(buf,"d,%i,%i\n",l,r);
      send(s,buf, strlen(buf), 0);
      usleep(1000);
      sprintf(buf,"l,9,%i\n",leds);
      send(s,buf, strlen(buf), 0);
      usleep(1000);
      sprintf(buf,"f,%i\n",light);
      send(s,buf, strlen(buf), 0);


  }
  return 1;
}

void usage(const char *argv) {
	printf("Usage:\n");
	printf("\t%s\n", argv);
}

int main(int argc, char **argv) {

	const char *bt_destination_address = "10:00:E8:52:BC:49";	//Feste Serveradresse eintragen

	if (strlen(bt_destination_address) != BT_ADDRESS_LENGTH) {
		fprintf(stderr, "Wrong Bluetooth address format\n");
		exit(1);
	}

	// request socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
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

	f=500;

	// Command for Drive: 'd,-1000,1000' , where 1000 is maximal value of left and right wheel
	// Type 'exit' to reset ePuck, close Connection and leave

	send(s, "l,9,1\n", sizeof("l,9,1\n"), 0); // flash leds for 1 second
	sleep(1);
	send(s, "l,9,0\n", sizeof("l,9,0\n"), 0);

	send(s, "f,1\n", sizeof("f,1\n"), 0); // flash light, type 'f,0' to switch off

	/*sprintf(input,"command$>");
	buf=readline(input);
	while(strncmp(buf,"exit",4)){
		status = send(s, buf, strlen(buf), 0);
		if (status < 0) {
			perror("error:send()\n");
		} else {
			printf("status code: %d\n",status);
		}		
		status = send(s, "\n", 1, 0);
		if (status < 0) {
			perror("error:send()\n");
		} else {
			printf("status code: %d\n",status);
		}
		buf=readline(input);
	}*/

	int done = 1;
	  if (SDL_Init (SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) != 0) {
	    printf ("Fehler: %s\n", SDL_GetError ());
	    return 1;
	  }
	  atexit (SDL_Quit);
	  if (SDL_SetVideoMode (256, 256, 16, 0) == NULL) {
	    printf ("Fehler: %s\n", SDL_GetError ());
	    return EXIT_FAILURE;
	  }
	  Joystick_Info ();
	  js = SDL_JoystickOpen (0);
	  if (js == NULL) {
	    printf("Konnte Joystick nicht öffnen:%s\n",SDL_GetError());
	  }
	  printf("Joystick erfolgreich geöffnet\n");
	  while( done ) {

	    done = eventloop_joystick ();

	    SDL_Delay(100);
	  }
	  SDL_JoystickClose (js);

	  send(s, "f,0\n", sizeof("f,0\n"), 0);
	  sleep(1);
	  send(s, "t,0\n", sizeof("t,0\n"), 0);
	  sleep(1);
	  send(s, "l,9,0\n", sizeof("l,9,0\n"), 0);
	status = send(s, "d,0,0\n", sizeof("d,0,0\n"), 0);
		if (status < 0) {
			perror("error:send()\n");
		} else {
			printf("status code: %d\n",status);
		}
	printf("bye\n");

	close(s);
	return EXIT_SUCCESS;

}
