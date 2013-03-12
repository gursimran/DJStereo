/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "altera_up_avalon_rs232.h"
#include <string.h>
#include "sound.h"
#include "Song.h"
#include "lists.h"

int main(){
	//configure_audio();
	//unsigned int *sound = (unsigned int *)malloc(1000000*sizeof(unsigned int));
	//sound = read_wav("laser_m.wav");
	//size = size_wav;
	//init_button_pio(sound);
	//alt_up_audio_enable_write_interrupt(audio_dev);
	listelement *test;
	song song1;
	song1.ID = 10;
	strcpy(song1.name,"song1");
	song song2;
	song2.ID = 20;
	strcpy(song2.name,"song2");
	song song3;
	song3.ID = 30;
	strcpy(song3.name, "song3");
	song song4;
	song4.ID = 40;
	strcpy(song4.name, "song4");
	AddItem(test, song1);
	AddItem(test,song2);
	AddItem(test,song3);
	AddItem(test,song4);

	song a = getItemAt(test, 2);
	printf("%d\n",a.ID);
	printf("%s", a.name);
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
