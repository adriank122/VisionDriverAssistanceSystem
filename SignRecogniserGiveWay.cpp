#include "SignRecogniserGiveWay.h"

SignRecogniserGiveWay::SignRecogniserGiveWay()
{
}


SignRecogniserGiveWay::~SignRecogniserGiveWay()
{
}

cv::Mat SignRecogniserGiveWay::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	contoursFiltration();
	conditionChecking();
	showResult();
	drawContours();

	return this->output;
}

void SignRecogniserGiveWay::preprocessInput() {
	cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

	cv::Scalar lower_yellow = cv::Scalar(20, 70, 70);
	cv::Scalar upper_yellow = cv::Scalar(40, 255, 255);

	cv::inRange(hsv, lower_yellow, upper_yellow, yellow);

	roi = cv::Mat::zeros(input.size(), CV_8UC1);
	cv::rectangle(roi, cv::Point(0, 0), cv::Point(input.cols, input.rows / 1.8), cv::Scalar(255), -1);
	cv::bitwise_and(roi, yellow, mask);

	cv::Mat kernel = cv::Mat::ones(cv::Size(3, 3), CV_8UC1);
	cv::morphologyEx(mask, closed, cv::MORPH_CLOSE, kernel, cv::Point(), 1);
}

void SignRecogniserGiveWay::contoursFiltration() {
	contours.clear(); 
	contoursSize.clear(); 
	triangles.clear(); 

	contoursPoint.clear();

	cv::findContours(closed, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
		if (cv::contourArea(contours[i]) > 400) {
			cv::approxPolyDP(contours[i], contoursPoint, 8, true);
			std::sort(contoursPoint.begin(), contoursPoint.end(), mySort);
			contoursSize.push_back(contoursPoint);
		}
	}

	for (int i = 0; i < contoursSize.size(); i++) {
		if (contoursSize[i].size() == 3) {
			triangles.push_back(contoursSize[i]);
		}
	}
}

void SignRecogniserGiveWay::conditionChecking() {
	signs.clear();
	signsChecked.clear();

	for (int i = 0; i < triangles.size(); i++) {
		if (
			(
			(triangles[i][0].x + triangles[i][1].x) / 2 <= (triangles[i][2].x + double(abs(triangles[i][0].x - triangles[i][1].x)) / 3)
				&&
				(triangles[i][0].x + triangles[i][1].x) / 2 >= (triangles[i][2].x - double(abs(triangles[i][0].x - triangles[i][1].x)) / 3)
				)
			&& (triangles[i][0].y >= (triangles[i][1].y - 10) && triangles[i][0].y <= (triangles[i][1].y + 10))
			)
		{
			signs.push_back(triangles[i]);
		}
	}

	for (int i = 0; i < signs.size(); i++) {
		cv::Rect rect = cv::boundingRect(signs[i]);
		rect = cv::Rect(rect.x - rect.width * 2, rect.y - rect.height * 2, rect.width * 5, rect.height * 5);
		if (rect.x < 0) {
			rect.x = 0;
		}
		if (rect.x + rect.width > input.cols - 1) {
			rect.width = input.cols - 1 - rect.x;
		}
		if (rect.y < 0) {
			rect.y = 0;
		}
		if (rect.y + rect.height > input.rows - 1) {
			rect.height = input.rows - 1 - rect.y;
		}
//		cv::rectangle(output, rect, cv::Scalar(0, 0, 255), 1, 8);
		closed(rect).copyTo(signSurrounding);

		std::vector<std::vector<cv::Point>> contoursSignSurrounding;
		cv::findContours(signSurrounding, contoursSignSurrounding, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
//		cv::putText(output, std::to_string(contoursSignSurrounding.size()), cv::Point(rect.x, rect.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, 8);
		if (contoursSignSurrounding.size() < 4) {
			signsChecked.push_back(signs[i]);
		}

	}
}

void SignRecogniserGiveWay::showResult() {
	for (int i = 0; i < signsChecked.size(); i++) {
		cv::Rect rect = boundingRect(signsChecked[i]);
		cv::rectangle(output, rect, cv::Scalar(0, 0, 255), 2, 8);
		cv::putText(output, "Znak: Ustap pierwszenstwa", cv::Point(rect.x, rect.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, 8);
		std::cout << "Give Way Sign!" << std::endl;
	}
}

void SignRecogniserGiveWay::drawContours() {
//	cv::drawContours(output, contours, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, contoursSize, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, triangles, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, signs, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, signsChecked, -1, cv::Scalar(0, 0, 255), 2, 8);
}