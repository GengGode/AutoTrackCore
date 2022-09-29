#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

struct XmlPtr
{
	char* ptr;
	int size;
};

//ͼƬ��Դ ������
class LoadGiMatchResource
{
public:
	LoadGiMatchResource(void);
	~LoadGiMatchResource(void);

public:
	cv::Mat PaimonTemplate;
	cv::Mat StarTemplate;
	cv::Mat MapTemplate;
	cv::Mat UID;
	cv::Mat UIDnumber[10];
	XmlPtr xmlPtr;
	
	void install();
	void release();

private:
	void StarRGBA2A();
	void UIDnumberRGBA2A();
};

double dis(cv::Point2d p);
cv::Point2d SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);
int getMaxID(double lis[], int len); 
int getMinID(double lis[], int len);

std::vector<cv::Point2f> Vector2UnitVector(std::vector<cv::Point2f> pLis);
double Line2Angle(cv::Point2f p);
cv::Point2d TransferTianLiAxes(cv::Point2d pos, cv::Point2d origin, double scale);
cv::Point2d TransferUserAxes(cv::Point2d pos, double x, double y, double scale);
cv::Point2d TransferTianLiAxes_Tr(cv::Point2d pos, cv::Point2d origin, double scale);
cv::Point2d TransferUserAxes_Tr(cv::Point2d pos, double x, double y, double scale);

