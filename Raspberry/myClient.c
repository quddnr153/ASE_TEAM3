/*
*   Adapted from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

/*
*
*   Advanced Software Engineering
*
*   Members :     Byungwook Lee, Hyesun Lim, Sunghoon Byun
*
*   File name :   myClient.c
*   Description : This program perform raspberry pi TCP/IP server.
*                 First receiving, we receive the data from Arduino and TIZEN Phone.
*                 From Arduino, pressure data (> 200, 54), switch data (55), and detecting data (= 56) are sent.
*                 When User push the switch, the TIZEN will be connected with Web page (192.168.0.185:8000/cam).
*                 The camera module is executed 10 sec. After then, restart SHGS.
*		  From TIZEN Phone, turn on the light (= 1), turn off the light (= 0), turn on the TV (= 3), and open or close the door (= 4/5) are sent.
*                 Second sending, we send the data to Arduino and TIZEN Phone.
*		  To Arduino, we send the data from TIZEN Phone command.
*		  To TIZEN Phone, we send the data from Arduino sensing data.
*                 We use UART communication (Rx, Tx) for connection with Arduino Uno.
*		  Also, we use TCP/IP communication for connection with TIZEN Phone.
*
*   Compile command : g++ myClient.c -o client -pthread
*   Execution command : sudo ./myClient
*
*   Final Revision : 2015.12.02
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NUM_THREADS 2

void error(char *msg);
int getData(int sockfd);
void sendData(int sockfd, int x);

void *TIZENToRaspberry(void *socketfd);
void *RaspberryToTIZEN(void *socketfd);

int main(int argc, char *argv[])
{
   // ready for socket
   int sockfd, portno = 11111, n;
   char serverIp[] = "192.168.0.185";
   struct sockaddr_in serv_addr;
   struct hostent *server;
//   char buffer[256];
   int data;

   // ready for thread
   pthread_t threads[NUM_THREADS];
   int returnCreate1, returnCreate2;
   long thread1 = 0, thread2 = 1;

   if(argc < 3){
      // error(const_cast<char *>("usage myClient2 hostname port\n"));
      printf("contacting %s on port %d\n", serverIp, portno);
      // exit(0);
   }
   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      error(const_cast<char *>("ERROR opening socket"));

   if((server = gethostbyname(serverIp)) == NULL) 
      error(const_cast<char *>("ERROR, no such host\n"));
    
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
      error(const_cast<char *>("ERROR connecting"));

   //---- wait for a number from client ---
   printf("In main: creating raspberry -> arduino thread %ld\n", thread1);
   returnCreate1 = pthread_create(&threads[0], NULL, TIZENToRaspberry, (void *)sockfd);
   if(returnCreate1){
      printf("ERROR; return code from pthread_create() is %d\n", returnCreate1);
      exit(-1);
   }

   //--- send new data back --- 
   printf("In main: creating raspberry -> arduino thread %ld\n", thread2);
   returnCreate2 = pthread_create(&threads[1], NULL, RaspberryToTIZEN, (void *)sockfd);
   if(returnCreate2){
      printf("ERROR; return code from pthread_create() is %d\n", returnCreate2);
      exit(-1);
   }

   pthread_exit(NULL);

/*
   for (n = 0; n < 10; n++){
      int tmp;
      scanf("%d", &tmp);
      sendData(sockfd, tmp);
      data = getData(sockfd);
      printf("%d ->  %d\n",n, data);
   }
   sendData(sockfd, -2);
*/
   close(sockfd);
   return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void sendData(int sockfd, int x)
{
  int n;

  char buffer[10];
  sprintf(buffer, "%d\n", x);
  if((n = write(sockfd, buffer, strlen(buffer))) < 0)
      error(const_cast<char *>("ERROR writing to socket"));
  buffer[n] = '\0';
}

int getData(int sockfd)
{
  char buffer[10];
  int n;

  if((n = read(sockfd,buffer,sizeof(buffer))) < 0)
       error(const_cast<char *>("ERROR reading from socket"));
  buffer[n] = '\0';
  return atoi(buffer);
}

void *RaspberryToTIZEN(void *socketfd)
{
   int tid;
   tid = (int)socketfd;
   printf("Hello World! It's me, RaspberryToTIZEN thread #%d!\n", tid);

   // ras to ardu
   int fd;
   int data;

   printf("\nRaspberry receiving Test\n");

   while(1){
      data = getData(tid);
      if(data == -1)break;
      else{
         printf("Raspberry -> TIZEN = %d\n", data);
      }
      fflush(stdout);
   }

   pthread_exit(NULL);
}

void *TIZENToRaspberry(void *socketfd)
{
   int tid;
   tid = (int)socketfd;
   printf("Hello World! It's me, TIZENToRaspberry thread #%d!\n", tid);

   // TIZEN to ras
   int data;

   printf("\nTIZEN sending Test\n");

   while(1){
      scanf("%d", &data);
      sendData(tid, data);
      printf("Send to raspberry from TIZEN %d\n", data);
      if(data == -1)
         break;
      fflush(stdout);
   }

   pthread_exit(NULL);
}
