#pragma once
#include <opencv2\opencv.hpp>

//二维卡尔曼滤波类
class Kalmanfilter
{
	int stateNum = 4;
	int measureNum = 2;

	cv::KalmanFilter KF;
	cv::Mat state; /* (phi, delta_phi) */
	cv::Mat processNoise;
	cv::Mat measurement;

public:
	Kalmanfilter();
	//~Kalmanfilter();

public:
	cv::Point2d filterting(cv::Point2d p);
	cv::Point2d reinitfilterting(cv::Point2d p);

};



