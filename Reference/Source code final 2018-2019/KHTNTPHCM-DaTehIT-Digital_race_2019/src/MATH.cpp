#include "MATH.h"
  
Mat ImageDraw;
Mat ImageLine;
int SIZE_LANE;
//Lấy value pixel ảnh kênh màu 2
int get_value_Mat_2D(const Mat &src, int y, int x)
{
    return (int)src.at<uchar>(y, x);
}

//Lấy value pixel ảnh kênh màu 3
int get_value_Mat_3D(const Mat &src, int y, int x, int j)
{
    return (int)src.at<Vec3b>(y, x).val[j];
}

Rect get_Roi_track(Point inpPoint)
{
    return Rect(inpPoint.x - W_TRACKING, inpPoint.y - H_TRACKING, 2 * W_TRACKING, 2 * H_TRACKING);
}

Rect get_Roi_tracking(Point inputPoint)
{
    return Rect(inputPoint.x - SIZE_TRACKING, inputPoint.y - H_TRACKING, 2 * SIZE_TRACKING, 2 * H_TRACKING);
}

Point get_Point_tracking(const Mat &src, Point pointTracking)
{
    int maxNonZero = 0;
    Point maxPoint;
    Rect roiTracking = get_Roi_track(pointTracking);

    for (int i = SIZE_TRACKING; i < 2 * W_TRACKING - SIZE_TRACKING; i++)
    {

        Rect myROI = get_Roi_tracking(Point(roiTracking.x+i+SIZE_TRACKING, roiTracking.y+H_TRACKING));
        Mat imgTracking = src(myROI);
        int nonZero = countNonZero(imgTracking);
        if (nonZero >= maxNonZero)
        {
            maxNonZero = nonZero;
            maxPoint = Point(roiTracking.x+i+SIZE_TRACKING, roiTracking.y+H_TRACKING);
        }
    }

 
    return maxPoint;
}

//oke
float * get_Function_Line(vector<Point> a2) // n la bậc
{
	vector<Point> a1;
	for(int i=0; i< a2.size(); i++){
		Point tmp = Point(a2[i].y, a2[i].x);
		a1.push_back(tmp);
	}
    int n=2;
	int i, j,k;
	int N = a1.size();
	float* X = new float[2 * n + 1];
	for ( i = 0; i < 2 * n + 1; i++)
	{
		X[i] = 0;
		for ( j = 0; j < N; j++)
			X[i] = X[i] + pow(a1[j].x, i);        //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	}
	float** B = new float*[n + 1];
	for (int i = 0; i < n + 1; i++)
		B[i] = new float[n + 2];
	float* a = new float[n + 1];            //B is the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
	for (i = 0; i <= n; i++)
		for (j = 0; j <= n; j++)
			B[i][j] = X[i + j];            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
	float* Y = new float[n + 1];                    //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	for (i = 0; i < n + 1; i++)
	{
		Y[i] = 0;
		for (j = 0; j < N; j++)
			Y[i] = Y[i] + pow(a1[j].x, i)*a1[j].y;        //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	}
	for (i = 0; i <= n; i++)
		B[i][n + 1] = Y[i];                //load the values of Y as the last column of B(Normal Matrix but augmented)
	n = n + 1;                //n is made n+1 because the Gaussian Elimination part below was for n equations, but here n is the degree of polynomial and for n degree we get n+1 equations

	for (i = 0; i < n; i++)                    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
		for (k = i + 1; k < n; k++)
			if (B[i][i] < B[k][i])
				for (j = 0; j <= n; j++)
				{
					float temp = B[i][j];
					B[i][j] = B[k][j];
					B[k][j] = temp;
				}

	for (i = 0; i < n - 1; i++)            //loop to perform the gauss elimination
		for (k = i + 1; k < n; k++)
		{
			float t = B[k][i] / B[i][i];
			for (j = 0; j <= n; j++)
				B[k][j] = B[k][j] - t * B[i][j];    //make the elements below the pivot elements equal to zero or elimnate the variables
		}
	for (i = n - 1; i >= 0; i--)                //back-substitution
	{                        //x is an array whose values correspond to the values of x,y,z..
		a[i] = B[i][n];                //make the variable to be calculated equal to the rhs of the last equation
		for (j = 0; j < n; j++)
			if (j != i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
				a[i] = a[i] - B[i][j] * a[j];
		a[i] = a[i] / B[i][i];            //now finally divide the rhs by the coefficient of the variable to be calculated
	}

	float *result = new float[3];
	result[0]= a[2];
	result[2]= a[0];
	result[1]= a[1];
	return result;
}
//oke
bool check_Point_in_Mat(const Mat&src, Point pointInput){
	if(pointInput.x<src.cols-W_TRACKING-1 && pointInput.x>=W_TRACKING){
		if(pointInput.y<src.rows-H_TRACKING && pointInput.y>=H_TRACKING)
			return true;
	}
	return false;
}

void myDraw(Mat src, vector<Point> ListPoint, const int color){
	int arrColor[3] = {0,0,0};
	int color2 = color;
	while(color2!=0){
		arrColor[color2%10]=255;
		color2=color2/10;
	}
	for(int i=0; i<ListPoint.size();i++){
        circle(src, ListPoint[i], 1, Scalar(arrColor[0], arrColor[1], arrColor[2]), 2, 8, 0);
    }
}

// void myDraw(Mat src, LINE lineDraw, const int color){
// 	int arrColor[3] = {0,0,0};
// 	int color2 = color;
// 	while(color2!=0){
// 		arrColor[color2%10]=255;
// 		color2=color2/10;
// 	}
// 	for(int y=0; y<src.rows; y++){
//         int x = lineDraw.get_X_by_Y(y);
//         circle(imgBRG, Point(x,y), 1, Scalar(arrColor[0], arrColor[1], arrColor[2]), 2, 8, 0);
//     }
// }

void myDraw(Mat src, Rect ROI, const int color){
	int arrColor[3] = {0,0,0};
	int color2 = color;
	while(color2!=0){
		arrColor[color2%10]=255;
		color2=color2/10;
	}
 	rectangle(src,ROI, Scalar(arrColor[0], arrColor[1], arrColor[2]), 1);
}

void myDraw(Mat src, Point point, const int color){
	int arrColor[3] = {0,0,0};
	int color2 = color;
	while(color2!=0){
		arrColor[color2%10]=255;
		color2=color2/10;
	}
	circle(src, point, 1, Scalar(arrColor[0], arrColor[1], arrColor[2]), 2, 8, 0);
}
int fixPixelValue(int value)
{
    if (value < 0)
        return 0;
    else if (value > 255)
        return 255;
    return value;
}
 
float get_Angle(Point topPoint, Point botPoint){
	if(topPoint.y > botPoint.y){
		Point tmp = topPoint;
		topPoint = botPoint;
		botPoint = tmp;
	}
	float pi = acos(-1.0);
    float dx = topPoint.x - botPoint.x;
    float dy = abs(botPoint.y - topPoint.y); 

	if (dx==0)
        return 0;
    else if (dy==0) 
        return 180;
    else if (dx < 0) 
         return -atan(-dx / dy) * 180 / pi; 
    else 
	return atan(dx / dy) * 180 / pi;
}

Point giai_hpt_2_an(float a1, float b1, float c1, float a2, float b2, float c2)
{
	return Point(abs((c1*b2-b1*c2)/(a1*b2-a2*b1)),abs((a1*c2-a2*c1)/(a1*b2 - a2 * b1)));
}

Point solve(float *Function, Point A, float d)
{
	float a = Function[0], b=Function[1], c=Function[2];
	float xB1, yB1, xB2, yB2;
	Point result;
	/*d la duong tiep tuyen cua duong cong c y=f'(x0)(x-x0)+y0 (=) y=f'(x0)x -f'(x0)*x0+y0
		a(d1) =2ax +b
		b(d1) =f'(x0)*x0+y0
	*/
	float a_d1 = 2 * a*A.x + b,                //f'(x0)
		  b_d1 = -(2 * a*A.x + b)*A.x + A.y;
	// d1  a_d1 *x - y + b_d1 = 0
	// d2 vuong goc d1   x + a_d2*y + b_d2 =0
	float a_d2 = a_d1,
	   	b_d2 = -(A.x + a_d2 * A.y);     
	/* ta giai he
		 1: pt d2         x + a_d2*y + b_d2 =0
		 2: pt khoang cach |a_d1 *x - y + b_d1| = d* sqrt(a_d1*a_d1+1) 
		 ( 2 TH)    
		 */
	return giai_hpt_2_an(1.0, a_d2, -b_d2, a_d1, -1.0, d* sqrt(a_d1*a_d1 + 1) - b_d1);
}