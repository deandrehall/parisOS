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
    string status = "IDLE";
    int currentProcessID = -1;
    int busyUntil = 0;
}Disk;

//CPU is a vector of cores?
struct core{
    int coreID = -1;
    bool active = false;
    int currentProcessID = -1;
    int busyUntil = 0;
};

struct instruction{
    string name = "?";
    //int arrival;
    int duration = 0;
    int remaining = 0;
    int currentLocation = -1;
    int processID = -1;
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

class CPU{
    protected:
        vector<core> cores;
        int numCores = 0;
        int numBusyCores = 0;
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
        void setBusyUntil(int, vector<Process> &tempProcess);
        core requestCore();
        int retFreeCoreID();
        int retNumBusyCores();
};

void setDiskBusy(instruction);
void freeDisk();
void preParse();

vector<Process> processList;
vector<Process> readyQueue;
vector<instruction> diskQueue;
CPU cpu;

int main(){

    fstream inputFile;
    inputFile.open("input.txt");
     
    instruction newInstruction;
    int processIDdummy = -1;
    Process newProcess;
    
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

    while((processList.size() > 0) || (readyQueue.size() > 0) || (diskQueue.size() > 0)){
        //instruction processFront = processList.front().instrList.front(); 
        if(processList.front().instrList.front().name == "CORE"){
           //if process requests core time and a core is free mark that
           //core as busy 
            if(cpu.requestCore().coreID >= 0){ 
                //if duration<=SLICE
                if(processList.front().instrList.front().duration <= SLICE){
                    cpu.setBusyUntil(CLOCK+processList.front().instrList.front().duration,
                            processList);
                    processList.front().instrList.front().duration=0;
                    cout<<processList.front().instrList.front().name<<processList.front().retPID()<<" has been completed in a single cycle"<<endl;
                    cpu.freeCore(processList.front().instrList.front().currentLocation); 
                }
                //if duration>SLICE
                else{
                    cpu.setBusyUntil(CLOCK+SLICE, processList);
                    processList.front().instrList.front().duration -= SLICE;

                    cout<<"sending instruction "<<processList.front().instrList.front().name<<processList.front().retPID()<<" to readyQueue from ProcessList with "<< processList.front().instrList.front().duration<<" remaining"<<endl;

                    cpu.freeCore(processList.front().instrList.front().currentLocation); 
                    readyQueue.push_back(processList.front());
                    processList.front().instrList.erase(processList.front().instrList.begin());
                   
                }
            }
            //if no core is free, add the process to the readyQueue and
            //remove that process from the top of the processList
            else{
                cout<<"no free core, sending "<<processList.front().instrList.front().name<<" to ready queue with "<<processList.front().instrList.front().duration<<" remaining"<<endl;
                readyQueue.push_back(processList.front());
                processList.erase(processList.begin());
            }
            //if process duration is 0, print report
            if(processList.front().instrList.front().duration == 0){
                //print report
                cout<<"instruction "<<processList.front().instrList.front().name<<" has been completed. removing it from processList"<<endl; 
                //pop instruction from processList
                processList.front().instrList.erase(processList.front().instrList.begin()); 
            }
        }
        if(readyQueue.front().instrList.front().name == "CORE"){
            //if process requests core time and a core is free mark that
           //core as busy 
            if(cpu.requestCore().coreID >= 0){ 
                //if duration<=SLICE
                if(readyQueue.front().instrList.front().duration <=SLICE){
                    cpu.setBusyUntil(CLOCK+readyQueue.front().instrList.front().duration,
                            processList);
                    processList.front().instrList.front().duration = 0;
                    cout<<readyQueue.front().instrList.front().name<<" has been completed"<<endl;
                    cpu.freeCore(processList.front().instrList.front().currentLocation);
                }
                //if duration>SLICE
                else{
                    cpu.setBusyUntil(CLOCK+SLICE,
                            readyQueue);
                    readyQueue.front().instrList.front().duration -= SLICE;

                    cout<<"sending instruction "<<readyQueue.front().instrList.front().name<<" to readyQueue from readyQueue with "<< readyQueue.front().instrList.front().duration<<" remaining"<<endl;

                    cpu.freeCore(readyQueue.front().instrList.front().currentLocation);
                    readyQueue.push_back(readyQueue.front());
                    readyQueue.erase(readyQueue.begin()); 
                }
            }
            //if no core is free, add the process to the readyQueue and
            //remove that process from the top of the readyQueue
            else{
                cout<<"no free core, sending "<<readyQueue.front().instrList.front().name<<" to ready queue with "<<readyQueue.front().instrList.front().duration<<" remaining"<<endl;
                readyQueue.push_back(readyQueue.front());
                readyQueue.erase(readyQueue.begin());
            }
            //if process duration is 0, print report
            if(readyQueue.front().instrList.front().duration == 0){
                //print report
                cout<<"instruction "<<readyQueue.front().instrList.front().name<<" has been completed. removing it from processList"<<endl; 
                //pop instruction from readyQueue
                readyQueue.front().instrList.erase(readyQueue.front().instrList.begin()); 
            }
        }
        
        if(processList.front().instrList.front().name == "DISK"){
            //if the disk is free, add the instruction to the disk
            if(Disk.status == "IDLE"){
                setDiskBusy(processList.front().instrList.front());
                cout<<"setting disk busy until: "<<(CLOCK +
                       processList.front().instrList.front().duration)<<endl;
                processList.front().instrList.front().duration = 0;
                //readyQueue.push_back(processList.front().instrList.front();
                processList.front().instrList.erase(processList.front().instrList.begin());
            }
            //if the disk is currently busy, add the instruction to the
            //diskQueue
            else{
                diskQueue.push_back(processList.front().instrList.front());
                processList.front().instrList.erase(processList.front().instrList.begin());
                cout<<"the disk is currently busy, adding instruction to the disk queue from the processList"<<endl;
            }
        }
        
        if(readyQueue.front().instrList.front().name == "DISK"){
            //if the disk is free, add the instruction to the disk
            if(Disk.status == "IDLE"){
               setDiskBusy(readyQueue.front().instrList.front()); 
               cout<<"setting disk busy until: "<<(CLOCK +
                       readyQueue.front().instrList.front().duration)<<endl;
               readyQueue.front().instrList.front().duration = 0;
               //readyQueue pushback into readyQueue goes here?
               readyQueue.front().instrList.erase(readyQueue.front().instrList.begin());
            }
            //if the disk is currently busy, add the instruction to the
            //diskQueue
            else{
                diskQueue.push_back(readyQueue.front().instrList.front());
                readyQueue.front().instrList.erase(readyQueue.front().instrList.begin());
                cout<<"the disk is currently busy, adding instruction to the disk queue from the readyQueue"<<endl;
            }
        }

        if((diskQueue.size()>0) && (diskQueue.front().name == "DISK")){ 
            //if the disk is free, add the instruction to the disk
            if(Disk.status == "IDLE"){
                setDiskBusy(diskQueue.front()); 
                cout<<"setting disk busy until: "<<(CLOCK +
                       readyQueue.front().instrList.front().duration)<<endl;
                diskQueue.front().duration = 0;
                diskQueue.erase(diskQueue.begin());
            }
            //if the disk is currently busy, add the instruction to the
            //diskQueue
            else{
                diskQueue.push_back(diskQueue.front());
                diskQueue.erase(diskQueue.begin());
                cout<<"the disk is currently busy, adding instruction to the disk queue from the diskQueue"<<endl;
            }
        }

        if((CLOCK >= Disk.busyUntil) && (Disk.status == "RUNNING")){
            cout<<"DISK IS NOW FREE"<<endl;
            freeDisk();
        }

    }
    cout<<"complete!!!"; 
    return 0;
}

void preParse(){
    
}


void setDiskBusy(instruction tempInstruction){
    Disk.status = "RUNNING";
    Disk.currentProcessID = tempInstruction.processID;
    Disk.busyUntil = CLOCK + tempInstruction.duration;
}

void freeDisk(){
    Disk.status = "IDLE";
    Disk.currentProcessID = -1;
    Disk.busyUntil = 0;
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
    this->numBusyCores--;
    this->cores[coreID].currentProcessID = -1;
    this->cores[coreID].busyUntil = CLOCK;
    this->cores[coreID].active = false;
}

void CPU::setBusyUntil(int reqTime, vector<Process> &tempProcess){
    int pID = tempProcess.front().retPID(); 
    this->numBusyCores++;
    int coreID = this->requestCore().coreID;
    this->cores[coreID].currentProcessID = pID; 
    this->cores[coreID].busyUntil = CLOCK + reqTime;
    this->cores[coreID].active = true;
    tempProcess.front().instrList.front().currentLocation = coreID;
    CLOCK += reqTime; 
}

core CPU::requestCore(){
    core DUMMY;
    for(int i = 0; i < this->cores.size(); i++){
        if(this->cores[i].active == false) return this->cores[i];
    } 
    return DUMMY;
}

int CPU::retNumBusyCores(){
    return this->numBusyCores;    
}
