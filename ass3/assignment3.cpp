//Deandre Hall
//PSID: 1241692
//COSC 3360
//Assignment 3: The Bridge

#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>

using namespace std;

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
            this->totalNumVehicles++;
        }

        void *enterBridge(void *arg);
        void *leaveBridge(void *arg);
        int numVehicles(){
            return this->totalNumVehicles;
        }
};


static int CLOCK = 0;
static int MAXBRIDGEWEIGHT = 0;
static int CURRENTBRIDGEWEIGHT = 0;

int main(int argc, char *argv[]){

    MAXBRIDGEWEIGHT = atoi(argv[1]);
    vehicle tempVehicle;
    vehicleObj vehicles;

    while(cin >> tempVehicle.vehicleID >> tempVehicle.timeSinceLastVehicle >> tempVehicle.weight >> tempVehicle.timeToCross){
        vehicles.addVehicle(tempVehicle);
    }

    cout << "Maximum bridge load is " << MAXBRIDGEWEIGHT <<" tons."<<endl;

    cout << "Total number of vehicles: " << vehicles.numVehicles() << endl;
    return 0;
}