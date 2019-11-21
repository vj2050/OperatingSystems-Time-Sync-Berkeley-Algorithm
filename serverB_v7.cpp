/*
@author : VAISHNAVI AVINASH JAMDADE
Description : To implement a program for Berkeley Time Synchronization algorithm on n-node distributed system using socket programming. 
Server side implementation
*/

#include <iostream>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include<string.h>
#include<unistd.h>

using namespace std; 

#define PORT 8561
//defining the number of clients to be 3
#define no_of_clients 3

pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lock= PTHREAD_COND_INITIALIZER;

int thread_counter=0;
char sendbuffer[50];
char recvbuffer[50]={'0'};
int globalfd[100];
//int globalclkvalue[100];        
int difference[10];  //global array to store differences between each individual clocks and time daemon
int avg;

int serv_clock;     //global variable which stores server time
int count=0;

bool running=true;

void calculation();
void send_offset();


void* worker_thread(void *args)
{
    int newsockfd = *((int*)args);
    cout <<"\nSocket for thread is: " << newsockfd << endl;
    
    pthread_mutex_lock(&mutex);   //lock 

    while(!&lock)
    {
        pthread_cond_wait(&lock,&mutex);
    }
    pthread_cond_signal(&lock);
    pthread_mutex_unlock(&mutex);

    globalfd[count]=newsockfd;   //store the client file descriptors in a global array
    count++;
    cout<<"\nsocket stored in array\n";
    if(thread_counter==no_of_clients)       //wait for calculation till all clients are connected
    {

        cout<<"\nAll clients have been connected now"<<endl;
        calculation();
        
    } 
    while(running)
    {

    }
    close(newsockfd);
    thread_counter--;
    pthread_exit(NULL);
}
void calculation()      //funtion to send time daemon clock time and calculating differences between local clock and time daemon
{
    //cout<<"global array"<<globalclkvalue;
    int temp_fd=0;
    char clock_val[50]={0};
    int temp_clock[50];
    
    int new_offset[50]; //array to calculate the new offset
    int sum_of_diff=0;
    
    char servbuffer[50];

    //Sending time daemon's clock value to each of the clients/nodes;
    for(int i=0;i<no_of_clients;i++)
    {
        int temp_fd=globalfd[i];  //temporary socket fd
        string server=to_string(serv_clock);
        strcpy(servbuffer,server.c_str());
        int servval=write(temp_fd,servbuffer,50);   //send time to clients
        if (servval<0)
        {
            cout<<"\nError in sending server value";
        }
        else 
        {
        memset(sendbuffer, '\0', sizeof(sendbuffer)); //initalize buffer to zero
        int diffread=read(temp_fd,recvbuffer,50);     //receiving the clock differences from clients
        
        difference[i]=stoi(recvbuffer);     //storing them in an array
        cout<<"\nDifferences received from clock["<<i<<"]=>"<<difference[i]<<endl;
        sum_of_diff=sum_of_diff+difference[i];
        }
    }           //end for loop
        //Calculating the average of time differences between time daemon and each local clocks
        avg=sum_of_diff/(no_of_clients+1);
        cout<<"\nAverage of differences=>"<<avg<<endl;
        send_offset();

}

//czlculating and sending the new offset value to the local clocks at client side
void send_offset()
{
    for(int i=0;i<no_of_clients;i++)
     {
        int temp_fd=globalfd[i];

        //calcualting the new offset to adjust local clock values.
        int new_offset= avg-difference[i];
        string newoff=to_string(new_offset);
        memset(sendbuffer, '\0', sizeof(sendbuffer));
        strcpy(sendbuffer,newoff.c_str());
        //send the offsets to respective clients for adjustments
        int send_off = write(temp_fd, sendbuffer, 50);
            if(send_off<0)
                cout<<"error in sending offset";   
     }   
    cout<<"\n Local time for time daemon=>"<<serv_clock<<endl;
    serv_clock=avg+serv_clock;      //synchronizing timedaemon time
    cout<<"\n Time updated for time daemon=>"<<serv_clock<<endl;
    exit(0);
}
     

int main (int argc, char *argv[])
{
    //Creation of Threads
    pthread_t t[no_of_clients];
    
    int sockfd, clientfd, addrlen;
    struct sockaddr_in servaddr,cliaddr;

    //Randomizing local clock values between 0 and 21
    time_t timer;
    srand((unsigned)time(&timer));
    serv_clock=rand()%20 + 1;

    // Creation of sockets
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        cout << "Error creating a socket" << endl;
        return -1;
    }
    //fill servaddr with zeros 
    memset(&servaddr,0, sizeof(struct sockaddr_in));

    //Assigning IP address with port 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    //bind IP address with port 
    if(bind(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ){
        cerr << "Binding Error: " << endl;
	exit(0);
    }
    else{
        cout<<"Binding........"<<endl;
    }

    //Listening for client connection requests
    if(listen(sockfd,5) < 0)
    {
        cerr << "Listening Error!! " << endl;
	exit(0);
    }
    else
    {
        cout<<"Listening for connections...... "<<endl;
    }  
    addrlen = sizeof(cliaddr);
    thread_counter = 0;

    ///creation of threads
    while(1){
        clientfd = accept(sockfd,(struct sockaddr * )&cliaddr,(socklen_t *)&addrlen);
        pthread_create(&t[thread_counter],NULL, worker_thread, (void*)&clientfd);

        thread_counter++;
        while(thread_counter > no_of_clients){}
    }
    for(int i = 0; i < no_of_clients; i++)
    {
        pthread_join(t[i], NULL);       //waiting for all threads to finish
    }


return 0;
}