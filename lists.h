//STRUCTURE FOR QUEUE
typedef struct {
    song     dataitem;
    struct listelement *link;
}listelement;

int send_num_songs = 3;
int sent_songs = 0;
//ADD, REMOVE, GET TO ITEM FUNCTIONS FOR QUEUES
listelement * AddItem (listelement * listpointer, song data);
listelement * RemoveItem (listelement * listpointer);
song getItem (listelement * listpointer);
int num_songs = 0;
int num_FX = 0;
char * songString;

//ADDITEM TO QUEUE
listelement * AddItem (listelement * listpointer, song data) {
    listelement * lp = listpointer;
    //IF LIST IS NOT NULL THEN CREATE A NEW STRUCT THEN ADD THE ITEM AND POINT THE LISTPOINTER OF THE LAST ITEM BEFORE ADDING TO THE NEW STRUCT

    if (listpointer != NULL) {
    	while (listpointer -> link != NULL)
            listpointer = listpointer -> link;

        listpointer -> link = (struct listelement  *) malloc (sizeof (listelement));
        listpointer = listpointer -> link;
        listpointer -> link = NULL;
        listpointer -> dataitem.ID = data.ID;
        strcpy(listpointer -> dataitem.name, data.name);
        strcpy(listpointer->dataitem.artist, data.artist);
        strcpy(listpointer->dataitem.realname, data.realname);
        listpointer -> dataitem.LENGTH = data.LENGTH;
        listpointer -> dataitem.Size = data.Size;
        num_songs++;
        return lp;
    }
    //IF LIST IS EMPTY CREATE A NEW STRUCT AND ADD THE ITEM
    else {
    	listpointer = (struct listelement  *) malloc (sizeof (listelement));
        listpointer -> link = NULL;
        listpointer -> dataitem.ID = data.ID;
        strcpy(listpointer -> dataitem.name, data.name);
        strcpy(listpointer->dataitem.artist, data.artist);
        strcpy(listpointer->dataitem.realname, data.realname);
        listpointer -> dataitem.LENGTH = data.LENGTH;
        listpointer -> dataitem.Size = data.Size;
        num_songs++;
        return listpointer;
    }
}
//ADDITEM TO QUEUE
listelement * AddItem2 (listelement * listpointer, song data) {
    listelement * lp = listpointer;
    //IF LIST IS NOT NULL THEN CREATE A NEW STRUCT THEN ADD THE ITEM AND POINT THE LISTPOINTER OF THE LAST ITEM BEFORE ADDING TO THE NEW STRUCT

    if (listpointer != NULL) {
    	while (listpointer -> link != NULL)
            listpointer = listpointer -> link;

        listpointer -> link = (struct listelement  *) malloc (sizeof (listelement));
        listpointer = listpointer -> link;
        listpointer -> link = NULL;
        listpointer -> dataitem.ID = data.ID;
        strcpy(listpointer -> dataitem.name, data.name);
        strcpy(listpointer->dataitem.artist, data.artist);
        strcpy(listpointer->dataitem.realname, data.realname);
        listpointer -> dataitem.LENGTH = data.LENGTH;
        listpointer -> dataitem.Size = data.Size;
        num_FX++;
        return lp;
    }
    //IF LIST IS EMPTY CREATE A NEW STRUCT AND ADD THE ITEM
    else {
    	listpointer = (struct listelement  *) malloc (sizeof (listelement));
        listpointer -> link = NULL;
        listpointer -> dataitem.ID = data.ID;
        strcpy(listpointer -> dataitem.name, data.name);
        strcpy(listpointer->dataitem.artist, data.artist);
        strcpy(listpointer->dataitem.realname, data.realname);
        listpointer -> dataitem.LENGTH = data.LENGTH;
        listpointer -> dataitem.Size = data.Size;
        num_FX++;
        return listpointer;
    }
}

//REMOVE ITEM FROM QUEUE
listelement * RemoveItem (listelement * listpointer) {
    //REMOVE FIRST ELEMENT AND FREE MEMORY
    listelement * tempp;
    tempp = listpointer -> link;
    free (listpointer);
    return tempp;
}
//GET THE FIRST ITEM IN QUEUE
song getItem (listelement * listpointer) {
    return listpointer -> dataitem;
}


void PrintQueue (listelement * listpointer) {

    if (listpointer == NULL)
	printf ("queue is empty!\n");
    else
	while (listpointer != NULL) {
	    printf ("%d\t %s\n", listpointer -> dataitem.ID, listpointer -> dataitem.name);
	    listpointer = listpointer -> link;
	}
    printf ("\n");
}


song getItemAt (listelement * listpointer, int location){
	//printf("getting called\n");
	int loc = location;
	//printf("loc: %d\n",loc);
	--loc;
	//printf("\n %d test \n",location);
	int i;
	if (listpointer != NULL){
		for (i=0;i<=loc; i++){
			//printf("%d\n",i);
			listpointer = listpointer -> link;
		}
	}
	return listpointer -> dataitem;
}

void song_string(listelement * listpointer){
	int l;
	char k[20];
	songString = (char*)malloc (num_songs * (sizeof(song)+(sizeof(char)*3)));
	//strcpy(songString, "");
	if (sent_songs + send_num_songs > num_songs){
		send_num_songs = num_songs - sent_songs;
	}
	sent_songs = sent_songs + send_num_songs;
	for(l=0; l<send_num_songs; l++){
		song m = getItemAt(listpointer, sent_songs-send_num_songs+l);
		sprintf(k, "%d", m.ID);
		if (l == 0){
			strcpy(songString, k);
		}
		else{
			strcat(songString, k);
		}
		strcat(songString, ":");
		strcat(songString, m.realname);
		strcat(songString, ":");
		sprintf(k, "%d", m.LENGTH);
		strcat(songString, k);
		strcat(songString, ":");
		strcat(songString, m.artist);
		if(sent_songs-send_num_songs+l== (num_songs-1)){
			if (sent_songs == num_songs){
				strcat(songString, "|xx");
				startedSendingList = 0;
			}
			strcat(songString, "\0");
		}
		else {
			strcat(songString, "|");
		}
	}
}
