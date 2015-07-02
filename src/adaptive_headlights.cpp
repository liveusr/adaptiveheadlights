#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"

#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include "opencv2/gpu/gpu.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#include "tegra_gpio.h"

using namespace std;
using namespace cv;
using namespace cv::gpu;

#define ENABLE_GPU 0

enum {
    BEAM_LOW,
    BEAM_HIGH,
    BEAM_LEFT,
    BEAM_RIGHT
};

#define LINE_OFFSET 100
#define H_THRESHOLD 110
#define V_THRESHOLD 5

RNG rng(12345);

#define HANDLE_KEYPRESS()                                   \
{                                                           \
    int _key = waitKey(30);                                 \
    if (_key == 'p' || _key == 32) {                        \
        cout << "paused... press r to continue" << endl;    \
        while (1) {                                         \
            _key = waitKey(30);                             \
            if (_key == 's') {                              \
                imwrite("frame.jpg", frame_orig);           \
                cout << "frame saved to frame.jpg" << endl; \
            }                                               \
            if (_key == 'r' || _key == 13 || _key == 27) {  \
                break;                                      \
            }                                               \
        }                                                   \
    }                                                       \
    else if (_key == 27) {                                  \
        sighandler(0);                                      \
        cout << "exiting program..." << endl;               \
        break;                                              \
    }                                                       \
}

void filter_tail_light(const Mat &frame_orig, Mat &frame_filter)
{
    Mat frame_hsv;

#if ENABLE_GPU
    GpuMat gpu_orig;
    GpuMat gpu_hsv;

    gpu_orig.upload(frame_orig);
    gpu::cvtColor(gpu_orig, gpu_hsv, CV_BGR2HSV);
    gpu_hsv.download(frame_hsv);
#else
    cvtColor(frame_orig, frame_hsv, CV_BGR2HSV );
    //imshow("hsv frame", frame_hsv);
#endif

    inRange(frame_hsv, Scalar(170, 100, 100), Scalar(180, 255, 255), frame_filter);

    //imshow("tail light filter", frame_filter);
}

void filter_head_light(const Mat &frame_orig, Mat &frame_filter)
{
    Mat frame_gray;

#if ENABLE_GPU
    GpuMat gpu_orig;
    GpuMat gpu_gray;
    GpuMat gpu_filter;

    gpu_orig.upload(frame_orig);
    gpu::cvtColor(gpu_orig, gpu_gray, CV_BGR2GRAY);
    gpu::threshold(gpu_gray, gpu_filter, 240, 255, CV_THRESH_BINARY);
    gpu_filter.download(frame_filter);
#else
    cvtColor(frame_orig, frame_gray, CV_BGR2GRAY );
    //imshow("gray frame", frame_gray);
    threshold(frame_gray, frame_filter, 240, 255, CV_THRESH_BINARY);
#endif

    //imshow("head light filter", frame_filter);
}

void merge_filter(const Mat &frame1, const Mat &frame2, Mat &frame_filter)
{
#if ENABLE_GPU
    GpuMat gpu_frame1;
    GpuMat gpu_frame2;
    GpuMat gpu_filter;

    gpu_frame1.upload(frame1);
    gpu_frame2.upload(frame2);
    gpu::bitwise_or(gpu_frame1, gpu_frame2, gpu_filter);
    gpu_filter.download(frame_filter);
#else
    bitwise_or(frame1, frame2, frame_filter);
#endif

    //imshow("merged filter", frame_filter);
}

void smoothen_filter(const Mat &frame_orig, Mat &frame_filter)
{
    Mat frame_temp;
    Mat frame_blur;
    Mat frame_thresh;
    Mat element;

#if ENABLE_GPU
    GpuMat gpu_orig;
    GpuMat gpu_temp;
    GpuMat gpu_blur;
    GpuMat gpu_thresh;
    GpuMat gpu_filter;

    gpu_orig.upload(frame_orig);

    gpu::blur(gpu_orig, gpu_temp, Size(3,3), Point(-1,-1));
    gpu::GaussianBlur(gpu_temp, gpu_blur, Size(3,3),0,0,BORDER_CONSTANT);

    gpu::threshold(gpu_blur, gpu_thresh, 64, 255, CV_THRESH_BINARY);

    element = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1) );
    gpu::morphologyEx(gpu_thresh, gpu_filter, CV_MOP_OPEN, element, Point(-1, -1), 2);

    gpu_filter.download(frame_filter);
#else
    blur( frame_orig, frame_temp, Size(3,3), Point(-1, -1) );
    GaussianBlur(frame_temp, frame_blur, Size(3,3),0,0,BORDER_CONSTANT);
    //imshow("blurred filter", frame_blur);

    threshold(frame_blur, frame_thresh, 64, 255, CV_THRESH_BINARY);

    element = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1) );
    morphologyEx(frame_thresh, frame_filter, CV_MOP_OPEN, element, Point(-1, -1), 2);
#endif

    //imshow("smoothen filter", frame_filter);
}

void find_lights(const Mat &frame_orig, const Mat &frame_objects, vector<Point> &pairs)
{
    // find contours
    Mat frame_contour = frame_orig.clone();
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( frame_objects, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    // find moments and get the mass centers
    vector<Point> obj( contours.size() );
    int obj_count = 0;
    for( int i = 0; i < contours.size(); i++ ) { 
        char txt[80];
        Scalar color = Scalar( 255, 0, 0);
        Moments moment = moments( contours[i], false ); 
        // remove objects which are very small in area 
        if(contourArea(contours[i]) > 16) {
            obj[obj_count] = Point( moment.m10/moment.m00 , moment.m01/moment.m00 ); 
            //sprintf(txt, "a%.0lf", contourArea(contours[obj_count]));
            sprintf(txt, "y%02d", obj[obj_count].y%100);
            putText(frame_contour, txt, Point(obj[obj_count].x, obj[obj_count].y-20),  FONT_HERSHEY_SIMPLEX, 0.4, color, 1, 8, false);
            sprintf(txt, "x%02d", obj[obj_count].x%100);
            putText(frame_contour, txt, Point(obj[obj_count].x, obj[obj_count].y-35),  FONT_HERSHEY_SIMPLEX, 0.4, color, 1, 8, false);
            circle(frame_contour, obj[obj_count], 1, color, 2, 8, 0);
            obj_count++;
        }
    }
    //imshow( "contour", frame_contour);

    // make pairs
    vector<int> flags( obj_count );
    for(int i = 0; i < obj_count; i++ ) {
        flags[i] = 0;
    }
    for( int i = 0; i< obj_count; i++ ) {
        for( int j = 0; j< obj_count; j++ ) {
            if((i != j) && (flags[j] == 0) && (abs(obj[i].y-obj[j].y) <= V_THRESHOLD) && (abs(obj[i].x-obj[j].x) <= H_THRESHOLD)) {
                if(flags[i] == 0) {
                    pairs.push_back(obj[i]);
                    flags[i] = 1;
                }
                pairs.push_back(obj[j]);
                flags[j] = 1;
            }
        }
    }
}

void mark_lights(Mat &frame_output, vector<Point> &pairs)
{
    // sort on y
    for( int i = 0; i < pairs.size(); i++ ) { 
        int small = i;
        for( int j = i; j < pairs.size(); j++ ) { 
            if(pairs[j].y < pairs[small].y) {
                small = j;
            }
        }
        Point temp = pairs[i];
        pairs[i] = pairs[small];
        pairs[small] = temp;

        circle(frame_output, pairs[i], 3, Scalar( 255, 0, 0), 2, 8, 0);
        circle(frame_output, pairs[i], 1, Scalar( 0, 255, 0), 2, 8, 0);
    }

    int x1, x2, y1, y2;
    if(pairs.size()) {
        x1 = x2 = pairs[0].x;
        y1 = y2 = pairs[0].y;
    }
    else {
        cout << "no pair found... press r to continue" << endl;    
#if 0
        while (1) {                                         
            int _key = waitKey(30);                             
            if (_key == 'r' || _key == 13 || _key == 27) {  
                break;                                      
            }                                               
        }                                                   
#endif
    }
    for( int i = 1; i < pairs.size(); i++ ) { 
        if(abs(pairs[i-1].y-pairs[i].y) <= V_THRESHOLD && abs(x2-pairs[i].x) <= H_THRESHOLD) {
            // save end points
            if(x1 > pairs[i].x) {
                x1 = pairs[i].x;
            }
            if(x2 < pairs[i].x) {
                x2 = pairs[i].x;
            }
            if(y1 > pairs[i].y) {
                y1 = pairs[i].y;
            }
            if(y2 < pairs[i].y) {
                y2 = pairs[i].y;
            }
        }
        else {
            // draw rect and save new points
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            rectangle(frame_output, Point(x1-10, y1-10), Point(x2+10, y2+10), color, 2, 8, 0);
            x1 = x2 = pairs[i].x;
            y1 = y2 = pairs[i].y;
        }
    }
    if(pairs.size()) {
        // draw remaining rect which was not drawn in above for loop
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        rectangle(frame_output, Point(x1-10, y1-10), Point(x2+10, y2+10), color, 2, 8, 0);
    }
}

void draw_lights(Mat &frame_output, int x, int y, int pos)
{
    switch(pos) {
        case BEAM_LOW:
            circle(frame_output, Point(x, y),   9, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x, y+1), 7, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x, y+2), 5, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x, y+3), 3, Scalar(255, 255, 255), 2, 8, 0);
            circle(frame_output, Point(x, y+4), 1, Scalar(255, 255, 255), 2, 8, 0);
            break;

        case BEAM_HIGH:
            circle(frame_output, Point(x, y),   9, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x, y),   7, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x, y),   5, Scalar(255, 255, 255), 2, 8, 0);
            circle(frame_output, Point(x, y),   3, Scalar(255, 255, 255), 2, 8, 0);
            circle(frame_output, Point(x, y),   1, Scalar(255, 255, 255), 2, 8, 0);
            break;

        case BEAM_LEFT:
            circle(frame_output, Point(x, y),   9, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x-1, y), 7, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x-2, y), 5, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x-3, y), 3, Scalar(255, 255, 255), 2, 8, 0);
            circle(frame_output, Point(x-4, y), 1, Scalar(255, 255, 255), 2, 8, 0);
            break;

        case BEAM_RIGHT:
            circle(frame_output, Point(x, y),   9, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x+1, y), 7, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x+2, y), 5, Scalar(255, 255, 255), 1, 8, 0);
            circle(frame_output, Point(x+3, y), 3, Scalar(255, 255, 255), 2, 8, 0);
            circle(frame_output, Point(x+4, y), 1, Scalar(255, 255, 255), 2, 8, 0);
            break;

        default:
            break;
    }
}

void draw_rect(Mat &frame_output, int x, int y, int w, int h, double alpha = 0.1)
{
    Mat roi = frame_output(Rect(x, y, w, h));            
    Mat color(roi.size(), CV_8UC3, Scalar(62, 125, 125));                 
    addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);              
    if(alpha < 0.2) {
        char txt[] = "low beam";
        putText(frame_output, txt, Point(x + ((w>160)?52:25), y - 10),  FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);
    }
    else {
        char txt[] = "high beam";
        putText(frame_output, txt, Point(x + ((w>160)?52:25), y - 10),  FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1, 8, false);
    }
}

void normalize_zone_values(int *z1, int *z2, int *z3, int *z4)
{
    const int QLEN = 10;
    static int z1Queue[QLEN], z2Queue[QLEN], z3Queue[QLEN], z4Queue[QLEN];
    static int qTail;
    int z1Sorted[QLEN], z2Sorted[QLEN], z3Sorted[QLEN], z4Sorted[QLEN];
    int i, j, z1Big, z2Big, z3Big, z4Big, temp;

    /* save the current sensor value at the end of circular queue */
    z1Queue[qTail] = *z1;
    z2Queue[qTail] = *z2;
    z3Queue[qTail] = *z3;
    z4Queue[qTail] = *z4;

    /* instead of running 4 memcpy, copy in one for loop */
    for(i = 0; i < QLEN; i++) {
        z1Sorted[i] = z1Queue[i];
        z2Sorted[i] = z2Queue[i];
        z3Sorted[i] = z3Queue[i];
        z4Sorted[i] = z4Queue[i];
    }

    /* optimized bubble sort */
    for(i = 0; i < QLEN; i++) {
        for(j = z1Big = z2Big = z3Big = z4Big = 0; j < (QLEN - i); j++) {
            if(z1Sorted[j] > z1Sorted[z1Big]) {
                z1Big = j;
            }
            if(z2Sorted[j] > z2Sorted[z2Big]) {
                z2Big = j;
            }
            if(z3Sorted[j] > z3Sorted[z3Big]) {
                z3Big = j;
            }
            if(z4Sorted[j] > z4Sorted[z4Big]) {
                z4Big = j;
            }
        }

        temp = z1Sorted[z1Big];
        z1Sorted[z1Big] = z1Sorted[(QLEN - 1) - i];
        z1Sorted[(QLEN - 1) - i] = temp;

        temp = z2Sorted[z2Big];
        z2Sorted[z2Big] = z2Sorted[(QLEN - 1) - i];
        z2Sorted[(QLEN - 1) - i] = temp;

        temp = z3Sorted[z3Big];
        z3Sorted[z3Big] = z3Sorted[(QLEN - 1) - i];
        z3Sorted[(QLEN - 1) - i] = temp;

        temp = z4Sorted[z4Big];
        z4Sorted[z4Big] = z4Sorted[(QLEN - 1) - i];
        z4Sorted[(QLEN - 1) - i] = temp;
    }

    *z1 = z1Sorted[QLEN / 2];
    *z2 = z2Sorted[QLEN / 2];
    *z3 = z3Sorted[QLEN / 2];
    *z4 = z4Sorted[QLEN / 2];

    qTail++;
    if(qTail >= QLEN) {
        qTail = 0;
    }
}

void sighandler(int sig) 
{
    cout << "signal " << sig << " received" << endl;
    init_motor();
    sleep(1);
    exit_gpio();

    exit(0);
}

void gpu_process_frame(const Mat &frame_orig, Mat &frame_output)
{
    Mat frame_hsv;
    Mat frame1;

    GpuMat gpu_orig;
    GpuMat gpu_frame1;
    GpuMat gpu_frame2;
    GpuMat gpu_filter;
    GpuMat gpu_temp;

    gpu_orig.upload(frame_orig);
    gpu::cvtColor(gpu_orig, gpu_temp, CV_BGR2HSV);
    gpu_temp.download(frame_hsv);
    inRange(frame_hsv, Scalar(170, 100, 100), Scalar(180, 255, 255), frame1);
    gpu_frame1.upload(frame1);

    gpu::cvtColor(gpu_orig, gpu_temp, CV_BGR2GRAY);
    gpu::threshold(gpu_temp, gpu_frame2, 240, 255, CV_THRESH_BINARY);

    gpu::bitwise_or(gpu_frame1, gpu_frame2, gpu_filter);

    gpu::blur(gpu_filter, gpu_frame1, Size(3,3), Point(-1,-1));
    gpu::GaussianBlur(gpu_frame1, gpu_frame2, Size(3,3),0,0,BORDER_CONSTANT);
    gpu::threshold(gpu_frame2, gpu_frame1, 64, 255, CV_THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1) );
    gpu::morphologyEx(gpu_frame1, gpu_filter, CV_MOP_OPEN, element, Point(-1, -1), 2);

    gpu_filter.download(frame_output);
}

int main(int argc,const char** arg)
{
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);

    init_gpio();
    init_motor();
    sleep(1);

    //namedWindow( "video", CV_WINDOW_AUTOSIZE );

    while(1) {
        VideoCapture cap("test.mp4");

        const int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
        const int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

        const Point center_top (width/2,           LINE_OFFSET), center_bot (width/2,           height);                   
        const Point left_top   (width/2 - width/5, LINE_OFFSET), left_bot   (width/2 - width/5, height);   
        const Point right_top  (width/2 + width/5, LINE_OFFSET), right_bot  (width/2 + width/5, height); 

        while(1) {
            Mat frame_orig;
            if(!cap.read(frame_orig)) {
                cout<< " ERROR: frame not read "<<endl;
                break;
            }

#if ENABLE_GPU
        Mat frame_objects;
        gpu_process_frame(frame_orig, frame_objects);
#else
            Mat frame_filter1;
            filter_tail_light(frame_orig, frame_filter1);

            Mat frame_filter2;
            filter_head_light(frame_orig, frame_filter2);

            Mat frame_filter;
            merge_filter(frame_filter1, frame_filter2, frame_filter);

            Mat frame_objects;
            smoothen_filter(frame_filter, frame_objects);
#endif

            vector<Point> pairs;
            find_lights(frame_orig, frame_objects, pairs);

            Mat frame_output = frame_orig.clone();
            mark_lights(frame_output, pairs);

            // set zones
            int z1 = 0, z2 = 0, z3 = 0, z4 = 0;
            for( int i = 0; i < pairs.size(); i++ ) { 
                if(pairs[i].x >= 0 && pairs[i].x <= left_top.x) {
                    z1 = 1;
                }
                else if(pairs[i].x >= left_top.x && pairs[i].x <= center_top.x) {
                    z2 = 1;
                }
                else if(pairs[i].x >= center_top.x && pairs[i].x <= right_top.x) {
                    z3 = 1;
                }
                else if(pairs[i].x >= right_top.x && pairs[i].x <= width) {
                    z4 = 1;
                }
                else {
                    cout << "you should never see this line!" << endl;
                }
            }
            normalize_zone_values(&z1, &z2, &z3, &z4);

            line( frame_output, center_top, center_bot, Scalar(0, 255, 255), 2, 8 );   
            line( frame_output, left_top,   left_bot,   Scalar(0, 255, 255), 1, 8 );       
            line( frame_output, right_top,  right_bot,  Scalar(0, 255, 255), 1, 8 );     

            // set left side light position 0ooo
            if(z1) {
                if(z2) {
                    // left side light center low 0ooo
                    select_motor(6,2);
                    usleep(10000);
                    select_motor(5,1);
                    usleep(10000);
                    draw_lights(frame_output, 197, 18, BEAM_LOW); // left side
                }
                else {
                    // left side light center high 0ooo
                    select_motor(6,2);
                    usleep(10000);
                    select_motor(5,2);
                    usleep(10000);
                    draw_lights(frame_output, 197, 18, BEAM_HIGH); // left side
                }
            }
            else {
                // left side light left high 0ooo
                select_motor(6,1);
                usleep(10000);
                select_motor(5,2);
                usleep(10000);
                draw_lights(frame_output, 197, 18, BEAM_LEFT); // left side
                draw_rect(frame_output, 0, LINE_OFFSET, left_bot.x, height - LINE_OFFSET, 0.3);            
            }

            // set left center light position o0oo
            if(z2) {
                // left center light low o0oo
                select_motor(4,1);
                usleep(10000);
                draw_lights(frame_output, 220, 21, BEAM_LOW); // left mid
                draw_rect(frame_output, left_top.x, left_top.y, center_bot.x-left_top.x, height - LINE_OFFSET, 0.1);
            }
            else {
                // left center light high o0oo
                select_motor(4,2);
                usleep(10000);
                draw_lights(frame_output, 220, 21, BEAM_HIGH); // left mid
                draw_rect(frame_output, left_top.x, left_top.y, center_bot.x-left_top.x, height - LINE_OFFSET, 0.3);
            }

            // set right center light position oo0o
            if(z3) {
                // right center light low oo0o
                select_motor(3,1);
                usleep(10000);
                draw_lights(frame_output, 420, 21, BEAM_LOW); // right mid
                draw_rect(frame_output, center_top.x, center_top.y, right_bot.x-center_top.x, height - LINE_OFFSET, 0.1);
            }
            else {
                // right center light high oo0o
                select_motor(3,2);
                usleep(10000);
                draw_lights(frame_output, 420, 21, BEAM_HIGH); // right mid
                draw_rect(frame_output, center_top.x, center_top.y, right_bot.x-center_top.x, height - LINE_OFFSET, 0.3);
            }

            // set right side light position ooo0
            if(z4) {
                if(z3) {
                    // right side light center low ooo0
                    select_motor(2,1);
                    usleep(10000);
                    select_motor(1,2);
                    usleep(10000);
                    draw_lights(frame_output, 443, 18, BEAM_LOW); // right side
                }
                else {
                    // right side light center high ooo0
                    select_motor(1,2);
                    usleep(10000);
                    select_motor(2,2);
                    usleep(10000);
                    draw_lights(frame_output, 443, 18, BEAM_HIGH); // right side
                }
            }
            else {
                // right side light right high ooo0
                select_motor(1,3);
                usleep(10000);
                select_motor(2,2);
                usleep(10000);
                draw_lights(frame_output, 443, 18, BEAM_RIGHT); // right side
                draw_rect(frame_output, right_top.x, right_top.y, width-right_top.x, height - LINE_OFFSET, 0.3);
            }

            Mat frame_lights = imread("bmwte.png");
            Mat roi = frame_output(Rect(168, 0, frame_lights.size().width, frame_lights.size().height));
            bitwise_or(roi, frame_lights, roi);

            imshow( "adaptive headlights", frame_output);

            //imshow( "video", frame_orig );

            HANDLE_KEYPRESS();
        }
    }

    exit_gpio();
    return 0;
}

