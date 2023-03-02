#include "RedLightsDetector.h"

RedLightsDetector::RedLightsDetector()
{
}


RedLightsDetector::~RedLightsDetector()
{
}

cv::Mat RedLightsDetector::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	contoursFiltration();
	conditionChecking();
	drawContours();

	return this->output;
}

void RedLightsDetector::preprocessInput() {
	cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

	roi = cv::Mat::zeros(input.size(), CV_8UC1);
	cv::rectangle(roi, cv::Point(0, 0), cv::Point(input.cols, input.rows / 1.9), cv::Scalar(255), -1, 8);

	cv::Scalar lower_red1 = cv::Scalar(0, 25, 210);
	cv::Scalar upper_red1 = cv::Scalar(26, 255, 255);
	cv::inRange(hsv, lower_red1, upper_red1, red1);

	cv::Scalar lower_red2 = cv::Scalar(150, 25, 210);
	cv::Scalar upper_red2 = cv::Scalar(180, 255, 255);
	cv::inRange(hsv, lower_red2, upper_red2, red2);

	cv::bitwise_or(red1, red2, red);

	cv::Scalar lower_max = cv::Scalar(0, 0, 255);
	cv::Scalar upper_max = cv::Scalar(180, 30, 255);
	cv::inRange(hsv, lower_max, upper_max, max);

	cv::bitwise_or(red, max, redMax);

	cv::bitwise_and(redMax, roi, mask);

	cv::Scalar lower_dark = cv::Scalar(0, 0, 0);
	cv::Scalar upper_dark = cv::Scalar(180, 255, 135);
	cv::inRange(hsv, lower_dark, upper_dark, dark);
}

void RedLightsDetector::contoursFiltration() {

	contours.clear();
	contoursSize.clear();
	contoursCircles.clear();
	contoursCirclesRed.clear();

	cv::findContours(mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
		if (cv::contourArea(contours[i]) < 600 && cv::contourArea(contours[i]) > 30) {
			contoursSize.push_back(contours[i]);
		}
	}

	for (int i = 0; i < contoursSize.size(); i++) {
		double perimeter = cv::arcLength(contoursSize[i], true);
		double circularity = (double)4 * CV_PI*cv::contourArea(contoursSize[i]) / (perimeter*perimeter);
		if (circularity > 0.7) {
			contoursCircles.push_back(contoursSize[i]);
		}
	}

	for (int i = 0; i < contoursCircles.size(); i++) {
		cv::Rect rect = cv::boundingRect(contoursCircles[i]);
		red(rect).copyTo(circleRoi);
		if ((double)cv::countNonZero(circleRoi) / (rect.width*rect.height) > 0.2) {
			contoursCirclesRed.push_back(contoursCircles[i]);
		}
	}
}

void RedLightsDetector::conditionChecking() {
	for (int i = 0; i < contoursCirclesRed.size(); i++) {
		cv::Rect rect = boundingRect(contoursCirclesRed[i]);
		cv::Rect roi = cv::Rect(cv::Point(rect.x - rect.width / 5, rect.y - rect.height / 5), cv::Point(rect.x + rect.width + rect.width / 5, rect.y + rect.height*3.5));
		if (roi.x >= 0 && roi.y >= 0 && roi.x + roi.width < input.cols&&roi.y + roi.height < input.rows) {
			dark(roi).copyTo(darkRoi);
//			std::cout << (double)cv::countNonZero(darkRoi) / (darkRoi.cols*darkRoi.rows) << std::endl;
			if ((double)cv::countNonZero(darkRoi) / (darkRoi.cols*darkRoi.rows) > 0.6) {
				cv::rectangle(output, roi, cv::Scalar(0, 0, 255), 4, 8);
				cv::putText(output, "Red light!", cv::Point(roi.x, roi.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, 8);
				std::cout << "Red Light!" << std::endl;
			}
		}
	}
}

void RedLightsDetector::drawContours() {
//	cv::drawContours(output, contours, -1, cv::Scalar(0, 255, 0), 2, 8);
//	cv::drawContours(output, contoursSize, -1, cv::Scalar(0, 255, 0), 2, 8);
//	cv::drawContours(output, contoursCircles, -1, cv::Scalar(0, 255, 0), 2, 8);
//	cv::drawContours(output, contoursCirclesRed, -1, cv::Scalar(0, 255, 0), 2, 8);
}