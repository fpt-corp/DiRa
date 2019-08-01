#include <IOInterface.h>
#include <signal.h>
#include <stdlib.h>

int SCAN_ROW = 210;
int SCAN_WIDTH = 40;
int ROAD_SIZE = 80;
int LANE_HUG = 1;
int DIFF_THRESHOLD = 50;
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
        x_scan=150+(ROAD_SIZE/2)*LANE_HUG;
        //io.setCamAngle(-20);
        sleep(0.5);
        io.printLCD(0,0,"Running!!!");
        int frame_id = 10;
        while(true){
            if (stop)
                break;
            color=io.getBGR();
            depth=io.getDepth();
            cvtColor(color,birdView,COLOR_BGR2GRAY);
            Point2f p1[4] = {Point2f(120, 80), Point2f(200, 80), Point2f(320, 240), Point2f(0, 240)};
            Point2f p2[4] = {Point2f(125, 0), Point2f(175, 0), Point2f(200, 290), Point2f(120, 290)};
            Mat M = getPerspectiveTransform(p1, p2);
            warpPerspective(birdView, birdView, M, Size(300, 300));
            //Mat r = getRotationMatrix2D(Point(150,300), -16, 1.0);
            //warpAffine(birdView, birdView, r, birdView.size());  
            // threshold(birdView,birdView,250,255,THRESH_BINARY);
            //doi lan x_scan+=LANE_HUG*ROAD_SIZE
            int x_white=-1;
            int base_color=birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2));
            for(int i = 1; i<SCAN_WIDTH; i++)
            { 
                if(abs(birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2-i))
                      -birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2-i+1)))>DIFF_THRESHOLD)
                // if(birdView.at<uchar>(SCAN_ROW,x_scan-LANE_HUG*(SCAN_WIDTH/2-i))!=base_color)
                {
                    x_white=x_scan-LANE_HUG*(SCAN_WIDTH/2-i);
                    break;
                }
            }
            if(x_white>-1){
                circle(birdView,Point(x_white,SCAN_ROW),3,50,-1);
                x_scan=x_white;
                D=x_scan-150-LANE_HUG*ROAD_SIZE/2-P;
                P=x_scan-150-LANE_HUG*ROAD_SIZE/2;
                //cout<<P<<":"<<D<<endl;
                angle=-(2.5*P+0.2*D);
            }else{
                // if((io.getIMUPose()[2]<140&&io.getIMUPose()[2]>100) && (frame_id > 3 * 30)){
                //     for(int i=0;i<0.5*30;i++){ // time sleep
                //         if (stop)
                //             break;
                //         angle=-1.2*(102-io.getIMUPose()[2]);
                //         io.setSpeed(20);
                //         io.setSteer(angle);            
                //         usleep(33333);
                //         cout<<angle<<endl;    
                //     }
                // }
            }            
            circle(birdView,Point(x_scan-SCAN_WIDTH/2,SCAN_ROW),3,100,-1);
            line(birdView,Point(x_scan-SCAN_WIDTH/2,SCAN_ROW),Point(x_scan+SCAN_WIDTH/2, SCAN_ROW),255);
            cvtColor(birdView,birdView,COLOR_GRAY2BGR);
	        video.write(birdView);
            int btn=io.getLCDkey();
            if(btn==5){
                break;
            }
            if (abs(angle) < 20)
                io.setSpeed(min(frame_id, 20 + extra_speed));
            else
                io.setSpeed(min(frame_id, 20 + extra_speed));
            io.setSteer(angle);
            frame_id++;
        }
    }
    video.release();
    abort();
    return 0;
}
