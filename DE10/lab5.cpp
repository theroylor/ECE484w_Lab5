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

//void PipeOut(cv::Mat,uint32_t,uint32_t,double);
void vidDisplay(cv::Mat);
cv::Mat OverlayImage(cv::Mat,cv::Mat image, double alpha=.5);
cv::Mat EqualizeHistogram(cv::Mat image,uint32_t,uint32_t);
std::vector<char> getCompleteMessage(uint32_t);
cv::Mat constructGrayscaleImage(std::vector<char>);
int32_t constructInt32_t(std::vector<char>);
void updateOutput();
void setMode(uint32_t);
void setBrightness(uint32_t);
void setContrast(uint32_t);
void setImageOverlay(cv::Mat);
bool isMessageComplete(uint32_t);
void sortMessages(uint32_t);
void storeBufferAsPacket(const char* , size_t arraySize=1024);
//void writeToPipe(const std::string&, const std::vector<char>&);

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


    // Set up the UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5005); // Listening port
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Binding failed!" << std::endl;
		close(sock);
        return -1;
    }

    std::cout << "Listening for messages..." << std::endl;

    char buffer[1024]; // Max UDP packet size
    sockaddr_in client_address;
    socklen_t client_length = sizeof(client_address);


	while (true) {
		int bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_length);

		if (bytes_received > 0) {
			uint32_t messageId;
			//buffer[bytes_received] = '\0'; // Null-terminate the received data if needed
			std::memcpy(&messageId, &buffer[0], sizeof(uint32_t));		
			storeBufferAsPacket(buffer);
			std::cerr << "In Bytes received >0" << std::endl;
		}
		if (capture.read(frame))
		{
			if( frame.empty() )
			{
				printf(" --(!) No captured frame -- Break!");
				break;
			}
			
			switch (command){
				case 0x30001:
					frame = OverlayImage(frame,imageOverlay,0);
				break;
				case 0x30002:
					frame = EqualizeHistogram(frame,brightness,contrast);
				break;
				case 0x30003:
					frame = OverlayImage(frame,imageOverlay);
				break;
				case 0x30004:
					frame = EqualizeHistogram(frame,brightness,contrast);
				break;
			}
		}
		
	   
		
		//-- Show what you got
		vidDisplay( frame );
		
		
		//-- bail out if escape was pressed
		int c = cv::waitKey(10);
		if ((char) c == 27) {
			std::cerr << "In the c==27 break" << std::endl;
			break;
		}
	}
	
    close(sock);
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




std::vector<char> getCompleteMessage(uint32_t messageID) {
    std::vector<char> completeData;

    // Get the inner map (sequenceNumber -> Packet) for the message
    auto& packets = messages[messageID];

    // Concatenate data in order of sequenceNumber
	for (const auto& pair : packets) {
	//	const auto& seqNum = pair.first;
		const auto& packet = pair.second;
		completeData.insert(completeData.end(), packet.data.begin(), packet.data.end());
	}
	

    // Free memory by erasing the entry
    messages.erase(messageID);

    return completeData;
}
cv::Mat constructGrayscaleImage(std::vector<char> data)
{
	// Replace IMREAD_COLOR with IMREAD_GRAYSCALE if needed
    cv::Mat image = cv::imdecode(data, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
         throw std::runtime_error("Error: Could not decode image from char array!");
    }
	return image;
}
int32_t constructInt32_t(std::vector<char> data)
{
	if (data.size() != sizeof(int32_t)) {
        throw std::runtime_error("Incorrect data size for int32_t.Data size: " + std::to_string(data.size()) + ". Expected: 4");
    }
	int32_t value;
    std::memcpy(&value, data.data(), sizeof(int32_t));
	std::cout << "Extracted int32_t value: " << value << std::endl;
	return value;
}

/**
*	------------------------------------
 *	sets the program mode
 *	0x1001 is imageBase display
 *	0x1002 is modeOverlay
 *	0x2001 is modeBrightnessContrast
 *	------------------------------------
 */
 bool isSameCommandMSB(uint32_t num1, uint32_t num2) {
    // Extract the most significant byte (MSB) of each number
    uint16_t msb1 = num1 >> 16; // Right-shift to isolate MSB
    uint16_t msb2 = num2 >> 16;
	std::cout << std::hex << "Are the MSBs of 0x" << num1 << " and 0x" << num2 << " equal?" << std::dec << std::endl; 
	std::cout << std::hex << "msb1 = 0x" << msb1 << ", msb2 = 0X" << msb2 << std::dec << std::endl;
    // Compare the msbs
	bool output = msb1 == msb2;
	std::cout << "msb1 == msb2: " << output << std::endl;
    return output;
}
void updateOutput()
{
	switch(command){
		case 0x10001:
		std::cout << "In command case 0x1001: No Overlay" << std::endl;
		displayImage(OverlayImage(imageBase,imageOverlay,0));
		break;
		case 0x10002:
		std::cout << "In command case 0x1002: Overlay" << std::endl;
		displayImage(OverlayImage(imageBase,imageOverlay,0));
		break;
		case 0x20001:
		std::cout << "In command case 0x2001: Histogram Equalization" << std::endl;
		displayImage(EqualizeHistogram(imageHistogram,brightness,contrast));
		break;
		/*
		case 0x30001:
		std::cout << "In command case 0x2001: Video Histogram Equalization" << std::endl;
		//displayImage(EqualizeHistogram(imageHistogram,brightness,contrast,0));
		//Alpha = 0;
		break;
		case 0x30002:
		std::cout << "In command case 0x2001: Video Histogram Equalization with Overlay" << std::endl;
		//displayImage(EqualizeHistogram(imageHistogram,brightness,contrast));
		break;
*/		
		default:
		break;
	}
}
void setMode(uint32_t data){
	if(!isSameCommandMSB(data,command)){
		cv::destroyWindow(windowTitle);
		cv::waitKey(100);
	}
	command = data;
	std::cout << "Set mode = 0x" << std::hex << data << std::dec << std::endl;
	updateOutput();
}
void setBrightness(uint32_t data){
	brightness = data;
	std::cout << "Set brightness = " << (int)data << std::endl;
	updateOutput();
}
void setContrast(uint32_t data){
	contrast = data;
	std::cout << "Set contrast = " << (int)data << std::endl;
	updateOutput();
}
void setImageBase(cv::Mat image){
	//cv::destroyAllWindows();
	imageBase = image;
	std::cout << "Base image rows, cols = " << image.rows << ", " << image.cols << std::endl;
	updateOutput();
}
void setImageOverlay(cv::Mat image){
	//cv::destroyAllWindows();
	imageOverlay = image;
	std::cout << "Overlay image rows, cols = " << image.rows << ", " << image.cols << std::endl;
	updateOutput();
}
void setImageHistogram(cv::Mat image){
	//cv::destroyAllWindows();
	imageHistogram = image;
	std::cout << "Histogram image rows, cols = " << image.rows << ", " << image.cols << std::endl;
	updateOutput();
}
bool isMessageComplete(uint32_t messageID){
	// Get the inner map for the message
    auto& packets = messages[messageID];
	std::cout << "MessageID: " << messageID << std::endl;
	std::cout << "packet.size() = " << packets.size() << ".  packets.begin()->second.totalChunks = " <<  packets.begin()->second.totalChunks << "." << std::endl;
	// Check that packets.size() == totalChunks;
	if (!packets.empty()) {
        return packets.size() == packets.begin()->second.totalChunks;
    }
	return false;
}

/*
void writeToPipe(const std::string& pipeName, const std::vector<char>& data){
	std::string pipePath <= "/tmp/"+pipeName;
	std::ofstream pipe(pipePath, std::ios::binary);
    if (!pipe) {
        std::cerr << "Error: Could not open pipe " << pipeName << " for writing" << std::endl;
        return;
    }
	pipe.write(data.data(), data.size());
    if (!pipe.good()) {
        std::cerr << "Error: Failed to write " << pipeName << " to pipe " << pipePath << std::endl;
    } else {
        std::cout << pipeName << " successfully written to pipe: " << pipePath << std::endl;
    }
	pipe.close();
}
*/

void sortMessages(uint32_t messageID){
	if(isMessageComplete(messageID))
	{
		switch(messageID){
			case 0x11:
			setImageBase(constructGrayscaleImage(getCompleteMessage(messageID)));
			break;
			case 0x12:
			//std::vector<char> data = getCompleteMessage(messageID);
			setImageOverlay(constructGrayscaleImage(getCompleteMessage(messageID)));
			//writeToPipe("Overlay",data);
			break;
			case 0x13:
			setImageHistogram(constructGrayscaleImage(getCompleteMessage(messageID)));
			break;
			case 0x20:
			//std::vector<char> data = getCompleteMessage(messageID)
			setMode(constructInt32_t(getCompleteMessage(messageID)));
			//writeToPipe("Mode",data);
			break;
			case 0x2B:
			//std::vector<char> data = getCompleteMessage(messageID)
			setBrightness(constructInt32_t(getCompleteMessage(messageID)));
			//writeToPipe("Brightness",data);
			break;
			case 0x2C:
			//std::vector<char> data = getCompleteMessage(messageID)
			setContrast(constructInt32_t(getCompleteMessage(messageID)));
			//writeToPipe("Contrast",data);
			break;
			default:
			std::cerr << "Unknown Message ID: " << std::hex << messageID << std::dec << std::endl;				
			break;
		}
	}
}

/**
 *
 *	
 */
void storeBufferAsPacket(const char* charArray, size_t arraySize) {
	
    // Ensure the array has enough bytes for metadata (16 bytes: messageID, sequenceNumber, totalChunks, dataSize)
    if (arraySize < sizeof(uint32_t) * 4) {
        throw std::runtime_error("Character array is too small to extract packet metadata");
    }
	
    uint32_t messageID, sequenceNumber, totalChunks, dataSize;
    std::memcpy(&messageID, charArray, sizeof(uint32_t)); // Extract messageID
    std::memcpy(&sequenceNumber, charArray + sizeof(uint32_t), sizeof(uint32_t)); // Extract sequenceNumber
    std::memcpy(&totalChunks, charArray + 2 * sizeof(uint32_t), sizeof(uint32_t)); // Extract totalChunks
    std::memcpy(&dataSize, charArray + 3 * sizeof(uint32_t), sizeof(uint32_t)); // Extract dataSize

    if (dataSize > arraySize - sizeof(uint32_t) * 4) {
        throw std::runtime_error("Character array is too small for the indicated data payload size");
    }

    //Extract data payload
    const char* dataStart = charArray + sizeof(uint32_t) * 4; // Payload starts after metadata
    std::vector<char> data(dataStart, dataStart + dataSize); // Copy the data payload

    //Create the Packet
    Packet packet = {sequenceNumber, totalChunks, data};
    messages[messageID][sequenceNumber] = packet;
	std::cout << "Recieved ID: " << messageID << "; Sequence " << sequenceNumber << " of " << totalChunks << ". " << dataSize << " bytes." << std::endl;

	sortMessages(messageID);
}


