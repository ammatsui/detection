#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/objdetect.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <chrono>

/* notes:
maybe use stringstreams instead of `pretty`;
add filesystems;
*/

using namespace cv;
using namespace std;

const int port = 0;
const int minArea = 5000;
const int maxArea = 50000;
const string path = "path\\to\\the\\folder";

const int yr = 1900;

Mat frame, firstFrame, gray, frameDelta, thresh;
vector<vector<Point> > cnts;
Mat fmask;


string pretty(int val)
{
    string str = to_string(val);
    return std::string(2 - std::min(2, int(str.length())), '0') + str;
}

bool motCond(int cntrArea)
{
    return (cntrArea < minArea or cntrArea > maxArea);
}

void save(Mat& frame) 
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    auto year = yr + ltm->tm_year;
    auto month =  1 + ltm->tm_mon;
    auto day = ltm->tm_mday;
    
    auto hour = ltm->tm_hour;
    auto mint = ltm->tm_min;
    auto sec  = ltm->tm_sec;
     
    string fpath = pretty(year) + "\\" + pretty(month) + "\\" + pretty(day) + "\\";
    string name = "IMG_" + pretty(hour) + pretty(mint) + pretty(sec) + ".png";  
              
    imwrite(path + name, frame);
}

vector<vector<Point> > compare(Mat& firstFrame, Mat& frame)
{
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    //equalizeHist(gray, gray);
    GaussianBlur(gray, gray, Size(21, 21), 0);

    /* difference between the first frame and the current frame */
    absdiff(firstFrame, gray, frameDelta);
    threshold(frameDelta, thresh, 30, 255, THRESH_BINARY);
        
    /* dilate the threshold frame, find pixel contours */ 
    dilate(thresh, thresh, Mat(), Point(-1,-1), 1);
    findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    return cnts;

}

vector<vector<Point> > backgroundSubtrCompare(Mat& frame, Ptr<BackgroundSubtractor> pBackSub)
{
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    //equalizeHist(gray, gray);
    GaussianBlur(gray, gray, Size(21, 21), 0);

    pBackSub->apply(gray, fmask);
        
    //imshow("FG Mask", fmask);

    findContours(fmask, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    return cnts;
}

void detBackSub(VideoCapture& camera)
{
    sleep(3);
    camera.read(frame);

    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorMOG2();
     
    while(camera.read(frame)) 
    {
        cnts = backgroundSubtrCompare(frame, pBackSub);  

        /* check */
        for (int i = 0; i < cnts.size(); i++) {
            
            if (motCond(contourArea(cnts[i]))) {
                continue;
            }

            save(frame);

            putText(frame, "Intruder Alert", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255), 2);
            auto rect = boundingRect(cnts[i]);
            rectangle(frame, rect, (0, 0, 255), 2);

        }
    
        cv::imshow("Camera", frame);
        
        if(waitKey(1) == 27){
            break;
        }
    
    }
}

void det(VideoCapture& camera)
{
    
    sleep(3);
    camera.read(frame);

    /* capture the first frame and convert to greyscale*/
    cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
    GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);
    

    auto start = chrono::steady_clock::now();
     
     
    /* new frames */
    while(camera.read(frame)) 
    {
        cnts = compare(firstFrame, frame);     

        /* check */
        for (int i = 0; i < cnts.size(); i++) {
            
            if (motCond(contourArea(cnts[i]))) {
                /* not detected */
                auto end = chrono::steady_clock::now();
                auto dur = chrono::duration_cast<chrono::minutes>(end - start).count();
                if (dur >= 10) {
                    /* new firstFrame */
                    camera.read(frame);    
                    cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
                    GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);
                    /* new start time */
                    start = chrono::steady_clock::now();
                }
                continue;
            }

            save(frame);

            putText(frame, "Intruder Alert", Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255), 2);
            auto rect = boundingRect(cnts[i]);
            rectangle(frame, rect, (0, 0, 255), 2);

        }
    
        cv::imshow("Camera", frame);
        
        if(waitKey(1) == 27){
            break;
        }
    
    }
}


int main()
{
    /* open camera */
    auto camera = VideoCapture(port);
    int method;
    cout << "0 - Basic Motion Detection\n1 - Motion Detection Using Background Substraction\n:";
    cin >> method;
    camera.set(3, 512);
    camera.set(4, 288);
    switch (method)
    {
        case 0:
        det(camera);
        break;
        case 1:
        detBackSub(camera);
        break;
        default:
        cout << "Try again.";
    }
    
    return 0;
}
