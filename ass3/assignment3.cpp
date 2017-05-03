//Deandre Hall
//PSID: 1241692
//COSC 3360
//Assignment 3: The Bridge

#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>

using namespace std;

struct testStruct{
    string val = "ok nice";
};

struct vehicle{
    string vehicleID = "";
    int timeSinceLastVehicle = 0;
    int weight = 0;
    int timeToCross = 0;
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

        void *enterBridge(void *arg);
        void *leaveBridge(void *arg);
        int numVehicles(){
            return totalNumVehicles;
        }
        vehicle retVehicle(){ 
            auto temp = this->vehicleList.front();
            this->vehicleList.erase(vehicleList.begin());
            return temp;
        }

};

static int CLOCK = 0;
static int MAXBRIDGEWEIGHT = 0;
static int CURRENTBRIDGEWEIGHT = 0;
pthread_mutex_t lock;

void *test(void *arg){
    vehicle* obj = (vehicle*) arg;
    
    pthread_mutex_lock(&lock);
    cout<<obj->vehicleID<<endl;
    cout<<obj->timeSinceLastVehicle<<endl;
    cout<<obj->weight<<endl;
    cout<<obj->timeToCross<<endl;
    pthread_mutex_unlock(&lock);

    pthread_exit((void*) 0);
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

    vector <pthread_t> ret(vehicles.numVehicles());
    vector<testStruct> temp(vehicles.numVehicles());

    pthread_mutex_init (&lock, 0);
    for(int i = 0; i < vehicles.numVehicles(); i++){
        pthread_create(ret.data()+i, NULL, &test, CARS.data()+i);
        pthread_join(ret[i], NULL);
    }
    pthread_mutex_destroy(&lock);

    cout << "Total number of vehicles: " << vehicles.numVehicles() << endl;
    pthread_exit(NULL);
    return 0;
}