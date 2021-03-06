#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "io.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "assert.h"
#include "sys/alt_irq.h"

char playing = 0;
char started = 0;
int k = 0;
alt_up_audio_dev * audio_dev=NULL;
int a = 0;
int y=0;
unsigned int * soundBuffer;
int noTimes = 0;
int reach1000;
char readMore = 0;
void readWavFromSDCARD(char *name, unsigned char *levelBricksToDraw);
void configure_audio();
unsigned int * read_wav(char *name, unsigned int size);
void play_wav();
void read_wav_buffer (char *name, int size);
int volume = 0;

int size=0;




static void init_button_pio( )
{
    alt_irq_register( AUDIO_0_IRQ,NULL, play_wav );
    alt_up_audio_disable_write_interrupt(audio_dev);
}

void play_song(int song_to_play){
	a = song_to_play % num_songs;
	//if (a<num_songs){
		song b = getItemAt(songList, a);
		size = b.Size/2;
		init_button_pio();
		playSong = 0;
		//unsigned char * sound;
		usleep(1000000);
		started = 0;
		printf("calling method now to read file for song: %d\n",a);
		read_wav_buffer(b.name, b.Size);
		printf("returned from method, no of times: %d\n", noTimes);
		//a++;
		while (started == 1);
		started = 0;
		playSong=1;;
		//alt_up_audio_enable_write_interrupt(audio_dev);
		//alt_up_audio_enable_write_interrupt(audio_dev);
	//}
}



void play_wav() {
		alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
				ALT_UP_AUDIO_RIGHT);
		alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
				ALT_UP_AUDIO_LEFT);
		if ((10000*noTimes) + 100 + k >= size) {
			k = 0;
			noTimes=0;
			//playSong = 1;
			started = 0;
			a++;
			y=0;
			playing = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
			alt_up_audio_reset_audio_core(audio_dev);
			free(soundBuffer);
		} else{
			k += 100;

			if (k == 10000){
				k = 0;
				noTimes++;
			}
		}
}

void stop_sound(){
	alt_up_audio_disable_write_interrupt(audio_dev);
	stop_currently_playing = 1;
	noTimes = 0;
	started = 0;
	k=0;
	pause = 0;
}

void pause_sound(){
	pause = 1;
	alt_up_audio_disable_write_interrupt(audio_dev);
}

void resume_sound(){
	pause = 0;
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void next_sound(){
	stop_sound();
	++a;
	playSong =1;
	//play_song(a);
}

void previous_sound(){
	stop_sound();
	--a;
	playSong=1;
	//play_song(a);
}

void set_volume (char * message){
	char temp[4];
	temp[0] = message[6];
	temp[1] = message[7];
	temp[2] = message[8];
	temp[3] = '\0';
	int tempV = atoi(temp);
	volume = 10 - (tempV/10);
	printf("volume: %d\n", volume);
}
void read_wav_buffer (char *name, int size){
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
				int fileHandle2;
				short dataRead;
				short dataRead2;
				soundBuffer = (unsigned int *)malloc(sizeof(unsigned int)*10000);
				int i;
				for (i=0;i<10000;i++){
					soundBuffer[i] = 0;
				}
				song x = getItemAt(songList, a+1);
				fileHandle2 = alt_up_sd_card_fopen(x.name, false);
				fileHandle = alt_up_sd_card_fopen(name, false);
				printf("filehandle: %d, filehandle2: %d", fileHandle, fileHandle2);
				int j=0;

				stop_currently_playing = 0;
				int whenToStart = 10000;
				if (size/2 < 10000){
					whenToStart = size/2;
				}
				int setVolume;
				unsigned int temp;
				unsigned int temp2;
				unsigned int temp3;
				unsigned int temp4;
				int z= 0;
				// Get first byte of file
				dataRead2 = alt_up_sd_card_read(fileHandle2);
				dataRead = alt_up_sd_card_read(fileHandle);
				//while(y <= 190000)
				while (dataRead > -1){
					if (stop_currently_playing == 1){
						break;
					}
					if (started == 1){
						int x = 0;
						while (pause == 1);
						while(abs(y-k)<2){
							x++;
							if (x == 10000000){

								printf("help, im stuck, with value of y: %d and k: %d\n",y,k);
								alt_up_audio_reset_audio_core(audio_dev);
								//k += 100;
								break;
							}
						}
					}
					temp = dataRead ;
					//temp3 = dataRead2;
					dataRead = alt_up_sd_card_read(fileHandle);
					//dataRead2 = alt_up_sd_card_read(fileHandle2);
					temp2 = dataRead ;
					//temp4 = dataRead2;
					dataRead = alt_up_sd_card_read(fileHandle);
					//dataRead2 = alt_up_sd_card_read(fileHandle2);
					soundBuffer[y] = ((temp2 << 8 | temp) << 8);// + ((temp4 << 8 | temp3) << 8);
					if(volume > 0){
						soundBuffer[y] = soundBuffer[y]/2;
					}
					y++;
					if (y-z == whenToStart){
						if (started == 0){
							alt_up_audio_enable_write_interrupt(audio_dev);
							started = 1;
							playing = 1;
						}
						y=0;
						j=0;
					}
				}
				alt_up_sd_card_fclose(fileHandle);
				alt_up_sd_card_fclose(fileHandle2);
				//a++;
			}
		}
	}
}

int abs (int n) {
    const int ret[2] = { n, -n };
    return ret [n<0];
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
	int j=0;
	int fileHandle;
	 short dataRead;
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


////////////////////
//////////////////
///////////////////
//////////////
/////////////////
////////////////
#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "io.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "assert.h"
#include "sys/alt_irq.h"

char playing = 0;
char started = 0;
int k = 0;
alt_up_audio_dev * audio_dev=NULL;
int a = 0;
int y=0;
unsigned int * soundBuffer;
int noTimes = 0;
int reach1000;
char readMore = 0;
void readWavFromSDCARD(char *name, unsigned char *levelBricksToDraw);
void configure_audio();
unsigned int * read_wav(char *name, unsigned int size);
void play_wav();
void read_wav_buffer (char *name, int size);
int volume = 0;

int size=0;




static void init_button_pio( )
{
    alt_irq_register( AUDIO_0_IRQ,NULL, play_wav );
    alt_up_audio_disable_write_interrupt(audio_dev);
}

void play_song(int song_to_play){
	a = song_to_play % num_songs;
	//if (a<num_songs){
		song b = getItemAt(songList, a);
		size = b.Size/2;
		init_button_pio();
		playSong = 0;
		//unsigned char * sound;
		usleep(1000000);
		started = 0;
		printf("calling method now to read file for song: %d\n",a);
		read_wav_buffer(b.name, b.Size);
		printf("returned from method, no of times: %d\n", noTimes);
		//a++;
		while (started == 1);
		started = 0;
		playSong=1;;
		//alt_up_audio_enable_write_interrupt(audio_dev);
		//alt_up_audio_enable_write_interrupt(audio_dev);
	//}
}



void play_wav() {
		alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
				ALT_UP_AUDIO_RIGHT);
		alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
				ALT_UP_AUDIO_LEFT);
		if ((190000*noTimes) + 100 + k >= size) {
			k = 0;
			noTimes=0;
			//playSong = 1;
			started = 0;
			a++;
			y=0;
			playing = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
			alt_up_audio_reset_audio_core(audio_dev);
			free(soundBuffer);
		} else{
			k += 100;

			if (k == 190000){
				k = 0;
				noTimes++;
			}
		}
}

void stop_sound(){
	alt_up_audio_disable_write_interrupt(audio_dev);
	stop_currently_playing = 1;
	noTimes = 0;
	started = 0;
	k=0;
	pause = 0;
}

void pause_sound(){
	pause = 1;
	alt_up_audio_disable_write_interrupt(audio_dev);
}

void resume_sound(){
	pause = 0;
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void next_sound(){
	stop_sound();
	++a;
	playSong =1;
	//play_song(a);
}

void previous_sound(){
	stop_sound();
	--a;
	playSong=1;
	//play_song(a);
}

void set_volume (char * message){
	char temp[4];
	temp[0] = message[6];
	temp[1] = message[7];
	temp[2] = message[8];
	temp[3] = '\0';
	int tempV = atoi(temp);
	volume = 10 - (tempV/10);
	printf("volume: %d\n", volume);
}
void read_wav_buffer (char *name, int size){
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
				int fileHandle2;
				short dataRead;
				short dataRead2;
				soundBuffer = (unsigned int *)malloc(sizeof(unsigned int)*190000);
				int i;
				for (i=0;i<190000;i++){
					soundBuffer[i] = 0;
				}
				song x = getItemAt(songList, a+1);
				fileHandle2 = alt_up_sd_card_fopen(x.name, false);
				fileHandle = alt_up_sd_card_fopen(name, false);
				printf("filehandle: %d, filehandle2: %d", fileHandle, fileHandle2);
				int j=0;

				stop_currently_playing = 0;
				int whenToStart = 190000;
				if (size/2 < 190000){
					whenToStart = size/2;
				}
				int setVolume;
				unsigned int temp;
				unsigned int temp2;
				unsigned int temp3;
				unsigned int temp4;
				int z= 0;
				// Get first byte of file
				dataRead2 = alt_up_sd_card_read(fileHandle2);
				dataRead = alt_up_sd_card_read(fileHandle);
				//while(y <= 190000)
				while (dataRead > -1){
					if (stop_currently_playing == 1){
						break;
					}
					if (started == 1){
						int x = 0;
						while (pause == 1);
						while(abs(y-k)<1000){
							x++;
							if (x == 10000000){

								printf("help, im stuck, with value of y: %d and k: %d\n",y,k);
								alt_up_audio_reset_audio_core(audio_dev);
								//k += 100;
								break;
							}
						}
					}
					while (z < 1000){
					temp = dataRead ;
					//temp3 = dataRead2;
					dataRead = alt_up_sd_card_read(fileHandle);
					//dataRead2 = alt_up_sd_card_read(fileHandle2);
					temp2 = dataRead ;
					//temp4 = dataRead2;
					dataRead = alt_up_sd_card_read(fileHandle);
					//dataRead2 = alt_up_sd_card_read(fileHandle2);
					soundBuffer[y] = ((temp2 << 8 | temp) << 8);// + ((temp4 << 8 | temp3) << 8);
					if(volume > 0){
						soundBuffer[y] = soundBuffer[y]/2;
					}
					y++;
					z++;
				}
				z = 0;
				while(z <= 1000){
					temp3 = dataRead2;
					dataRead2 = alt_up_sd_card_read(fileHandle2);
					temp4 = dataRead2;
					//dataRead2 = alt_up_sd_card_read(fileHandle2);
					//y++;
					z++;
					soundBuffer[y-z] = soundBuffer[y-z] + ((0 << 8 | 0) << 8);
					if (y-z == whenToStart){
						if (started == 0){
							alt_up_audio_enable_write_interrupt(audio_dev);
							started = 1;
							playing = 1;
						}
						y=0;
						j=0;
					}
				}
				z=0;
				}
				alt_up_sd_card_fclose(fileHandle);
				alt_up_sd_card_fclose(fileHandle2);
				//a++;
			}
		}
	}
}

int abs (int n) {
    const int ret[2] = { n, -n };
    return ret [n<0];
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
	int j=0;
	int fileHandle;
	 short dataRead;
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

