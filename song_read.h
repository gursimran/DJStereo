#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <altera_up_sd_card_avalon_interface.h>

char **song_list;
int i=1;
int num_songs;
listelement *songlist;

int readSongsFromSDCard(){

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

	if(alt_up_sd_card_find_first(".", song_list[0]) != 0){
		printf("Could not find any files in the SD card");
		return 0;
	}
	else{
		if(strstr(song_list[0],"WAV")==NULL)
			i=0;
		else{
			song y;
			y.ID=0;
			strcpy(y.name, song_list[0]);
			songlist = AddItem(songlist, y);

			//printf("%s", songlist->dataitem.name);

		}

		while(alt_up_sd_card_find_next(song_list[i])==0){
			if(strstr(song_list[i],"WAV")!=NULL)
			{

				song x;
				x.ID=i;
				strcpy(x.name, song_list[i]);
				songlist = AddItem(songlist, x);
				i++;
			}
		}
		if(strstr(song_list[i],"WAV")!=NULL)
			i--;
	}
}
