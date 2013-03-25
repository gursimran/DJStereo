#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>

listelement *songList;
unsigned char * data;


int getSizeOfSong(char * name){
	int fileHandle;
		short dataRead;
		// Get file handle
		fileHandle = alt_up_sd_card_fopen(name, false);
		int n = 0;
		data = (unsigned char *) malloc (32 * sizeof (unsigned char));
		// Get first 8 bytes of file
		while (n < 32){
			// If new line character then write data to next line
			// and (increment column)
			dataRead = alt_up_sd_card_read(fileHandle);
			//dataRead=dataRead & '0000000011111111';
			data[n] = dataRead;
			//Print the bytes that are read
			//printf("%x ",data[n]);
			n++;
		}

		int total_size = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
		//printf("%d ", total_size);

		total_size +=8;
		alt_up_sd_card_fclose(fileHandle);

		return total_size;
}
int getLengthOfSong(int total_size ){


	//Printing the size of the file in bytes as well as the byte rate
	//printf("%d\n", total_size);
	//
	int byte_rate = (data[31] << 24) | (data[30] << 16) | (data[29] << 8) | data[28];
	//printf("%d\n", byte_rate);
	int time = (total_size * 1000.0)/byte_rate;

	//Close file
	free(data);
	return time;
}


void readSongsFromSDCard() {
	num_songs = 0;
	char songFileName[20];
	alt_up_sd_card_dev *device_reference = NULL;
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

	if (alt_up_sd_card_find_first(".", songFileName) != 0) {
		printf("Could not find any files in the SD card");
		return 0;
	} else {
		if (strstr(songFileName, "WAV") != NULL) {
			song x;
			x.ID = num_songs;
			x.Size = getSizeOfSong(songFileName);
			x.LENGTH = getLengthOfSong(x.Size);
			strcpy(x.name, songFileName);
			songList = AddItem(songList, x);
		}
		while (alt_up_sd_card_find_next(songFileName) == 0) {
			if (strstr(songFileName, "WAV") != NULL) {
				song x;
				x.ID = num_songs;
				x.Size = getSizeOfSong(songFileName);
				x.LENGTH=getLengthOfSong(x.Size);
				strcpy(x.name, songFileName);
				songList = AddItem(songList, x);
			}
		}
	}
}
