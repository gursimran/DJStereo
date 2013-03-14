#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>

listelement *songList;

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
