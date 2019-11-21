/*
@author : VAISHNAVI AVINASH JAMDADE
@email id : vjamdad1@umbc.edu
Description : To implement a program for Berkeley Time Synchronization algorithm on n-node distributed system using socket programming. 
Client Side Implementation
*/

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include<string.h>

using namespace std;
#define PORT 8561

int local_clock;
int server_clock;


int calculate();        //function to calculate differences

int main(int argc, char *argv[])
{	

    time_t timer;       
    srand((unsigned)time(&timer));      
    sleep(1);           
    local_clock=rand()%20+1;        //randomize time for each thread between 1 to 20

    int clientfd,clientlen;
    struct sockaddr_in servaddr;
    //Socket creation for threads
    clientfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    clientlen = sizeof(servaddr);

    //Connect to server
    connect(clientfd,(struct sockaddr *)&servaddr,clientlen);
//initializing buffers and variables
char sendbuffer[50]={0};
char recvbuffer[50]={0};
char servbuffer[50]={0};
int diff=0;
int avg=0;

//read time daemon time from server
int servvalue=read(clientfd,servbuffer,50);
if(servvalue<0)
{
    cout<<"error in server value";
}
else 
{
    cout<<"Local clock time :"<<local_clock<<endl;
    server_clock=stoi(servbuffer);
    cout<<"Server time received : "<<server_clock<<endl;
    
    //calculate differences between local clock and server clock
    diff=local_clock-server_clock;
    cout<<"Sending difference between two clocks=>"<<diff<<endl;
    string diffstr=to_string(diff);
    memset(sendbuffer, '\0', sizeof(sendbuffer));
    strcpy(sendbuffer,diffstr.c_str());
    //send differences to time daemon
    int writetime=write(clientfd, sendbuffer,50);
if(writetime<0)
{
    cout<<"error";
}
else
{
    cout<<"\nDifference sent to server is : "<<sendbuffer<<endl;
}

//Receive offset from time daemon
int valueread=read(clientfd,recvbuffer,50);
if(valueread<0)
{
    cout<<"Error receiving offset";
}
else{
    int offset_new=stoi(recvbuffer);
    cout<<"Offset received from daemon process : "<<offset_new<<"\n";
    //calculate synchronized time
    int updated_time=offset_new+local_clock;
    cout<<"Updated local clock time is => "<<updated_time<<endl;
}

cout<<"++++++++++++++++++++++++++++++++++++++++++++++++="<<endl;
    
    close(clientfd);
    return 0;
}
}

