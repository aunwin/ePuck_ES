/**
 * \file l2cap_client.c ändern in RFCOMM Socket
 *
 *
 * \author Nico Laum <nico.laum@uni-rostock.de>
 * \author Christian Lerche <christian.lerche@uni-rostock.de>
 *
 * \author Florian Grützmacher <florian.gruetzmacher@uni-rostock.de>
 * \author Simeon Wiedenmann <simeon.wiedenmann@uni-rostock.de>
 *
 * modified for ePuck Robot by Florian Grützmacher and Simeon Wiedenmann
 *
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
int l=0;		/* Geschwindigkeit linkes Rad */
int r=0;		/* Geschwindigkeit rechtes Rad */
int light=1;	/* front light */
int leds=0;		/* LEDs */
int r2=0;		/* Geschwindigkeit rechts mit Geschw.-Faktor, nicht verwendet */
int s;			/* status variable */
int f;			/* Geschwindigkeitsfaktor */
int t=0;		/* Sound Variable??? */
char* input;
char* buf;
static void Joystick_Info (void) {
  int num_js, i;	/* num_js ist numberOfJoysticks, i Zählvariable */
  num_js = SDL_NumJoysticks();	/* Rückgabe der Anzahl */
  printf ("Es wurde(n) %d Joystick(s) auf dem System gefunden\n",
     num_js);
  if (num_js == 0) {
    printf ("Es wurde kein Joystick gefunden\n");
    return;
  }

  for (i = 0; i < num_js; i++) {
    js = SDL_JoystickOpen (i); /* joystick wird geöffnet, itterativ für alle */
    if (js == NULL) {
      printf ("Kann Joystick %d nicht öffnen\n", i);
    }
    else {
    	/* Informationen zum Joystick */
    	printf ("Joystick %d\n", i);
    	printf ("\tName:       %s\n", SDL_JoystickName(i));
    	printf ("\tAxen:       %i\n", SDL_JoystickNumAxes(js));
    	printf ("\tTrackballs: %i\n", SDL_JoystickNumBalls(js));
    	printf ("\tButtons:   %i\n",SDL_JoystickNumButtons(js));
    	SDL_JoystickClose (js); /* JS wird geschlossen */
    }
  }
}
static int eventloop(void) {
  SDL_Event event;	/* event variable*/
  /* nach event polln und wenn event da, schleife ausführen,
   * sdl_pollevent erwartet pointer, also mit & addresse geben
   * */
  while (SDL_PollEvent (&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
      break;
    /*case SDL_JOYAXISMOTION:
      printf ("Joystick %d, Achse %d bewegt nach %d\n",
         event.jaxis.which, event.jaxis.axis, event.jaxis.value);
         ---> könne wir nicht benutzen, weil die achsen werte ungleichmäßig gemappt sind
      break;*/
    case SDL_JOYBUTTONUP:
    	/* on button release */
    	switch (event.jbutton.button){
      	  case 7:
      		  l=0;	/* linkes rad stop */
      		  break;
      	  case 2:
      		  break;
      	  case 6:
      		  r=0;	/* rechtes rad stop */
      		  break;
      	  case 5:
      		  l=0; 	/* linkes rad stop */
      		  break;
      	  case 4:
      		  r=0;	/* rechtes rad stop */
      		  break;
    	}break;
    case SDL_JOYBUTTONDOWN:
    	/* on button pressed */
      	  switch (event.jbutton.button){
      	  case 0:
      		  f=1000;	/* schneller gang */
      		  break;
      	  case 8:if(event.jbutton.state==1){
      		  /* toggle led state */
      		  if(leds==1){
      			  leds=0;
      		  }else{
      			  leds=1;
      		  }}
      		  break;
      	  case 9:if(event.jbutton.state==1){
      		  /* toggle light state */
      		  if(light==1){
      			  light=0;
      		  }else{
      			  light=1;
      		  }}
      		  break;
      	  case 1:
      		  /* langsamer gang */
      		  f=500;
      		  break;
      	  case 2:
      		/* sound 5 abspielen, direkt in der eventloop */
      		sprintf(buf,"t,5\n");
      		send(s,buf, strlen(buf), 0);
      		break;
      	  case 3:
      		  /* sound 1 abspielen */
      		sprintf(buf,"t,1\n");
      		      		      send(s,buf, strlen(buf), 0);
      		  break;
      	  case 10:
      		  /* alle sounds beenden */
      		 sprintf(buf,"t,0\n");
      		      send(s,buf, strlen(buf), 0);
      		break;
      	  case 11:
      		return 0;
      		break;
      	  case 7:
      		  if(event.jbutton.state==1){
      			  l=f;	/* links fährt mit Geschw. f */
      		  }else{ l=0;}
      		  break;
      	  case 6:
      		if(event.jbutton.state==1){
      			r=f;	/* rechts fährt mit Geschw. f */
      		}else{ r=0;}
      		break;
      	  case 5:
      		if(event.jbutton.state==1){
      			l=-1*f;	/* links fährt mit Geschw. f rückwärts */
      		}else{ l=0;}
      		break;
      	  case 4:
      		if(event.jbutton.state==1){
      			r=-1*f;
      		}else{ r=0;	/* rechts fährt mit Geschw. f rückwärts */
      	  }break;}
      break;
    case SDL_QUIT:
      return 0;
      	  }
      sprintf(buf,"d,%i,%i\n",l,r);		/* geschwindigkeit der Räder senden */
      send(s,buf, strlen(buf), 0);
      usleep(1000);
      sprintf(buf,"l,9,%i\n",leds);		/* led state senden */
      send(s,buf, strlen(buf), 0);
      usleep(1000);
      sprintf(buf,"f,%i\n",light);		/* frontlight state senden */
      send(s,buf, strlen(buf), 0);


  }
  return 1;
}

int main(int argc, char **argv) {
	buf=malloc(32);		/* buffer speicher alloziieren */
	const char *bt_destination_address;	//Serveradresse

	if(argc>1){
		bt_destination_address=argv[1];
	}else{
		bt_destination_address = "10:00:E8:52:BC:49";	/* standard Adresse des ePuck 1031 */
	}

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

	f=500;		/* standard geschwindigkeit */

	// Command for Drive: 'd,-1000,1000' , where 1000 is maximal value of left and right wheel
	// Type 'exit' to reset ePuck, close Connection and leave

	send(s, "l,9,1\n", sizeof("l,9,1\n"), 0); // flash leds for 1 second
	sleep(1);
	send(s, "l,9,0\n", sizeof("l,9,0\n"), 0);

	send(s, "f,1\n", sizeof("f,1\n"), 0); // flash light, type 'f,0' to switch off

	/* command line implementation, not used because joysticks are more awesome */
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
	  Joystick_Info ();
	  js = SDL_JoystickOpen (0);

	  if (js == NULL) {
	    printf("Konnte Joystick nicht öffnen:%s\n",SDL_GetError());
	  }else{
		  printf("Joystick erfolgreich geöffnet\n");
	  }

	  while( done ) {
		  done = eventloop();
		  SDL_Delay(100);
	  }

	  SDL_JoystickClose (js);
	  status = send(s, "d,0,0\n", sizeof("d,0,0\n"), 0);	/* räder auf 0 */
	  		if (status < 0) {
	  			perror("error:send()\n");
	  		} else {
	  			printf("status code: %d\n",status);
	  		}
	  sleep(1);
	  send(s, "f,0\n", sizeof("f,0\n"), 0);	/* licht aus */
	  send(s, "t,0\n", sizeof("t,0\n"), 0);	/* sound aus */
	  send(s, "l,9,0\n", sizeof("l,9,0\n"), 0);/* LEDs aus */
	printf("bye\n");
	free(buf);	/* buffer wieder freigeben */
	close(s);	/* Buetooth-rbindung beenden */
	return EXIT_SUCCESS;

}
