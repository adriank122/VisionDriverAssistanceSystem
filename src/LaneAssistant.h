#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class LaneAssistant
{
private:
	cv::Mat input, gray, hsv, blurred, roi, edges, edgesRoi, trackMask, output, white, whiteRoi;

		//WSPӣRZ�DNE LINI LEWEJ I PRAWEJ  
		//-> LINIA MA POSTA� DLA LEWEJ L1(l1_x, l1_y), L2(l2_x, l2_y), 
		//DLA PRAWEJ R1(r1_x, r1_y), R2(r2_x, r2_y)
	int l1_x, l1_y, l2_x, l2_y;
	int r1_x, r1_y, r2_x, r2_y;

		//CZY POKAZA� LINIE
		//CZY LINIE S� BLISKO LINII BIA�EJ
	bool leftToShow, rightToShow;
	bool leftNearToWhite, rightNearToWhite;

		//DANE DO WY�WIETLANIA KOMUNIKATU
		//CZY ODLICZANIE JEST W��CZONE (KOMUNIKAT JEST WY�WIETLANY)
		//CZAS DO KO�CA WY�WIETALNIA KOMUNIKATU
	bool isCounterOnLeft, isCounterOnRight;
	int warningCounterLeft, warningCounterRight;

		//DANE DO ZARZ�DZANIA WEKTOREM PAMI�CI
		//DANE OSTATNICH KILKU LINII S� ZAPISANE ODPOWIEDNIO W WEKTORZE DLA LINII LEWEJ I PRAWEJ
		//DANE W POSTACI DW�CH LICZB DOUBLE, PONIEWA� PRZECHOWYWANE S� W NICH INFORMACJE O WSPӣCZYNNIKU KIERUNKOWYM (A) I WYRAZIE WOLNYM (B)
		//PO UP�YWIE ILO�CI KLATEK R�WNEJ toRestartCounter WEKTORY S� ZEROWANE
	std::vector<cv::Vec2d> lastCoefficientsLeft;	
	std::vector<cv::Vec2d> lastCoefficientsRight;

	bool isFirstLoop;
	int toRestartCounter;

		//WEKTOR ODNALEZIONYCH NA OBRAZIE LINII
	std::vector<cv::Vec4i> lines;

		//WEKTOR PRZECHOWUJ�CY NUMERY LINII, KT�RE S� NAJBLI�EJ �RODKA DROGI
	std::vector<int> numbersOfTheNearestLines;

		//WARTO�CI ODLEG�O�CI OD �RODKA DROGI LINII B�D�CYCH NAJBLI�EJ �RODKA DROGI
	int theLowestValuePositive, theLowestValueNegative;

		//WSPӣCZYNNIKI OPISUJ�CE OBYDWIE LINIE
	double slopeLeft, constLeft, slopeRight, constRight;
		
		//ILO�� OSTATNICH WARTO�CI LINII, KT�RE S� ZAPAMI�TYWANE
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