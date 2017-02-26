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
    int busyUntil = 999999999;
}Disk;

//CPU is a vector of cores?
struct core{
    int coreID = -1;
    bool active = false;
    int currentProcessID = -1;
    int busyUntil = 999999999;
};

struct display{
    string status = "IDLE";
    int  currentProcessID = -1;
    int busyUntil = 999999999;
}Display;

struct instruction{
    string name = "?";
    int duration = 0;
    int remaining = 0;
    int currentLocation = -1;
    int processID = -1;
};

class Process{
    protected:
        int processID;
        int startTime;
                
    public:
        string status = "IDLE";
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
        string retStatus();
        void setBlocked();
        void setRunning();
        void freeProcess();
        void killProcess();
};

class CPU{
    protected:
        vector<core> cores;
    public:
    
        int numCores = 0;
        int numBusyCores = 0;

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
        
        int retMinBusyUntil();
        void freeCore(int);
        void setBusyUntil(int, vector<Process> &tempProcess);
        core requestCore();
        int retFreeCoreID();
        int retNumBusyCores();
        void checkFreeCore();
};

void setDiskBusy(instruction);
void setDisplayBusy(instruction);
void setInputBusy(instruction);
void freeDisk();
void freeDisplay();
void freeInput();
void checkEmpty(vector<Process> &tempList);
void queueLoader();
void loadCPU();
void parseDiskRequest();
void parseDisplayRequest();
void diskUnblockCheck();
void displayUnblockCheck();
void displayOrDiskCheck();
void inbetweenChecks();

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
            if((CLOCK < val) && (processList.size() == 0)) CLOCK = val;

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

		//loads processes into readyQueue based on their arrival time
        queueLoader();
        
        //unblocks a process if the busyUntil time has been reached
        displayUnblockCheck();   

        checkEmpty(processList);
        checkEmpty(readyQueue);

        //unblocks disk 
        diskUnblockCheck();

        checkEmpty(processList);
        checkEmpty(readyQueue);

        if(readyQueue.front().instrList.front().name == "CORE"){
            //if process requests core time and a core is free mark that
           //core as busy 
            if(cpu.requestCore().coreID >= 0){ 
               loadCPU();
                cpu.checkFreeCore();
            }
            
            //if no core is free, add the process to the readyQueue and
            //remove that process from the top of the readyQueue
            else{
                cout<<"no free core, sending "<<readyQueue.front().instrList.front().name<<readyQueue.front().instrList.front().processID<<" to ready queue with "<<readyQueue.front().instrList.front().duration<<" remaining"<<endl;
                readyQueue.push_back(readyQueue.front());
                readyQueue.erase(readyQueue.begin());
            }
        }

        checkEmpty(processList);
        checkEmpty(readyQueue);
        //unblocks a process if the display busyUntil time has been reached
        displayUnblockCheck();
        //unblocks the disk if the busyUntil time has been reached
        diskUnblockCheck();
        //processes disk requests
        parseDiskRequest();
        //blocks readyQueue if the top instruction is DISPLAY or INPUT
        parseDisplayRequest(); 

        

    }
    return 0;
}

//function loads core processes into cores, as long as a core is empty
//and the timeing is correct;
void loadCPU(){

    int lastDuration = 0;
	for(int i = 0; i < readyQueue.front().instrList.size(); i++){
        //if a core is free
        if(cpu.requestCore().coreID >= 0){

        	cpu.checkFreeCore();
            //if the duration is <= SLICE
            cout<<"NEXT IS "<<readyQueue.front().instrList.front().name;
            cout<<readyQueue.front().retPID();
            cout<<" with duration :"<<readyQueue.front().instrList.front().duration;
            
            if(readyQueue.front().instrList.front().duration <= SLICE){
                cout<<" it will complete at "<<CLOCK+readyQueue.front().instrList.front().duration<<endl;
                cpu.setBusyUntil(readyQueue.front().instrList.front().duration,
                        readyQueue);
                lastDuration = readyQueue.front().instrList.front().duration;
                readyQueue.front().instrList.front().duration = 0;

                cout<<"instruction "<<readyQueue.front().instrList.front().name<<readyQueue.front().retPID()<<" has been completed CLOCK = "<<CLOCK<<endl; 
                //pop instruction from readyQueue
                readyQueue.front().instrList.erase(readyQueue.front().instrList.begin());
                checkEmpty(readyQueue);
            }
            //if the duration > SLICE
            else{
                cout<<" it will complete at "<<CLOCK+SLICE<<endl;
                cpu.setBusyUntil(SLICE,readyQueue);

                lastDuration = SLICE;
                readyQueue.front().instrList.front().duration -= SLICE;
                readyQueue.push_back(readyQueue.front());
                readyQueue.erase(readyQueue.begin());
            }
            //exits if next instruction in the list isnt CORE request
            if(readyQueue.front().instrList.front().name != "CORE"){
                if(readyQueue.size() > 1){
                    if(readyQueue.begin()[1].instrList.front().name == "CORE"){
                        //if the second process in the readyQueue is
                        //core request send the top process to end of Q
                        readyQueue.push_back(readyQueue.front());
                        readyQueue.erase(readyQueue.begin());
                    }
                    else break;
                }
                //TODO:FINISH THIS
                if(readyQueue.front().instrList.size() > 1 && 
                    (readyQueue.front().instrList.front().duration
                        < lastDuration)){

                    if(readyQueue.front().instrList.front().name == "DISK"){
                        diskUnblockCheck();
                        parseDiskRequest();
                        cout<<"DISK REQUEST from process";
                        cout<<readyQueue.front().retPID()<<" for ";
                        cout<<readyQueue.front().instrList.front().duration<<endl;
                        cout<<"SKIPPING AHEAD CLOCK = "<<CLOCK;
                        cout<<" -> "<<Disk.busyUntil<<endl;
                        CLOCK = Disk.busyUntil;
                        diskUnblockCheck();
                    }
                    else if(readyQueue.front().instrList.front().name == "DISPLAY" ||
                            readyQueue.front().instrList.front().name == "INPUT"){
                        displayUnblockCheck();
                        parseDisplayRequest();
                        cout<<"???DISPLAY REQUEST???";
                        cout<<readyQueue.front().retPID()<<" for ";
                        cout<<readyQueue.front().instrList.front().duration<<endl;
                        cout<<"SKIPPING AHEAD CLOCK = "<<CLOCK;
                        cout<<" -> "<<Display.busyUntil<<endl;
                        CLOCK = Display.busyUntil;
                        displayUnblockCheck();
                    }
                }
                else break;
            }
        }
    }
}

void inbetweenChecks(){
    if(readyQueue.size() > 1){
        //checks the instrname of the process behind the front process.
        //if it isnt core and it ends before the earliest core busyUntil
        //time, handle that process. do this for as long as there are
        //instructions that have duration < min core busyUntil
        while((readyQueue.begin()[1].instrList.front().name != "CORE")
                && (readyQueue.begin()[1].instrList.front().duration <
                    readyQueue.front().instrList.front().duration) &&
                (readyQueue.size()>1) && (CLOCK < cpu.retMinBusyUntil())){
            cout<<"im in the open while loop"<<endl;
            if(readyQueue.begin()[1].instrList.front().name == "DISK"){
                if(Disk.status == "IDLE"){
                   setDiskBusy(readyQueue.begin()[1].instrList.front()); 
                   cout<<"setting disk busy until2:"<<(Disk.busyUntil)<<endl;
                   readyQueue.begin()[1].instrList.front().duration = 0;
                   readyQueue.begin()[1].instrList.erase(readyQueue.begin()[1].instrList.begin());
                   checkEmpty(readyQueue);
                }
            //if the disk is currently busy, add the instruction to the
            //diskQueue
                else{
                    diskQueue.push_back(readyQueue.begin()[1].instrList.front());
                    readyQueue.begin()[1].instrList.erase(readyQueue.begin()[1].instrList.begin());
                    checkEmpty(readyQueue);
                }

                if((diskQueue.size()>0) && (diskQueue.front().name == "DISK")){ 
                    //if the disk is free, add the instruction to the disk
                    if(Disk.status == "IDLE"){
                        setDiskBusy(diskQueue.front()); 
                        cout<<"setting disk busy until3:"<<(Disk.busyUntil)<<endl;
                        diskQueue.front().duration = 0;
                        diskQueue.erase(diskQueue.begin());
                    }
                    //if the disk is currently busy leave it at the front of the
                    //queue
                }
            }
            displayOrDiskCheck();
            if ((readyQueue.begin()[1].instrList.front().name == "DISPLAY") ||
                (readyQueue.begin()[1].instrList.front().name == "INPUT")){
                if(Display.status == "IDLE"){
                    setDisplayBusy(readyQueue.begin()[1].instrList.front());
                    cout<<"NEXT IS "<<readyQueue.begin()[1].instrList.front().name;
                    cout<<readyQueue.begin()[1].retPID();
                    cout<<" with duration :"<<readyQueue.begin()[1].instrList.front().duration;
                    cout<<" it will complete at "<<CLOCK+readyQueue.begin()[1].instrList.front().duration<<endl; 
                }
            }
            displayOrDiskCheck();
        }
    }
}

void displayOrDiskCheck(){

    int minTime = 0;

    //find the smallest core busyUntil time
    minTime = cpu.retMinBusyUntil();
    //if the disk busyuntil time is less than the earliest
    //core busyuntil time,free the disk, set to clock to the disk
    //busyUntil time, then check if any processes have
    //arrived and if any cores have been freed up
    if((Disk.busyUntil<Display.busyUntil)){
        if((Disk.busyUntil < minTime) && (Disk.status == "RUNNING")){
            CLOCK = Disk.busyUntil;
            diskUnblockCheck();
            displayUnblockCheck(); 
            queueLoader();
            cpu.checkFreeCore();
        }
        if((Display.busyUntil < minTime) && (Display.status == "RUNNING")){ 
            CLOCK = Display.busyUntil;
            displayUnblockCheck();
            diskUnblockCheck();
            queueLoader();
            cpu.checkFreeCore();
        }
    }
    else{
        if((Display.busyUntil < minTime) && (Display.status == "RUNNING")){

            CLOCK = Display.busyUntil;
            displayUnblockCheck();
            diskUnblockCheck();
            queueLoader();
            cpu.checkFreeCore();

        }
        if((Disk.busyUntil < minTime) && (Disk.status == "RUNNING")){

            CLOCK = Disk.busyUntil;
            diskUnblockCheck(); 
            displayUnblockCheck();
            queueLoader();
            cpu.checkFreeCore();
        }

    }

    CLOCK = minTime;
    //cpu.checkFreeCore();
    inbetweenChecks();

}

void displayUnblockCheck(){
    if((CLOCK < Display.busyUntil) && (Display.status == "RUNNING")){

         if ((readyQueue.front().instrList.front().name == "DISPLAY") ||
            (readyQueue.front().instrList.front().name == "INPUT")){
            if(readyQueue.size() == 1){ 
                CLOCK = Display.busyUntil;
            }
            cout<<"DISPLAY removed from readyQueue2"<<endl;
            freeDisplay();
            readyQueue.front().instrList.erase(readyQueue.front().instrList.begin());
        }
    }
    else if((CLOCK >= Display.busyUntil) && (Display.status == "RUNNING")){
        //find the process that was blocked by the display and remove
        //DISPLAY from the front of the readyQueue
        for(int i = 0; i < readyQueue.size(); i++){
            if((readyQueue.begin()[i].instrList.front().name == "DISPLAY") ||
                (readyQueue.begin()[i].instrList.front().name == "INPUT")){
                if(Display.currentProcessID == readyQueue.begin()[i].instrList.front().processID){
                    readyQueue.begin()[i].instrList.erase(readyQueue.begin()[i].instrList.begin());
                    freeDisplay();
                    break;
                }
            }
        }
    }

}

void diskUnblockCheck(){
    if((CLOCK >= Disk.busyUntil) && (Disk.status == "RUNNING")){ 
        if (diskQueue.size() == 0){
            freeDisk();
            cout<<"freeing DISK CLOCK = "<<CLOCK<<endl;
        }
        else{
            freeDisk();
            setDiskBusy(diskQueue.front());
            cout<<"freeing DISK CLOCK = "<<CLOCK<<endl;
            diskQueue.erase(diskQueue.begin());
        }
    }
}

void parseDiskRequest(){
    if(readyQueue.front().instrList.front().name == "DISK"){
        //if the disk is free, add the instruction to the disk
        if(Disk.status == "IDLE"){
           setDiskBusy(readyQueue.front().instrList.front()); 
           cout<<"setting disk busy until2:"<<(Disk.busyUntil)<<endl;
           readyQueue.front().instrList.front().duration = 0;
           readyQueue.front().instrList.erase(readyQueue.front().instrList.begin());
           checkEmpty(readyQueue);
        }
        //if the disk is currently busy, add the instruction to the
        //diskQueue
        else{
            diskQueue.push_back(readyQueue.front().instrList.front());
            readyQueue.front().instrList.erase(readyQueue.front().instrList.begin());
            checkEmpty(readyQueue);
        }
    }

    if((diskQueue.size()>0) && (diskQueue.front().name == "DISK")){ 
        //if the disk is free, add the instruction to the disk
        if(Disk.status == "IDLE"){
            setDiskBusy(diskQueue.front()); 
            cout<<"setting disk busy until3:"<<(Disk.busyUntil)<<endl;
            diskQueue.front().duration = 0;
            diskQueue.erase(diskQueue.begin());
        }
        //if the disk is currently busy leave it at the front of the
        //queue
    }

}

void parseDisplayRequest(){
    if ((readyQueue.front().instrList.front().name == "DISPLAY") ||
            (readyQueue.front().instrList.front().name == "INPUT")){
        if(Display.status == "IDLE"){
            setDisplayBusy(readyQueue.front().instrList.front());
            cout<<"NEXT IS "<<readyQueue.front().instrList.front().name;
            cout<<readyQueue.front().retPID();
            cout<<" with duration :"<<readyQueue.front().instrList.front().duration;
            cout<<" it will complete at "<<CLOCK+readyQueue.front().instrList.front().duration<<endl;
        }
    }
}

void queueLoader(){
    for(int i = 0; i < processList.size(); i++){
        if(CLOCK >= processList.front().retStartTime()){
            cout<<"loading "<<processList.front().instrList.front().name<<" into rQ";
            cout<<" CLOCK = "<<CLOCK<<endl;
            processList.front().setRunning();
            readyQueue.push_back(processList.front());
            processList.erase(processList.begin());
        }
    }
}

void checkEmpty(vector<Process> &tempList){
    for(int i = 0; i < tempList.size(); i++){
        if(tempList[i].instrList.size() == 0){
            tempList[i].killProcess();
            Process deadProcess = tempList[i];
            tempList.erase(tempList.begin()+i);
            cout<<endl;
            cout<<"CURRENT STATE OF THE SYSTEM AT t = "<<CLOCK;
            cout<<" ms"<<endl;
            cout<<"Current number of busy cores: "<<cpu.numBusyCores<<endl;
            cout<<"READY QUEUE:"<<endl;
            if(readyQueue.size() == 0 || (readyQueue.size() == 1)){
                cout<<"empty"<<endl;
            }
            else{
                for(auto x: readyQueue){
                    cout<<x.retPID()<<" ";
                }
                cout<<endl;
            }

            cout<<"DISK QUEUE:"<<endl;
            if(diskQueue.size() == 0){
                cout<<"empty"<<endl;
            }
            else{
                for(auto x: diskQueue){
                    cout<<x.processID<<" ";
                }
                cout<<endl;
            }

            cout<<"PROCESS TABLE:"<<endl;
            for(Process x: tempList){
                cout<<"Process "<<x.retPID()<<" started at "<<x.retStartTime()<<" ms and is ";
                cout<<x.status<<endl;
            }
            cout<<"Process "<<deadProcess.retPID()<<" started at ";
            cout<<deadProcess.retStartTime()<<" ms and is ";
            cout<<deadProcess.status<<endl<<endl;
            
        }
    }
}

void setDisplayBusy(instruction tempInstruction){
    Display.status = "RUNNING";
    Display.currentProcessID = tempInstruction.processID;
    Display.busyUntil = CLOCK + tempInstruction.duration;
}

void setDiskBusy(instruction tempInstruction){
    Disk.status = "RUNNING";
    Disk.currentProcessID = tempInstruction.processID;
    Disk.busyUntil = CLOCK + tempInstruction.duration;
}

void freeDisk(){
    Disk.status = "IDLE";
    Disk.currentProcessID = -1;
    Disk.busyUntil = 999999999;
}

void freeDisplay(){
    Display.status = "IDLE";
    Display.currentProcessID = -1;
    Display.busyUntil = 999999999;
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

string Process::retStatus(){
    return this->status;
}

void Process::setBlocked(){
    this->status = "BLOCKED";
}

void Process::setRunning(){
    this->status = "RUNNING";
}

void Process::freeProcess(){
    this->status = "IDLE";
}

void Process::killProcess(){
    this->status = "TERMINATED";
}

int CPU::retMinBusyUntil(){
    int minTime = 999999999;
    for(int i = 0; i < this->numCores; i++){
        if(this->cores[i].active){
            if(this->cores[i].busyUntil < minTime){
                minTime = this->cores[i].busyUntil;
            }
        }
    }
    return minTime;
}

int CPU::retFreeCoreID(){
    return this->requestCore().coreID;
}

void CPU::checkFreeCore(){
	for(int i = 0; i<this->cores.size(); i++){
		if((CLOCK >= this->cores[i].busyUntil) && (this->cores[i].active == true)){
			freeCore(i);	
		}
	}
}

void CPU::freeCore(int coreID){
    this->numBusyCores--;
    this->cores[coreID].currentProcessID = -1;
    this->cores[coreID].busyUntil = 999999999;
    this->cores[coreID].active = false;
}

void CPU::setBusyUntil(int reqTime, vector<Process> &tempProcess){
	cpu.checkFreeCore();
    int pID = tempProcess.front().retPID(); 
    this->numBusyCores++;
    int coreID = this->requestCore().coreID;
    this->cores[coreID].currentProcessID = pID; 
    this->cores[coreID].busyUntil = CLOCK + reqTime;
    this->cores[coreID].active = true;
    tempProcess.front().instrList.front().currentLocation = coreID;
    inbetweenChecks();
    CLOCK += reqTime;
    inbetweenChecks();
    displayOrDiskCheck();
    queueLoader();
    cpu.checkFreeCore();
    
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
