#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/*
THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR MATERIALS OTHER THAN THIS SEMESTER'S COURSE MATERIALS. Brandon Valyan-Clark
*/

#define MAXSIZE 80;

	int fd[2];
	pid_t childpid;
	char string[80];
	int stream;
	pid_t childpid2;
	char  sortedText[80];
	char  sortedText2[80];
	char buildString[80];
	char suppressedString[80];
	int fd2[2];
	int whom;
	int status;

/*char* fileRead(char* input, char inString[80]){
	FILE* fileptr = fopen(input, "r"); //open given text file
	fgets(inString, 80, fileptr);
	return inString;
}
*/

/*void pipeRead(int file, char string[80])
{
  //printf("Let's do some pipe reading!\n");
  FILE *stream;
  stream = fdopen (file, "r");
  //printf("Opened file\n");
  while((fgets(string + strlen(string), 80, stream))){
    //printf("So this is your text? \n%s the length %d\n", string, strlen(string));
}
  //printf("done reading\n");
  fclose (stream);
}
*/

void pipeReadAndSuppress(int file, char string[80])
{
  
  //printf("Let's do some pipe reading and suppressing!\n");
  FILE *stream;
  FILE *stream2;
  stream = fdopen(fd2[0], "r"); //open pipe for reading in words
  stream2 = fdopen (STDOUT_FILENO, "w"); //open stdout for output to terminal
  char* savedString = malloc(80); //String to save the current word
  char* prevString = malloc(80); //string that contains the previous word
  while(fgets(savedString, 80, stream)!=NULL){ //loop through all words in the stream
  	//printf("in loop\n");
  if(strcmp(savedString,prevString) != 0 ){ //compare current word to previous word
  	//printf("%s\n", savedString );
  	fputs(savedString, stream2); //if word is not previous word output to stdout
  	
  }
  	strcpy(prevString,savedString); // save new word as previous word for next iteration
  //fgets(string + strlen(string), 80, stream);
  	
	
}
  //printf("So this is your text? \n%s\n", string );
  fclose(stream); //close the streams
  fclose(stream2);
}

/*void pipeWrite(int file, char string[80]){
	FILE *stream;
	stream = fdopen (file, "w");
	fputs(string, stream);
	fclose(stream);
}*/



void parser(char buildString [80]){
	FILE *stream;// prepare pointer for opening stream
	close(fd[0]);// close reading side of pipe in preparation for write
	stream = fdopen (fd[1], "w"); //open pipe for writing
	char currentLetter;
	int buildCount = 0;
	while((currentLetter = getc(stdin)) != EOF ){ // read from stdin until EOF
		if(isalpha((int) currentLetter)){ //if the current char is a letter
			char letter;
			letter = (char) tolower((int) currentLetter); //convert current letter to lower case
			//printf("currentletter:%c\n", letter);
			buildString[buildCount] = letter; //add formatted letter to string
			buildCount++; //increment counter to keep string building accurate
		}
		else{
			if (buildCount > 0){ //if there is a word
				buildString[buildCount] = '\n'; // add a new line to seperate the word
				//printf("%s\n", buildString );
				fputs(buildString, stream); // store the new word to the stream
				memset(buildString,0,strlen(buildString));// (optional) clear string
			}
			buildCount = 0; // reset counter
			
		}
	//sprintf(buildString + strlen(buildString), "%s", "\0");
}
fclose(stream); //close the write stream

	//printf("%s string length %d\n", buildString, strlen(buildString) );
	//printf("Done parsing!\n");
}

/*void suppressor(){


}*/

int main(int argc, char* argv[]){

	pipe(fd2);// secondary pipe
	pipe(fd); //primary pipe

	if((childpid = fork()) == -1){ // fork to first child, which will handle sorting
		perror("fork");
		exit(1);
	}

	  	if(childpid == 0){ //this is the child!
	  		//printf("child working here!\n");
	  		//printf("Child ID: %d\n", childpid);
			close(fd[1]);
			//printf("Closed writing pipe.\n");
			//printf("read in text \n" );
			//sortRead = fdopen(fd[0], "r"); //open file from pipe where text was read in
			
			//printf("new pipe made\n");
			close(fd2[0]); //close secondary pipe reading side 
			//sortWrite = fdopen(fd2[1], "w");// open secondary pipe to write
			dup2(fd2[1], STDOUT_FILENO);// redirect the second pipe for suppress to SDTOUT
			dup2(fd[0], STDIN_FILENO); //redirect the pipe to intercept STDIN
			
			execl("/bin/sort", "/bin/sort", 0);//sort input text and write to pipe 
			close(fd2[1]);
			close(1);
			close(fd[0]);
			_exit(0);
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	else{// this is the parent!
		//printf("parent ID: %d\n", childpid);
		int returnStatus; // declare return status for parental wait
		parser(buildString); //run the parser on the stdin input
		//printf("\n%s is being written to the pipe...\n", buildString);
		//printf("Wrote to pipe!\n");

		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// second child of parent 
		childpid2 = fork(); //fork for second child, who will suppress

		if(childpid2 == 0){// in the second child
			close(fd2[1]);//close write pipe
			pipeReadAndSuppress(fd2[0],suppressedString); // Kick out sorted input to process that will read in from pipe and suppress duplicates
			close(fd2[0]); //close reading pipe
			_exit(0); //exit process
		}
		else{
			exit(0);
		}

		int i;
		waitpid(childpid, &status, 0); //wait for first child
		waitpid(childpid2, &status, 0); //wait for second child

		for(i = 0; i <= 1; i++){ //check wait status
			if((whom = wait(&status)) == -1){
				perror("wait");
				exit(1);
			}

			if(whom == childpid){
				printf("parser\n");
			}
			if (whom == childpid2)
			{
				printf("sort\n");
			}
	}

}

	
}