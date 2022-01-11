#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/objdetect.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <ctime>
//#include <boost/filesystem.hpp> 




using namespace cv;
using namespace std;

const int port = 0;
const int cntrArea = 10000;
const string path = "path\\to\\the\\folder";

const int yr = 1900;

int main(int argc, char **argv) 
{
    
    Mat frame, gray, frameDelta, thresh, firstFrame;
    vector<vector<Point> > cnts;
    /* open camera */
    auto camera = VideoCapture(port);
    
    camera.set(3, 512);
    camera.set(4, 288);

    //sleep(20);
    sleep(3);
    camera.read(frame);
    
    /* capture the first frame and convert to greyscale*/
    
    cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
    GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);
    
    /* new frames */
    while(camera.read(frame)) {
        /* greyscale */
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        //equalizeHist(gray, gray);
        GaussianBlur(gray, gray, Size(21, 21), 0);

        /* difference between the first frame and the current frame */
        absdiff(firstFrame, gray, frameDelta);
        threshold(frameDelta, thresh, 30, 255, THRESH_BINARY);
        
        /* dilate the threshold frame, find pixel contours */ 
        dilate(thresh, thresh, Mat(), Point(-1,-1), 1);
        findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        
        /* check if object detected */
        for (int i = 0; i < cnts.size(); i++) {
            
            if(contourArea(cnts[i]) < cntrArea/2 or contourArea(cnts[i]) > 5*cntrArea) {
                /* not detected */
                continue;
            }

            // auto rect = boundingRect(cnts[i]);
            // rectangle(frame, rect, (0, 0, 255), 2);

            // putText(frame, "Intruder Alert", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255), 2);
            /* save frame as image */
            //++ count;
            time_t now = time(0);
            tm *ltm = localtime(&now);
            auto year = yr + ltm->tm_year;
            auto month =  1 + ltm->tm_mon;
            auto day = ltm->tm_mday;
            
            auto hour = ltm->tm_hour;
            auto mint = ltm->tm_min;
            auto sec  = ltm->tm_sec;
            string fpath = std::to_string(year) + "\\" + std::to_string(month) + "\\" + std::to_string(day) + "\\";
            string name = "IMG_" + std::to_string(hour) + std::to_string(mint) + std::to_string(sec) + ".png";    
            /* create directory if doesn't exist */ 
            namespace fs = std::filesystem;

            try {
                fs.create_directories(path+fpath);
            }
            catch (std::exception& e) { 
                std::cout << e.what() << std::endl;
            }
            


            imwrite(path + fpath + name, frame);
            //sleep(3);
            putText(frame, "Intruder Alert", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255), 2);
            auto rect = boundingRect(cnts[i]);
            rectangle(frame, rect, (0, 0, 255), 2);

        }
    
        cv::imshow("Camera", frame);
        
        if(waitKey(1) == 27){
            break;
        }
    
    }

    return 0;
}
