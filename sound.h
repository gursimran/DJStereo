#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "io.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "assert.h"


int k = 0;
alt_up_audio_dev * audio_dev=NULL;
int a = 0;
unsigned int * soundBuffer;
int noTimes =0;

void readWavFromSDCARD(char *name, unsigned char *levelBricksToDraw);
void configure_audio();
unsigned int * read_wav(char *name, unsigned int size);
void play_wav();

int size=0;




static void init_button_pio()
{
    alt_irq_register( AUDIO_0_IRQ,NULL, play_wav );
    printf("4\n");
}
void play_song(int song_to_play){
	a = song_to_play % num_songs;
	if (a<num_songs){
		song b = getItemAt(songList, a);
		read_wav_buffer(b.name, b.Size);
		size = b.Size;
		init_button_pio();
		alt_up_audio_enable_write_interrupt(audio_dev);
	}
}



void play_wav() {


		alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
				ALT_UP_AUDIO_RIGHT);
		alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
				ALT_UP_AUDIO_LEFT);

		if ((noTimes*100000)+k >= size) {
			k = 0;
			noTimes=0;
			alt_up_audio_disable_write_interrupt(audio_dev);
			alt_up_audio_reset_audio_core(audio_dev);
			a++;
			free(soundBuffer);
			play_song(a);

			//printf("\n");
		} else{
			k += 100;
			if (k == 100000){
				k = 0;
				noTimes++;
			}
		}
			//printf("%d",k);

}

void stop_sound(){
	alt_up_audio_disable_write_interrupt(audio_dev);
	k=0;
}

void pause_sound(){
	alt_up_audio_disable_write_interrupt(audio_dev);
}

void resume_sound(){
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void next_sound(){
	++a;
	play_song(a);
}

void previous_sound(){
	--a;
	play_song(a);
}

void read_wav_buffer (char *name, unsigned int size){
	alt_up_sd_card_dev *device_reference = NULL;
	device_reference = alt_up_sd_card_open_dev(
			"/dev/Altera_UP_SD_Card_Avalon_Interface_0");
	if (device_reference == NULL) {
		printf("Could not read from the SDcard.\n");
	} else {
		if (!alt_up_sd_card_is_Present()) {
			printf("The SDcard is not present!\n");
		}

		else {
			if (!alt_up_sd_card_is_FAT16()) {
				printf(
						"The SDcard is not formatted to be FAT16 and could not be read.\n");
			} else {
				int fileHandle;
				short dataRead;
				soundBuffer = (unsigned int *)malloc(sizeof(unsigned int)*100000);
				unsigned char * temp_array;
				temp_array = (unsigned char *)malloc(200000*sizeof(unsigned char));
				fileHandle = alt_up_sd_card_fopen(name, false);
				int j=0;
				int y=0;
				// Get first byte of file
				dataRead = alt_up_sd_card_read(fileHandle);
				while (dataRead > -1) {
					temp_array[j] = dataRead;
					j++;
					dataRead = alt_up_sd_card_read(fileHandle);
					temp_array[j] = dataRead;
					j++;
					dataRead = alt_up_sd_card_read(fileHandle);
					while(k<y);
					soundBuffer[y] = (temp_array[j] << 8 | temp_array[j-1]) << 8;
					y++;
					if (y == 100000){
						alt_up_audio_enable_write_interrupt(audio_dev);
						y=0;
						j=0;
					}
				}
				alt_up_sd_card_fclose(fileHandle);
			}
		}
	}
}



unsigned int * read_wav(char *name, unsigned int size)
{
	unsigned char *levelBricksToDraw;
	levelBricksToDraw = (unsigned char *) malloc(size * sizeof(unsigned char));
	readWavFromSDCARD(name, levelBricksToDraw);

	unsigned int * temp_array;
	temp_array = (unsigned int *) malloc((size / 2) * sizeof(unsigned int));
	int x;
	int y = 0;
	for (x = 0; x < size; x += 2) {
		unsigned int sample = (levelBricksToDraw[x + 1] << 8 | levelBricksToDraw[x]) << 8; //original
		temp_array[y] = sample;
		y++;
	}
	free(levelBricksToDraw);
	return temp_array;
}

/////////////////////////////////////////*******?//////////////////////////////////////////////
void readWavFromSDCARD(char * name, unsigned char *levelBricksToDraw) {
	// Create row an column variables for iteration
	int j=0;
	// Variables needed for sdcard reading
	int fileHandle;
	 short dataRead;
	//int number_bytes = 0;

	// File name to be opened

	alt_up_sd_card_dev *device_reference = NULL;
	device_reference = alt_up_sd_card_open_dev(
			"/dev/Altera_UP_SD_Card_Avalon_Interface_0");
	if (device_reference == NULL) {
		printf("Could not read from the SDcard.\n");
	} else {
		if (!alt_up_sd_card_is_Present()) {
			printf("The SDcard is not present!\n");
		}

		else {
			if (!alt_up_sd_card_is_FAT16()) {
				printf(
						"The SDcard is not formatted to be FAT16 and could not be read.\n");
			}
			else{
				fileHandle = alt_up_sd_card_fopen(name, false);

					// Get first byte of file
					dataRead = alt_up_sd_card_read(fileHandle);

					// Keep reading till eof
					while (dataRead > -1) {
						levelBricksToDraw[j] = dataRead;
						j++;
						dataRead = alt_up_sd_card_read(fileHandle);
					}

					printf("number of reads: %d\n", j);

					alt_up_sd_card_fclose(fileHandle);
			}
		}
	}
}

//////////////////////////////////////////////////******/////////////////////////////////////////
void configure_audio()
{
	alt_up_av_config_dev* audio_config;
	audio_config = alt_up_av_config_open_dev(AUDIO_AND_VIDEO_CONFIG_0_NAME);

	alt_up_av_config_reset(audio_config);


	while (!alt_up_av_config_read_ready(audio_config)) {
	}


	// open the Audio port
	audio_dev = alt_up_audio_open_dev(AUDIO_0_NAME);
	if (audio_dev == NULL)
		printf("Error: could not open audio device \n");
	else
		printf("Opened audio device \n");

	alt_up_audio_reset_audio_core(audio_dev);
}

