//Deandre Hall
//PSID: 1241692
//COSC 3360
//Assignment 2: A public key server

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <limits.h>
#include <cstring>
#include <cstdlib>
#include <sstream>

using namespace std;

int main(){
    
    unsigned short portNumber = 0;
    const int MAXHOSTNAMESIZE = 256;
    char tempHost[MAXHOSTNAMESIZE];
    char readBuffer[256];
    string serverHostName = "";
    string userInput = "";
    int sock, dummySocket;

    struct sockaddr_in serverAddress;
    struct hostent *serverPointer;

    cout<<"Enter a server host name: ";
    getline(cin, serverHostName);

    sprintf(tempHost, "%s", serverHostName.c_str());
    while(serverHostName != "program.cs.uh.edu"){
        cout<<"invalid server host name, please enter a valid server host name: ";
        cin>>serverHostName;
    }

    gethostname(tempHost, MAXHOSTNAMESIZE);
    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverPointer = gethostbyname(serverHostName.c_str());

    cout<<"Enter server port number: ";
    cin>>portNumber;

    serverAddress.sin_family = serverPointer->h_addrtype;
    serverAddress.sin_port = htons((u_short)portNumber);

    while(true){
        //create socket
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            cout<<"Something went wrong while creating the socket"<<endl;
            return -1;
        }

        if(connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
            cout<<"Something went wrong while connecting to the socket"<<endl;
            return -1;
        }

        cout<<"Enter a username: ";
        cin >> userInput;

        dummySocket = write(sock, userInput.c_str(), userInput.length());
        bzero(readBuffer, 256);
        dummySocket = read(sock, readBuffer, 255);

        if(userInput == "Terminate."){
            cout<<"The client program will now terminate..."<<endl;
            close(sock);
            return 0;
        }

        cout<<"The public key for user "<<userInput<<" is "<<readBuffer<<endl;
        
        userInput = "";
        close(sock);

    }

    return 0;
}