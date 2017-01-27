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
    int busyUntil = 0;
}Disk;

//CPU is a vector of cores?
struct core{
    int coreID = -1;
    bool active = false;
    int currentProcessID = -1;
    int busyUntil = 0;
};

class CPU{
    protected:
        vector<core> cores;
        int numCores = 0;
    public:
        CPU(){
        }

        CPU(int numCores){
            this->numCores = numCores;

            for(int i=0; i<numCores; i++){
                core newCore;
                newCore.coreID = i;
                newCore.active = false;
                this->cores.push_back(newCore);
            }

        }

        ~CPU(){
        }

        void freeCore(int);
        void setBusyUntil(int, int);
        core requestCore();
        int retFreeCoreID();
};

struct instruction{
    string name;
    //int arrival;
    int duration;
    int remaining;
    int currentLocation;
    int processID;
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

vector<Process> processList;
vector<Process> readyQueue;
vector<Process> diskQueue;

int main(){

    fstream inputFile;
    inputFile.open("input.txt");
     
    instruction newInstruction;
    int processIDdummy = -1;
    Process newProcess;
    CPU cpu;

    string command = "";
    int val = 0;

    while(cin >> command >> val){

        cout<<command<<"\t"<<val<<endl;

        if(command == "NCORES"){
           cpu = CPU(val); 
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
            newInstruction.processID = ++processIDdummy;
            processList.back().instrList.push_back(newInstruction);
        }
    }

    while(processList.size() > 0){
        instruction processFront = processList.front().instrList.front(); 
        if(processFront.name == "CORE"){
           //if process requests core time and a core is free mark that
           //core as busy 
            if(cpu.requestCore().coreID >= 0){
                //if duration<=SLICE
                if(processFront.duration <= SLICE){
                    cpu.setBusyUntil(CLOCK+processFront.duration,
                            processList.front().retPID());
                    processList.front().instrList.front().duration=0;
                    cout<<processFront.name<<" has been completed"<<endl;
                    cpu.freeCore(processList.front().instrList.front().currentLocation); 
                }
                //if duration>SLICE
                else{
                    cpu.setBusyUntil(CLOCK+SLICE,
                            processList.front().retPID());
                    processList.front().instrList.front().duration -= SLICE;

                    cout<<"sending instruction "<<processList.front().instrList.front().name<<" to readyQueue with "<< processList.front().instrList.front().duration<<" remaining"<<endl;

                    cpu.freeCore(processList.front().instrList.front().currentLocation); 
                }
            }
            //if no core is free, add the process to the readyQueue and
            //remove that process from the top of the processList
            else{
                cout<<"sending "<<processFront.name<<" to ready queue with "<<processFront.duration<<" remaining"<<endl;
                readyQueue.push_back(processList.front());
                processList.erase(processList.begin());
            }
            if(processList.front().instrList.front().duration == 0){
                //print report
                cout<<"instruction "<<processList.front().instrList.front().name<<" has been completed. removing it from processList"<<endl; 
                //pop instruction from processList
                processList.front().instrList.erase(processList.front().instrList.begin()); 
            }


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

int CPU::retFreeCoreID(){
    return this->requestCore().coreID;
}

void CPU::freeCore(int coreID){
    this->numCores--;
    this->cores[coreID].currentProcessID = -1;
    this->cores[coreID].busyUntil = 0;
    this->cores[coreID].active = false;
    
}

void CPU::setBusyUntil(int reqTime, int pID){
    this->numCores++;
    int coreID = this->requestCore().coreID;
    this->cores[coreID].currentProcessID = pID; 
    this->cores[coreID].busyUntil = reqTime;
    this->cores[coreID].active = true;
    
}

core CPU::requestCore(){
    core DUMMY;
    for(int i = 0; i < this->cores.size(); i++){
        if(this->cores[i].active == false) return this->cores[i];
    } 
    return DUMMY;
}
