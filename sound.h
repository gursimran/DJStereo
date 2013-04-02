#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "io.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "assert.h"
#include "sys/alt_irq.h"

char mute = 0;
char playing = 0;
char started = 0;
int k = 0;
int i = 0;
int m = 0;
alt_up_audio_dev * audio_dev = NULL;
int a = 0;
int song1;
int song2;
int y = 0;
unsigned int * soundBuffer;
unsigned char * soundBuffer1DJ;
unsigned char * soundBuffer2DJ;
unsigned char * FX1Buffer;
unsigned char * FX2Buffer;
unsigned char * FX3Buffer;

int noTimes = 0;
int reach1000;
char readMore = 0;
void readWavFromSDCARD(char *name, unsigned char *levelBricksToDraw);
void configure_audio();
void read_wav(char *name, unsigned int size);
void read_FX(char *name, unsigned int size, unsigned char * buffer);
void read_wav2(char*name, unsigned int size);
void play_wav();
void dj_play_wav();
void read_wav_buffer(char *name, int size);
int volume = 5;
int djvolume1 = 5;
int djvolume2 = 5;
int speed1 = 1;
int speed2 = 1;
int stop = 1;
int FX1=0;
int FX2=0;
int FX3=0;

int buffer_size = 10000;

int size = 0;

static void init_button_pio() {
	alt_irq_register(AUDIO_0_IRQ, NULL, play_wav);
	alt_up_audio_disable_write_interrupt(audio_dev);
}

static void init_dj_interrupts() {
	printf("set dj interrupts\n");
	alt_irq_register(AUDIO_0_IRQ, NULL, dj_play_wav);
	alt_up_audio_disable_write_interrupt(audio_dev);
}

void play_song(int song_to_play) {

	a = song_to_play % num_songs;
	//if (a<num_songs){
	song b = getItemAt(songList, a);
	size = b.Size / 2;
	init_button_pio();
	playSong = 0;
	//unsigned char * sound;
	//usleep(1000000);
	started = 0;
	y = 0;
	printf("calling method now to read file for song: %d\n", a);
	read_wav_buffer(b.name, b.Size);
	printf("returned from method, no of times: %d\n", noTimes);
	//a++;
	while (started == 1)
		;
	started = 0;
	//playSong=1;
	//alt_up_audio_enable_write_interrupt(audio_dev);
	//alt_up_audio_enable_write_interrupt(audio_dev);
	//}
}

void DJPlay(int song1, int song2) {

	song1 = song1 % num_songs;
	song2 = song2 % num_songs;
	djplaysong = 0;

	song song11 = getItemAt(songList, song1);
	song song22 = getItemAt(songList, song2);
	song smallsong;
	song bigsong;
	int smallsize;

	if (song11.Size > song22.Size) {
		size = song11.Size;
		smallsize = song22.Size;
		smallsong = song22;
		bigsong = song11;
	} else {
		size = song22.Size;
		smallsize = song11.Size;
		smallsong = song11;
		bigsong = song22;
	}

	if (size > 3000000)
		size = 3000000;
	if (smallsize > 3000000)
		smallsize = 3000000;

	init_dj_interrupts();
	playSong = 0;
	djplaysong = 0;
	//unsigned char * sound;
	//usleep(1000000);
	started = 0;
	//y = 0;
	soundBuffer1DJ = (unsigned char *) malloc(sizeof(unsigned char) * size);
	soundBuffer2DJ
			= (unsigned char *) malloc(sizeof(unsigned char) * smallsize);
	soundBuffer = (unsigned int *) malloc(sizeof(unsigned int) * buffer_size);

	song fx1 = getItemAt(FXList,0);
	song fx2 = getItemAt(FXList, 1);
	song fx3 = getItemAt(FXList,2);
	FX1Buffer = (unsigned char *) malloc(sizeof(unsigned char)*fx1.Size);
	FX2Buffer = (unsigned char *) malloc(sizeof(unsigned char)*fx2.Size);
	FX3Buffer = (unsigned char *) malloc(sizeof(unsigned char)*fx3.Size);
	read_FX(fx1.name,fx1.Size,FX1Buffer );
	read_FX(fx2.name,fx2.Size,FX2Buffer );
	read_FX(fx3.name,fx3.Size,FX3Buffer );

	read_wav(bigsong.name, size);
	read_wav2(smallsong.name, smallsize);
	size = size / 2;

	//whenToStart gets the smaller of buffer_size or size
	int whenToStart = buffer_size;
	if (size < buffer_size) {
		whenToStart = size;
	}
	k = 0;
	int fx1point=0;
	int fx2point=0;
	int fx3point=0;
	int i = 0;
	int m = 0;
	int j = 0;
	int x = 0;
	int startedDJ = 0;
	int size1 = size * 2;
	int size2 = smallsize * 2;
	int speedcounter1=0;
	int speedcounter2=0;
	int speed1count1=0;
	int speed0count1=0;
	int speed2count1=0;
	int speed1count2=0;
	int speed2count2=0;
	int speed0count2=0;
	while ((i < size2 || m < size1) && stop == 0) {
		//Pausing reading of file if reading of file catches up to where playing of file is
		if (startedDJ == 1) {
			while (abs(j - k) < 2) {
				x++;
				if (x == 10000000) {

					printf("help, im stuck, with value of y: %d and k: %d\n",
							j, k);
					alt_up_audio_reset_audio_core(audio_dev);
					//k += 100;
					break;
				}
			}
		}
		unsigned int temp = (soundBuffer1DJ[m + 1] << 8) | soundBuffer1DJ[m];
		if ((temp & 0x8000) > 0) {
			temp = temp | 0xFFFF0000; // 2's complement
		}
		if (djvolume1 == 0) {
			temp = 0;
		} else {
			temp = temp << djvolume1;
		}
		if (m > size1){
			temp = 0;
		}
		else {
			if (speed1 == 1) {
				m = m + 2;
				speed1count1+=2;
			} else if (speed1 == 2) {
				m = m + 4;
				speed2count1+=4;
			} else if (speed1 == 0) {
				if(speedcounter1%2==0){
					m = m + 2;
					speedcounter1++;
				}
				speedcounter1++;
				speed0count1++;
			}
		}
		soundBuffer[j] = temp;
		temp = (soundBuffer2DJ[i + 1] << 8) | soundBuffer2DJ[i];
		if ((temp & 0x8000) > 0) {
			temp = temp | 0xFFFF0000; // 2's complement
		}
		if (djvolume2 == 0) {
			temp = 0;
		} else {
			temp = temp << djvolume2;
		}
		if (i > size2)
			temp = 0;
		else {
			if (speed2 == 1) {
				i = i + 2;
				speed1count2+=2;
			} else if (speed2 == 2) {
				i = i + 4;
				speed2count2+=4;
			} else if (speed2 == 0) {
				if(speedcounter2%2==0){
					i = i + 2;
					speedcounter2=0;
				}
				speedcounter2++;
				speed0count2++;
			}
		}
		size = size1/2 - m + (speed1count1 / 2) +(speed2count1 / 4) +speed0count1;
		smallsize = size2/2 - i + (speed1count2 / 2) + (speed2count2 / 4) + speed0count2;
		if (smallsize > size){
			int tempsize = size;
			size = smallsize;
			smallsize = tempsize;
		}

		soundBuffer[j] = soundBuffer[j] + (temp);

		if(FX1==1 && fx1point < fx1.Size){
			temp= (FX1Buffer[fx1point + 1] << 8) | FX1Buffer[fx1point];
			fx1point+=2;
			soundBuffer[j] = soundBuffer[j] + (temp<<8);
		}else{
			FX1=0;
			fx1point=0;
		}
		if(FX2==1 && fx2point < fx2.Size){
			temp= (FX2Buffer[fx2point + 1] << 8) | FX2Buffer[fx2point];
			fx2point+=2;
			soundBuffer[j] = soundBuffer[j] + (temp<<8);

		}else{
			FX2=0;
			fx2point=0;
		}
		if(FX3==1 && fx3point < fx3.Size){
			temp= (FX3Buffer[fx1point + 1] << 8) | FX3Buffer[fx1point];
			fx3point+=2;
			soundBuffer[j] = soundBuffer[j] + (temp<<8);

		}else{
			FX3=0;
			fx3point=0;
		}
		j++;



		if (j == whenToStart) {
			alt_up_audio_enable_write_interrupt(audio_dev);
			j = 0;
			startedDJ = 1;
		}
	}
}

void play_wav() {
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_RIGHT);
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_LEFT);
	if ((buffer_size * noTimes) + 100 + k >= size) {
		k = 0;
		noTimes = 0;
		//playSong = 1;
		started = 0;
		//a++;
		y = 0;
		playing = 0;
		alt_up_audio_disable_write_interrupt(audio_dev);
		alt_up_audio_reset_audio_core(audio_dev);
		free(soundBuffer);
		free(soundBuffer1DJ);
		free(soundBuffer2DJ);
	} else {
		k += 100;
		if (k == buffer_size) {
			k = 0;
			noTimes++;
		}
	}
}

void dj_play_wav() {
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_RIGHT);
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_LEFT);

	if ((buffer_size * noTimes) + 100 + k >= size) {
		k = 0;
		noTimes = 0;
		alt_up_audio_disable_write_interrupt(audio_dev);
		alt_up_audio_reset_audio_core(audio_dev);
		stop = 1;
		free(soundBuffer);
		free(soundBuffer1DJ);
		free(soundBuffer2DJ);
		free(FX1Buffer);
		free(FX2Buffer);
		free(FX3Buffer);
	} else {
		k += 100;
		if (k == buffer_size) {
			k = 0;
			noTimes++;
		}
	}
}

void stop_sound() {
	alt_up_audio_disable_write_interrupt(audio_dev);
	stop_currently_playing = 1;
	noTimes = 0;
	started = 1;
	k = 0;
	pause = 0;
	stop = 1;
	free(soundBuffer);
}

void pause_sound() {
	pause = 1;
	alt_up_audio_disable_write_interrupt(audio_dev);
}

void resume_sound() {
	pause = 0;
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void next_sound() {
	stop_sound();
	++a;
	started = 0;
	playSong = 1;
	//play_song(a);
}

void previous_sound() {
	stop_sound();
	--a;
	started = 0;
	playSong = 1;
	//play_song(a);
}

void set_song(char * message) {
	char temp[20];
	sscanf(message, "%s %d", temp, &a);
	if (started == 1) {
		stop_sound();
		started = 0;
	}
	playSong = 1;
	//djplaysong=0;
}

void set_dj(char * message) {
	if (started == 1) {
		stop_sound();
		started = 0;
	}
	printf("in dj\n");
	char temp[20];
	sscanf(message, "%s %d %d", temp, &song1, &song2);
	playSong = 0;
	speed1 = 1;
	speed2 = 1;
	stop = 0;
	djplaysong = 1;

	printf("djPlaySong: %d\n", djplaysong);
	//playSong=0;
}

void set_djvolume(char * message) {
	int tempV;
	int tempV2;
	char temp[20];
	sscanf(message, "%s %d %d", temp, &tempV, &tempV2);
	printf("tempV: %d\n", tempV);

	djvolume1 = tempV / 10;
	djvolume2 = tempV2 / 10;
	printf("volume1: %d volume2: %d\n", djvolume1, djvolume2);
}

void set_djspeed(char * message) {
	char temp[20];
	sscanf(message, "%s %d %d", temp, &speed1, &speed2);
	printf("speed1: %d\n speed2: %d\n", speed1, speed2);
}

void rewind_dj(char * message){
	char temp[20];
	int rewind1;
	int rewind2;

	sscanf(message, "%s %d %d", temp, &rewind1, &rewind2);
	if(rewind1 == 1){
		rewind1=0;
		if(m > 250000)
			m-=250000;
		else
			m=0;
	}
	if (rewind2==1){
		rewind2=0;
		if(i >250000)
			i-=250000;
		else
			i=0;
	}
}

void fastforward_dj(char * message){
	char temp[20];
	int ff1;
	int ff2;

	sscanf(message, "%s %d %d", temp, &ff1, &ff2);
	if(ff1 == 1){
		ff1=0;
		if(m > 250000)
			m-=250000;
		else
			m=0;
	}
	if (ff2==1){
		ff2=0;
		if(i >250000)
			i-=250000;
		else
			i=0;
	}
}

void set_volume(char * message) {
	int tempV;
	char temp[20];
	sscanf(message, "%s %d", temp, &tempV);
	printf("tempV: %d\n", tempV);
	volume = tempV / 10;
	printf("volume: %d\n", volume);
}
void read_wav_buffer(char *name, int size) {
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
				soundBuffer = (unsigned int *) malloc(sizeof(unsigned int)
						* buffer_size);
				int i;
				for (i = 0; i < buffer_size; i++) {
					soundBuffer[i] = 0;
				}
				//song x = getItemAt(songList, a+1);
				//fileHandle2 = alt_up_sd_card_fopen(x.name, false);
				fileHandle = alt_up_sd_card_fopen(name, false);
				//printf("filehandle: %d, filehandle2: %d", fileHandle, fileHandle2);
				int j = 0;

				stop_currently_playing = 0;
				int whenToStart = buffer_size;
				if (size / 2 < buffer_size) {
					whenToStart = size / 2;
				}
				unsigned int temp;
				unsigned int temp2;
				// Get first byte of file
				dataRead = alt_up_sd_card_read(fileHandle);
				while (dataRead > -1) {
					if (stop_currently_playing == 1) {
						break;
					}
					if (started == 1) {
						int x = 0;
						while (pause == 1)
							;
						while (abs(y - k) < 2) {
							x++;
							if (x == 10000000) {

								printf(
										"help, im stuck, with value of y: %d and k: %d\n",
										y, k);
								alt_up_audio_reset_audio_core(audio_dev);
								//k += 100;
								break;
							}
						}
					}
					temp = dataRead;
					dataRead = alt_up_sd_card_read(fileHandle);
					temp2 = dataRead;
					dataRead = alt_up_sd_card_read(fileHandle);
					soundBuffer[y] = ((temp2 << 8 | temp));
					if ((soundBuffer[y] & 0x8000) > 0) {
						soundBuffer[y] = soundBuffer[y] | 0xFFFF0000; // 2's complement
					}
					if (volume == 0) {
						soundBuffer[y] = 0;
					} else {
						soundBuffer[y] = soundBuffer[y] << volume;
					}
					y++;
					if (y == whenToStart) {
						if (started == 0) {
							alt_up_audio_enable_write_interrupt(audio_dev);
							started = 1;
							playing = 1;
						}
						y = 0;
						j = 0;
					}
				}

				alt_up_sd_card_fclose(fileHandle);
				//alt_up_sd_card_fclose(fileHandle2);
				//a++;
			}
		}
	}
}

int abs(int n) {
	const int ret[2] = { n, -n };
	return ret[n < 0];
}
void read_FX(char *name, unsigned int size, unsigned char * buffer) {
	int j = 0;
	int i = 0;
	int fileHandle;
	short dataRead;
	unsigned char temp, temp2;
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
				fileHandle = alt_up_sd_card_fopen(name, false);

				// Get first byte of file
				dataRead = alt_up_sd_card_read(fileHandle);

				// Keep reading till eof
				while (dataRead > -1) {
					buffer[j] = dataRead;
					dataRead = alt_up_sd_card_read(fileHandle);
					j++;
				}
				alt_up_sd_card_fclose(fileHandle);
			}
		}
	}
}
void read_wav(char *name, unsigned int size) {
	int j = 0;
	int i = 0;
	int fileHandle;
	short dataRead;
	unsigned char temp, temp2;
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
				fileHandle = alt_up_sd_card_fopen(name, false);

				// Get first byte of file
				dataRead = alt_up_sd_card_read(fileHandle);

				// Keep reading till eof
				while (dataRead > -1 && j < 3000000) {
					soundBuffer1DJ[j] = dataRead;
					dataRead = alt_up_sd_card_read(fileHandle);
					j++;
				}

				printf("number of reads from file: %d, %s\n", j, name);

				alt_up_sd_card_fclose(fileHandle);
			}
		}
	}
	//	readWavFromSDCARD(name, soundBuffer);
	//
	//	unsigned int * temp_array;
	//	temp_array = (unsigned int *) malloc((size / 2) * sizeof(unsigned int));
	//	int x;
	//	int y = 0;
	//	for (x = 0; x < size; x += 2) {
	//		unsigned int sample = (levelBricksToDraw[x + 1] << 8 | levelBricksToDraw[x]) << 8; //original
	//		temp_array[y] = sample;
	//		if(y%1000==0)
	//			printf("%d",y);
	//		y++;
	//	}
	//	free(levelBricksToDraw);
	//	return temp_array;
}

void read_wav2(char*name, unsigned int size) {
	int j = 0;
	int i = 0;
	int fileHandle;
	short dataRead;
	unsigned char temp, temp2;
	unsigned int tempSample;
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
				fileHandle = alt_up_sd_card_fopen(name, false);

				// Get first byte of file
				dataRead = alt_up_sd_card_read(fileHandle);

				// Keep reading till eof
				while (dataRead > -1 && j < 3000000) {
					soundBuffer2DJ[j] = dataRead;
					dataRead = alt_up_sd_card_read(fileHandle);
					j++;
				}

				printf("number of reads from file: %d, %s\n", j, name);

				alt_up_sd_card_fclose(fileHandle);
			}
		}
	}
}

///////////////////////////////////////////*******?//////////////////////////////////////////////
//void readWavFromSDCARD(char * name, unsigned char *levelBricksToDraw) {
//	int j=0;
//	int fileHandle;
//	 short dataRead;
//	alt_up_sd_card_dev *device_reference = NULL;
//	device_reference = alt_up_sd_card_open_dev(
//			"/dev/Altera_UP_SD_Card_Avalon_Interface_0");
//	if (device_reference == NULL) {
//		printf("Could not read from the SDcard.\n");
//	} else {
//		if (!alt_up_sd_card_is_Present()) {
//			printf("The SDcard is not present!\n");
//		}
//
//		else {
//			if (!alt_up_sd_card_is_FAT16()) {
//				printf(
//						"The SDcard is not formatted to be FAT16 and could not be read.\n");
//			}
//			else{
//				fileHandle = alt_up_sd_card_fopen(name, false);
//
//					// Get first byte of file
//					dataRead = alt_up_sd_card_read(fileHandle);
//
//					// Keep reading till eof
//					while (dataRead > -1 && j < 2500000) {
//						levelBricksToDraw[j] = dataRead;
//						j++;
//						dataRead = alt_up_sd_card_read(fileHandle);
//					}
//
//					printf("number of reads: %d\n", j);
//
//					alt_up_sd_card_fclose(fileHandle);
//			}
//		}
//	}
//}

//////////////////////////////////////////////////******/////////////////////////////////////////
void configure_audio() {
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
