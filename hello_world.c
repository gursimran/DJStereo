//Global Variables
volatile char playSong = 0;
volatile char djplaysong=0;
volatile char stop_currently_playing = 0;
volatile char pause = 0;volatile char startedSendingList = 0;

//Files
#include <stdlib.h>
#include <system.h>
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

	//Initialize songList to null
	songList = NULL;

	//Read song list from SD card
	readSongsFromSDCard();

	//init timer
	init_Timer();

    
    //print first 3 songs of song list
	song_string(songList);
	printf("%s\n", songString);

    
    //If user wants to play dj or regular song
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