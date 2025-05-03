#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <time.h>
#include <stdbool.h>
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

        bool isAdjacentLeft(Car bCar){
            if((getPositionX() - bCar.getPositionX()) == 1){
                // m=60 - m=0 marche pas -> (m=60  + max_vicinity) % grid_y - m=0 
                int dist = (getPositionY() + MAX_VICINITY - bCar.getPositionY()) % GRID_Y;
                if(( dist < MAX_VICINITY && abs(dist) < MAX_VICINITY)){
                    return true;
                }
            }
            return false;
        }

        bool isAdjacentRight(Car bCar){
            if((getPositionX() - bCar.getPositionX()) == -1){
                int dist = (getPositionY() + MAX_VICINITY - bCar.getPositionY()) % GRID_Y;
                if(( dist < MAX_VICINITY && abs(dist) < MAX_VICINITY)){
                    return true;
                }
            }
            return false;
        }
        bool isAdjacentFront(Car bCar){
            int dist = (getPositionY() + MAX_VICINITY - bCar.getPositionY()) % GRID_Y;
            if(( dist < MAX_VICINITY && abs(dist) < MAX_VICINITY)){
                return true;
            }
            return false;
        }
        bool isOnHighSpeedWay(){
            if(getPositionX()==0){
                return true;
            }
            return false;
        }
        bool isOnLowSpeedWay(){
            if(getPositionX()==4){
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

        Car* getCars(){
            return cars;
        }

        int getNbCars(){
            return sizeof(cars)/sizeof(cars[0]);
        }

        

        void update(){
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
            int dy = GRID_Y;
            bool carAtLeft = false;
            bool carAtRight = false;
            bool carAtFront = false;
            for(Car aCar: cars){
                for(Car anotherCar: cars){
                    
                    if ((anotherCar.getPositionY() != aCar.getPositionY()) && (anotherCar.getPositionX() != aCar.getPositionX()))
                    {
                        dy = anotherCar.getPositionY() - aCar.getPositionY();
                        carAtLeft = aCar.isAdjacentLeft(anotherCar);
                        carAtRight =  aCar.isAdjacentRight(anotherCar);
                        carAtFront =  aCar.isAdjacentFront(anotherCar);
                        if((dy<MAX_VICINITY) && (carAtFront) && (carAtLeft) && (carAtRight)){
                            aCar.decreaseSpeed();
                            break;
                        }
                        else if((dy<MAX_VICINITY) && (carAtFront) && (carAtLeft) && (!carAtRight)){
                            aCar.decreaseSpeed();
                            break;
                        }
                        else if((dy<MAX_VICINITY) && (carAtFront) && (!carAtLeft) && (carAtRight)){
                            aCar.increaseSpeed(); 
                            aCar.goLeft();
                            break;                          
                        }
                        else if((dy<MAX_VICINITY) && (carAtFront) && (!carAtLeft) && (!carAtRight)){
                            aCar.increaseSpeed(); 
                            aCar.goLeft();
                            break;
                        }
                        else if((dy<MAX_VICINITY) && (!carAtFront) && (carAtLeft) && (carAtRight)){
                            // do nothing
                            break;
                        }
                        else if((dy<MAX_VICINITY) && (!carAtFront) && (carAtLeft) && (!carAtRight)){
                            aCar.decreaseSpeed(); 
                            aCar.goRight();
                            break;
                        }
                        else if((dy<MAX_VICINITY) && (!carAtFront) && (!carAtLeft) && (carAtRight)){
                            aCar.increaseSpeed();
                            break;
                        }
                        else if((dy<MAX_VICINITY) && (!carAtFront) && (!carAtLeft) && (!carAtRight)){
                            aCar.increaseSpeed(); 
                            aCar.goLeft();
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (carAtFront) && (carAtLeft) && (carAtRight)){
                            // do nothing
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (carAtFront) && (carAtLeft) && (!carAtRight)){
                            // do nothing
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (carAtFront) && (!carAtLeft) && (carAtRight)){
                            aCar.increaseSpeed();
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (carAtFront) && (!carAtLeft) && (!carAtRight)){
                            aCar.increaseSpeed();
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (!carAtFront) && (carAtLeft) && (carAtRight)){
                            // do nothing
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (!carAtFront) && (carAtLeft) && (!carAtRight)){
                            aCar.decreaseSpeed(); 
                            aCar.goRight();
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (!carAtFront) && (!carAtLeft) && (carAtRight)){
                            aCar.increaseSpeed();
                            break;
                        }
                        else if(!(dy<MAX_VICINITY) && (!carAtFront) && (!carAtLeft) && (!carAtRight)){
                            aCar.increaseSpeed(); 
                            aCar.goRight();
                            break;
                        }
                        else{
                            throw std::invalid_argument( "update implementation not valid" );
                        }

                    }   
                    
                }
                dy = 0;
                carAtLeft = false;
                carAtRight = false;
                carAtFront = false;
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
                        std::cout << "#";
                    }
                    else{
                        std::cout << "_";
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
    traffic.drawTraffic();
    // define car 
    // while simulation true
    // draw 
    // calculate
    // update 
    // sleep
    sleep(4);
    //std::this_thread::sleep_for(std::chrono::milliseconds(x));
    return 0;
}
