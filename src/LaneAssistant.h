#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class LaneAssistant
{
private:
	cv::Mat input, gray, hsv, blurred, roi, edges, edgesRoi, trackMask, output, white, whiteRoi;

		//WSPÓ£RZÊDNE LINI LEWEJ I PRAWEJ  
		//-> LINIA MA POSTAÆ DLA LEWEJ L1(l1_x, l1_y), L2(l2_x, l2_y), 
		//DLA PRAWEJ R1(r1_x, r1_y), R2(r2_x, r2_y)
	int l1_x, l1_y, l2_x, l2_y;
	int r1_x, r1_y, r2_x, r2_y;

		//CZY POKAZAÆ LINIE
		//CZY LINIE S¥ BLISKO LINII BIA£EJ
	bool leftToShow, rightToShow;
	bool leftNearToWhite, rightNearToWhite;

		//DANE DO WYŒWIETLANIA KOMUNIKATU
		//CZY ODLICZANIE JEST W£¥CZONE (KOMUNIKAT JEST WYŒWIETLANY)
		//CZAS DO KOÑCA WYŒWIETALNIA KOMUNIKATU
	bool isCounterOnLeft, isCounterOnRight;
	int warningCounterLeft, warningCounterRight;

		//DANE DO ZARZ¥DZANIA WEKTOREM PAMIÊCI
		//DANE OSTATNICH KILKU LINII S¥ ZAPISANE ODPOWIEDNIO W WEKTORZE DLA LINII LEWEJ I PRAWEJ
		//DANE W POSTACI DWÓCH LICZB DOUBLE, PONIEWA¯ PRZECHOWYWANE S¥ W NICH INFORMACJE O WSPÓ£CZYNNIKU KIERUNKOWYM (A) I WYRAZIE WOLNYM (B)
		//PO UP£YWIE ILOŒCI KLATEK RÓWNEJ toRestartCounter WEKTORY S¥ ZEROWANE
	std::vector<cv::Vec2d> lastCoefficientsLeft;	
	std::vector<cv::Vec2d> lastCoefficientsRight;

	bool isFirstLoop;
	int toRestartCounter;

		//WEKTOR ODNALEZIONYCH NA OBRAZIE LINII
	std::vector<cv::Vec4i> lines;

		//WEKTOR PRZECHOWUJ¥CY NUMERY LINII, KTÓRE S¥ NAJBLI¯EJ ŒRODKA DROGI
	std::vector<int> numbersOfTheNearestLines;

		//WARTOŒCI ODLEG£OŒCI OD ŒRODKA DROGI LINII BÊD¥CYCH NAJBLI¯EJ ŒRODKA DROGI
	int theLowestValuePositive, theLowestValueNegative;

		//WSPÓ£CZYNNIKI OPISUJ¥CE OBYDWIE LINIE
	double slopeLeft, constLeft, slopeRight, constRight;
		
		//ILOŒÆ OSTATNICH WARTOŒCI LINII, KTÓRE S¥ ZAPAMIÊTYWANE
	int memorySizeOfLastLineCoefficients;

public:
	LaneAssistant();
	~LaneAssistant();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void findLaneLines();
	void lineVerificationBasedOnHistory();
	void lineCoefficientsCalculation();
	void laneChangingDetection();
	void memoryOfLastLineCoefficientsRefreshing();
	void lineExtension();
	void checkingWhiteSurroundingOfLines();
	void laneFieldMarking();
	void lineMarking();
	void showFoundedLinesSelection();
};