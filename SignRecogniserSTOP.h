#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class SignRecogniserSTOP
{
private:
	cv::Mat input, hsv, roi, output;
	cv::Mat red1, red2, red, white, redWhite;
	cv::Mat mask, maskClosed;
	cv::Mat signWhite, signRed;

	std::vector<std::vector<cv::Point>> contours, contoursSize, contoursCircles, contoursVertexes, signs;
	std::vector<cv::Point> contourPoints;
public:
	SignRecogniserSTOP();
	~SignRecogniserSTOP();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void contoursFiltration();
	void conditionChecking();
	void showResult();
	void drawContours();
};