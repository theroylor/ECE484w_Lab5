#include <unistd.h>	
#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <opencv2/opencv.hpp>
#include <cstdint> // For fixed-width integer types like uint32_t
#include <cstdlib> // For std::memcpy
#include <unordered_map>

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <sys/time.h>
#include "D8MCapture.h"
#include "hps_0.h"

#define CAPTURE_RAM_DEVICE "/dev/f2h-dma-memory"


void vidDisplay(cv::Mat);
cv::Mat OverlayImage(cv::Mat,cv::Mat image, double alpha=.5);
cv::Mat EqualizeHistogram(cv::Mat image,uint32_t,uint32_t);


cv::Mat imageBase, imageOverlay,imageHistogram,vidSrc;
int32_t brightness=0,contrast=99;
uint32_t command=0;
double pixel_scale;
double alpha =.5;
std::string windowTitle = "Lab5 Output";

struct Packet {
    uint32_t sequenceNumber;
    uint32_t totalChunks;
//    uint32_t dataSize; // accessed before storage
    std::vector<char> data; // Data payload
};
std::unordered_map<uint32_t, std::map<uint32_t, Packet>> messages;

/**
 *
 *	storeBufferAsPacket
 */
int main( void ) {
	cv::D8MCapture capture(TV_DECODER_TERASIC_STREAM_CAPTURE_BASE, CAPTURE_RAM_DEVICE);
    cv::Mat frame;
	if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }



	while (capture.read(frame))
	{
		if( frame.empty() )
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
		
		cv::Mat output = EqualizeHistogram(frame,0,10);

		//-- Show what you got frame.
		vidDisplay( output );
	
	
		//-- bail out if escape was pressed
		int c = cv::waitKey(10);
		if ((char) c == 27) 
		{
			std::cerr << "In the c==27 break" << std::endl;
			break;
		}
	}
	
	return 0;
}


void vidDisplay(cv::Mat frame)
{
	char str[100];
	static struct timeval last_time;
	struct timeval current_time;
	static float last_fps;
	float t;
	float fps;	
		
	gettimeofday(&current_time, NULL);
	t = (current_time.tv_sec - last_time.tv_sec) + (current_time.tv_usec - last_time.tv_usec) / 1000000.;
	fps = 1. / t;
	fps = last_fps * 0.8 + fps * 0.2;
	last_fps = fps;
	last_time = current_time;
	sprintf(str, "%2.2f", fps);
	//cout << str << endl;
	cv::putText(frame, str, cv::Point(20, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255));
		
    //-- Show what you got
    imshow( windowTitle, frame );
}



























void displayImage(cv::Mat image){
	int timeMult;
	if (cv::getWindowProperty(windowTitle, cv::WND_PROP_AUTOSIZE) == -1){
		timeMult = 20;
	}
	else {timeMult = 1;}
	cv::imshow(windowTitle,image);
	cv::waitKey(1*timeMult);
}

/**
 *	------------------------------------
 *	accepts cv::Mat image_base, cv::Matimage_overlay, and double alpha
 *	performs overlay, resulting image is returned 
 */	
cv::Mat OverlayImage(cv::Mat image_base,cv::Mat image_overlay, double alpha)
{
   if (image_base.empty() || imageOverlay.empty()) {
		cv::destroyWindow(windowTitle);
        std::cerr << "Error: Could not load one of the images!" << std::endl;
        return cv::Mat();
    }
	cv::Mat output;
	// do not modify output and overlay
	output = image_base.clone();
	cv::Mat overlay = imageOverlay.clone();

    // crop overlay if larger than base
    int rows = std::min(output.rows, overlay.rows);
    int cols = std::min(output.cols, overlay.cols);
    cv::Mat croppedOverlay = overlay(cv::Rect(0, 0, cols, rows));

    // Define ROI on the base image
    cv::Mat roi = output(cv::Rect(0, 0, cols, rows));
    
	// Apply the overlay to the ROI
	cv::addWeighted(croppedOverlay, alpha, roi, 1.0 - alpha, 0.0, roi);

	return output;
}
/**
 *	------------------------------------
 *	Accepts cv::Matimage, uint32_t brightness, and uint32_t contrast to
 *	performs histogram equalization
 *	contrast is the equalization range
 *	brightness is added to pixel values
 *	resulting image is returned
 *	------------------------------------
 */
cv::Mat EqualizeHistogram(cv::Mat image, uint32_t bright, uint32_t contr)
{
    if (image.empty()) {
		cv::destroyWindow(windowTitle);
        std::cerr << "Error: Could not load image!" << std::endl;
        return cv::Mat();
    }
	std::cout << "imageHistogram.channels() = " << imageHistogram.channels() << std::endl; 
	cv::Mat output;
	std::cout << "cv::Mat output" << std::endl;
	/* this should never be invoked
	if(imageHistogram.channels() == 3){
		cv::cvtColor(imageHistogram, output, cv::COLOR_GRAY2BGR);
	}
	else{
		output = imageHistogram.clone();
	} */
	output = image.clone();
	
	int rows = output.rows;
	int cols = output.cols;

    //innitialize histogram for new image
	std::vector<int> histogram(256, 0);
    pixel_scale = 1.0 /(rows * cols);

    for(int x = 0; x < cols; x++)
    {
        for(int y = 0; y < rows; y++)
        {
            // vector stores count of pixel with index value
            int pixelValue = output.at<uchar>(y, x);
            histogram[pixelValue]++;
        }
    }

    int sum = 0;
    for(int i = 0; i < 256; i++)
    {
        // resolve histogram equalization
        sum += histogram[i];
		histogram[i]=bright + sum * contr * pixel_scale;
    }

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
            // pixel value at a coordinate is an index
            // value at index is the replacement value for that pixel
			int pixelValue = output.at<uchar>(y, x);
			pixelValue = histogram[pixelValue];
			output.at<uchar>(y, x) = std::min(255, std::max(0, pixelValue));
		}
	}

    // return file
	return output;
}
