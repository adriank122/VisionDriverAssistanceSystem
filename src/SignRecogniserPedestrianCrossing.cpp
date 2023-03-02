#include "SignRecogniserPedestrianCrossing.h"



SignRecogniserPedestrianCrossing::SignRecogniserPedestrianCrossing()
{
	isFound = false;
	counterToResetIsFound = 0;
}


SignRecogniserPedestrianCrossing::~SignRecogniserPedestrianCrossing()
{
}

cv::Mat SignRecogniserPedestrianCrossing::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	contoursFiltration();
	conditionChecking();
	drawContours();
	resetIsFound();

	return this->output;
}

void SignRecogniserPedestrianCrossing::preprocessInput() {
	cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

	cv::Scalar lower_blue = cv::Scalar(90, 150, 40);
	cv::Scalar upper_blue = cv::Scalar(140, 255, 255);
	cv::inRange(hsv, lower_blue, upper_blue, blue);

	cv::Scalar lower_white = cv::Scalar(0, 0, 200);
	cv::Scalar upper_white = cv::Scalar(180, 80, 255);
	cv::inRange(hsv, lower_white, upper_white, white);

	roi = cv::Mat::zeros(input.size(), CV_8UC1);
	cv::rectangle(roi, cv::Point(120, 120), cv::Point(input.cols - 120, input.rows / 2), cv::Scalar(255), -1);
	cv::bitwise_and(roi, blue, mask);

	cv::Mat kernel = cv::Mat::ones(cv::Size(3, 3), CV_8UC1);
	cv::morphologyEx(mask, maskOpened, cv::MORPH_OPEN, kernel, cv::Point(), 1);
	cv::morphologyEx(white, whiteClosed, cv::MORPH_CLOSE, kernel, cv::Point(), 1);
}

void SignRecogniserPedestrianCrossing::contoursFiltration() {
	contours.clear();
	contoursSize.clear();
	contoursTriangles.clear();

	contoursPoint.clear();

	cv::findContours(maskOpened, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++) {
//		std::cout << cv::contourArea(contours[i]) << std::endl;
		if (cv::contourArea(contours[i]) > 200) {
			cv::approxPolyDP(contours[i], contoursPoint, 8, true);
			std::sort(contoursPoint.begin(), contoursPoint.end(), mySort);
			contoursSize.push_back(contoursPoint);
		}
	}

	for (int i = 0; i < contoursSize.size(); i++) {
		if (contoursSize[i].size() == 3) {
			contoursTriangles.push_back(contoursSize[i]);
		}
	}
}

void SignRecogniserPedestrianCrossing::conditionChecking() {
	contoursSignTriangles.clear();
	roiContours.clear();

	roiContoursPoint.clear();

	signRois.clear();

	for (int i = 0; i < contoursTriangles.size(); i++) {
		if (contoursTriangles[i][1].y >= contoursTriangles[i][0].y - (double)abs(contoursTriangles[i][1].x - contoursTriangles[i][0].x) / 3
			&& contoursTriangles[i][1].y <= contoursTriangles[i][0].y + (double)abs(contoursTriangles[i][1].x - contoursTriangles[i][0].x) / 3) {
			if (contoursTriangles[i][1].x < contoursTriangles[i][0].x) {
				if (contoursTriangles[i][2].x >= contoursTriangles[i][1].x - (double)abs(contoursTriangles[i][2].y - contoursTriangles[i][1].y) / 3
					&& contoursTriangles[i][2].x <= contoursTriangles[i][1].x + (double)abs(contoursTriangles[i][2].y - contoursTriangles[i][1].y) / 3) {
					contoursSignTriangles.push_back(contoursTriangles[i]);
				}
			}
			else {
				if (contoursTriangles[i][2].x >= contoursTriangles[i][0].x - (double)abs(contoursTriangles[i][2].y - contoursTriangles[i][0].y) / 3
					&& contoursTriangles[i][2].x <= contoursTriangles[i][0].x + (double)abs(contoursTriangles[i][2].y - contoursTriangles[i][0].y) / 3) {
					contoursSignTriangles.push_back(contoursTriangles[i]);
				}
			}
		}
	}

	for (int i = 0; i < contoursSignTriangles.size(); i++) {
		cv::Rect rect = cv::boundingRect(contoursSignTriangles[i]);
		rect = cv::Rect(rect.x, rect.y, rect.width * 3, rect.height * 1.5);
		if (rect.x < 0) {
			rect.x = 0;
		}
		if (rect.y < 0) {
			rect.y = 0;
		}
		if (rect.x + rect.width > input.cols - 1) {
			rect.width = input.cols - 1 - rect.x;
		}
		if (rect.y + rect.height > input.rows - 1) {
			rect.height = input.rows - 1 - rect.y;
		}
//		cv::rectangle(output, rect, cv::Scalar(255, 0, 0), 2, 8);
		if (rect.height != 0) {
			if (double(rect.width / rect.height) >= 0.7 && double(rect.width / rect.height) <= 1.5) {
				signRois.push_back(rect);
//				cv::rectangle(output, rect, cv::Scalar(0, 0, 255), 2, 8);
			}
		}
	}

	for (int i = 0; i < signRois.size(); i++) {
		cv::Mat signRoi;
		if (signRois[i].x + signRois[i].width < input.cols && signRois[i].y + signRois[i].height < input.rows) {
			mask(signRois[i]).copyTo(signRoi);
			cv::bitwise_not(signRoi, signRoi);
			cv::Mat kernel = cv::Mat::ones(cv::Size(3, 3), CV_8UC1);
			cv::morphologyEx(signRoi, signRoi, cv::MORPH_CLOSE, kernel, cv::Point(), 1);
			cv::morphologyEx(signRoi, signRoi, cv::MORPH_ERODE, kernel, cv::Point(), 5);

			cv::findContours(signRoi, roiContours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point());

			for (int j = 0; j < roiContours.size(); j++) {
//				std::cout << cv::contourArea(roiContours[j]) << std::endl;
				if (cv::contourArea(roiContours[j]) > 30) {
					cv::approxPolyDP(roiContours[j], roiContoursPoint, 5, true);
				}
				if (roiContoursPoint.size() == 3) {
					sort(roiContoursPoint.begin(), roiContoursPoint.end(), mySort);
					if (
						(
						(roiContoursPoint[1].x + roiContoursPoint[2].x) / 2 >= roiContoursPoint[0].x - abs(roiContoursPoint[1].x - roiContoursPoint[2].x) / 4 
							&& (roiContoursPoint[1].x + roiContoursPoint[2].x) / 2 <= roiContoursPoint[0].x + abs(roiContoursPoint[1].x - roiContoursPoint[2].x) / 4
							)
						&& 
						(roiContoursPoint[1].y >= roiContoursPoint[2].y - 4 && roiContoursPoint[1].y <= roiContoursPoint[2].y + 4)
						&& 
						(roiContoursPoint[1].y - roiContoursPoint[0].y < abs(roiContoursPoint[1].x - roiContoursPoint[2].x)*1.5)
						) 
					{
						cv::Rect detectedSign = boundingRect(roiContoursPoint);
						cv::rectangle(output, signRois[i], cv::Scalar(0, 0, 255), 2, 8);
						putText(output, "Znak: Przejscie dla pieszych", cv::Point(signRois[i].x, signRois[i].y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, 8);
						isFound = true;
						counterToResetIsFound = 15;
						std::cout << "Pedestrian Crossing Sign!" << std::endl;
					}
				}
			}
		}
	}
}

void SignRecogniserPedestrianCrossing::drawContours() {
//	cv::drawContours(output, contours, -1, cv::Scalar(0, 0, 255), 2, 8);
//	cv::drawContours(output, contoursSize, -1, cv::Scalar(255, 0, 0), 2, 8);
//	cv::drawContours(output, contoursTriangles, -1, cv::Scalar(0, 255, 0), 2, 8);
//	cv::drawContours(output, contoursSignTriangles, -1, cv::Scalar(0, 255, 255), 2, 8);
}

bool SignRecogniserPedestrianCrossing::getIsFound() {
	return this->isFound;
}

void SignRecogniserPedestrianCrossing::resetIsFound(){
	if (counterToResetIsFound == 0) {
		isFound = false;
	}
	else {
		counterToResetIsFound--;
	}
}
