#include "vat_can.h"

double vat_can::angle =0;
int vat_can::start = 0;
int vat_can::a = 0;
int vat_can::bien_bao = 0;
int vat_can::lane = 0;
int vat_can::co = 0;
bool vat_can::b3 = false;

vat_can::vat_can(){
    vat_can_publisher = vc.advertise<std_msgs::Float32>("/set_vat_can",10);
}
vat_can::~vat_can(){}


void vat_can::vat_can_a()
{
  std_msgs::Float32 la;
  if ( vat_can::a == 4){

    // ben phai
    if(vat_can::bien_bao == 1){
	if ( vat_can::angle >= 30 && vat_can::angle <= 35 && vat_can::co == 0) vat_can::co = 1;
	if (  vat_can::angle > 35 && vat_can::co == 1) vat_can::co = 2;
	if ( vat_can::angle >= 20 && vat_can::angle <= 25 && vat_can::co == 2)
        {
          vat_can::lane = 2;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::co = 3;
        }
        if ( vat_can::co == 3 ){
          sleep(2);
          vat_can::lane = 4;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::co = 4;
        }
        if ( vat_can::angle >= -179 && vat_can::angle <= -170 && vat_can::co == 4 && vat_can::b3 == false){
          vat_can::lane = 1;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          sleep(1);
          vat_can::lane = 0;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::a = 0;
          vat_can::co = 0;
        }
        if ( vat_can::angle > 150 && vat_can::angle < 155 && vat_can::co == 4 && vat_can::b3 == true ){ vat_can::co = 5; cout << "1" << endl; }
        if ( vat_can::angle < 150  && vat_can::co == 5 && vat_can::b3 == true ){ vat_can::co = 6; cout << "2" << endl; }
        if ( vat_can::angle >= 130 && vat_can::angle <= 138 && vat_can::co == 6 && vat_can::b3 == true ) {
          vat_can::lane = 1;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          sleep(1);
          vat_can::lane = 0;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::a = 0;
          vat_can::co = 0;
	  cout << "3" << endl; 
        }
    }
    // ben trai
    if(vat_can::bien_bao == 2 ){
        if ( vat_can::angle >= -30 && vat_can::angle <= -25 && vat_can::co == 0) vat_can::co = 1;
	      if ( vat_can::angle < -30  && vat_can::co == 1) vat_can::co = 2;
        if ( vat_can::angle >= -30 && vat_can::angle <= -25 && vat_can::co == 2)
        {
          vat_can::lane = 1;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::co = 3;
        }
        if ( vat_can::co == 3 ){
          sleep(2);
          vat_can::lane = 4;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::co = 4;
        }
        if ( vat_can::angle > 170 && vat_can::a == 4 && vat_can::b3 == false){
          vat_can::lane = 2;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          sleep(1);
          vat_can::lane = 0;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::a = 0;
          vat_can::co = 0;
        }
        if ( vat_can::angle > -145 && vat_can::angle < -140 && vat_can::co == 4 && vat_can::b3 == true ) vat_can::co = 5;
        if ( vat_can::angle >-140  && vat_can::co == 5 && vat_can::b3 == true ) vat_can::co = 6;
        if ( vat_can::angle > -125 && vat_can::angle < -120 && vat_can::co == 6 && vat_can::b3 == true ){
          vat_can::lane = 2;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          sleep(1);
          vat_can::lane = 0;
          la.data = vat_can::lane;   
          vat_can_publisher.publish(la);
          vat_can::a = 0;
          vat_can::co = 0;
        }
    }
  }
  
   
  
  la.data = vat_can::lane;   
  vat_can_publisher.publish(la);
}
