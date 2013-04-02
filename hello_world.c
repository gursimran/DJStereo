#include <stdlib.h>
#include <system.h>
char playSong = 0;
char djplaysong=0;
char stop_currently_playing = 0;
char pause = 0;char startedSendingList = 0;
#include "altera_up_sd_card_avalon_interface.h"
#include <stdio.h>
#include "altera_up_avalon_rs232.h"
#include <string.h>
#include "Song.h"
#include "lists.h"
#include "song_read.h"
#include "sound.h"
#include "Timer.h"




int main(){

	//Initializing components that run sound
	configure_audio();
	//sound = (unsigned int *)malloc(1000000*sizeof(unsigned int));

	//Initialize songList to null
	songList = NULL;

	//Read songs from SD card
	readSongsFromSDCard();
	readsongFXFromSDCard();

	//print all song names
	int x;
	for(x=0; x<num_songs; x++){
		song a = getItemAt(songList, x);
		printf("%d\n",a.ID);
		printf("%s\n", a.name);
		printf("%d\n", a.LENGTH);
		printf("%d\n", a.Size);
		printf("%s\n", a.realname);
		printf("%s\n", a.artist);
	}
	for(x=0; x<num_FX; x++){
		song a = getItemAt(FXList, x);
		printf("%d\n",a.ID);
		printf("%s\n", a.name);
		printf("%d\n", a.LENGTH);
		printf("%d\n", a.Size);
		printf("%s\n", a.realname);
		printf("%s\n", a.artist);
	}

	//init timer
	init_Timer();



	song_string(songList);
	printf("%s\n", songString);

	//Playing a sound
	//sound = read_wav(getItemAt(songList,x).name);
	//size = size_wav;
	//init_button_pio(sound);
	//alt_up_audio_enable_write_interrupt(audio_dev);
	//play_song();
	while(1){
		if (playSong == 1){
			printf("gonna play song\n");
			play_song(a);
			djplaysong=0;
		}
		else if(djplaysong==1){
			printf("gonna play dj songs\n");
			DJPlay(song1, song2);
			playSong=0;
		}
	}

}
//int main() {
//	printf("Hello from Nios II!\n");
//	int i;
//	unsigned char data;
//	unsigned char parity;
//	unsigned char message[] = "EECE381 is so much fun";
//	unsigned char *dataReceived;
//	unsigned char *reversedDataReceived;
//
//	printf("UART Initialization\n");
//	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
//
//	while (1) {
//		printf("Clearing read buffer to start\n");
//		while (alt_up_rs232_get_used_space_in_read_FIFO(uart)) {
//			alt_up_rs232_read_data(uart, &data, &parity);
//		}
//
//		//Now receive the message from the middleman
//
//		printf("Waiting for data to come back from Middleman\n");
//		while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0)
//			;
//
//		//First byte is the number of characters in our message
//
//		alt_up_rs232_read_data(uart, &data, &parity);
//		int num_to_receive = (int) data;
//
//		printf("About to receive %d characters:\n", num_to_receive);
//
//		dataReceived = (unsigned char *) malloc(sizeof(unsigned char)
//				* num_to_receive + 1);
//		reversedDataReceived = (unsigned char *) malloc(sizeof(unsigned char)
//				* num_to_receive + 1);
//		//dataReceived[0] = num_to_receive;
//		//reversedDataReceived[0] = num_to_receive;
//
//		for (i = 0; i < num_to_receive; i++) {
//			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0)
//				;
//			alt_up_rs232_read_data(uart, &data, &parity);
//
//			dataReceived[i] = data;
//		}
//		dataReceived[num_to_receive] = '\0';
//
//		printf("%s\n", dataReceived);
//		printf("Message echo complete\n");
//
//		//Flip the string
//		for (i = 0; i < num_to_receive; i++) {
//
//			reversedDataReceived[i] = dataReceived[num_to_receive - i - 1];
//			printf("%c", reversedDataReceived[i]);
//		}
//		reversedDataReceived[num_to_receive] = '\0';
//		printf("Sending the message to the Middleman\n");
//		printf("%s\n", reversedDataReceived);
//
//		alt_up_rs232_write_data(uart, (unsigned char) strlen(
//				reversedDataReceived));
//
//		//Now send the actual message to the mIddleman
//
//		for (i = 0; i < strlen(reversedDataReceived); i++) {
//			alt_up_rs232_write_data(uart, reversedDataReceived[i]);
//		}
//
//	}
//
//	return 0;
//}
