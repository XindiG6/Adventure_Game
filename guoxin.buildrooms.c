/******************************************
*Program: guoxin.buildrooms.c
*Author: Xindi Guo
*Date: 2/7/2020
*Description: builds rooms for the game.
*Work cited: canvas materials
            https://benpfaff.org/writings/clc/shuffle.html
******************************************/


#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

char folderName[256];
char *room_names[10] = {"yellow", "blue", "white", "red", "black", "brown", "pink", "green", "purple", "orange"};
char *room_types[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};

struct Room{
    char *name;
    char *type;
    int connections[7];
    int numconnections;
};

/****************************************************
name: create_dir
fucntionality: create the directory with prefix
*****************************************************/
char *create_dir(){
  sprintf(folderName, "guoxin.rooms.%d", getpid());
  mkdir(folderName, 0700);
  return folderName;
}

/****************************************************
name:shuffle
fucntionality: shuffle the array that contains the numbers
*****************************************************/
void shuffle(int *randoms){
  int n=10;
  int i,j,t;
  if(n>1){  //https://stackoverflow.com/questions/6127503/shuffle-array-in-c
          for (i = 0; i < n - 1; i++)
          {
            j = i + rand()/(RAND_MAX / (n - i) + 1); //shuffle the array that holds the numbers
            t = randoms[j];
            randoms[j] = randoms[i];
            randoms[i] = t;
          }
      }
  }

  /****************************************************
  name: create_rooms
  fucntionality: initialize the rooms
  *****************************************************/
void create_rooms(struct Room *rooms){
  int i,j,p;
  int *randoms = malloc(10 * sizeof *randoms);
  for(p=0; p<10; p++){
	randoms[p]=p;
  }
  shuffle(randoms);// call function to shuffle
  for(i=0;i<7;i++){
    rooms[i].name=room_names[randoms[i]]; //randomly set the room info
    rooms[i].type=room_types[1];
    rooms[i].numconnections=0;
  }
  rooms[0].type=room_types[0];// set the first room and the last room as the start and end room
  rooms[6].type=room_types[2];
  free(randoms);
}

/****************************************************
name: room_connections
fucntionality: make room connects to each other
*****************************************************/
void room_connections(struct Room *rooms){
  int i, j, k, l, count=0;
  for(i=0;i<7;i++){
    for(j=0;j<7;j++){
      //not to connect to itself
      if(i==j)
        rooms[i].connections[j]=0;
      else{
        rooms[i].connections[j]=rand()%2;
        rooms[j].connections[i]=rooms[i].connections[j];
      }
    }
  }
  //check to make sure  every room has at least 3 connections
  for(i=0;i<7;i++){
    for(j=0;j<7;j++){
      if(rooms[i].connections[j]==1)
        count++;
    }
    //re-connect if less than 3 connections
    if(count<3){
      for(l=0;l<(3-count);l++){
        while(1){
          k=rand()%7;
          if(k!=i && rooms[i].connections[k]==0){
            rooms[i].connections[k]=1;
            rooms[k].connections[i]=1;
            break;
          }
        }
      }
    }
    count=0;
  }
  //count the connection counts for each line
  for(i=0;i<7;i++){
    for(j=0;j<7;j++){
      if(rooms[i].connections[j]==1)
        count++;
    }
    //store the value into the room, reset for the next room
    rooms[i].numconnections=count;
    count=0;
  }
}

/****************************************************
name: print_files
fucntionality: to print all the files to read from
*****************************************************/
void print_files(char* folder, struct Room *rooms){
  FILE* room_file;
  int k=0;
  chdir(folder);                //go into the right floder
  int i,j;
  for(i = 0; i < 7; i++){       //create a file to put names for each room
    room_file = fopen(rooms[i].name, "w");
    fprintf(room_file, "ROOM NAME: %s\n", rooms[i].name);
    for(j = 0; j < 7; j++){     //Print the connections for each file. We use j+ 1 because we want to print connections starting at 1, not 0
      if(rooms[i].connections[j]==1){
      	 k++;
     	 fprintf(room_file, "CONNECTION %d: %s\n", k, rooms[j].name);

      }
    }
    k=0;
    fprintf(room_file, "ROOM TYPE: %s\n", rooms[i].type);    //Get the type and print it to the room

    fclose(room_file);       //close the file
 }

}

int main(){
  srand(time(NULL));                //Seed the randomization
  char* folder = create_dir();      //create directory
  struct Room rooms[7];
  create_rooms(rooms);              //create rooms
  room_connections(rooms);          //make room connections
  print_files(folder, rooms);       //print file

  return 0;
}
