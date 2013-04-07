#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>

volatile alt_up_sd_card_dev *device_reference;
volatile listelement *songList;
volatile listelement *FXList;
volatile unsigned char * data;

int getSizeOfSong(char * name) {
	int fileHandle;
	short dataRead;
	// Get file handle
	fileHandle = alt_up_sd_card_fopen(name, false);
	int n = 0;
	data = (unsigned char *) malloc(32 * sizeof(unsigned char));
	// Get first 8 bytes of file
	while (n < 32) {
		// If new line character then write data to next line
		// and (increment column)
		dataRead = alt_up_sd_card_read(fileHandle);
		//dataRead=dataRead & '0000000011111111';
		data[n] = dataRead;
		//Print the bytes that are read
		//printf("%x ",data[n]);
		n++;
	}

	int total_size = (data[7] << 24) | (data[6] << 16) | (data[5] << 8)
			| data[4];
	//printf("%d ", total_size);

	total_size += 8;
	alt_up_sd_card_fclose(fileHandle);

	return total_size;
}
int getLengthOfSong(int total_size) {

	//Printing the size of the file in bytes as well as the byte rate
	//printf("%d\n", total_size);
	//
	int byte_rate = (data[31] << 24) | (data[30] << 16) | (data[29] << 8)
			| data[28];
	//printf("%d\n", byte_rate);
	int time = (total_size * 1000.0) / byte_rate;

	//Close file
	free(data);
	return time;
}

void getSongName(char * name, char * songname, char * songartist) {
	int fileHandle;
	short dataRead, dataRead2, dataRead3, dataRead4;
	// Get file handle
	fileHandle = alt_up_sd_card_fopen(name, false);
	int n = 0;
	//free(data);
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

	while ((!(dataRead == dataRead2 && dataRead2 == dataRead3) && n < 10)) {
		dataRead3 = dataRead2;
		dataRead2 = dataRead;
		dataRead = alt_up_sd_card_read(fileHandle);
		n++;
	}

	dataRead = alt_up_sd_card_read(fileHandle);
	n = 0;
	while (dataRead != 0) {
		data[n] = dataRead;
		n++;
		dataRead = alt_up_sd_card_read(fileHandle);
	}
	data[n] = '\0';
	strcpy(songartist, data);

	alt_up_sd_card_fclose(fileHandle);
}

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

	if (alt_up_sd_card_find_first(".", songFileName) != 0) {
		printf("Could not find any files in the SD card");
		return 0;
	} else {
		do {
			if (strstr(songFileName, "WAV") != NULL) {
				song x;
				x.Size = getSizeOfSong(songFileName);
				x.LENGTH = getLengthOfSong(x.Size);
				strcpy(x.name, songFileName);

				if (strstr(songFileName, "FX") == NULL) {
					x.ID = num_songs;
					getSongName(songFileName, x.realname, x.artist);
					free(data);
					songList = AddItem(songList, x, 0);
				} else {
					x.ID = num_FX;
					strcpy(x.realname, "dummy");
					strcpy(x.artist, "dummy");
					FXList = AddItem(FXList, x, 1);
				}

			}
		} while (alt_up_sd_card_find_next(songFileName) == 0);
	}
}
