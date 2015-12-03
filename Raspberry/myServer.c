/*
*
*   Advanced Software Engineering
*
*   Members :     Byungwook Lee, Hyesun Lim, Sunghoon Byun
*
*   File name :   myServer.c
*   Description : This program perform raspberry pi TCP/IP server.
*                 First receiving, we receive the data from Arduino and TIZEN Phone.
*                 From Arduino, pressure data (> 200, 54), switch data (55), and detecting data (= 56) are sent.
*                 When User push the switch, camera module start for using fork() process (node camera.js).
*                 The camera module is executed 10 sec. After then, restart SHGS.
*		  From TIZEN Phone, turn on the light (= 1), turn off the light (= 0), turn on the TV (= 2), and open or close the door (= 3 or 4) are sent.
*                 Second sending, we send the data to Arduino and TIZEN Phone.
*		  To Arduino, we send the data from TIZEN Phone command.
*		  To TIZEN Phone, we send the data from Arduino sensing data.
*                 We use UART communication (Rx, Tx) for connection with Arduino Uno.
*		  Also, we use TCP/IP communication for connection with TIZEN Phone.
*
*   Compile command : g++ myServer.c -o server -pthread -lwiringPi
*   Execution command : sudo ./myServer
*
*   Final Revision : 2015.12.02
*
*/

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/wait.h>

#define NUM_THREADS 2


int func(int a);
void error(char *msg);
int getData(int sockfd);
void sendData(int sockfd, int x);

void *ArduinoToRaspberry(void *socketfd);
void *RaspberryToArduino(void *socketfd);

char intToChar(int a);

int main(int argc, char *argv[]) {
   // ready for socket
   int sockfd, newsockfd, portno = 11111, clilen;
//   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n;
   int data;

   // ready for thread
   pthread_t threads[NUM_THREADS];
   int returnCreate1, returnCreate2;
   long thread1 = 0, thread2 = 1;

   printf("using port #%d\n", portno);
    
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sockfd < 0) 
      error(const_cast<char *>("ERROR opening socket"));
   bzero((char *) &serv_addr, sizeof(serv_addr));

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      error(const_cast<char *>("ERROR on binding"));
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
  
   //--- infinite wait on a connection ---
   while(1){
      printf("waiting for new client...\n");
      if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen)) < 0)
         error(const_cast<char *>("ERROR on accept"));
      printf("opened new communication with client\n");

      while(1){
         //---- wait for a number from client ---
         printf("In main: creating raspberry -> arduino thread %ld\n", thread1);
         returnCreate1 = pthread_create(&threads[0], NULL, RaspberryToArduino, (void *)newsockfd);
         if(returnCreate1){
            printf("ERROR; return code from pthread_create() is %d\n", returnCreate1);
            exit(-1);
         }

         //--- send new data back --- 
         printf("In main: creating arduino -> raspberry thread %ld\n", thread2);
         returnCreate2 = pthread_create(&threads[1], NULL, ArduinoToRaspberry, (void *)newsockfd);
         if(returnCreate2){
             printf("ERROR; return code from pthread_create() is %d\n", returnCreate2);
            exit(-1);
         }
         pthread_exit(NULL);
      }
      printf("asdhfkjhasdkjfhd\n");
      close(newsockfd);
   }
   /* Last thing that main() should do */

   return 0; 
}

void error(char *msg)
{
   perror(msg);
   exit(1);
}

int getData(int sockfd)
{
   char buffer[10];
   int n;

   if((n = read(sockfd, buffer, sizeof(buffer))) < 0)
      error(const_cast<char *>("ERROR reading from socket"));
   buffer[n] = '\0';
   return atoi(buffer);
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

void *ArduinoToRaspberry(void *socketfd)
{
   int tid;
   tid = (int)socketfd;
   printf("Hello World! It's me, RaspberryToArduino thread #%d!\n", tid);

   // ras to ardu
   int fd;
   int data;
   if(wiringPiSetupGpio() == -1)
      pthread_exit(NULL);

   if((fd = serialOpen("/dev/ttyAMA0", 9600)) < 0){
      fprintf(stderr, "Uable to open serial device: %s\n", strerror(errno));
      pthread_exit(NULL);
   }

   printf("\nRaspberry Pi UART Test (Arduino - > Raspberry)\n");

   while(1){
      data = serialGetchar(fd);
      if(data == -1) break;
      else{
         printf("Arduino -> Raspberry = %d\n", data);
         serialPutchar(fd, data);
         serialPuts(fd, "\n");
         printf("Sending data to TIZEN Phone %d\n", data);
         sendData(tid, data);
         if(data == 55){
            pid_t pid, wait_child;
            int state;
            pid = fork();
            if(pid == -1){
               printf("cannnot fork, erro\n");
               exit(0);
            }
            if(pid == 0){
               printf("exec camera module\n");
               execl("/usr/local/bin/node", "node", "camera.js", NULL);
               printf("exit camera module\n");
               exit(0);
            }else{
               printf("PARENT_PID: %d child PID: %d\n", getpid(), pid);
               wait_child = wait(&state);
               printf("\t Child pid = %d\n", wait_child);
               printf("\t return value = %d\n", WEXITSTATUS(state));
               sleep(10);
            }
         }
      }
      fflush(stdout);
   }

   pthread_exit(NULL);
}

void *RaspberryToArduino(void *socketfd)
{
   int tid;
   tid = (int)socketfd;
   printf("Hello World! It's me, ArduinoToRaspberry thread #%d!\n", tid);

   // ardu to ras
   int fd;
   int data;
   if(wiringPiSetupGpio() == -1)
      pthread_exit(NULL);

   if((fd = serialOpen("/dev/ttyAMA0", 9600)) < 0){
      fprintf(stderr, "Uable to open serial device: %s\n", strerror(errno));
      pthread_exit(NULL);
   }

   printf("\nRaspberry Pi UART Test (Raspberry - > Arduino)\n");

   while(1){
//      printf("\nRaspberry -> Arduino = ");
      data = getData(tid);
      printf("got %d from TIZEN Phone\n", data);
      if(data == -1){
         sendData(tid, data);
         break;
      }
//      scanf("%c", &data);
      serialPutchar(fd, intToChar(data));
//      serialPutchar(fd, data);
//      serialPuts(fd, "\n");
      fflush(stdout);
   }

   pthread_exit(NULL);
}

char intToChar(int a)
{
   switch(a){
   case 0:
      return '0';
   case 1:
      return '1';
   case 2:
      return '2';
   case 3:
      return '3';
   case 4:
      return '4';
   case 5:
      return '5';
   case 6:
      return '7';
   case 8:
      return '8';
   }
}
