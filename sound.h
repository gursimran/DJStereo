#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "io.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "assert.h"
#include "sys/alt_irq.h"

//METHOD DECLARATIONS
void readWavFromSDCARD(char *name, unsigned char *levelBricksToDraw);
void configure_audio();
void read_wav(char *name, unsigned int size, unsigned char* soundbuff);
void play_wav();
void dj_play_wav();
void read_wav_buffer(char *name, int size);

//GLOBAL VARIABLES FOR SOUND
volatile char mute = 0;
volatile  char playing = 0;
volatile  char started = 0;
volatile  int k = 0;
volatile  int i = 0;
volatile  int m = 0;
volatile  alt_up_audio_dev * audio_dev = NULL;
volatile  int a = 0;
volatile  int song1;
volatile  int song2;
volatile  int y = 0;
volatile  char stillComputingDataToBeSent = 0;
volatile  unsigned int * soundBuffer;
volatile  unsigned char * soundBuffer1DJ;
volatile  unsigned char * soundBuffer2DJ;
volatile  unsigned char * FX1Buffer;
volatile  unsigned char * FX2Buffer;
volatile  unsigned char * recordBuffer;
volatile  int record = 0;
volatile  int noTimes = 0;
volatile  int reach1000;
volatile  char readMore = 0;
int volume = 5;
int djvolume1 = 7;
int djvolume2 = 7;
int speed1 = 1;
int speed2 = 1;
int stop = 1;
int FX1 = 0;
int FX2 = 0;
int record_fileHandle;
int record_done;
int buffer_size = 10000;
int rwff=0;
int song1edit=0;
int song2edit=0;
int fx1point = 0;
int size = 0;
int smallsize = 0;


//SET SOUND INTERRUPT ISR FOR REGULAR MUSIC PLAY
static void init_button_pio() {
	alt_irq_register(AUDIO_0_IRQ, NULL, play_wav);
	alt_up_audio_disable_write_interrupt(audio_dev);
}

//SET SOUND INTERRUPT ISR FOR DJ SONG
static void init_dj_interrupts() {
	printf("set dj interrupts\n");
	alt_irq_register(AUDIO_0_IRQ, NULL, dj_play_wav);
	alt_up_audio_disable_write_interrupt(audio_dev);
}


//Method to play next song
void play_song(int song_to_play) {
	a = song_to_play % num_songs;
    //Get song info
	song b = getItemAt(songList, a);
	
    //set size
    size = b.Size / 2;
	
    //set interrupts
    init_button_pio();
    
	playSong = 0;
	started = 0;
	y = 0;
	printf("calling method now to read file for song: %d\n", a);
	//dynamically load file
    read_wav_buffer(b.name, b.Size);
	printf("returned from method, no of times: %d\n", noTimes);
	//wait till song finishes
    while (started == 1);
	started = 0;
}

void DJPlay(int song1, int song2) {

	song1 = song1 % num_songs;
	song2 = song2 % num_songs;
	djplaysong = 0;
	char file_start[85] = { 0x52, 0x49, 0x46, 0x46, 0x50, 0xA6, 0x0E, 0x00,
			0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00,
			0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x7D, 0x00, 0x00, 0x00, 0xFA,
			0x00, 0x00, 0x02, 0x00, 0x10, 0x00, 0x4C, 0x49, 0x53, 0x54, 0x24,
			0x00, 0x00, 0x00, 0x49, 0x4E, 0x46, 0x4F, 0x49, 0x4E, 0x41, 0x4D,
			0x0C, 0x00, 0x00, 0x00, 0x41, 0x57, 0x45, 0x53, 0x4F, 0x4D, 0x45,
			0x20, 0x4D, 0x49, 0x58, 0x00, 0x49, 0x41, 0x52, 0x54, 0x04, 0x00,
			0x00, 0x00, 0x59, 0x4F, 0x55, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00 };

	song song11 = getItemAt(songList, song1);
	song song22 = getItemAt(songList, song2);
	song smallsong;
	song bigsong;

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
	//unsigned char * sound;
	//usleep(1000000);
	//y = 0;
	soundBuffer1DJ = (unsigned char *) malloc(sizeof(unsigned char) * size);
	soundBuffer2DJ
			= (unsigned char *) malloc(sizeof(unsigned char) * smallsize);
	soundBuffer = (unsigned int *) malloc(sizeof(unsigned int) * buffer_size);
	recordBuffer = (unsigned char *) malloc(sizeof(unsigned char) * 960088);
	//3000000 bytes (3MB) equates to about 40 seconds of song

	song fx1 = getItemAt(FXList, 0);
	song fx2 = getItemAt(FXList, 1);
	FX1Buffer = (unsigned char *) malloc(sizeof(unsigned char) * fx1.Size);
	FX2Buffer = (unsigned char *) malloc(sizeof(unsigned char) * fx2.Size);
	read_wav(fx1.name, fx1.Size, FX1Buffer);
	read_wav(fx2.name, fx2.Size, FX2Buffer);

	read_wav(bigsong.name, size, soundBuffer1DJ);
	read_wav(smallsong.name, smallsize, soundBuffer2DJ);
	sendData("djdjdjdjdjdjdjdjdjdjdjdj");
	size = size / 2;
	smallsize = smallsize / 2;

	//whenToStart gets the smaller of buffer_size or size
	int whenToStart = buffer_size;
	if (size < buffer_size) {
		whenToStart = size;
	}
	k = 0;
	int fx2point = 0;
	i = 0;
	m = 0;
	int j = 0;
	int x = 0;
	int startedDJ = 0;
	int size1 = size * 2;
	int size2 = smallsize * 2;
	int speedcounter1 = 0;
	int speedcounter2 = 0;
	int record_point = 0;
	record = 0;
	unsigned char byteToRecord;
	record_done = 0;
	while ((i < size2 || m < size1) && stop == 0) {
		//Pausing reading of file if reading of file catches up to where playing of file is
		if (startedDJ == 1) {
			while (abs(j - k) < 2) {
				x++;
				if (x == 10000000) {

					printf("help, im stuck, with value of i: %d and m: %d\n",
							i, m);
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
		if (m >= size1) {
			temp = 0;
			m = size1 + 1;
		} else if(rwff==1){
			temp=0;
		}else {
			if (speed1 == 1) {
				m = m + 2;
			} else if (speed1 == 2) {
				m = m + 4;
				size--;
			} else if (speed1 == 0) {
				if (speedcounter1 % 2 == 0) {
					m = m + 2;
					speedcounter1++;
				}
				speedcounter1++;
				size++;
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
		if (i >= size2) {
			temp = 0;
			i = size2 + 1;
		}else if(rwff==2){
			temp=0;
		}else {
			if (speed2 == 1) {
				i = i + 2;
			} else if (speed2 == 2) {
				i = i + 4;
				smallsize--;
			} else if (speed2 == 0) {
				if (speedcounter2 % 2 == 0) {
					i = i + 2;
					speedcounter2 = 0;
				}
				speedcounter2++;
				smallsize++;
			}
		}

		soundBuffer[j] = soundBuffer[j] + (temp);

		if (FX1 == 1 && fx1point < fx1.Size) {
			temp = (FX1Buffer[fx1point + 1] << 8) | FX1Buffer[fx1point];
			fx1point += 2;
			soundBuffer[j] = soundBuffer[j] + (temp << 8);
		}else {
			rwff=0;
			FX1 = 0;
			fx1point = 0;
			song1edit=0;
			song2edit=0;
		}
		if (FX2 == 1 && fx2point < fx2.Size) {
			temp = (FX2Buffer[fx2point + 1] << 8) | FX2Buffer[fx2point];
			fx2point += 2;
			soundBuffer[j] = soundBuffer[j] + (temp << 8);

		} else {
			FX2 = 0;
			fx2point = 0;
		}
		if (record == 1 && record_point < 85) {
			if (record_point == 0)
				printf("starting to record\n");
			byteToRecord = file_start[record_point];
			recordBuffer[record_point] = byteToRecord;
			record_done = 1;
			record_point++;

			//alt_up_sd_card_write(record_fileHandle, byteToRecord);
		} else if (record == 1 && record_point < 960087) {
			if (record_point == 85)
				printf("recording\n");
			byteToRecord = (soundBuffer[j] >> 16) & 0xff;
			recordBuffer[record_point] = byteToRecord;
			record_point++;

			//alt_up_sd_card_write(record_fileHandle,byteToRecord);
			byteToRecord = (soundBuffer[j] >> 8) & 0xff;
			recordBuffer[record_point] = byteToRecord;
			record_point++;

			//alt_up_sd_card_write(record_fileHandle,byteToRecord);

		} else if (record == 1 && record_point >= 960087) {
			printf("finished recording\n");
			record = 0;
			record_point = 0;
			//alt_up_sd_card_fclose(record_fileHandle);

		} else {
			record_point = 0;
			record = 0;
		}

		j++;
		if (j == whenToStart) {
			alt_up_audio_enable_write_interrupt(audio_dev);
			j = 0;
			startedDJ = 1;
		}
	}
	size = (buffer_size * noTimes) + 100 + k;
	smallsize = (buffer_size * noTimes) + 100 + k;

}


//ISR for regular music song
void play_wav() {
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_RIGHT);
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_LEFT);
    
    //check if number of bytes played have been played as size of song
	if ((buffer_size * noTimes) + 100 + k >= size) {
		//reset song variables and disable song interrupt
        k = 0;
		noTimes = 0;
		started = 0;
		y = 0;
		playing = 0;
		alt_up_audio_disable_write_interrupt(audio_dev);
		alt_up_audio_reset_audio_core(audio_dev);

        //free all buffers
		free(soundBuffer);
		free(soundBuffer1DJ);
		free(soundBuffer2DJ);
	} else {
        
        //load the next 100 bytes of data in sound buffer
		k += 100;
		if (k == buffer_size) {
			k = 0;
			noTimes++;
		}
	}
}


//ISR for dj songs
void dj_play_wav() {
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_RIGHT);
	alt_up_audio_write_fifo(audio_dev, &(soundBuffer[k]), 100,
			ALT_UP_AUDIO_LEFT);

    //check if the bytes played is equal to the size of song
	if (((buffer_size * noTimes) + 100 + k >= size && (buffer_size * noTimes)
			+ 100 + k >= smallsize) || stop == 1) {
        //reset variables to be played again
		k = 0;
		noTimes = 0;
		alt_up_audio_disable_write_interrupt(audio_dev);
		alt_up_audio_reset_audio_core(audio_dev);
        
        //free buffers
		free(soundBuffer);
		free(soundBuffer1DJ);
		free(soundBuffer2DJ);
		free(FX1Buffer);
		free(FX2Buffer);
        
        //if user chose to record then write the recording to file
		if (record_done == 1) {
			int x;
			for (x = 0; x < 960087; x++)
				alt_up_sd_card_write(record_fileHandle, recordBuffer[x]);
			alt_up_sd_card_fclose(record_fileHandle);
			printf("done writing to sd card\n");
		}
        
        //free record buffer
		free(recordBuffer);

        //reset other variables
		mute = 0;
		playing = 0;
		started = 0;
		i = 0;
		m = 0;
		a = 0;
		y = 0;
		size = 0;
		smallsize = 0;
		stop_sound();
	}
    //load next 100 bytes to be played
    else {
		k += 100;
		if (k == buffer_size) {
			k = 0;
			noTimes++;
		}
	}
}

//initialize the 
void record_song() {
	record_fileHandle = alt_up_sd_card_fopen("MIX.WAV", false);
	record = 1;
}


//Stop currently playing
void stop_sound() {
    
    //reset all variables
	alt_up_audio_disable_write_interrupt(audio_dev);
	stop_currently_playing = 1;
	noTimes = 0;
	started = 1;
	k = 0;
	pause = 0;
	playSong = 0;
	djplaysong=0;
	free(soundBuffer);
}


//Pause currently playing song
void pause_sound() {
	pause = 1;
	alt_up_audio_disable_write_interrupt(audio_dev);
}


//Resume song that was being played
void resume_sound() {
	pause = 0;
	alt_up_audio_enable_write_interrupt(audio_dev);
}


//Get song to play in music player mode
void set_song(char * message) {
	char temp[20];
	sscanf(message, "%s %d", temp, &a);
	if (started == 1) {
		stop_sound();
		started = 0;
	}
	playSong = 1;
}

//Get the two songs to play in dj 
void set_dj(char * message) {
		stop_sound();
		started = 0;
	char temp[20];
	sscanf(message, "%s %d %d", temp, &song1, &song2);
	playSong = 0;
	speed1 = 1;
	speed2 = 1;
	stop = 0;
	djplaysong = 1;
}

//Set volume of each song in dj mode
void set_djvolume(char * message) {
	int tempV;
	int tempV2;
	char temp[20];
	sscanf(message, "%s %d %d", temp, &tempV, &tempV2);

	djvolume1 = tempV / 10;
	djvolume2 = tempV2 / 10;
}

//Set speed of each song in DJ mode
void set_djspeed(char * message) {
	char temp[20];
	sscanf(message, "%s %d %d", temp, &speed1, &speed2);
	printf("speed1: %d\n speed2: %d\n", speed1, speed2);
}


//rewind song by 5 secs
void rewind_dj(char * message) {
	char temp[5];
	int rewind1;
	int rewind2;

    //get which song to rewind from android command
    
	sscanf(message, "%s %d %d", temp, &rewind1, &rewind2);
	
    //if song 1 needs to be rewinded
    if (rewind1 == 1) {
		rewind1 = 0;
        
        //if current position is less than 250,000 then set it to 0 (start of data) else minus 250,000
		if (m > 250000) {
			m -= 250000;
			size += 125000;
		} else {
			size += m/2;
			m = 0;
		}
		rwff=1;
        
         //play scratching effect
		if(song1edit==0)
		size+=17429-(fx1point/2);
		song1edit=1;
	}
    
    //if song 2 needs to be rewinded
	if (rewind2 == 1) {
		rewind2 = 0;
        
        //if current position is less than 250,000 then set it to 0 (start of data) else minus 250,000
		if (i > 250000) {
			i -= 250000;
			smallsize += 125000;
		} else {
			smallsize += i/2;
			i = 0;
		}
		rwff=2;
        
         //play scratching effect
		if(song2edit==0)
			smallsize+=17429-(fx1point/2);
		song2edit=1;

	}
}


//fast forward song by 5 secs
void fastforward_dj(char * message) {
	char temp[5];
	int ff1;
	int ff2;

    //get which song to fast forward from android command
	sscanf(message, "%s %d %d", temp, &ff1, &ff2);
    
    //if song 1 needs to be fast forward
	if (ff1 == 1) {
		ff1 = 0;
        
        //if current position is less than 2,750,000 then set it to 3,000,000 (end of data) else add 250,000
		if (m < 2750000) {
			m += 250000;
			size -= 125000;
		} else {
			size -= (3000000 - m)/2;
			m = 3000000;
		}
        
        //play scratching effect
		rwff=1;
		if(song1edit==0)
			size+=17429-(fx1point/2);
		song1edit=1;
	}
    
    //if song 2 needs to be fast forward
	if (ff2 == 1) {
		ff2 = 0;
        
        //if current position is less than 2,750,000 then set it to 3,000,000 (end of data) else add 250,000
		if (i < 2750000) {
			i += 250000;
			smallsize -= 125000;
		} else {
			smallsize -= (3000000 - i)/2;
			i = 3000000;
		}
		rwff=2;
        
        //play scratching effect 
		if(song2edit==0)
			smallsize+=17429-(fx1point/2);
		song2edit=1;
	}
}


//Get volume from command sent by android
void set_volume(char * message) {
	int tempV;
	char temp[20];
	sscanf(message, "%s %d", temp, &tempV);
	volume = tempV / 10;
}


//Dynamically play wav file
void read_wav_buffer(char *name, int size) {

	int fileHandle;
	short dataRead;
    
    //malloc  to buffer
	soundBuffer = (unsigned int *) malloc(sizeof(unsigned int) * buffer_size);
	int q;
    
    //set soundbuffer to 0
	for (q = 0; q < buffer_size; q++) {
		soundBuffer[q] = 0;
	}
    
    //get file handle
	fileHandle = alt_up_sd_card_fopen(name, false);
	int j = 0;

	stop_currently_playing = 0;
	int whenToStart = buffer_size;
	
    //set size
    if (size / 2 < buffer_size) {
		whenToStart = size / 2;
	}
	unsigned int temp;
	unsigned int temp2;
	// Get first byte of file
	dataRead = alt_up_sd_card_read(fileHandle);
	
    //keep reading till eof
    while (dataRead > -1) {
        
        //if want to stop then break out of loop
		if (stop_currently_playing == 1) {
			break;
		}
        
        //if song is playing then make sure do not override data that has not been consumed by audio buffer
		if (started == 1) {
			int x = 0;
            //if user wants to pause then stay in while loop
			while (pause == 1);
            
            //make sure puttin data in buffer and consumption data is atleast 2 bytes apart
			while (abs(y - k) < 2) {
				x++;
				if (x == 10000000) {
                    //check if stuck in while loop
					printf("help, im stuck, with value of y: %d and k: %d\n",
							y, k);
					alt_up_audio_reset_audio_core(audio_dev);
					break;
				}
			}
		}
        
        //read 2 bytes of data from sd card
		temp = dataRead;
		dataRead = alt_up_sd_card_read(fileHandle);
		temp2 = dataRead;
		dataRead = alt_up_sd_card_read(fileHandle);
        
        //create one sample from 2 bytes
		soundBuffer[y] = ((temp2 << 8 | temp));
        
        // take 2's compliment if sample is greater than 32768
		if ((soundBuffer[y] & 0x8000) > 0) {
			soundBuffer[y] = soundBuffer[y] | 0xFFFF0000; // 2's complement
		}
        
        //if volume is zero then mute else set volume
		if (volume == 0) {
			soundBuffer[y] = 0;
		} else {
			soundBuffer[y] = soundBuffer[y] << volume;
		}
		y++;
        
        //reset position of where to read next sample when reached the end of buffer
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
}

//get absolute value of number
int abs(int n) {
	const int ret[2] = { n, -n };
	return ret[n < 0];
}


// Read wav to buffer
void read_wav(char*name, unsigned int size, unsigned char * soundbuff) {
	int j = 0;
	int fileHandle;
	short dataRead;
	fileHandle = alt_up_sd_card_fopen(name, false);

	// Get first byte of file
	dataRead = alt_up_sd_card_read(fileHandle);

	// Keep reading till eof
	while (dataRead > -1 && j < 3000000) {
		soundbuff[j] = dataRead;
		dataRead = alt_up_sd_card_read(fileHandle);
		j++;
	}

	printf("number of reads from file: %d, %s\n", j, name);

	alt_up_sd_card_fclose(fileHandle);
}


//COnfigure audio device
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
