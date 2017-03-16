//Deandre Hall
//PSID: 1241692
//COSC 3360
//Assignment 2: A public key server

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

struct student{
    string username = "";
    string key = "";
};

int main(){

    string filename = "";
    string username = "";
    string key = "";

    cout<<"Enter a file name: ";
    cin>>filename;
    fstream inputFile;
    inputFile.open(filename);

    while(inputFile >> username >> key){
        cout<<username<<"\t"<<key<<endl;
    }

    return 0;
}