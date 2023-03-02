#include "SignRecogniserSpeedLimit.h"

SignRecogniserSpeedLimit::SignRecogniserSpeedLimit()
{
	tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
	tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

	signText = "";
}


SignRecogniserSpeedLimit::~SignRecogniserSpeedLimit()
{
	tess.End();
}

cv::Mat SignRecogniserSpeedLimit::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	contoursFiltration();
	conditionChecking();
	drawContours();

	return this->output;
}

void SignRecogniserSpeedLimit::preprocessInput() {
	cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

	cv::Scalar lower_white = cv::Scalar(0, 0, 140);
	cv::Scalar upper_white = cv::Scalar(180, 120, 255);
	cv::inRange(hsv, lower_white, upper_white, white);
}

void SignRecogniserSpeedLimit::contoursFiltration() {
	contours.clear();
	contoursSize.clear();
	contoursCircles.clear();

	cv::findContours(white, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
//		std::cout << cv::contourArea(contours[i]) << std::endl;
		if (cv::contourArea(contours[i]) > 300 && cv::contourArea(contours[i]) < 1500) {
			contoursSize.push_back(contours[i]);
		}
	}
	
	for (int i = 0; i < contoursSize.size(); i++) {
		double perimeter = cv::arcLength(contoursSize[i], true);
		double circularity = 4 * CV_PI*(cv::contourArea(contoursSize[i]) / (perimeter*perimeter));
//		std::cout << circularity << std::endl;
		if (circularity > 0.8) {
			contoursCircles.push_back(contoursSize[i]);
		}
	}
}


void SignRecogniserSpeedLimit::conditionChecking() {
	for (int i = 0; i < contoursCircles.size(); i++) {
		cv::Rect rect = cv::boundingRect(contoursCircles[i]);
		white(rect).copyTo(circle);
		
		circleMask = cv::Mat::ones(rect.height, rect.width, CV_8UC1) * 255;
		int radius;
		if (circleMask.cols > circleMask.rows) {
			radius = circleMask.rows / 2;
		}
		else {
			radius = circleMask.cols / 2;
		}
		cv::circle(circleMask, cv::Point(circleMask.cols / 2, circleMask.rows / 2), radius, cv::Scalar(0), -1, 8);

		cv::bitwise_or(circle, circleMask, circleClear);

		tess.SetImage((uchar*)circleClear.data, circleClear.cols, circleClear.rows, 1, circleClear.step);
		signText = std::string(tess.GetUTF8Text());
//		std::cout << "Picture number " + std::to_string(i) + ": " << signText << std::endl;

		std::string value;

		for (int j = 0; j < signText.size(); j++) {
			for (int k = 48; k <= 57; k++) {			//48-57  -> numbers from 0 to 9 in ASCII code
				if (signText.at(j) == k) {
					value.push_back(signText.at(j));
				}
			}
		}

		for (int j = 2; j < 13; j++) {
			std::stringstream ss;
			ss << j * 10;
			std::string speedValue = ss.str();
			if (value == speedValue) {
				cv::circle(output, cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2), radius, cv::Scalar(0, 0, 255), 2, 8);
				std::string textToShow = "Ograniczenie predkosci " + speedValue + " km/h!";
				putText(output, textToShow, contoursCircles[i][0], cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, 8);
				std::cout << "Speed Limit "<< speedValue << " km/h Sign!" << std::endl;
			}
		}
	}
}

void SignRecogniserSpeedLimit::drawContours() {
//	cv::drawContours(output, contours, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, contoursSize, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, contoursCircles, -1, cv::Scalar(0, 0, 255), 2, 8);
}
