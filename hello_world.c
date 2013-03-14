#include <stdlib.h>
#include <system.h>

#include "altera_up_sd_card_avalon_interface.h"
#include <stdio.h>
#include "altera_up_avalon_rs232.h"
#include <string.h>
#include "sound.h"
#include "Song.h"
#include "lists.h"
#include "song_read.h"



int main(){

	//Initializing components that run sound
	configure_audio();
	//unsigned int *sound = (unsigned int *)malloc(1000000*sizeof(unsigned int));
	songlist->link = NULL;
	//Reading in the names of all songs from the SD card
	song_list=(char **)malloc(400*sizeof(char *));
	int x;
	for(x=0 ; x < 400; x++){
		song_list[x]=(char*)malloc(20*sizeof(char));
	}
	readSongsFromSDCard();
	num_songs=i;
//	printf("%d\n",num_songs);
//	while(i>0){
//		printf("%s\n",song_list[i-1]);
//		i--;
//	}

//	printf("%d\n", songlist->dataitem.ID);
//	printf("%s", songlist->dataitem.name);

	for(x=0; x<num_songs; x++){
		song a = getItemAt(songlist, x);
		printf("%d\n",a.ID);
		printf("%s", a.name);
	}

	//Playing a sound
	//sound = read_wav("laser_m.wav");
	//size = size_wav;
	//init_button_pio(sound);
	//alt_up_audio_enable_write_interrupt(audio_dev);

//	listelement *test = NULL;
//	song song1;
//	song1.ID = 10;
//	strcpy(song1.name,"song1");
//	song song2;
//	song2.ID = 20;
//	strcpy(song2.name,"song2");
//	song song3;
//	song3.ID = 30;
//	strcpy(song3.name, "song3");
//	song song4;
//	song4.ID = 40;
//	strcpy(song4.name, "song4");
//	test = AddItem(test, song1);
//	test = AddItem(test,song2);
//	test = AddItem(test,song3);
//	test = AddItem(test,song4);
//	song2.ID = 1000;
//	song a = getItemAt(test, 2);
//	printf("%d\n",a.ID);
//	printf("%s", a.name);

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
