#include "LaneAssistant.h"



LaneAssistant::LaneAssistant()
{
	isCounterOnLeft = false;
	isCounterOnRight = false;
	warningCounterLeft = 0;
	warningCounterRight = 0;

	isFirstLoop = true;
	toRestartCounter = 0;

	memorySizeOfLastLineCoefficients = 20;
}


LaneAssistant::~LaneAssistant()
{
}

cv::Mat LaneAssistant::start(cv::Mat &input, cv::Mat &output) {
	this->input = input;
	this->output = output;

	preprocessInput();
	findLaneLines();
	lineCoefficientsCalculation();
	lineVerificationBasedOnHistory();
	laneChangingDetection();
	memoryOfLastLineCoefficientsRefreshing();
	lineExtension();
	checkingWhiteSurroundingOfLines();
	laneFieldMarking();
	lineMarking();
	showFoundedLinesSelection();

	return this->output;
}

void LaneAssistant::preprocessInput() {
	cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

	//MASKA DO OGRANICZENIA ROZPATRYWANEGO POLA DO PRZEWIDYWANEGO OBSZARU WYSTÊPOWANIA PASA RUCHU
	roi = cv::Mat::zeros(input.rows, input.cols, CV_8UC1);

	//PUNKTY DO USTALANIA PRZEWIDYWANEGO OBSZARU WYSTÊPOWANIA PASA RUCHU 1
	std::vector<cv::Point> roiPoints1;
	roiPoints1.push_back(cv::Point(0, 830));
	roiPoints1.push_back(cv::Point(750, 830));
	roiPoints1.push_back(cv::Point(940, 620));
	roiPoints1.push_back(cv::Point(820, 620));
	const cv::Point* elementPoints1[1] = { &roiPoints1[0] };
	int npt1 = (int)roiPoints1.size();

	//PUNKTY DO USTALANIA PRZEWIDYWANEGO OBSZARU WYSTÊPOWANIA PASA RUCHU 2
	std::vector<cv::Point> roiPoints2;
	roiPoints2.push_back(cv::Point(1110, 830));
	roiPoints2.push_back(cv::Point(1960, 830));
	roiPoints2.push_back(cv::Point(1060, 620));
	roiPoints2.push_back(cv::Point(940, 620));
	const cv::Point* elementPoints2[1] = { &roiPoints2[0] };
	int npt2 = (int)roiPoints2.size();

	//OKREŒLENIE NA MASCE PRZEWIDYWANEGO OBSZARU WYSTÊPOWANIA PASA RUCHU
	cv::fillPoly(roi, elementPoints1, &npt1, 1, 255, 8, 0);
	cv::fillPoly(roi, elementPoints2, &npt2, 1, 255, 8, 0);

	//DETEKCJA BIA£EJ BARWY
	cv::Scalar lower_white = cv::Scalar(0, 0, 190);		
	cv::Scalar upper_white = cv::Scalar(180, 80, 255);
	cv::inRange(hsv, lower_white, upper_white, white);

	cv::bitwise_and(white, roi, white);

	//PODSTAWOWE PRZESZTA£CENIA (GRAY, BLUR) I WYZNACZENIE KRAWÊDZI ZA POMOC¥ ALGORYTMU CANNY
	cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0, 0);

	int thresholdCanny = 90;
	cv::Canny(blurred, edges, thresholdCanny, thresholdCanny * 3);

	//POZOSTAWIENIE JEDYNIE TYCH KRAWÊDZI, KTÓRE ZNAJDUJ¥ SIÊ W PRZEWIDYWANYM OBSZARZE WYSTÊPOWANIA PASA RUCHU (OKREŒLONE PRZEZ MASK)
	bitwise_and(edges, roi, edgesRoi);
}

void LaneAssistant::findLaneLines() {
	//UZYSKANIE WEKTORA LINII Z KRAWÊDZI OBRAZU ROI_EDGES
	lines.clear();	
	HoughLinesP(edgesRoi, lines, 1, CV_PI / 180, 20, 70, 180);

	//INICJACJA WARTOŒCI WSPÓ£CZYNNIKÓW PUNKTÓW WYNIKOWEJ LINII LEWEJ I PRAWEJ - PÓKI NIE ZOSTAN¥ WYKRYTE LINIE SK£ADOWE TO WYNIKOWA LINIA SIÊ NIE POJAWI (FLAGA WYŒWIETLENIA = FALSE)
	leftToShow = false;
	rightToShow = false;


	numbersOfTheNearestLines = { NULL, NULL };
	theLowestValuePositive = 500;
	theLowestValueNegative = 500;

	//ZNALEZIENIE LINII PO£O¯ONYCH NAJBLI¯EJ ŒRODKA NA WYSOKOŒCI 710 PIKSELI
	for (int i = 0; i < lines.size(); i++) {
		cv::Vec4i line = lines[i];
		int theNearestPoint = 500;

		//PRRZYJÊTY ŒRODEK POJAZDU TO (930,710)
		//WSPÓ£CZYNNIK NACHYLENIA LINII
		double slopeNearest = (double)(line[3] - line[1]) / (line[2] - line[0]);  //WSPÓ£CZYNNIK A
		double constNearest = (double)(line[3] - slopeNearest * line[2]);	//WSPÓ£CZYNNIK B

		if (slopeNearest<-0.4 || slopeNearest>0.4) {
			theNearestPoint = (int)(710 - constNearest) / slopeNearest - 930;
		}

		if (theNearestPoint > 0 && slopeNearest > 0) {		//JEŒLI LINIA PRAWA
			if (theNearestPoint < theLowestValuePositive) {
				theLowestValuePositive = theNearestPoint;
				numbersOfTheNearestLines[1] = i;
				rightToShow = true;
			}
		}
		if (theNearestPoint < 0 && slopeNearest < 0) {		//JEŒLI LINIA LEWA
			if (abs(theNearestPoint) < theLowestValueNegative) {
				theLowestValueNegative = abs(theNearestPoint);
				numbersOfTheNearestLines[0] = i;
				leftToShow = true;
			}
		}
	}
}

void LaneAssistant::laneChangingDetection() {
	//W PRAWO
	if (theLowestValueNegative != 500 && theLowestValuePositive != 500) {
		if (theLowestValueNegative > 180 && theLowestValuePositive < 90) {
			isCounterOnRight = true;
			warningCounterRight = 25;
		}
	}

	//W LEWO
	if (theLowestValueNegative != 500 && theLowestValuePositive != 500) {
		if (theLowestValueNegative < 60 && theLowestValuePositive > 190) {
			isCounterOnLeft = true;
			warningCounterLeft = 25;
		}
	}

	if (theLowestValueNegative != 500) {
		line(output, cv::Point(930, 710), cv::Point(930 - theLowestValueNegative, 710), cv::Scalar(0, 255, 175), 3, 8);
	}
	if (theLowestValuePositive != 500) {
		line(output, cv::Point(930, 710), cv::Point(930 + theLowestValuePositive, 710), cv::Scalar(0, 255, 175), 3, 8);
	}
	cv::circle(output, cv::Point(930, 710), 5, cv::Scalar(0, 0, 0), 3, -1);
	std::string distanceToBothLines = "(" + std::to_string(theLowestValueNegative) + ", " + std::to_string(theLowestValuePositive) + ")";
	cv::putText(output, distanceToBothLines, cv::Point(930 - 100, 710), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4, 8);

	if (warningCounterRight > 0) {
		cv::putText(output, "Zmiana pasa ruchu w prawo!", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 4, 8);
	}
	if (warningCounterLeft > 0) {
		cv::putText(output, "Zmiana pasa ruchu w lewo!", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 4, 8);
	}

	if (isCounterOnRight = true) {
		warningCounterRight--;
	}
	if (isCounterOnLeft = true) {
		warningCounterLeft--;
	}

	if (warningCounterRight == 0) {
		isCounterOnRight = false;
	}
	if (warningCounterLeft == 0) {
		isCounterOnLeft = false;
	}


	if (toRestartCounter > 0) {
		toRestartCounter--;
	}


}

void LaneAssistant::lineCoefficientsCalculation() {
	cv::Vec4i lineTempLeft = lines[numbersOfTheNearestLines[0]];
	l1_x = lineTempLeft[2];
	l1_y = lineTempLeft[3];
	l2_x = lineTempLeft[0];
	l2_y = lineTempLeft[1];

	cv::Vec4i lineTempRight = lines[numbersOfTheNearestLines[1]];
	r1_x = lineTempRight[0];
	r1_y = lineTempRight[1];
	r2_x = lineTempRight[2];
	r2_y = lineTempRight[3];

	//WSPÓ£CZYNNIKI PROSTYCH
	slopeLeft = (double)(l2_y - l1_y) / (l2_x - l1_x);
	constLeft = l2_y - l2_x * slopeLeft;

	slopeRight = (double)(r2_y - r1_y) / (r2_x - r1_x);
	constRight = r2_y - r2_x * slopeRight;
}

void LaneAssistant::lineVerificationBasedOnHistory() {
	//SPRAWDZENIE CZY OTRZYMANY WYNIK NIE RÓ¯NI SIÊ BARDZO OD POPRZEDNICH
	cv::Vec2d lineCoefficientsLeft;
	lineCoefficientsLeft = { slopeLeft, constLeft };

	if (lastCoefficientsLeft.size() < memorySizeOfLastLineCoefficients) {
		lastCoefficientsLeft.push_back(lineCoefficientsLeft);
	}
	else {
		double meanOfSlopes = 0;
		for (int i = 0; i < lastCoefficientsLeft.size(); i++) {
			meanOfSlopes += lastCoefficientsLeft[i][0];
		}
		meanOfSlopes = meanOfSlopes / lastCoefficientsLeft.size();

		if (abs((slopeLeft - meanOfSlopes) / meanOfSlopes) < 0.3) {
			lastCoefficientsLeft.erase(lastCoefficientsLeft.begin());
			lastCoefficientsLeft.push_back(lineCoefficientsLeft);
		}
		else {
			slopeLeft = lastCoefficientsLeft[lastCoefficientsLeft.size() - 1][0];
			constLeft = lastCoefficientsLeft[lastCoefficientsLeft.size() - 1][1];
		}
	}

	cv::Vec2d lineCoefficientsRight;
	lineCoefficientsRight = { slopeRight, constRight };

	if (lastCoefficientsRight.size() < memorySizeOfLastLineCoefficients) {
		lastCoefficientsRight.push_back(lineCoefficientsRight);
	}
	else {
		double meanOfSlopes = 0;
		for (int i = 0; i < lastCoefficientsRight.size(); i++) {
			meanOfSlopes += lastCoefficientsRight[i][0];
		}
		meanOfSlopes = meanOfSlopes / lastCoefficientsRight.size();
		if (abs((slopeRight - meanOfSlopes) / meanOfSlopes) < 0.3) {
			lastCoefficientsRight.erase(lastCoefficientsRight.begin());
			lastCoefficientsRight.push_back(lineCoefficientsRight);
		}
		else {
			slopeRight = lastCoefficientsRight[lastCoefficientsRight.size() - 1][0];
			constRight = lastCoefficientsRight[lastCoefficientsRight.size() - 1][1];
		}
	}
}

void LaneAssistant::memoryOfLastLineCoefficientsRefreshing() {
	//ZEROWANIE WEKTORA LINII JEŒLI TO NIE JEST PIERWSZE PRZEJŒCIE ALGORYTMU
	if (lastCoefficientsLeft.size() == memorySizeOfLastLineCoefficients && lastCoefficientsRight.size() == memorySizeOfLastLineCoefficients && toRestartCounter == 0) {
		if (isFirstLoop) {
			toRestartCounter = 60;
			isFirstLoop = false;
		}
		else {
			lastCoefficientsLeft.clear();
			lastCoefficientsRight.clear();
			toRestartCounter = 60;
		}
	}
}

void LaneAssistant::lineExtension() {
	//LINIA PROWADZ¥CA DO SAMEGO DO£U EKRANU
	l1_y = input.rows;
	l1_x = (l1_y - constLeft) / slopeLeft;
	r1_y = input.rows;
	r1_x = (r1_y - constRight) / slopeRight;
	
	//I DO PUNKTU PRZECIÊCIA OBYDWU LINII
	l2_x = (constLeft - constRight) / (slopeRight - slopeLeft);
	l2_y = slopeLeft * l2_x + constLeft;
	r2_x = (constLeft - constRight) / (slopeRight - slopeLeft);
	r2_y = slopeRight * r2_x + constRight;
}

void LaneAssistant::checkingWhiteSurroundingOfLines() {
	//SPRAWDZENIE CZY ZNALEZIONA LINIA MA W OKOLICY SIEBIE BIA£¥ LINIÊ
	int accumulatorLeft = 0;
	if (slopeLeft != 0) {
		for (int i = 830; i > 700; i -= 10) {
			for (int j = -10; j <= 10; j++) {
				if (white.at<uchar>(i, (i - constLeft) / slopeLeft + j) == 255) {
					accumulatorLeft++;
				}
			}
		}
	}
//	std::cout << accumulatorLeft << std::endl;
	if (accumulatorLeft > 20) {
		leftNearToWhite = true;
	}
	else {
		leftNearToWhite = false;
	}

	int accumulatorRight = 0;
	if (slopeRight != 0) {
		for (int i = 830; i > 700; i -= 10) {
			for (int j = -10; j <= 10; j++) {
				if (white.at<uchar>(i, (i - constRight) / slopeRight + j) == 255) {
					accumulatorRight++;
				}
			}
		}
	}
//	std::cout << accumulatorRight << std::endl;
	if (accumulatorRight > 20) {
		rightNearToWhite = true;
	}
	else {
		rightNearToWhite = false;
	}
}

void LaneAssistant::laneFieldMarking() {
	//MASKA DO WYŒWIETLENIA PASA RUCHU NA OBRAZIE WYJŒCIOWYM
	trackMask = cv::Mat(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));

	//JEŒLI ZNALEZIONO OBYDWIE LINIE TO OZNACZ PAS RUCHU
	if (leftToShow&&rightToShow) {

		//PUNKTY OKREŒLAJ¥CE PO£O¯ENIE ZNALEZIONEGO PASA RUCHU
		std::vector<cv::Point> trackMaskPoints;
		trackMaskPoints.push_back(cv::Point(l1_x, l1_y));
		trackMaskPoints.push_back(cv::Point(l2_x, l2_y));
		trackMaskPoints.push_back(cv::Point(r2_x, r2_y));
		trackMaskPoints.push_back(cv::Point(r1_x, r1_y));
		const cv::Point* elementPoints2[1] = { &trackMaskPoints[0] };
		int npt2 = (int)trackMaskPoints.size();

		//OKREŒLENIE NA MASCE WYZNACZONEGO PASA RUCHU
		if (leftNearToWhite&&rightNearToWhite) {
			cv::fillPoly(trackMask, elementPoints2, &npt2, 1, cv::Scalar(0, 255, 0), 8, 0);		//BLISKO BIA£EJ LINII Z OBYDWU STRON - ZIELONY KOLOR
		}
		else {
			cv::fillPoly(trackMask, elementPoints2, &npt2, 1, cv::Scalar(0, 255, 255), 8, 0);	//KTÓRAŒ Z LINII NIE JEST BLISKO BIA£EJ - ¯Ó£TY KOLOR
		}
	}

	//NA£O¯ENIE NA OBRAZ WYJŒCIOWY MASKI Z WYZNACZONYM PASEM RUCHU
	addWeighted(output, 0.7, trackMask, 0.3, 0, output);
}

void LaneAssistant::lineMarking() {
	//WYŒWIETL LINIE
	if (leftToShow&&leftNearToWhite) {
		cv::line(output, cv::Point(l1_x, l1_y), cv::Point(l2_x, l2_y), cv::Scalar(0, 255, 0), 3);
	}
	if (leftToShow && !leftNearToWhite) {
		cv::line(output, cv::Point(l1_x, l1_y), cv::Point(l2_x, l2_y), cv::Scalar(0, 255, 255), 3);
	}
	if (rightToShow&&rightNearToWhite) {
		cv::line(output, cv::Point(r1_x, r1_y), cv::Point(r2_x, r2_y), cv::Scalar(0, 255, 0), 3);
	}
	if (rightToShow && !rightNearToWhite) {
		cv::line(output, cv::Point(r1_x, r1_y), cv::Point(r2_x, r2_y), cv::Scalar(0, 255, 255), 3);
	}
}

void LaneAssistant::showFoundedLinesSelection() {
	//SPRAWDZENIE SELEKCJI LINII: 
	//LINIE ZBYT POZIOME ZOSTAJ¥ OZNACZONE NA CZERWONO (FINALNIE ICH SIÊ NIE BIERZE POD UWAGÊ) 
	//LINIE Z NACHYLENIEM DODATNIM OZNACZONE NA ZIELONO TO LINIE PRAWE
	//LINIE Z NACHYLENIEM UJEMNYM OZNACZONE NA NIEBIESKO TO LINIE LEWE
//	for (int i = 0; i < lines.size(); i++) {
//		cv::Vec4i l = lines[i];
//		double slope = (double)(l[3] - l[1]) / (l[2] - l[0]);
////		std::cout << slope << std::endl;
//		if (slope < 0.4&&slope>-0.4) {
//			cv::line(input, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 2);
//		}
//		else {
//			if (slope > 0) {
//				cv::line(input, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 255, 0), 2);
//			}
//			if (slope < 0) {
//				cv::line(input, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2);
//			}
//		}
//	}
}