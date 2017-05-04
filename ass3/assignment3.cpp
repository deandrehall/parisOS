//Deandre Hall
//PSID: 1241692
//COSC 3360
//Assignment 3: The Bridge

#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>

using namespace std;

struct testStruct{
    string val = "ok nice";
};

struct vehicle{
    string vehicleID = "";
    int timeSinceLastVehicle = 0;
    int weight = 0;
    int timeToCross = 0;
    int arrivalTime = 0;
};

class vehicleObj{
    protected:
        vector<vehicle> vehicleList;
        int totalNumVehicles = 0;
    public:
        vehicleObj(){

        }
        ~vehicleObj(){

        }
        void addVehicle(vehicle temp){
            this->vehicleList.push_back(temp);
            totalNumVehicles++;
        }

        int numVehicles(){
            return totalNumVehicles;
        }
};

static int CLOCK = 0;
static int MAXBRIDGEWEIGHT = 0;
static int CURRENTBRIDGEWEIGHT = 0;
static pthread_mutex_t lock;
static pthread_cond_t condClock = PTHREAD_COND_INITIALIZER;
static pthread_cond_t condWeight = PTHREAD_COND_INITIALIZER;

void *leaveBridge(vehicle obj){

    if(obj.weight > MAXBRIDGEWEIGHT){
        pthread_exit((void*) NULL);
    }
    //cout<<obj.vehicleID<<" is waiting to leave the bridge"<<endl;

    pthread_mutex_lock(&lock);
    /*
    while(CLOCK < (obj.arrivalTime + obj.timeToCross)){
        pthread_cond_wait(&condClock, &lock);
    }
    */
    cout<<"Vehicle "<<obj.vehicleID<<" leaves the bridge."<<endl;
    CURRENTBRIDGEWEIGHT-=obj.weight;
    pthread_cond_signal(&condWeight);
    cout<<"The current bridge load is "<<CURRENTBRIDGEWEIGHT<<" tons."<<endl;
    pthread_mutex_unlock(&lock);
    
    pthread_exit((void*) 0);
}

void *enterBridge(void *arg){
    vehicle* obj = (vehicle*) arg;

    if(obj->weight > MAXBRIDGEWEIGHT){
        cout<<"Vehicle "<<obj->vehicleID<<" is too heavy to cross" <<endl;
        pthread_exit((void*) NULL);
    }

    usleep(obj->timeSinceLastVehicle);

    obj->arrivalTime = CLOCK;
    //cout<<"Current time is "<<CLOCK<<endl;
    cout<<"Vehicle "<<obj->vehicleID<<" arrives at bridge."<<endl;

    pthread_mutex_lock(&lock);
    cout<<"The current bridge load is "<<CURRENTBRIDGEWEIGHT<<" tons."<<endl;
 
    if((obj->weight <= MAXBRIDGEWEIGHT) && ((CURRENTBRIDGEWEIGHT+obj->weight) <= MAXBRIDGEWEIGHT)){
        cout<<"Vehicle "<<obj->vehicleID<<" goes on bridge."<<endl;
        CURRENTBRIDGEWEIGHT+=obj->weight;
        cout<<"The current bridge load is "<<CURRENTBRIDGEWEIGHT<<" tons."<<endl;  
    }
    else if((obj->weight <= MAXBRIDGEWEIGHT) && ((CURRENTBRIDGEWEIGHT+obj->weight) > MAXBRIDGEWEIGHT)){
        cout<<"Waiting for bridge load to lessen..."<<endl;
        while((CURRENTBRIDGEWEIGHT+obj->weight) > MAXBRIDGEWEIGHT){
            pthread_cond_wait(&condWeight, &lock);
        }
        cout<<"Vehicle "<<obj->vehicleID<<" goes on bridge."<<endl;
        CURRENTBRIDGEWEIGHT+=obj->weight;
        cout<<"The current bridge load is "<<CURRENTBRIDGEWEIGHT<<" tons."<<endl;
    }
    pthread_mutex_unlock(&lock);

    usleep(obj->timeToCross);
    leaveBridge(*obj);

    pthread_exit((void*) NULL);
}

int main(int argc, char *argv[]){

    MAXBRIDGEWEIGHT = atoi(argv[1]);
    vehicle tempVehicle;
    vehicleObj vehicles;
    static vector<vehicle> CARS;

    while(cin >> tempVehicle.vehicleID >> tempVehicle.timeSinceLastVehicle >> tempVehicle.weight >> tempVehicle.timeToCross){
        vehicles.addVehicle(tempVehicle);
        CARS.push_back(tempVehicle);
    }

    cout << "Maximum bridge load is " << MAXBRIDGEWEIGHT <<" tons."<<endl;

    vector <pthread_t> enterThreads(vehicles.numVehicles());
    vector <pthread_t> leaveThreads(vehicles.numVehicles());

    pthread_mutex_init (&lock, NULL);
    pthread_cond_init (&condClock, NULL);
    pthread_cond_init (&condWeight, NULL);
    for(int i = 0; i < vehicles.numVehicles(); i++){
        pthread_create(enterThreads.data()+i, NULL, &enterBridge, CARS.data()+i);
        //pthread_create(leaveThreads.data()+i, NULL, &leaveBridge, CARS.data()+i);
        pthread_join(enterThreads[i], NULL);
        //pthread_join(leaveThreads[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condClock);
    pthread_cond_destroy(&condWeight);

    cout << "\nTotal number of vehicles: " << vehicles.numVehicles() << endl;
    pthread_exit(NULL);
    return 0;
}