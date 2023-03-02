#pragma once

#include "RedLightsDetector.h"
#include "SignRecogniserSTOP.h"
#include "SignRecogniserGiveWay.h"
#include "SignRecogniserSpeedLimit.h"
#include "SignRecogniserPedestrianCrossing.h"
#include "LaneAssistant.h"
#include "PedestrianDetector.h"

#include <iostream>		
#include <conio.h>		//_getch function
#include <time.h>		//fps counter

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class Interface
{
private:
	RedLightsDetector redLightsDetector;
	SignRecogniserSTOP signRecogniserSTOP;
	SignRecogniserGiveWay signRecogniserGiveWay;
	SignRecogniserSpeedLimit signRecogniserSpeedLimit;
	SignRecogniserPedestrianCrossing signRecogniserPedestrianCrossing;
	LaneAssistant laneAssistant;
	PedestrianDetector pedestrianDetector;

	bool isLaneAssistSelected, isSignRecognitionSelected, isRedLightsDetectionSelected, isPedestrianDetectionSelected;
	bool isProgramStarted;
	cv::Mat input, dst, output;
	cv::VideoCapture cap;
	cv::VideoWriter out;
	time_t timeBegin, timeNow;
	int fps;
	std::string fpsInfo;
	int frameCounter;
public:
	Interface();
	~Interface();

	void startProgram();
	void loadInput();
	void showOutput();
	void showMenu();
	void getUserChoice();
	void countFPS();
	void saveOutput();

	cv::Mat getInput();
	void setInput(cv::Mat input);
	cv::Mat getOutput();
	void setOutput(cv::Mat output);
};

