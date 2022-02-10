/******************************************
*Program: guoxin.adventure.c
*Author: Xindi Guo
*Date: 2/7/2020
*Description: build the game.
*Work cited: canvas materials
            http://man7.org/linux/man-pages/man3/strftime.3.html
            http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
            stack overflow: goo.gl/g69qLl and goo.gl/jU2QoT
            https://github.com/aw-leigh/CS344/blob/master/Program2/wilsoan6.adventure.c
******************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

char *room_names[10] = {"yellow", "blue", "white", "red", "black", "brown", "pink", "green", "purple", "orange"};
char *room_types[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Room{
    char name[256];
    char type[256];
    char connections[6][256];
    int numconnections;
};

/****************************************************
name:getDirectoryName
fucntionality: get the dir name we need get info from
*****************************************************/
char *getDirectoryName(){
  int newestDirTime = -1; // Modified timestamp of newest subdir examined
  char targetDirPrefix[32] = "guoxin.rooms."; // needed prefix
  char newestDirName[256]; // to hold the name of dir that contains prefix
  char *strToReturn = malloc(sizeof(char)*256); //stores filename which is going to be returned
  memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck;  //to hold the directory
  struct dirent *fileInDir;  //to hold the current subdir
  struct stat dirAttributes;  // to hold the info we gained from subdir

  dirToCheck = opendir("."); //open the directory

  if (dirToCheck > 0){ //check if  the current directory can be opened
    while ((fileInDir = readdir(dirToCheck)) != NULL){ //check each entry
      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){ // entry has prefix
        stat(fileInDir->d_name, &dirAttributes); // get attributes of the entry
        if ((int)dirAttributes.st_mtime > newestDirTime){
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName, '\0', sizeof(newestDirName));
          strcpy(newestDirName, fileInDir->d_name); //copy the value
        }
      }
    }
  }
  closedir(dirToCheck); //close directory
  strcpy(strToReturn, newestDirName); //copy the filename
  return strToReturn; // return the file name
}

/****************************************************
name:chopStr
fucntionality: get off the the string that not needed
*****************************************************/
void chopStr(char *line){
  int i, start, end,len;
  char *str;
  //set up the needed part string for the start and end room index
  for(i=0;i<strlen(line);i++){
    if(line[i]==':')
      start=i;
    if(line[i]=='\n')
      end=i;
  }
  str=malloc(sizeof(char)*(end-start-1));
  //using index obtained to store the value in the new string
  for(i=0;i<(end-start);i++)
    str[i]=line[start+2+i];
  //change the original string to a new one
  strcpy(line, str);

  len = strlen(line);
  //subttitute '\n' by '\0'
  if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
  free(str);
}

/****************************************************
name: roomInfo
fucntionality: get the room info from the room file
*****************************************************/
void roomInfo(struct Room *rooms){
  char* directory = getDirectoryName();
  FILE *roomFile[7];
  DIR *dir;
  //change to the folder found and open the current directory
  chdir(directory);
  dir = opendir(".");
  struct dirent *fileInDir;
  //stores one line of contents from the file
  char line[256];
  int i=0, numLine=0;

  if (dir > 0){
    while ((fileInDir = readdir(dir)) != NULL){
      //only open file if the file names are not "." or ".."
      if(strcmp(".", fileInDir->d_name) && strcmp("..", fileInDir->d_name)){
        //read only permission
        roomFile[i] = fopen(fileInDir->d_name,"r");
        //read file line by line
        while (fgets(line, sizeof(line), roomFile[i])) {
          //call function to remove the parts not needed
          chopStr(line);
          if(numLine==0){
            strcpy(rooms[i-2].name, line);
          //store in the type array once the value matches room types,
        }else if(!strcmp(line, room_types[0]) || !strcmp(line, room_types[1]) || !strcmp(line, room_types[2])){
            strcpy(rooms[i-2].type, line);
          //store the values in the connection array
          }else{
            strcpy(rooms[i-2].connections[numLine-1], line);
          }
          //counting the number of connections
          numLine++;
        }
        //store the number of connections
        rooms[i-2].numconnections=numLine-2;
        fclose(roomFile[i]);
        numLine=0;  //reset
      }
      i++;
    }
  }
}

/****************************************************
name: checkConnection
fucntionality: check if the connection exsists and update the current location
*****************************************************/
int checkConnection(int * iposition, char *input, struct Room *rooms){
  int i,j;  //input checking
  for(i=0;i<rooms[*iposition].numconnections;i++){
    //loop through to compare the input with each connection
    if(!strcmp(input, rooms[*iposition].connections[i])){
      for(j=0;j<7;j++){
        //update the integer of the location
        if(!strcmp(input, rooms[j].name)){
          *iposition=j;
          return 1;
        }
      }

    }
  }
  //error infomation
  printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
  return 0;
}

/****************************************************
name: getTime
fucntionality: get the time info and put into a file
*****************************************************/
void* getTime(){
  /**FILE* myfile;                             //referencea from stack overflow: goo.gl/g69qLl and goo.gl/jU2QoT
  myfile = fopen("currentTime.txt", "w+");  //create and write to the file
  char buffer[100];
  struct tm *t;

  time_t now = time (0);
  t = gmtime (&now);

  strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
  fputs(buffer, myfile); //put the info to the buffer
  fclose(myfile);    // close file
  pthread_mutex_unlock(&mutex); **/

  pthread_mutex_lock(&mutex);
  int i = 0;
  char * timeFilename = "currentTime.txt";
  char timeString[128];
  memset(timeString, '\0', 128);
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  ///take time string
  strftime(timeString, 128, "%I:%M%p, %A, %B %d, %G", &tm);

  // set to lowercase
  timeString[5] = tolower(timeString[5]);
  timeString[6] = tolower(timeString[6]);

  //deal with leading 0 on the hour, shift to the left
  if(timeString[0] == 48){ //ASCII of 0
      while(timeString[i]){
          timeString[i] = timeString[i+1];
          i++;
      }
  }

  // open the file for writing
  FILE *f = fopen(timeFilename, "w");
  if (f == NULL)
  {
      printf("Error opening file!\n");
  }
  fprintf(f, "%s\n", timeString);
  //close file
  fclose(f);
  pthread_mutex_unlock(&mutex);
}

/****************************************************
name: displayTime
fucntionality: display the time from the file
*****************************************************/
void displayTime(){

    char buffer[255];
    memset(buffer, '\0', 255);
    // read from file
    FILE *myfile = fopen("currentTime.txt", "r");
    if (myfile == NULL) //check if the file exsists
    {
        printf("No such a file!\n");
    }
    fgets(buffer,255,myfile); //get the time from myfile to the buffer
    printf("\n %s", buffer);
    fclose(myfile);
}

int main(){
  int startr, endr,steps=0;
  char input[256];
  char path[256][256];
  char *buffer;
  size_t bufferSize = 64;
  buffer = (char *)calloc(bufferSize, sizeof(char));
  struct Room rooms[7];
  pthread_t thread;
  pthread_create(&thread, NULL, getTime, NULL);
  //set rooms and read from files
  int x;
  for(x=0;x<7;x++){
	strcpy(rooms[x].name,"name");
	strcpy(rooms[x].type,"type");
	rooms[x].numconnections=0;
  }
  roomInfo(rooms);
  chdir("..");
  //set the start and end room
  int i,j,k,p=0;
  for(i=0;i<7;i++){
    if(!strcmp(rooms[i].type, room_types[0]))
      startr=i;
    if(!strcmp(rooms[i].type, room_types[2]))
      endr=i;
  }

  int iposition = startr;
  //game play
  do{
    printf("CURRENT LOCATION: %s\n", rooms[iposition].name);
    printf("POSSIBLE CONNECTIONS: ");
    for(i=0;i<rooms[iposition].numconnections;i++){
      printf("%s", rooms[iposition].connections[i]);
      if(i!=rooms[iposition].numconnections-1){
	      printf(",");
      }
      else{
		printf(".");
      }
    }
    //ask for user input
    printf("\nWHERE TO? >" );
    scanf("%s", input);
    //if the input is time, then print time
    if(strcmp(input,"time")==0){
        memset(buffer, '\0', bufferSize);
        //http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
        pthread_mutex_unlock(&mutex);
        pthread_join(thread, NULL);
        pthread_mutex_lock(&mutex);
        pthread_create(&thread, NULL, getTime, NULL);
        displayTime();
    } //if the user input a connection, check the connection
    else if(checkConnection(&iposition, input, rooms)){
          strcpy(path[steps],input); //copy all the steps the user made
          steps++;
    }
    printf("\n");
  }while(iposition!=endr); //end game

  printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
  for(i=0;i<steps;i++){ //print the steps that the user made
    printf("%s\n", path[i]);
  }
}
