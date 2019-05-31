

 #include "detectlane.h"
// #include "signclassify.h"
// #include "objectdetect.h"

DetectLane::~DetectLane() {}
DetectLane::DetectLane() {}
 
// SignClassify *signClassify;
// ObjectDetect *objectDetect;

//hàm update lại vị trí lPoint và rPoint
//input: frame hiện tại
//output: thay đổi thông số lPoint và rPoint là điểm muốn đến
void DetectLane::update(const Mat &src)
{
   //get_ImageLine(src);
   ImageLine = get_ImageLine(src);
   cvtColor(ImageLine, ImageDraw, CV_GRAY2RGB);

   // signClassify = new SignClassify();
   // objectDetect = new ObjectDetect();
   // objectDetect->update(src);
   
   // vector<int> ClassIds = objectDetect->ClassIds;
   // vector<Rect> Data = objectDetect->Data;

 	
   // for(int i = 0; i < (int)Data.size(); ++i){
   // 	Mat img;
   // 	if(signClassify->cropImage(src,img,Data[i]) == -1) continue;
   //      int id = signClassify->classify(img);
   // }        
      

   //Mat ImageDraw =  get_ImageBirdView(binImg);
   //Mat road = getRoad(src, ImageLine.cols-10);
   //imshow("TEST HSV", road);
   if (test == true)
   {
      lLine.init_Line(LEFT);
      rLine.init_Line(RIGHT);
      SIZE_LANE = rLine.BeginPoint.x - lLine.BeginPoint.x;
      //cout<<rLine.BeginPoint<<lLine.BeginPoint<<endl;
      //waitKey(1000);

      test = false;
   }

   cout << "----------------------------------" << endl;
   if(lLine.Status ==true)
       lLine.get_ListPoint(ImageLine, lLine.BeginPoint);
   if(rLine.Status ==true)
      rLine.get_ListPoint(ImageLine, rLine.BeginPoint);

   if (lLine.Status == false && rLine.Status == true)
      lLine = get_left_by_right(rLine);
   if (rLine.Status == false && lLine.Status == true)
      rLine = get_right_by_left(lLine);
   //}
   //}
   if(rLine.Status == true){
      midLine = get_mid_by_right(rLine);
      
      // midLine.Function[0] = rLine.Function[0];
      // midLine.Function[1] = rLine.Function[1];
      // midLine.Function[2] = rLine.Function[2] - SIZE_LANE / 2;
   }else if(lLine.Status == true){
        midLine = get_mid_by_left(lLine);
   }
   

   //  if(rLine.Status == true){
   //    cout<<"RIGHT"<<endl;
   //    midLine = get_mid_by_right(rLine);
   //    if(lLine.Status==false){
   //       cout<<"NO LEFT"<<endl;
   //       lLine = get_left_by_right(rLine);
   //    }

   //  }else if(lLine.Status == true){
   //     cout<<"NO RIGHT"<<endl;
   //     midLine = get_mid_by_left(lLine);
   //     rLine = get_right_by_left(lLine);
   //  }
   //  if(rLine.Status == false && lLine.Status == false)
   //    cout<<"NOOOOOOOOOOOOOOOOOOO"<<endl;

   //  if(lLine.Status == false){
   //     cout<<"NO LEFT"<<endl;
   //    midLine = get_mid_by_right(rLine);
   //    lLine = get_left_by_right(rLine);
   //  }

   //midLine.Function = get_Function_Line(midLine.ListPoint);

   //rLine.get_ListPoint(ImageDraw,rLine.BeginPoint);

   //circle(imgBRG, lLine.BeginPoint, 1, Scalar(0, 0, 255), 2, 8, 0);
   //imshow("BGR View", imgBRG);
   // for(int i=0; i<lLine.ListPoint.size();i++){
   //     circle(imgBRG, lLine.ListPoint[i], 1, Scalar(0, 0, 255), 2, 8, 0);
   // }
   if (lLine.Status == true)
   {
      circle(ImageDraw, lLine.BeginPoint, 1, Scalar(255, 0, 0), 20, 8, 0);
      for (int i = 0; i < lLine.ListPoint.size(); i++)
      {
         circle(ImageDraw, lLine.ListPoint[i], 1, Scalar(255, 0, 0), 8, 8, 0);
         for (int y = 0; y < ImageDraw.rows; y++)
         {
            int xDraw = lLine.get_X_by_Y(y);
            Point drawLine = Point(xDraw, y);
            //cout<<"Point"<<drawLine<<endl;
            circle(ImageDraw, drawLine, 1, Scalar(255, 0, 0), 1, 1, 0);
         }
      }
   }
   if (rLine.Status == true)
   {
      circle(ImageDraw, rLine.BeginPoint, 1, Scalar(0, 255, 0), 20, 8, 0);
      for (int i = 0; i < rLine.ListPoint.size(); i++)
      {
         circle(ImageDraw, rLine.ListPoint[i], 1, Scalar(0, 255, 0), 8, 8, 0);
         for (int y = 0; y < ImageDraw.rows; y++)
         {
            int xDraw = rLine.get_X_by_Y(y);
            Point drawLine = Point(xDraw, y);
            //cout<<"Point"<<drawLine<<endl;
            circle(ImageDraw, drawLine, 1, Scalar(0, 255, 0), 1, 1, 0);
         }
      }
   }
      
      for (int y = 0; y < ImageDraw.rows-1; y++)
      {
         int xDraw = midLine.get_X_by_Y(y);
         Point drawLine = Point(xDraw, y);
         //ImageDraw.at<Vec3b>(drawLine.y, drawLine.x) = Vec3b(0, 0, 255);
         circle(ImageDraw, drawLine, 1, Scalar(0, 0, 255), 1, 1, 0);
         //cout<<"Point"<<drawLine<<endl;
     
      }
      // for(int y=0; y<imgBRG.rows; y++){
      //     int xDraw = rLine.get_X_by_Y(y);
      //     Point drawLine = Point(xDraw,y);
      //     //cout<<"Point"<<drawLine<<endl;
      //     circle(imgBRG, drawLine, 1, Scalar(255, 255, 0), 2, 8, 0);
      // }
   
    
      //waitKey(0);
}

Mat DetectLane::get_ImageBirdView(const Mat &src)
{
   Mat lambda, result;
   Point2f inQuad[4] = {Point(0, 100), Point(COLS - 1, 100), Point(0, ROWS - 1), Point(COLS - 1, ROWS - 1)};
   Point2f outQuad[4] = {Point(0, 100), Point(COLS - 1, 100), Point(115, ROWS - 1), Point(COLS - 115, ROWS - 1)};
   lambda = getPerspectiveTransform(inQuad, outQuad);
   warpPerspective(src, result, lambda, result.size());
   return result;
}

Mat DetectLane::get_ImageLine(const Mat &src)
{

   Mat hsvImgInput, get_ImageLine, imgGRAY;
   //  cvtColor(src, hsvImgInput, COLOR_BGR2HSV);
   //  Scalar lower = Scalar(0, 0, 230);
   //  Scalar higher = Scalar(180, 25, 255);
   //  cv::inRange( hsvImgInput, lower, higher, get_ImageLine);

   cvtColor(src, imgGRAY, COLOR_BGR2GRAY);
   threshold(imgGRAY, get_ImageLine, 150, 255, 0);

   get_ImageLine = get_ImageBirdView(get_ImageLine);

   //erode(lane, lane, getStructuringElement(MORPH_ELLIPSE, Size(2, 2 )));
   //dilate(lane, lane, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

   //  blur(get_ImageLine, get_ImageLine, Size(3,3));
   //  Canny(get_ImageLine, get_ImageLine, 5, 100, 3);

   //imshow("get_ImageLine",get_ImageLine);
   //Rect ROI(0, 0, get_ImageLine.cols - 1, get_ImageLine.rows - 40);
   return get_ImageLine;
}

LINE DetectLane::get_mid_by_right(const LINE &rightLine)
{
   LINE midLine;
   if (rightLine.Status == true)
   {
      for (int i = 0; i < rightLine.ListPoint.size() - 1; i++)
      {
         Point top = rightLine.ListPoint[i + 1];
         Point bot = rightLine.ListPoint[i];
         float angle = get_Angle(top, bot);
         Point tmp;
         cout << "Angle:" << angle << endl;
         if (angle == 0)
         {
            tmp.y = bot.y;
            tmp.x = bot.x - (SIZE_LANE / 2);
         }
         else if (angle > 0)
         {
            tmp.y = bot.y - abs(sin(angle * PI / 180)) * (SIZE_LANE / 2);
            tmp.x = bot.x - abs(cos(angle * PI / 180)) * (SIZE_LANE / 2);
         }
         else if (angle < 0)
         {
            tmp.x = bot.x - abs(cos(-angle * PI / 180)) * (SIZE_LANE / 2);
            tmp.y = bot.y + abs(sin(-angle * PI / 180)) * (SIZE_LANE / 2);
         }
         // Point test = Point(rightLine.ListPoint[i].y, rightLine.ListPoint[i].x);
         // Point tmp = solve(rightLine.Function, test,SIZE_LANE/2);
         // test = Point(tmp.y, tmp.x);
      
         // circle(ImageDraw, test, 1, Scalar(0, 0, 255), 2, 8, 0);
         if (tmp.x>=0 && tmp.x<ImageDraw.cols && tmp.y >= 0 && tmp.y<=ImageLine.rows){
            midLine.ListPoint.push_back(tmp);
            
         }
            
      }
      
      if (midLine.ListPoint.size() > 2)
      {
         midLine.Status = true;
         midLine.BeginPoint = midLine.ListPoint[2];
         midLine.Function = get_Function_Line(midLine.ListPoint);
         
      }
   }
   return midLine;
}

LINE DetectLane::get_mid_by_left(const LINE &leftLine)
{
   LINE midLine;
   if (leftLine.Status == true)
   {
      cout << leftLine.ListPoint.size() - 1 << endl;
      for (int i = 0; i < leftLine.ListPoint.size() - 1; i++)
      {
         Point top = leftLine.ListPoint[i + 1];
         Point bot = leftLine.ListPoint[i];
         float angle = get_Angle(top, bot);
         Point tmp;
         if (angle == 0)
         {
            tmp.y = bot.y;
            tmp.x = bot.x + (SIZE_LANE / 2);
         }
         else if (angle > 0)
         {
            tmp.y = bot.y + abs(sin(angle * PI / 180)) * (SIZE_LANE / 2);
            tmp.x = bot.x + abs(cos(angle * PI / 180)) * (SIZE_LANE / 2);
         }
         else if (angle < 0)
         {
            tmp.x = bot.x + abs(cos(-angle * PI / 180)) * (SIZE_LANE / 2);
            tmp.y = bot.y - abs(sin(-angle * PI / 180)) * (SIZE_LANE / 2);
         }
         // cout<<"bot: "<<bot<<endl;
         // cout<<"top: "<<top<<endl;
         // cout<<"angle: "<<angle<<endl;
         // cout<<"tmp: "<<tmp<<endl;
         if (tmp.x>=0 && tmp.x<ImageLine.cols && tmp.y >= 0 && tmp.y<=ImageLine.rows)
            midLine.ListPoint.push_back(tmp);
      }
      if (midLine.ListPoint.size() > 3)
      {
         midLine.Status = true;
         midLine.BeginPoint = midLine.ListPoint[3];
         midLine.Function = get_Function_Line(midLine.ListPoint);
      }
   }
   return midLine;
}

LINE DetectLane::get_right_by_left(const LINE &leftLine)
{
   LINE rightLine;
   // cout<<"---------------------------------------------------"<<endl;
   rightLine.ListPoint.clear();
   if (leftLine.Status == true)
   {
      for (int i = 0; i < leftLine.ListPoint.size() - 1; i++)
      {
         Point top = leftLine.ListPoint[i + 1];
         Point bot = leftLine.ListPoint[i];
         float angle = get_Angle(top, bot);
         Point tmp;
         if (angle == 0)
         {
            tmp.y = bot.y;
            tmp.x = bot.x + (SIZE_LANE);
         }
         else if (angle > 0)
         {
            tmp.y = bot.y + abs(sin(angle * PI / 180)) * (SIZE_LANE);
            tmp.x = bot.x + abs(cos(angle * PI / 180)) * (SIZE_LANE);
         }
         else if (angle < 0)
         {
            tmp.x = bot.x + abs(cos(-angle * PI / 180)) * (SIZE_LANE);
            tmp.y = bot.y - abs(sin(-angle * PI / 180)) * (SIZE_LANE);
         }
         // cout<<"bot: "<<bot<<endl;
         // cout<<"top: "<<top<<endl;
         // cout<<"angle: "<<angle<<endl;
         // cout<<"tmp: "<<tmp<<endl;
         if (check_Point_in_Mat(ImageDraw, tmp))
         {
            circle(ImageDraw, tmp, 1, Scalar(255, 0, 0), 2, 2, 0);
            rightLine.get_ListPoint(ImageLine, tmp);
            if (rightLine.Status == true)
               return rightLine;
         }
      }
   }
   return rightLine;
}

LINE DetectLane::get_left_by_right(const LINE &rightLine)
{
   LINE leftLine;
   // cout<<"---------------------------------------------------"<<endl;
   leftLine.ListPoint.clear();
   if (rightLine.Status == true)
   {
      for (int i = 0; i < rightLine.ListPoint.size() - 1; i++)
      {
         Point top = rightLine.ListPoint[i + 1];
         Point bot = rightLine.ListPoint[i];
         float angle = get_Angle(top, bot);
         Point tmp;
         if (angle == 0)
         {
            tmp.y = bot.y;
            tmp.x = bot.x - (SIZE_LANE);
         }
         else if (angle > 0)
         {
            tmp.y = bot.y - abs(sin(angle * PI / 180)) * (SIZE_LANE);
            tmp.x = bot.x - abs(cos(angle * PI / 180)) * (SIZE_LANE);
         }
         else if (angle < 0)
         {
            tmp.x = bot.x - abs(cos(-angle * PI / 180)) * (SIZE_LANE);
            tmp.y = bot.y + abs(sin(-angle * PI / 180)) * (SIZE_LANE);
         }
         // cout<<"bot: "<<bot<<endl;
         // cout<<"top: "<<top<<endl;
         // cout<<"angle: "<<angle<<endl;
         // cout<<"tmp: "<<tmp<<endl;
         if (check_Point_in_Mat(ImageDraw, tmp))
         {
            circle(ImageDraw, tmp, 1, Scalar(0, 255, 0), 2, 2, 0);
            leftLine.get_ListPoint(ImageLine, tmp);

            if (leftLine.Status == true)
               return leftLine;
         }
      }
   }
   return leftLine;
}
//  void DetectLane::get_mid_by_LeftAndRight(LINE &rightLine,const LINE &leftLine, const LINE &rightLine){

//  }

Mat DetectLane::getRoad(const Mat &src, int y_Value)
{
    Mat roadHSV, road;
    cvtColor(src, roadHSV, COLOR_BGR2HSV);
    //GaussianBlur(roadHSV, roadHSV,Size(3,3),0,0);
    imshow("Blur", roadHSV);
    for (int i = -5; i < 5; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Mat tmp;
            int h_Road = get_value_Mat_3D(roadHSV, roadHSV.rows - 1 - j, roadHSV.cols / 2 + i, 0);
            int s_Road = get_value_Mat_3D(roadHSV, roadHSV.rows - 1 - j, roadHSV.cols / 2 + i, 1);
            int v_Road = get_value_Mat_3D(roadHSV, roadHSV.rows - 1 - j, roadHSV.cols / 2 + i, 2);

            Scalar minHsvRoad = Scalar(fixPixelValue(h_Road - 15), fixPixelValue(s_Road - 30), fixPixelValue(v_Road - 30));
            Scalar maxHsvRoad = Scalar(fixPixelValue(h_Road + 15), fixPixelValue(s_Road + 30), fixPixelValue(v_Road + 30));
            inRange(roadHSV, minHsvRoad, maxHsvRoad, tmp);
            imshow("tmp", tmp);
            if (i == -5)
            {
                road = tmp.clone();
            }
            else
            {
                road = road + tmp.clone();
            }
        }
    }
    //Khử nhiễu
    int dilation_size = 1;
    //dilate(road, road, getStructuringElement(MORPH_ELLIPSE, Size(2 * dilation_size + 1, 2 * dilation_size + 1)));
    //erode(road, road, getStructuringElement(MORPH_ELLIPSE, Size(3 * dilation_size + 1, 3 * dilation_size + 1)));
    return road;
}
