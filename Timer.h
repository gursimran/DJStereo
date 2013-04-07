#include "sys/alt_timestamp.h"
#include "sys/alt_irq.h"
#include "system.h"



volatile int s = 0;

//SEND MESSAGE TO MIDDILEMAN
void sendData(char * message) {
	int length = strlen(message);
	int q;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	for (q = 0; q < length; q++) {
		alt_up_rs232_write_data(uart, message[q]);
	}
}

//INTERRUPT SERVICE ROUTINE FOR TIMER INTERRUPT TO CHECK IF ANY DATA HAS BEEN RECEIVED FROM MIDDLEMAN
void ReadData(void * context, unsigned int irq_id) {
	unsigned char data;
	unsigned char parity;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	if (alt_up_rs232_get_used_space_in_read_FIFO(uart) != 0) {
		alt_up_rs232_read_data(uart, &data, &parity);
		int num_to_receive = (int) data;
		char * command = (char *) malloc(sizeof(char) * (num_to_receive + 1));
		int q;
        
        //GET DATA IN BUFFER
		for (q = 0; q < num_to_receive; q++) {
			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0)
				;
			alt_up_rs232_read_data(uart, &data, &parity);
			command[q] = data;
		}
		command[num_to_receive] = '\0';
        
        //CHECK WHAT WAS RECEIVED
		if (command[0] == 'p') {
            //Play song
			set_song(command);
		} else if (command[0] == 'r') {
            //resume song
			resume_sound();
		} else if (command[0] == 'x') {
            //pause song
			pause_sound();
		} else if (command[0] == 's') {
            //stop song
			stop_sound();
		} else if (command[0] == 'l') {
            //send song list
            
            //stop song
			stop_sound();
			started=0;
			send_num_songs = 3;
			sent_songs = 0;
            
            //get songs from sd card
			readSongsFromSDCard();
			song_string(songList);
			sendData( songString);
			free(songString);
			startedSendingList = 1;
		}  else if (command[0] == 'v'){
            //set volume of song
			set_volume(command);
		} else if (command[0] == 'a'){
            //acknowledgement from android that songs sent have been received and send next chunk of songs
			if (startedSendingList == 1){
			song_string(songList);
			sendData(songString);
			free(songString);
			}
		} else if (command[0] == 'd'){
            //play dj songs
			set_dj(command);
		} else if (command[0]=='i'){
            //set individual volume of the two dj songs
			set_djvolume(command);
		} else if (command[0] == 't'){
            //set the speed of two songs in dj
			set_djspeed(command);
		} else if(command[0]=='1'){
            //play effect 1
			FX2=1;
		} else if (command[0]=='w'){
            //rewind dj song
			FX1=1;
			rewind_dj(command);
		} else if (command[0] == 'y'){
            //fast forward dj song
			FX1=1;
			fastforward_dj(command);
		} else if (command[0] == 'h'){
            //record dj song
			record_song();
		} else if(command[0]=='o'){
            //stop dj song
			stop=1;
		}
	}
    
    //reset timer interrupt
	char data2 = IORD(LEDS_BASE, 0);
	data2 = data2 ^ 0x00FF;
	IOWR(LEDS_BASE, 0, data2);
	IOWR(TIMER_0_BASE, 0, 0);
}


//INITIALIZE TIMERS TO INTERRUPT EVERY 500MS
static void init_Timer() {
	IOWR(TIMER_0_BASE + 8, 0, 0x7840);
	IOWR(TIMER_0_BASE + 12, 0, 0x017D);

	IOWR(TIMER_0_BASE + 4, 0, 0x7);
	alt_irq_register(TIMER_0_IRQ, NULL, (void *) ReadData);
	alt_irq_enable( TIMER_0_IRQ);
    
	printf("Timer Interrupt\n");
}
