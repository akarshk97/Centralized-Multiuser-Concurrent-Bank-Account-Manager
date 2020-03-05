#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <pthread.h>
#include<vector>
#include<sstream>
#include<chrono>

using namespace std;
fstream file; 
string timestamp,accountnumber,line,accountholder,balance; 
//creating data structures for storing records
int accbr[100],bal[100];
string acchld[100];
//declarations for sockets
int clientSocket,*client_sock;
int t_stmp,acc,amt;
string operation;
int counter=0;
char buf2[4096];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *proc(void *); 
void interest(); 
int main()
{
	int count=0;
	// opening file 
	file.open("Records.txt"); 
	while(file >>accountnumber>>accountholder>>balance)
	{
	//counting number of records 
		getline(file,line);
		count=count+1;
	}
	counter=count;
	file.close();
	file.open("Records.txt");
	cout<<"total number of records: "<<count<<endl;
    // storing records data from Records.txt 
	int i=0;
    while (file >>accountnumber>>accountholder>>balance) 
    { 
	accbr[i]=stoi(accountnumber);
	acchld[i]=accountholder;
	bal[i]=stoi(balance);
	i++;
	cout<<accountnumber<<" "<<accountholder<<" "<<balance<<endl;         
    } 
	file.close();//closing the file
	//calling interest function once 
	interest();
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }
 
    // Binding the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listening, (sockaddr*)&hint, sizeof(hint));
 
    //listen
    listen(listening, SOMAXCONN);
	
	pthread_mutex_init(&mutex,NULL);
 
    // Waiting for connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
	
	while(int clientSocket = accept(listening, (sockaddr*)&client, &clientSize)) 
	{  
	
	pthread_t p_thread;
	
	 client_sock= (int*) malloc(sizeof(int*));
	*client_sock=clientSocket;
	//creating thread for each client
	
	pthread_create(&p_thread,NULL, proc, (void*)client_sock);	
	
	}  
	
	// Close listening socket
    	close(listening);

 	pthread_exit(NULL);

	file.close(); 

	cout<<"closing the client port"<<endl;
    // Close the socket
    close(clientSocket);
 
    return 0;
}

//routine for the client thread
void *proc(void *fd)
{
	int sock=*(int *)fd;
	cout<<"Thread"<<pthread_self()<<endl;
	int n=0;
	//clearing the buffer before reading next received data from client
	bzero(buf2,4096);
	 while(n=read(sock,buf2,4096))
	{
		cout<<buf2<<" Data received from client "<<sock;
		if(sizeof(buf2)==0)
		{
			continue;
		}
			//tokenizing the data from transactions	
		vector<string> token;
		stringstream check(buf2);
		string inter;
		while(getline(check,inter,' '))
		{
			token.push_back(inter);	
		}
		t_stmp=stoi(token[0]);
		acc=stoi(token[1]);
		operation=token[2];
		amt=stoi(token[3]);
	
		//declaring flag to exit the loop after required record found
		int flag;
		//iterating to find our appropriate record
		for(int i=0;i<counter;i++)
		{
		
		 flag=0;

			if(acc==accbr[i])
			{
			
				if(operation=="w")
				{
				//checking if the account has sufficient funds at first
					if(bal[i]-amt<0)
					cout<<" Insufficient balance";
					else
					{
					pthread_mutex_lock(&mutex);
					bal[i]=bal[i]-amt;
					pthread_mutex_unlock(&mutex);
					cout<<"| Withdrawn : "<<amt<<"in account "<<acc;
				int m=write(sock," Amount Withdrawn",20);
			
					}
				
				}
		
				else if(operation=="d")
				{
				//adding money to the person account
				pthread_mutex_lock(&mutex);
				bal[i]=bal[i]+amt;
				pthread_mutex_unlock(&mutex);
				cout<<"| Deposited : "<<amt<<" in account "<<acc;
				int m=write(sock,"deposited",10);
				if(m<0) cout<<"write() error"<<endl;	
				}
				else
				{
				cout<<"please give the proper transaction details 				and try again "<<endl;
				}
			flag=1;
			
			}
			file.open("Records.txt",ios::out|ios::trunc);
			//wrting updated data into records
			for(int i=0;i<counter;i++)
			{
			//inserting updated data into Records
		file<<to_string(accbr[i])<<" "<<acchld[i]<<" "<<to_string(bal[i])<<endl;
			
	
			}
			file.close();
			if(flag==1)
			{
			cout<<"| balance : "<<bal[i]<<endl;
			break;
			}
		}//closing for loop
		if(flag==0)
		{
		cout<<"No Records found for the account number,try 				again!"<<acc<<" "<<endl;
		}
	
	
	}//closing while loop
	close(sock);
//fflush(file);

}//proc end

void interest()
{
	//calculating interest and adding to the balance
	float roi=3.1;
	
		for(int i=0;i<counter;i++)
		{
		bal[i]=bal[i]+bal[i]*3.2;
		}

	file.open("Records.txt");
			
			for(int i=0;i<counter;i++)
			{
			
		file<<to_string(accbr[i])<<" "<<acchld[i]<<" "<<to_string(bal[i])<<endl;
			
	
			}
			for(int i=0;i<counter;i++)
			{
			cout<<"Interest added| Acc no: "<<accbr[i]<<" latest balance: "<<bal[i]<<endl;
			}
	file.close();//closing the file
	
}

