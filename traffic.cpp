#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <time.h>
#include <set>
/*SOME FANCY DIRECTIVE FOR C++11
#include <chrono>
#include <thread>
*/
#define GRID_X 4
#define GRID_Y 80
#define NUMBER_CAR 30
#define MAX_SPEED 4
#define ACCELERATION 1
#define MAX_VICINITY 3


class Car {
    private:
        int position_x;
        int position_y;
        int speed;
    
    public:
        Car(){
            position_x = 0;
            position_y = 0;
            speed = 0;
        }
        Car(int x,int y, int v){
            position_x = x;
            position_y = y;
            speed = v;
        }

        ~Car(){}


        int getPositionX(){
            return position_x;
        }
        int getPositionY(){
            return position_y;
        }
        int getSpeed(){
            return speed;
        }
        void setPositionX(int x){
            position_x = x;
        }
        void setPositionY(int y){
            position_y = y;
        }
        void setSpeed(int s){
            speed = s;
        }
        bool isAdjacentFront(Car bCar){
            bool res= false;
            int bPosY = bCar.getPositionY();
            int aPosY = getPositionY();
            int dy = GRID_Y;
            // if B position is superior to A
            if ( bPosY >= aPosY){
                dy = bPosY - aPosY;
            }
            //otherwise add max grid to the smallest number
            else{
                dy = bPosY+ GRID_Y - aPosY;
            }
            if(dy < MAX_VICINITY){
                res = true;
            }
            return res;
        }
        bool isAdjacent(Car bCar, int direction){
            bool res = false;
            if((getPositionX() - bCar.getPositionX()) == direction){
                if(true == isAdjacentFront(bCar)){
                    res = true;
                }
            }
            return res;
        }
        bool isOnHighSpeedWay(){
            if(getPositionX()==0){
                return true;
            }
            return false;
        }
        bool isOnLowSpeedWay(){
            if(getPositionX()== GRID_X){
                return true;
            }
            return false;
        }
        void decreaseSpeed(){
           setSpeed(getSpeed() - ACCELERATION); 
        }
        void increaseSpeed(){
           setSpeed(getSpeed() + ACCELERATION); 
        }
        void goLeft(){
            if(getPositionX() != 0){
                setPositionX(getPositionX() -1);
            }
        }
        void goRight(){
            if(getPositionX() != (GRID_X - 1)){
                setPositionX(getPositionX() + 1);
            }
        }
        
        

};

class Traffic {
    private:
        Car cars[NUMBER_CAR] ;

    public:
        Traffic(int nb_cars){
            int random_x = 0;
            int random_y = 0;
            int random_speed = 0 ;
            std::set<std::pair<int,int>> usedPostions;
            int stopCondition = 0;
            for(int i = 0;i<nb_cars && i < NUMBER_CAR;i++){
                do{
                    random_x = rand() % GRID_X;
                    random_y = rand() % GRID_Y;
                    stopCondition++;
                }while(usedPostions.find({random_x,random_y}) != usedPostions.cend() && stopCondition < 100);
                usedPostions.insert({random_x,random_y});
                stopCondition = 0;
                random_speed = (rand() % MAX_SPEED) + 1;
                cars[i]=Car(random_x,random_y,random_speed);
            }
        }

        ~Traffic(){}
        enum adjacent {LEFT = 1, RIGHT = -1, FRONT = 0};

        Car* getCars(){
            return cars;
        }

        int getNbCars(){
            return sizeof(cars)/sizeof(cars[0]);
        }
        void updateCar(Car& aCar ,bool carAtLeft, bool carAtRight, bool carAtFront, bool carOnLeftWay, bool carOnRightWay){
            if (carAtFront) {
            if (carOnRightWay) {
                if (carAtLeft) {
                    if (carAtRight) {
                        aCar.decreaseSpeed();  // cautious
                    } else {
                        aCar.decreaseSpeed();  // still cautious
                    }
                } else {
                    if (carAtRight) {
                        if (!aCar.isOnHighSpeedWay()) aCar.goLeft();
                        aCar.increaseSpeed();
                    } else {
                        if (!aCar.isOnHighSpeedWay()) aCar.goLeft();
                        aCar.increaseSpeed();
                    }
                }
            } else { // not on right lane
                if (carAtLeft) {
                    if (carAtRight) {
                        // box-in
                        aCar.setSpeed(aCar.getSpeed()); // maintain
                    } else {
                        if (!aCar.isOnLowSpeedWay()) aCar.goRight();
                        aCar.decreaseSpeed();
                    }
                } else {
                    if (carAtRight) {
                        aCar.increaseSpeed();
                    } else {
                        if (!aCar.isOnHighSpeedWay()) aCar.goLeft();
                        aCar.increaseSpeed();
                    }
                }
            }
        } else { // no car ahead
            if (carOnRightWay) {
                if (carAtLeft) {
                    if (carAtRight) {
                        aCar.setSpeed(aCar.getSpeed()); // steady
                    } else {
                        aCar.setSpeed(aCar.getSpeed()); // steady
                    }
                } else {
                    if (carAtRight) {
                        aCar.increaseSpeed();
                    } else {
                        aCar.increaseSpeed();
                    }
                }
            } else { // not on right
                if (carAtLeft) {
                    if (carAtRight) {
                        aCar.setSpeed(aCar.getSpeed()); // hold
                    } else {
                        if (!aCar.isOnLowSpeedWay()) aCar.goRight();
                        aCar.decreaseSpeed();  // conservative lane change
                    }
                } else {
                    if (carAtRight) {
                        aCar.increaseSpeed();
                    } else {
                        if (!aCar.isOnLowSpeedWay()) aCar.goRight();
                        aCar.increaseSpeed();  // fast path
                    }
                }
            }
        }
        // Update position with wrap-around on Y axis
        int newY = (aCar.getPositionY() + aCar.getSpeed()) % GRID_Y;
        aCar.setPositionY(newY);

        // Cap speed between 0 and MAX_SPEED
        if (aCar.getSpeed() > MAX_SPEED) {
            aCar.setSpeed(MAX_SPEED);
        }
        if (aCar.getSpeed() < 0) {
            aCar.setSpeed(0);
        }
    }

        void updateAllCars(){
            /* Implementation
            si      voiture sur trois case et     voiture fil de droite et     autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> ralenti 
            si      voiture sur trois case et     voiture fil de droite et     autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> ralenti
            si      voiture sur trois case et     voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> gauche + accelere ATTENTION BORDER
            si      voiture sur trois case et     voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite ->  gauche + accelere ATTENTION BORDER
            si      voiture sur trois case et pas voiture fil de droite et     autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> constant
            si      voiture sur trois case et pas voiture fil de droite et     autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> ralenti + droite
            si      voiture sur trois case et pas voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> accelere
            si      voiture sur trois case et pas voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> gauche + accelere ATTENTION BORDER
            si  pas voiture sur trois case et     voiture fil de droite et     autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> constant
            si  pas voiture sur trois case et     voiture fil de droite et     autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> constant 
            si  pas voiture sur trois case et     voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> accelere 
            si  pas voiture sur trois case et     voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> accelere
            si  pas voiture sur trois case et pas voiture fil de droite et     autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> constant
            si  pas voiture sur trois case et pas voiture fil de droite et     autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> droite + ralenti
            si  pas voiture sur trois case et pas voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et      autres voiture a droite -> accelere
            si  pas voiture sur trois case et pas voiture fil de droite et pas autres voiture sur fil  de gauche a distance position et pas  autres voiture a droite -> droite + accelere
            */
            
            for(Car& aCar: cars){
                int dy = GRID_Y;
                bool carAtLeft = false;
                bool carAtRight = false;
                bool carAtFront = false;
                bool carOnLeftWay = false;
                bool carOnRightWay = false;
                int maxVicinity = 0;
                carOnLeftWay = aCar.isOnHighSpeedWay();
                carOnRightWay = aCar.isOnLowSpeedWay();
                for(Car anotherCar: cars){
                    if ((anotherCar.getPositionY() == aCar.getPositionY()) && (anotherCar.getPositionX() == aCar.getPositionX())) continue;
                    carAtLeft |= aCar.isAdjacent(anotherCar,Traffic::LEFT);
                    carAtRight |=  aCar.isAdjacent(anotherCar,Traffic::RIGHT);
                    carAtFront |=  aCar.isAdjacent(anotherCar,Traffic::FRONT); 
                }
                updateCar(aCar,carAtLeft,carAtRight,carAtFront,carOnLeftWay,carOnRightWay);
            }
        }

        void drawTraffic(){
            int nbCars = getNbCars();
            Car* cars = getCars();
            bool carFinAtPosition = false; 
            for(int i=0; i < GRID_X;i++){
                for(int j=0; j< GRID_Y;j++){
                    for (int k=0; k< nbCars; k++){
                        if(cars[k].getPositionX() == i && cars[k].getPositionY() == j){
                            carFinAtPosition = true;
                            
                            break;
                        }       
                    }
                    if(carFinAtPosition == true){
                        std::cout << "#" << std::flush;
                    }
                    else{
                        std::cout << "_" << std::flush;
                    }
                    carFinAtPosition = false;
                }
                std::cout << std::endl;
            }
        }
};


int main(){
    srand(time(NULL));
    std::cout << "hello world !" << std::endl;
    Traffic traffic = Traffic(NUMBER_CAR);
    std::system("clear");
    traffic.drawTraffic();
    int i = 0;
    do{
        traffic.drawTraffic();
        traffic.updateAllCars();
        usleep(100000);
        //sleep(4);
        std::system("clear");
        i++;
    }while(i<100);
    //std::this_thread::sleep_for(std::chrono::milliseconds(x));
    return 0;
}
