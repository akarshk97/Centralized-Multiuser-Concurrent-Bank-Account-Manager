#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include<fstream>
#include<chrono>

using namespace std;
using namespace std :: chrono;

int main()
{
    //	Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return 1;
    }

    //	Create a hint structure for the we're connecting with
    int port = 54000;
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }

    //	While loop:
    char buf[4096];
    string userInput;
	fstream file;
	string line;
	file.open("Transactions.txt");

	auto start = high_resolution_clock::now();
    while(file) {
        //		Enter lines of text
	
        getline(file,line);

        //		Send to server
	auto start = high_resolution_clock::now();
        int sendRes = send(sock, line.c_str(), line.size() + 1, 0);
        if (sendRes == -1)
        {
            cout << "Could not send to server\r\n";
            continue;
        }

        //		Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\r\n";
        }
        else
        {
            //		Display response
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop-start);
            cout << "SERVER> "<<"Transaction time "<<duration.count()<<" micorseconds " << string(buf, bytesReceived) << "\r\n";
        }
    } 
	
	cout<<"exiting"<<endl;

	file.close();
    //	Close the socket
	close(sock);
    

    return 0;
}

