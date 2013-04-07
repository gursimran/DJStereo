#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>


volatile alt_up_sd_card_dev *device_reference;
volatile listelement *songList;
volatile listelement *FXList;
volatile unsigned char * data;

//GET SIZE OF SONG
int getSizeOfSong(char * name) {
	int fileHandle;
	short dataRead;
	// Get file handle
	fileHandle = alt_up_sd_card_fopen(name, false);
	int n = 0;
	data = (unsigned char *) malloc(32 * sizeof(unsigned char));
	// Get first 32 bytes of file (size and byte rate) 
	while (n < 32) {
		dataRead = alt_up_sd_card_read(fileHandle);
		data[n] = dataRead;
		n++;
	}

    //Close the file
    alt_up_sd_card_fclose(fileHandle);
    
    //Calculate size
	int total_size = (data[7] << 24) | (data[6] << 16) | (data[5] << 8)
			| data[4];

    //Add 8 bytes to size
	total_size += 8;
	
	return total_size;
}

//GET LENGTH OF SONG IN MILLISECONDS
int getLengthOfSong(int total_size) {

	//Get byte rate of song
	int byte_rate = (data[31] << 24) | (data[30] << 16) | (data[29] << 8)
			| data[28];
    
    //Time = size * 1000 /byte rate
	int time = (total_size * 1000.0) / byte_rate;

	//Free file date
	free(data);
    
	return time;
}

//METHOD TO GET SONG NAME AND ARTIST IN HEADER OF FILE
void getSongName(char * name, char * songname, char * songartist) {
	
    int fileHandle;
	short dataRead, dataRead2, dataRead3, dataRead4;
	
    // Get file handle
	fileHandle = alt_up_sd_card_fopen(name, false);
	int n = 0;
	
    data = (unsigned char *) malloc(30 * sizeof(unsigned char));
	
    while (n < 56) {
		//Read 56 times to get to the right point in the file to read song name
		dataRead = alt_up_sd_card_read(fileHandle);
		n++;
	}
	n = 0;
	
    dataRead = alt_up_sd_card_read(fileHandle);

	//Read song name
	while (dataRead != 0 || n < 2) {
		data[n] = dataRead;
		n++;
		dataRead = alt_up_sd_card_read(fileHandle);
	}

	data[n] = '\0';
	strcpy(songname, data);

	dataRead3 = 1;
	dataRead2 = 2;
	dataRead4 = 3;
	dataRead = alt_up_sd_card_read(fileHandle);
	n = 0;

    //KEEP READING TILL "..." in FILE, REPRESENTS NEXT BYTE IS ARTIST
	while ((!(dataRead == dataRead2 && dataRead2 == dataRead3) && n < 10)) {
		dataRead3 = dataRead2;
		dataRead2 = dataRead;
		dataRead = alt_up_sd_card_read(fileHandle);
		n++;
	}

    //GET ARTIST
	dataRead = alt_up_sd_card_read(fileHandle);
	n = 0;
	while (dataRead != 0) {
		data[n] = dataRead;
		n++;
		dataRead = alt_up_sd_card_read(fileHandle);
	}
	data[n] = '\0';
	strcpy(songartist, data);

    //CLOST FILE
	alt_up_sd_card_fclose(fileHandle);
}

//METHOD TO READ ALL SONGS FROM SD CARD
void readSongsFromSDCard() {
	num_songs = 0;
	num_FX = 0;
	char songFileName[20];
	device_reference = alt_up_sd_card_open_dev(
			"/dev/Altera_UP_SD_Card_Avalon_Interface_0");

	if (device_reference == NULL) {
		printf("Could not read from the SDcard.\n");
		return 0;
	} else {
		if (!alt_up_sd_card_is_Present()) {
			printf("The SDcard is not present!\n");
			return 0;
		}

		else {
			if (!alt_up_sd_card_is_FAT16()) {
				printf(
						"The SDcard is not formatted to be FAT16 and could not be read.\n");
				return 0;
			}
		}
	}
   
    //CHECK IF FILES EXIST IN DCARD
	if (alt_up_sd_card_find_first(".", songFileName) != 0) {
		printf("Could not find any files in the SD card");
		return 0;
	} else {
		do {
            //CHECK IF FILE IS WAV FILE
			if (strstr(songFileName, "WAV") != NULL) {
				song x;
                
                //GET SIZE AND LENGTH OF SONG
				x.Size = getSizeOfSong(songFileName);
				x.LENGTH = getLengthOfSong(x.Size);
                
                //SAVE FILENAME TO SONG STRUCT
				strcpy(x.name, songFileName);

                //CHECK IF FILE IF AN EFFECT OR A SONG
				if (strstr(songFileName, "FX") == NULL) {
                    //GET SONG ID
					x.ID = num_songs;
                    
                    //GET ARTIST AND FULL SONG NAME FROM FILE HEARDER
					getSongName(songFileName, x.realname, x.artist);
                    
                    //FREE DATA READ FROM FILE
					free(data);
                    
                    //ADD SONG TO SONG LIST
					songList = AddItem(songList, x, 0);
				} else {
                    //GET EFFECTS ID
					x.ID = num_FX;
                    
                    //SET FULL NAME AND ARTIST TO DUMMY
					strcpy(x.realname, "dummy");
					strcpy(x.artist, "dummy");
                    
                    //ADD EFFECT TO EFFECT LIST
					FXList = AddItem(FXList, x, 1);
				}

			}
		} while (alt_up_sd_card_find_next(songFileName) == 0);
	}
}
