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

struct user{
    string username = "";
    string key = "";
};

class userObj{
    protected:
        vector<user> vec;
    public:
        userObj(){
            
        }

        ~userObj(){

        }

        void addUser(user temp){
            this->vec.push_back(temp);
        }

        string findUser(string);

};

int main(){

    string filename = "";
    string username = "";
    string inputName = "";
    string retrivedKey = "";
    user tempStudent;
    userObj students;
    string key = "";

    unsigned short portNumber = 0;
    int sock, tempSock, dummySocket;
    struct sockaddr_in socketAddress;
    struct hostent *hp;
    const int MAXHOSTNAMESIZE = 18;
    char hostname[MAXHOSTNAMESIZE+1];
    char readBuffer[256];

    cout<<"Enter a file name: ";
    cin>>filename;
    cout<<endl;
    fstream inputFile;
    inputFile.open(filename);
    //parse file into vector
    while(inputFile >> tempStudent.username >> tempStudent.key){
        students.addUser(tempStudent);
        cout<<"added "<<tempStudent.username<<"\t"<<tempStudent.key<<endl;
    }

    inputFile.close();
    //user input of port number
    cout<<"Enter server port number: "; 
    cin>>portNumber;

    //create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout<<"Error connecting to socket"<<endl;
        return -1;
    }

    cout<<"socket created"<<endl;
    memset(&socketAddress, 0, sizeof(struct sockaddr_in));
    gethostname(hostname, MAXHOSTNAMESIZE);
    hp = gethostbyname(hostname);

    if( hp == NULL){
        cout <<"Error retriving hostname"<<endl;
        return -1;
    }
    socketAddress.sin_family = hp->h_addrtype;
    socketAddress.sin_addr.s_addr = htons(INADDR_ANY);
    socketAddress.sin_port = htons(portNumber);

    //binding address to socket
    bind(sock, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr_in)); 

    //socket is listening while input != "Terminate."
    listen(sock, 8);
    cout<<"I am listening..."<<endl;

    while(true){

        if((tempSock = accept(sock, NULL, NULL)) < 0){
            return -1;
        }

        bzero(readBuffer, 256);
        dummySocket = read(tempSock, readBuffer, 255); //accepting input

        inputName = readBuffer;
        //Program exits if input is "Terminate."
        if(inputName == "Terminate."){ 
            cout<<"Terminating...";
            close(sock);
            close(tempSock);
            return 0;
        }

        retrivedKey = students.findUser(inputName);
        dummySocket = write(tempSock, retrivedKey.c_str(), retrivedKey.length());

        inputName = "";

    }

    //closing sockets
    close(sock);
    close(tempSock);

    return 0;
}

string userObj::findUser(string name){
    for(auto element: this->vec){
        if(element.username == name) return element.key;
    }
}