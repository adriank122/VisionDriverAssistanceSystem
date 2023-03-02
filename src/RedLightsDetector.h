#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class RedLightsDetector
{
private:
	cv::Mat input, hsv, roi, output;
	cv::Mat red1, red2, red, max, redMax, mask, dark;
	cv::Mat circleRoi, darkRoi;

	std::vector<std::vector<cv::Point>> contours, contoursSize, contoursCircles, contoursCirclesRed;

public:
	RedLightsDetector();
	~RedLightsDetector();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void contoursFiltration();
	void conditionChecking();
	void drawContours();
};