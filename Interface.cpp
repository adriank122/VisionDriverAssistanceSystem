#include "Interface.h"

Interface::Interface()
{
	isLaneAssistSelected = false;
	isSignRecognitionSelected = false;
	isRedLightsDetectionSelected = false;
	isPedestrianDetectionSelected = false;

	isProgramStarted = false;

	frameCounter = 0;

	fpsInfo = "";
}


Interface::~Interface()
{
}


void Interface::startProgram() {
	int count = 0;
	timeBegin = time(0);

	while (cap.isOpened()) {
		cap >> input;
		count++;
		std::cout << count << std::endl;
		input.copyTo(output);

		frameCounter++;

		if (isLaneAssistSelected) {
			output = laneAssistant.start(input, output);
		}

		if (isSignRecognitionSelected) {
			output = signRecogniserSTOP.start(input, output);
			output = signRecogniserGiveWay.start(input, output);
			output = signRecogniserSpeedLimit.start(input, output);
			output = signRecogniserPedestrianCrossing.start(input, output);			
		}

		if (isRedLightsDetectionSelected) {
			output = redLightsDetector.start(input, output);
		}

		if (isPedestrianDetectionSelected) {
			if (signRecogniserPedestrianCrossing.getIsFound()) {
				output = pedestrianDetector.start(input, output);
			}
		}

		timeNow = time(0) - timeBegin;
		if (timeNow >= 1) {
			countFPS();
			timeBegin = time(0);
		}

		cv::putText(output, fpsInfo, cv::Point(0, output.rows - 10), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(0, 0, 0), 2, 8);
				
		showOutput();

		saveOutput();
		
		if (cv::waitKey(1) == 27) {
			break;
		}
	}
	cap.release();
	out.release();
	cv::destroyAllWindows();
}

void Interface::loadInput() {

		//YOUR FILE
//	cap = cv::VideoCapture("");

		//CAMERA LIVE
	cap = cv::VideoCapture(0);

	if (!cap.isOpened()) {
		std::cout << "Blad odczytu pliku!" << std::endl;
	}
}

void Interface::showOutput() {
	cv::namedWindow("DriverSupportSystem", cv::WINDOW_NORMAL);
	cv::imshow("DriverSupportSystem", output);
}

void Interface::showMenu() {
	while (!isProgramStarted) {
		system("cls");

		std::cout << "Select Modules!" << std::endl;

		std::cout << "1 - Lane Assistant ";
		if (isLaneAssistSelected) {
			std::cout << "\t\tSELECTED" << std::endl;
		}
		else {
			std::cout << std::endl;
		}

		std::cout << "2 - Sign Recognition ";
		if (isSignRecognitionSelected) {
			std::cout << "\t\tSELECTED" << std::endl;
		}
		else {
			std::cout << std::endl;
		}

		std::cout << "3 - Red Lights Detection ";
		if (isRedLightsDetectionSelected) {
			std::cout << "\tSELECTED" << std::endl;
		}
		else {
			std::cout << std::endl;
		}

		std::cout << "4 - Pedestrian Detection ";
		if (isPedestrianDetectionSelected) {
			std::cout << "\tSELECTED" << std::endl;
		}
		else {
			std::cout << std::endl;
		}

		std::cout << "\n5 - Start Program" << std::endl;

		getUserChoice();
	}
}

void Interface::getUserChoice() {
	char userChoice = _getch();
	if (userChoice == '1') {
		isLaneAssistSelected = !isLaneAssistSelected;
	}else
	if (userChoice == '2') {
		isSignRecognitionSelected = !isSignRecognitionSelected;
	}else
	if (userChoice == '3') {
		isRedLightsDetectionSelected = !isRedLightsDetectionSelected;
	}else
	if (userChoice == '4') {
		isPedestrianDetectionSelected = !isPedestrianDetectionSelected;
	}else
	if (userChoice == '5') {
		isProgramStarted = true;
	}
}

void Interface::countFPS() {
	fps = frameCounter;
	fpsInfo = "fps = " + std::to_string(fps);
	frameCounter = 0;
}

void Interface::saveOutput() {
	out.write(output);
}

cv::Mat Interface::getInput() {
	return input;
}

void Interface::setInput(cv::Mat input) {
	this->input = input;
}

cv::Mat Interface::getOutput() {
	return output;
}

void Interface::setOutput(cv::Mat output) {
	this->output = output;
}