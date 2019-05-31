#include "IOInterface.h"
IOInterface::IOInterface(string ip) {
    if (ConnectSimulator(ip, 6231))
    {
        cout<<"Connected to simulator "<<clientSd<<endl;
        tr=thread(&IOInterface::getDataThreadFunc, this);
    }else{
        cout<<"Can't connect to simulator"<<endl;
    }
    usleep(1000000);
}
IOInterface::~IOInterface() {
    tr.join();
}

Mat IOInterface::getBGR(){
    return bgr;
}
Mat IOInterface::getDepth(){
    return depth;
}
Vec3f IOInterface::getIMUPose(){
    return imu_pose;
}
void IOInterface::setSteer(float val){
    steer=val;
}
void IOInterface::setSpeed(float val){
    speed=val;
}
void IOInterface::setCamAngle(float val){
    cam_angle=val;
}
void IOInterface::getDataThreadFunc(){
    do{
        string data = to_string(steer)+"|"+to_string(speed)+"|"+to_string(cam_angle);
        send(clientSd, data.c_str(), 100, 0);  
        int sizeRecv=recv(clientSd, (char*)&msg, 50000, 0);
        imu_pose[0]=bytesToFloat(msg,0);
        imu_pose[1]=bytesToFloat(msg,4);
        imu_pose[2]=bytesToFloat(msg,8);
        vector<char> arr1(msg+20, msg + 20 + (int)bytesToFloat(msg,12));
        resize(imdecode(arr1,CV_LOAD_IMAGE_UNCHANGED),bgr,Size(320,240));
        vector<char> arr2(msg+20+(int)bytesToFloat(msg,12), msg + 20 + (int)bytesToFloat(msg,12)+(int)bytesToFloat(msg,16));
        resize(imdecode(arr2,CV_LOAD_IMAGE_UNCHANGED),depth,Size(320,240));        
    }while(true);
}
bool IOInterface::ConnectSimulator(string serverIp, int port)
{
    struct hostent *host = gethostbyname(serverIp.c_str());
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr)) < 0)
        return false;
    return true;
}
int IOInterface::chooseCarMode(){
    return 0;
}
void IOInterface::ready(){
}

int IOInterface::getLCDkey(){
    return -1;
}
void IOInterface::printLCD(int x,int y,string msg){
    cout<<msg<<endl;
}
void IOInterface::clearLCD(){

}
float IOInterface::bytesToFloat(char msg[],int begin)
{
    float f;
    memcpy(&f, msg+begin, 4);
    return f;
}