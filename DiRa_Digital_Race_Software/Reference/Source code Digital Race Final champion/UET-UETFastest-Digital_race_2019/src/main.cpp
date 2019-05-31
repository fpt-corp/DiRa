#include <IOInterface.h>
#include <signal.h>
#include <stdlib.h>

int SCAN_ROW = 210;
int SCAN_WIDTH = 60;
int ROAD_SIZE = 80;
int LANE_HUG = 1;
int DIFF_THRESHOLD = 40;
IOInterface io;
Mat color,depth,birdView;
Vec3f imuPose;
float speed,angle,P=0,D=0;
int x_scan=200;
bool stop = false;
int extra_speed = 0;
VideoWriter video("outvideo.avi",CV_FOURCC('M','J','P','G'),20, Size(300,300));

void my_handler(int s)
{
    if (stop)
        abort();
    cout << "quit" << endl;
    io.setSpeed(0);
    stop = true;
}

int main(){
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    io.setCamAngle(0);
    
    while(true){
        int mode=io.chooseCarMode();
        stop = false;
        io.ready();
        
        sleep(0.5);
        io.printLCD(0,0,"Running!!!");
        int frame_id = 10;
        auto start = std::chrono::system_clock::now();
        double tick=0;
        bool trigger=true;
        LANE_HUG==1;
        x_scan=150+ROAD_SIZE/2*LANE_HUG;
        io.setCamAngle(-20*LANE_HUG);
        usleep(200000);
        while(true){
            if (stop)
                break;
            tick=chrono::duration_cast<chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
            if(tick<6100){
               if(LANE_HUG==1){
                   LANE_HUG = -1;
                   x_scan+=1.5*ROAD_SIZE*LANE_HUG;
                   io.setCamAngle(-20*LANE_HUG);
               }
            }
            else if(tick<12000){
               if(LANE_HUG==-1){
                   LANE_HUG = 1;
                   x_scan+=ROAD_SIZE*LANE_HUG;
                   io.setCamAngle(-20*LANE_HUG);
                   usleep(200000);
               }
            }
            else if(tick<50000){
               if(LANE_HUG==-1){
                   LANE_HUG = 1;
                   x_scan+=ROAD_SIZE*LANE_HUG;
                   io.setCamAngle(-20*LANE_HUG);
                   usleep(200000);
               }
            }
            // else if(tick<60000){
            //    if(LANE_HUG==1){
            //        LANE_HUG = -1;
            //        x_scan+=ROAD_SIZE*LANE_HUG;
            //        io.setCamAngle(-20*LANE_HUG);
            //        usleep(200000);
            //    }
            // }
            // else if(tick<7000){
            //     if(LANE_HUG==1){
            //        LANE_HUG = -1;
            //        x_scan+=ROAD_SIZE*LANE_HUG;
            //        io.setCamAngle(-20*LANE_HUG);
            //     }
            // }
            // else if(tick<10000){
            //     if(LANE_HUG==-1){
            //        LANE_HUG =1;
            //        x_scan+=ROAD_SIZE*LANE_HUG;
            //        io.setCamAngle(-20*LANE_HUG);
            //     }
            // }
            cout<<"HUG"<<LANE_HUG<<endl;
            //     LANE_HUG = -1;
            //     ROAD_SIZE=90;
            //     x_scan+=ROAD_SIZE*LANE_HUG;
            //     LANE_HUG = 1;
            //     ROAD_SIZE=90;
            //     x_scan=150+(ROAD_SIZE/2)*LANE_HUG;
            // } else if(tick<6000){
            //     LANE_HUG = -1;
            //     ROAD_SIZE=90;
            //     x_scan+=ROAD_SIZE*LANE_HUG;
            // } else if(tick<10000){
            //     LANE_HUG = 1;
            //     ROAD_SIZE=80;
            //     x_scan=150+(ROAD_SIZE/2)*LANE_HUG;
            //     // LANE_HUG = -1;
            //     // x_scan=150+(ROAD_SIZE/2)*LANE_HUG;
            // } else
            // { 
            //     break;}
            color=io.getBGR();
            depth=io.getDepth();
            cvtColor(color,birdView,COLOR_BGR2GRAY);
            Point2f p1[4] = {Point2f(110, 80), Point2f(190, 80), Point2f(320, 240), Point2f(0, 240)};
            Point2f p2[4] = {Point2f(125, 0), Point2f(175, 0), Point2f(190, 290), Point2f(110, 290)};
            Mat M = getPerspectiveTransform(p1, p2);
            warpPerspective(birdView, birdView, M, Size(300, 300));
            Mat r = getRotationMatrix2D(Point(150,300), -16*LANE_HUG, 1.0);
            warpAffine(birdView, birdView, r, birdView.size());
            threshold(birdView,birdView,220,255,THRESH_BINARY);
            int x_white=-1;
            int base_color=birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2));
            for(int i = 1; i<SCAN_WIDTH; i++)
            { 
                // if(abs(birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2-i))
                //       -birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2-i+1)))>DIFF_THRESHOLD)
                if(birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2-i))!=base_color)
                {
                    x_white=x_scan-LANE_HUG*(SCAN_WIDTH/2-i);
                    break;
                }
            }
            if(x_white>-1){
                circle(birdView,Point(x_white,SCAN_ROW),3,50,-1);
                x_scan=x_white;
            //    x_scan=150-ROAD_SIZE/2;
                D=x_scan-150-LANE_HUG*ROAD_SIZE/2-P;
                P=x_scan-LANE_HUG*ROAD_SIZE/2-150;
                //cout<<P<<":"<<D<<endl;
                angle=-(2.5*P);
                cout<<angle<<endl;
            }   
            
            circle(birdView,Point(x_scan-LANE_HUG*SCAN_WIDTH/2,SCAN_ROW),3,100,-1);
            line(birdView,Point(x_scan-SCAN_WIDTH/2,SCAN_ROW),Point(x_scan+SCAN_WIDTH/2, SCAN_ROW),255);
            cvtColor(birdView,birdView,COLOR_GRAY2BGR);
	        video.write(birdView);
            int btn=io.getLCDkey();
            if(btn==5){
                break;
            }
            if (abs(angle) < 20)
                io.setSpeed(min(frame_id, 18 + extra_speed));
            else
                io.setSpeed(min(frame_id, 18 + extra_speed));
            io.setSteer(angle);
            frame_id++;
        }
        cout<<"tick"<<tick<<endl;
    }
    video.release();
    abort();
    return 0;
}
