#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class SignRecogniserGiveWay
{
private:
	cv::Mat input, hsv, roi, output;
	cv::Mat yellow, mask, closed;
	cv::Mat signSurrounding;

	std::vector<std::vector<cv::Point>> contours, contoursSize, triangles, signs, signsChecked;
	std::vector<cv::Point> contoursPoint;
public:
	SignRecogniserGiveWay();
	~SignRecogniserGiveWay();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void contoursFiltration();
	void conditionChecking();
	void showResult();
	void drawContours();

	struct MySort {
		bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
	} mySort;
};

