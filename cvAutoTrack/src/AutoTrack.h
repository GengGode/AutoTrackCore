#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
//#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <Windows.h>

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"


// �����ǲ�������
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);


	bool init();
	bool uninit();
	int  GetGpuCount();
	bool SetGpuDevice(int deviceId = 0);
	
	bool SetUseBitbltCaptureMode();
	bool SetUseDx11CaptureMode();
	
	bool SetHandle(long long int handle = 0);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	bool GetTransform(float &x, float &y, float &a);
	bool GetTransformOfMap(float& x, float& y, float& a, int& mapId);
	bool GetPosition(double &x, double &y);
	bool GetPositionOfMap(double& x, double& y, int& mapId);
	bool GetDirection(double &a);
	bool GetRotation(double &a);
	//��ȡ���ֵ���ͫ����,isEndΪ����Ϊ��ǰ�����е����һ����ͫ
	bool GetStar(double &x, double &y, bool &isEnd);
	//��ȡ���ֵ���ͫ���꣬��json�ַ�����ʽ
	bool GetStarJson(char *jsonBuff);
	bool GetUID(int &uid);
	bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
	bool GetInfoLoadVideo(char * path, char * pathOutFile);
	int GetLastError();
	const char* GetLastErrorStr();
	bool startServe();
	bool stopServe();

private:
	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();
	FlowWork wPaimon;
	FlowWork wMiniMap;
	FlowWork wAvatar;
	FlowWork wRotating;
	FlowWork wStar;

	FlowWork wUID;

	FlowWork wForAfter;

	Kalmanfilter posFilter;
private:
	int error_code = 0;

private:
	bool is_init_end = false;

private:

#define MAP_3_0

#ifdef MAP_1_7_1
	int minHessian = 400;
	float ratio_thresh = 0.66f;
	float mapScale = 1.68f;//1.3;
	int someSizeR = 106;
	float MatchMatScale = 2.0;
#endif

#ifdef MAP_1_7_2
	int minHessian = 400;
	float ratio_thresh = 0.66f;
	float mapScale = 1.68f;//1.3;
	int someSizeR = 106;
	float MatchMatScale = 2.0;


	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5452;
	double WorldCenter_Y = 9432;
	double WorldScale = 1.0;

	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 8866;
	double MapWorldAbsOrigin_Y = 11457;

	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2;

	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025);
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	double MapWorldScale = 1.0;

#endif

#ifdef MAP_2_0
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;


	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352;
	double WorldCenter_Y = 9432;
	double WorldScale = 1.0;

	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 1703;// 1690;
	double MapWorldAbsOrigin_Y = 1924;// 1911;

	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557;

	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025);
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	double MapWorldScale = 1.0;

#endif

#ifdef MAP_2_1
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

	//�û�����ӳ���ϵ����
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	// ������������ ������������ϵ��
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 5352; //Abs
	double WorldCenter_Y = 9432; //Abs
	double WorldScale = 1.0; //Abs

	//��Ծ��Կռ�ԭ������
	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 1703; //from diff Image
	double MapWorldAbsOrigin_Y = 1718; //from diff Image

	//��Ծ��Կռ�����ϵ��
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//������ɶ�ˣ��ǵ������Ƶ�
	//09.07 �Ǹ��̶�ֵ�����ñ�
	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025); // ? forget
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	//Ҳ����
	//2022.07.13 ����û������
	double MapWorldScale = 1.0;

#endif

#ifdef MAP_3_0
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

	//�û�����ӳ���ϵ����
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;

	// ������������ ������������ϵ��
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 7400; //Abs 2871   * MapAbsScale = 7392.2870
	double WorldCenter_Y = 9432; //Abs 3015.5 * MapAbsScale = 7710.6335
	double WorldScale = 1.0; //Abs

	//��Ծ��Կռ�ԭ������
	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOrigin_X = 7.7130; // 7400 - 7392.2870
	double MapWorldAbsOrigin_Y = 1721.3665; // 9432 - 7710.6335

	//��Ծ��Կռ�����ϵ��
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//������ɶ�ˣ��ǵ������Ƶ�
	//09.07 �Ǹ��̶�ֵ�����ñ�
	cv::Point2d MapWorldOffset = cv::Point2d(3412, 2025); // ? forget
	//double MapWorldOffset_X = 3412;
	//double MapWorldOffset_Y = 2025;

	//Ҳ����
	//2022.07.13 ����û������
	double MapWorldScale = 1.0;

#endif

	// ???
	double screen_scale = 1;


private:
#ifdef oldValue
	//cv::Ptr<cv::xfeatures2d::SURF>
	void* _detectorAllMap = nullptr;
	//cv::Ptr<cv::xfeatures2d::SURF>
	void* _detectorSomeMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointAllMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointSomeMap = nullptr;
	//std::vector<cv::KeyPoint>
	void* _KeyPointMiniMap = nullptr;
	//cv::Mat
	void* _DataPointAllMap = nullptr;
	//cv::Mat
	void* _DataPointSomeMap = nullptr;
	//cv::Mat
	void* _DataPointMiniMap = nullptr;
#else
	//
	cv::Ptr<cv::xfeatures2d::SURF> _detectorAllMap;
	//
	cv::Ptr<cv::xfeatures2d::SURF> _detectorSomeMap ;
	//
	std::vector<cv::KeyPoint> _KeyPointAllMap;
	//std::vector<cv::KeyPoint>
	std::vector<cv::KeyPoint> _KeyPointSomeMap;
	//std::vector<cv::KeyPoint>
	std::vector<cv::KeyPoint> _KeyPointMiniMap;
	//cv::Mat
	cv::Mat _DataPointAllMap;
	//cv::Mat
	cv::Mat _DataPointSomeMap;
	//cv::Mat
	cv::Mat _DataPointMiniMap;
#endif

private:
	bool isOnCity = false;
	bool isContinuity = false;
	bool isConveying = false;
	cv::Point2d _TransformHistory[3];
	bool is_Auto_getHandle = true;
	bool isStarVisible = false;

private:
	double check_match_paimon_params = 0.9;
private:
	// GPU�豸����
	int gpuDeviceNumber = 0;
	// ʹ��GPU�豸id
	int gpuDeviceId = -1;
	
	// ���洦���ֱ�ģʽ
	bool isHandleGameMode = false;

private:
	HWND giHandle;
	RECT giRect;
	RECT giClientRect;
	cv::Size giClientSize;
	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;
	cv::Mat giStarRef;
	
private:
	bool getAutoTrackIsInit();
	bool getGengshinImpactWnd();
	bool getGengshinImpactRect();
	bool getGengshinImpactScale();
	bool getGengshinImpactScreen();

	bool getPaimonRefMat();
	bool getMiniMapRefMat();
	bool getUIDRefMat();
	bool getAvatarRefMat();

private:
	bool clear_error_logs();
private:
	bool check_paimon(cv::Rect &paimon_rect);
	//bool check_minimap(cv::Rect minimap_rect);	
	//bool check_uid(cv::Rect uid_rect);
	//bool check_avatar(cv::Rect avatar_rect);
	//bool check_star(cv::Rect star_rect);
};


inline void draw_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches)
{
	cv::Mat img_matches, imgmap, imgminmap;
	drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

inline void calc_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double mapScale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy)
{
#ifdef _DEBUG
	std::vector<cv::DMatch> good_matches;
#endif
	for (size_t i = 0; i < KNN_m.size(); i++)
	{
		if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
		{
#ifdef _DEBUG
			good_matches.push_back(KNN_m[i][0]);
#endif
			if (KNN_m[i][0].queryIdx >= keypoint_object.size())
			{
				continue;
			}
			lisx.push_back(((img_object.cols / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.x) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.x));
			lisy.push_back(((img_object.rows / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.y) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.y));
			sumx += lisx.back();
			sumy += lisy.back();
		}
	}
#ifdef _DEBUG
	draw_good_matches(img_scene, keypoint_scene, img_object, keypoint_object, good_matches);
#endif
}

