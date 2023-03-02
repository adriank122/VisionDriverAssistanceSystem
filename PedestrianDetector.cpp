#include "PedestrianDetector.h"



PedestrianDetector::PedestrianDetector()
{
	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}


PedestrianDetector::~PedestrianDetector()
{
}

cv::Mat PedestrianDetector::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	searchForPedestrians();
	showInfo();

	return this->output;
}

void PedestrianDetector::preprocessInput() {
	cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

	roi = cv::Mat::zeros(input.size(), CV_8UC1);
	cv::rectangle(roi, cv::Point(input.cols / 3, input.rows / 2), cv::Point(input.cols / 3 * 2, input.rows / 4 * 3), cv::Scalar(255), -1, 8);
	cv::bitwise_and(gray, roi, mask);
}

void PedestrianDetector::searchForPedestrians() {
	found.clear();
	hog.detectMultiScale(mask, found, 0, cv::Size(16, 16), cv::Size(16, 16), 1.1, 2);
}

void PedestrianDetector::showInfo() {
	for (int i = 0; i < found.size(); i++) {
		rectangle(output, found[i], cv::Scalar(0, 0, 255), 3);
		cv::putText(output, "Uwaga! Pieszy!", cv::Point(input.cols / 4, input.rows / 2), cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(0, 0, 255), 2, 8);
		std::cout << "Pieszy!" << std::endl;
	}
}