//STRUCTURE FOR QUEUE
typedef struct {
    song     dataitem;
    struct listelement *link;
}               listelement;


//ADD, REMOVE, GET TO ITEM FUNCTIONS FOR QUEUES
listelement * AddItem (listelement * listpointer, song data);
listelement * RemoveItem (listelement * listpointer);
song getItem (listelement * listpointer);
int num_songs = 0;

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
        listpointer -> dataitem.LENGTH = data.LENGTH;
        num_songs++;
        return lp;
    }
    //IF LIST IS EMPTY CREATE A NEW STRUCT AND ADD THE ITEM
    else {
    	listpointer = (struct listelement  *) malloc (sizeof (listelement));
        listpointer -> link = NULL;
        listpointer -> dataitem.ID = data.ID;
        strcpy(listpointer -> dataitem.name, data.name);
        listpointer -> dataitem.LENGTH = data.LENGTH;
        num_songs++;
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
