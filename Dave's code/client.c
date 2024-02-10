#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* the logic of my program/server
   1: make sure the user started me with the right parameters
   2: create a socket (STREAM/connection oriented)
   3: ask ther user for a filename NOTE: not really doing file IO
   4) figure out length of 'filename'
   5) put the server's address info into my datastructure
   6) connect() to the server
   7) send length of filename
   8) send filename
*/


int main(int argc, char *argv[])
{
  int sd; /* socket descriptor */
  int rc; 
  struct sockaddr_in sin_addr;

  int portNumber;
  if (argc < 3){
    printf ("Usage is: client <portNumber> <ipaddress> \n");
    exit (1);
  }

  
  // get the port number
  portNumber = atoi(argv[1]); 

  // create a socket, note i do NOT have to bind to it since
  // this is a client. 
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("error opening stream socket");
    exit(1);
  }

  /* add in SERVER address information i got from command line*/
  sin_addr.sin_family = AF_INET;
  sin_addr.sin_port = htons(portNumber);

  // this is a funky function that will convert something like
  // 9.1.2.44 into the address for the computer/network needs
  char *ipaddress = argv[2];
  rc = inet_pton(AF_INET, ipaddress, &(sin_addr.sin_addr));
  if (rc < 0){
    printf ("failed to convert ip address\n");
    exit(1);
  }

  /* establish connection with server, since this is a stream socket, connection is required */
  // if the server says "NO I CAN"T TALK, i need to handle that
  rc = connect(sd, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in));
  if(rc < 0){
    close(sd); // clean up!
    perror("error connecting stream socket");
    exit(1);
  }
  /* if here then the socket has been connected */

  char fileName[100] = "myfile.c"; // hardcoding for this example
  int fileNameSize = strlen(fileName); // simple string length

  // NOTE: the protocol for this program says send the size of the filename
  // then send the filename

  // NOTE: i have to convert the integer to network order
  int convertedLength;
  convertedLength = htonl(fileNameSize); // does the endianess conversion

  rc = write (sd, &convertedLength, sizeof (int));
  if (rc < 4){ // i know an int on this box is 4 bytes,
    printf ("error sending fileNameSize\n");
    exit (1);
  }

  // now send the filename (hardcoded)
  rc = write (sd, fileName, fileNameSize);
  
  if (rc < fileNameSize){ // i know how many bytes i wanted to send
    printf ("error sending fileName\n");
    exit (1);
  }

  close (sd);
  printf ("goodbye\n");

  
}
