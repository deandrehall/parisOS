//Deandre Hall
//PSID: 1241692
//COSC 3360
//Assignment 1: Process Scheduling

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

int CLOCK = 0;
int SLICE = 0;

struct disk{
    string status;
}Disk;

//CPU is a vector of cores?
struct core{
    int coreID;
    bool active = false;
    int currentProcessID = -1;
};

vector<core> CPU;

struct instruction{
    string name;
    //int arrival;
    int duration;
    int remaining;
    //std::string status;
};

class Process{
    protected:
        int processID;
        int startTime;
        
    public:
        vector<instruction> instrList;
        
        Process(){
        }

        Process(int val){
            static int pIDincrementer = 0;

            this->processID = pIDincrementer;
            this->startTime = val;
            pIDincrementer++;
        }
        
        ~Process(){
        }

        int retSize();
        int retPID();
        int retStartTime(); 
};

core freeCore(core);
int retFreeCore();

int main(){

    fstream inputFile;
    inputFile.open("input.txt");
     
    vector<Process> processList;
    instruction newInstruction;
    Process newProcess;

    string command = "";
    int val = 0;

    while(inputFile>>command>>val){
        cout<<command<<"\t"<<val<<endl;
        if(command == "NCORES"){
            for(int i=0; i<val; i++){
                core newCore;
                newCore.coreID = val;
                newCore.active = false;
                CPU.push_back(newCore);
            }
        }
        else if(command == "SLICE"){
            SLICE = val;
        }
        else if(command == "NEW"){
            newProcess = Process(val);
            processList.push_back(newProcess);
        }
        else{
            newInstruction.name = command; 
            newInstruction.duration = val;
            newInstruction.remaining = val;
            
            processList.back().instrList.push_back(newInstruction);
        }
    }
    
    return 0;
}

int Process::retSize(){
    return this->instrList.size();
}

int Process::retPID(){
    return this->processID;
}

int Process::retStartTime(){
    return this->startTime;
}

core freeCore(core tempCore){
    tempCore.active = false;
    tempCore.currentProcessID = -1;
    return tempCore;
}

int retFreeCore(){
    
    for(int i = 0; i<CPU.size(); i++){
        if(CPU[i].active = false) return i;
    } 
    return -1;
}
