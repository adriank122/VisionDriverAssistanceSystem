#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class SignRecogniserSpeedLimit
{
private:
	cv::Mat input, hsv, output;
	cv::Mat white;
	cv::Mat circleMask, circleClear, circle, sign;

	std::string signText;

	tesseract::TessBaseAPI tess;

	std::vector<std::vector<cv::Point>> contours, contoursSize, contoursCircles;
public:
	SignRecogniserSpeedLimit();
	~SignRecogniserSpeedLimit();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void contoursFiltration();
	void conditionChecking();
	void drawContours();
};

