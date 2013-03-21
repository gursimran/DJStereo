#include "sys/alt_timestamp.h"

int s = 0;

void sendData(char * message){
	int length = strlen(message);
	int i;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
//	while (alt_up_rs232_get_used_space_in_read_FIFO(uart)) {
//		alt_up_rs232_read_data(uart, &data, &parity);
//	}
//	alt_up_rs232_write_data(uart, 'h');
	for (i = 0; i < length; i++) {
				alt_up_rs232_write_data(uart, message[i]);
	}
}


void ReadData(){

	unsigned char data;
	unsigned char parity;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	if (alt_up_rs232_get_used_space_in_read_FIFO(uart) != 0){
		alt_up_rs232_read_data(uart, &data, &parity);
		int num_to_receive = (int) data;
		char * command = (char *)malloc(sizeof(char)*(num_to_receive+1));
		int i;
		for (i =0; i< num_to_receive; i++){
			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
			alt_up_rs232_read_data(uart, &data, &parity);
			command[i]=data;
			printf("%c",data);
		}
		command[num_to_receive]= '\0';
		if(strstr(command, "play")!= NULL){
			a=command[4]-48;
			play_song(a);
		}
		else if (strstr(command, "resume")!= NULL){
			resume_sound();
		}
		else if (strstr(command, "pause")!= NULL){
			pause_sound();
		}
		else if (strstr(command, "stop")!= NULL){
			stop_sound();
		}
		else if (strstr(command, "list") != NULL){
			readSongsFromSDCard();
			song_string(songList);
			sendData(songString);
			free(songString);
		}
		else if (strstr(command, "previous") != NULL){
			previous_sound();
		}
		else if (strstr(command, "next") != NULL){
			next_sound();
		}
		printf("\n");
	}
	IOWR(TIMER_0_BASE,0,0);
}



static void init_Timer()
{
	IOWR(TIMER_0_BASE+4, 0, 0x7);
	//alt_avalon_timer_sc_init ((void *)ReadData, TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, TIMER_0_FREQ);
	alt_irq_register( TIMER_0_IRQ, NULL, (void *)ReadData );
    printf("Timer Interrupt\n");
}
