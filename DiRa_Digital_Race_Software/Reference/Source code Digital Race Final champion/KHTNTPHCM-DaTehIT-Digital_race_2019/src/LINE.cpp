#include "LINE.h"

 LINE::LINE(){
    Function = new float[3];
    Status = false;
  }
 LINE::~LINE(){

 }

 bool LINE::init_Line(int type){

    if(type == LEFT)
        for(int y = ImageLine.rows-1-H_TRACKING ; y>H_TRACKING; y = y - 2*H_TRACKING){
            
            int maxNonZero = 0;
            Point maxPoint;
            for(int x=W_TRACKING; x<ImageLine.cols/2; x=x+2*W_TRACKING){                       
                Rect roiTracking = get_Roi_track(Point(x,y));       
                for(int i=SIZE_TRACKING; i<2*W_TRACKING-SIZE_TRACKING; i++){
                    
                    Rect myROI = get_Roi_tracking(Point(roiTracking.x+i,roiTracking.y+H_TRACKING));           
                    Mat imgTracking = ImageLine(myROI);
                    int nonZero = countNonZero(imgTracking);
                    
                    if(nonZero>=maxNonZero){
                        
                        maxNonZero = nonZero;
                        maxPoint = Point(roiTracking.x+i,roiTracking.y+H_TRACKING);                        
                    }
                }                                                  
            }
            if(maxNonZero>0) {
                BeginPoint = maxPoint;
                Status = true;
                return true;
            }
        }
    if(type == RIGHT)
        for(int y = ImageLine.rows-1-H_TRACKING ; y>H_TRACKING; y = y - 2*H_TRACKING){
            int maxNonZero = 0;
            Point maxPoint;
            for(int x=ImageLine.cols-W_TRACKING; x>ImageLine.cols/2; x=x-2*W_TRACKING){                       
                     Rect roiTracking = get_Roi_track(Point(x,y));       
                for(int i=SIZE_TRACKING; i<2*W_TRACKING-SIZE_TRACKING; i++){
                    
                    Rect myROI = get_Roi_tracking(Point(roiTracking.x+i,roiTracking.y+H_TRACKING));           
                    Mat imgTracking = ImageLine(myROI);
                    int nonZero = countNonZero(imgTracking);
                    
                    if(nonZero>=maxNonZero){
                        
                        maxNonZero = nonZero;
                        maxPoint = Point(roiTracking.x+i,roiTracking.y+H_TRACKING);                        
                    }
                }                                                  
            }
            if(maxNonZero>0) {
                BeginPoint = maxPoint;
                Status = true;
                return true;
            }
        }
        return false;
 }

void LINE::get_ListPoint(const Mat &ImageLine, Point &startPoint){
    //test
     //Check down
    Point nextPoint = Point(startPoint.x,startPoint.y+2*H_TRACKING);
    ListPoint.clear();
    while(check_Point_in_Mat(ImageLine,nextPoint)){
        Rect roiTracking = get_Roi_track(nextPoint);
        int maxNonZero = 0;
        Point maxPoint;
        myDraw(ImageDraw,roiTracking,2);
        for(int i=SIZE_TRACKING; i<2*W_TRACKING-SIZE_TRACKING; i++){
                    
            Rect myROI = get_Roi_tracking(Point(roiTracking.x+i,roiTracking.y+H_TRACKING));           
            Mat imgTracking = ImageLine(myROI);
            int nonZero = countNonZero(imgTracking);
                    
           if(nonZero>=maxNonZero){                
                maxNonZero = nonZero;
                maxPoint = Point(roiTracking.x+i,roiTracking.y+H_TRACKING);                        
            }
        }
        
         if(maxNonZero>0){
            nextPoint=maxPoint;
            ListPoint.insert(ListPoint.begin(),nextPoint);
        }else break;
        nextPoint.y += 2*H_TRACKING;
        
       
    }
    //Check up
    nextPoint = startPoint;
    while(check_Point_in_Mat(ImageLine,nextPoint)){
        Rect roiTracking = get_Roi_track(nextPoint);
        int maxNonZero = 0;
        Point maxPoint;
        myDraw(ImageDraw,roiTracking,2);
        for(int i=SIZE_TRACKING; i<2*W_TRACKING-SIZE_TRACKING; i++){
                    
            Rect myROI = get_Roi_tracking(Point(roiTracking.x+i,roiTracking.y+H_TRACKING));           
            
            Mat imgTracking = ImageLine(myROI);
            int nonZero = countNonZero(imgTracking);
                    
           if(nonZero>=maxNonZero){                
                maxNonZero = nonZero;
                maxPoint = Point(roiTracking.x+i,roiTracking.y+H_TRACKING);                        
            }
        }
        if(maxNonZero>0){
            nextPoint=maxPoint;
            ListPoint.push_back(nextPoint);
            //myDraw(ImageDraw,nextPoint,3);
        }else break;
        
         
        // nextPoint = get_Point_tracking(ImageLine,nextPoint);
        nextPoint.y -= H_TRACKING;
        //if(!check_Point_in_Mat(ImageLine,nextPoint)){
            
        //}
        // cout<<"+++ nextPoint: "<<nextPoint<<endl;
    }     
    if(ListPoint.size()>3){
        BeginPoint = ListPoint[3]; 
        EndPoint = ListPoint[ListPoint.size()-1];
        Function = get_Function_Line(ListPoint) ;
        for(int i=0; i<ListPoint.size();i++){
            ListPoint[i].x = get_X_by_Y(ListPoint[i].y);
        }
        Status = true;
    }else{
        BeginPoint = Point(-1,-1);
        ListPoint.clear();
        Status = false;
    }
}

int LINE::get_X_by_Y(int y){
    return Function[0]*y*y+Function[1]*y + Function[2];
}

// int LINE::fy(int y){
//     int delta = Function[1]*Function[1]-4*Function[0]*(Function[2]-y);
//     cout<<delta<<endl;
//     if(delta>=0)
//         return (-Function[1]+sqrt(delta))/(2*Function[0]);
//     else
//         return -1;
        
//}
Point LINE::get_Point_angle(){
    int y = -Function[1]/(2*Function[0]);
    int x = get_X_by_Y(y);
    return Point(x,y);
}
