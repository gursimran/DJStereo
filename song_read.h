#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>

listelement *songList;
unsigned char * data;

int getLengthOfSong(char * name){

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
		printf("%d ",dataRead);
		n++;
	}
	printf("\n");

	unsigned int total_size = (data[6] << 24 )| (data[7]<<16) | (data[5]<<8) | data[4];
	int byte_rate = (data[30]<<24) | (data[31]<<16) | (data[29]<<8) | data[28];
	total_size +=8;
	printf("%d\n", total_size);
	printf("%d\n", byte_rate);
	int time = (total_size/byte_rate);

	//Close file
	alt_up_sd_card_fclose(fileHandle);
	return time;
}

int readSongsFromSDCard() {
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
			strcpy(x.name, songFileName);
			songList = AddItem(songList, x);
		}
		while (alt_up_sd_card_find_next(songFileName) == 0) {
			if (strstr(songFileName, "WAV") != NULL) {
				song x;
				x.ID = num_songs;
				strcpy(x.name, songFileName);
				songList = AddItem(songList, x);
			}
		}
	}

}
