#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <objdetect.hpp>

class PedestrianDetector
{
private:
	cv::Mat input, gray, roi, mask, output;

	cv::HOGDescriptor hog;

	std::vector<cv::Rect> found;
public:
	PedestrianDetector();
	~PedestrianDetector();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void searchForPedestrians();
	void showInfo();
};

