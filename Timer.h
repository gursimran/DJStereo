#include "sys/alt_timestamp.h"
#include "sys/alt_irq.h"

#include "system.h"



volatile int s = 0;
//char playSong = 0;

void sendData(char * message) {
	int length = strlen(message);
	int q;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	//	while (alt_up_rs232_get_used_space_in_read_FIFO(uart)) {
	//		alt_up_rs232_read_data(uart, &data, &parity);
	//	}
	//	alt_up_rs232_write_data(uart, 'h');
	for (q = 0; q < length; q++) {
		alt_up_rs232_write_data(uart, message[q]);
	}
}

void ReadData(void * context, unsigned int irq_id) {
	//printf("interrupt\n");
	unsigned char data;
	unsigned char parity;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	if (alt_up_rs232_get_used_space_in_read_FIFO(uart) != 0) {
		alt_up_rs232_read_data(uart, &data, &parity);
		int num_to_receive = (int) data;
		char * command = (char *) malloc(sizeof(char) * (num_to_receive + 1));
		int q;
		for (q = 0; q < num_to_receive; q++) {
			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0)
				;
			alt_up_rs232_read_data(uart, &data, &parity);
			command[q] = data;
			printf("%c", data);
		}
		command[num_to_receive] = '\0';
		printf("\nhere\n");
		//pause_sound();
		if (command[0] == 'p') {
			set_song(command);
		} else if (command[0] == 'r') {
			resume_sound();
		} else if (command[0] == 'x') {
			pause_sound();
		} else if (command[0] == 's') {
			stop_sound();
		} else if (command[0] == 'l') {
			stop_sound();
			started=0;
			send_num_songs = 3;
			sent_songs = 0;
			readSongsFromSDCard();
			song_string(songList);
			sendData( songString);
			free(songString);
			startedSendingList = 1;
		}  else if (command[0] == 'v'){
			set_volume(command);
		} else if (command[0] == 'a'){
			if (startedSendingList == 1){
			song_string(songList);
			sendData(songString);
			free(songString);
			}
		} else if (command[0] == 'd'){
			set_dj(command);
		} else if (command[0]=='i'){
			set_djvolume(command);
		} else if (command[0] == 't'){
			set_djspeed(command);
		} else if(command[0]=='1'){
			//play FX1
			FX2=1;
		} else if (command[0]=='w'){
			FX1=1;
			rewind_dj(command);

		} else if (command[0] == 'y'){
			FX1=1;
			fastforward_dj(command);
		} else if (command[0] == 'h'){
			record_song();
		} else if(command[0]=='o'){
			stop=1;
		}

		printf("\n");
		//resume_sound();
	}
	char data2 = IORD(LEDS_BASE, 0);
	data2 = data2 ^ 0x00FF;
	IOWR(LEDS_BASE, 0, data2);
	IOWR(TIMER_0_BASE, 0, 0);
}

static void init_Timer() {
	IOWR(TIMER_0_BASE + 8, 0, 0x7840);
	IOWR(TIMER_0_BASE + 12, 0, 0x017D);

	IOWR(TIMER_0_BASE + 4, 0, 0x7);
	//alt_avalon_timer_sc_init ((void *)ReadData, TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, TIMER_0_FREQ);
	alt_irq_register(TIMER_0_IRQ, NULL, (void *) ReadData);
	alt_irq_enable( TIMER_0_IRQ);

	//alt_avalon_timer_sc_init (TIMER_0_BASE, TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,
	//		TIMER_0_IRQ, TIMER_0_FREQ);
	printf("Timer Interrupt\n");
}
