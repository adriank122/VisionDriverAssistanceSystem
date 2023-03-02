#include "SignRecogniserSTOP.h"

SignRecogniserSTOP::SignRecogniserSTOP()
{
}


SignRecogniserSTOP::~SignRecogniserSTOP()
{
}

cv::Mat SignRecogniserSTOP::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	contoursFiltration();
	conditionChecking();
	showResult();
	drawContours();

	return this->output;
}

void SignRecogniserSTOP::preprocessInput() {
	cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

	cv::Scalar lower_red1 = cv::Scalar(0, 60, 70);
	cv::Scalar upper_red1 = cv::Scalar(25, 255, 255);
	cv::inRange(hsv, lower_red1, upper_red1, red1);

	cv::Scalar lower_red2 = cv::Scalar(150, 60, 70);
	cv::Scalar upper_red2 = cv::Scalar(180, 255, 255);
	cv::inRange(hsv, lower_red2, upper_red2, red2);

	cv::bitwise_or(red1, red2, red);

	roi = cv::Mat::zeros(input.size(), CV_8UC1);
	cv::rectangle(roi, cv::Point(0, 0), cv::Point(input.cols, input.rows / 1.8), cv::Scalar(255), -1, 8);
	cv::bitwise_and(roi, red, mask);

	cv::Mat kernel = cv::Mat::ones(cv::Size(3, 3), CV_8UC1);
	cv::morphologyEx(mask, maskClosed, cv::MORPH_CLOSE, kernel, cv::Point(), 1);

	cv::Scalar lower_white = cv::Scalar(0, 0, 190);
	cv::Scalar upper_white = cv::Scalar(180, 80, 255);
	cv::inRange(hsv, lower_white, upper_white, white);

	cv::bitwise_or(red, white, redWhite);
}

void SignRecogniserSTOP::contoursFiltration() {
	contours.clear(); 
	contoursSize.clear(); 
	contoursCircles.clear(); 
	contoursVertexes.clear(); 

	contourPoints.clear();

	cv::findContours(maskClosed, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
		if (cv::contourArea(contours[i]) > 500 && cv::contourArea(contours[i]) < 12000) {
//			std::cout << cv::contourArea(contours[i])<< std::endl;
			cv::approxPolyDP(contours[i], contourPoints, 6, true);
			contoursSize.push_back(contourPoints);
//			cv::putText(output, std::to_string(contourPoints.size()), contourPoints[0], cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(0, 0, 225), 2, 8);
		}
	}

	for (int i = 0; i < contoursSize.size(); i++) {
		double perimeter = cv::arcLength(contoursSize[i], true);
		double circularity = (double)4 * CV_PI*cv::contourArea(contoursSize[i]) / (perimeter*perimeter);
//		std::cout << circularity << std::endl;
		if (circularity > 0.7) {
			contoursCircles.push_back(contoursSize[i]);
		}
	}

	for (int i = 0; i < contoursCircles.size(); i++) {
		if (contoursCircles[i].size() >= 6 && contoursCircles[i].size() <= 10) {
			contoursVertexes.push_back(contoursCircles[i]);
		}
	}
}

void SignRecogniserSTOP::conditionChecking() {
	signs.clear();

	for (int i = 0; i < contoursVertexes.size(); i++) {
		cv::Rect rect = cv::boundingRect(contoursVertexes[i]);
		rect = cv::Rect(cv::Point(rect.x, rect.y + rect.height / 4), cv::Point(rect.x + rect.width, rect.y + rect.height / 4 * 3));
		white(rect).copyTo(signWhite);
		std::vector<std::vector<cv::Point>> contoursRoi;
		cv::findContours(signWhite, contoursRoi, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		if (contoursRoi.size() > 4) {
			red(rect).copyTo(signRed);
//			std::cout << (double)cv::countNonZero(signRed) / (rect.width*rect.height) << std::endl;
			if ((double)cv::countNonZero(signRed) / (rect.width*rect.height) > 0.5) {
				signs.push_back(contoursVertexes[i]);
			}
		}
	}
}

void SignRecogniserSTOP::showResult() {
	for (int i = 0; i < signs.size(); i++) {
		cv::Rect signField = cv::boundingRect(signs[i]);
		cv::rectangle(output, signField, cv::Scalar(0, 0, 255), 2, 8);
		cv::putText(output, "STOP!", cv::Point(signField.x + signField.width, signField.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, 8);
		std::cout << "STOP Sign!" << std::endl;
	}
}

void SignRecogniserSTOP::drawContours() {
//	cv::drawContours(output, contours, -1, cv::Scalar(0, 255, 0), 2, 8);
//	cv::drawContours(output, contoursSize, -1, cv::Scalar(0, 255, 0), 2, 8);
//	cv::drawContours(output, contoursCircles, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, contoursVertexes, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, signs, -1, cv::Scalar(0, 0, 255), 2, 8);
}