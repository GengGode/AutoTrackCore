#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
//#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <Windows.h>

#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"
#include "capture/Capture.h"
#include "resources/Resources.h"
#include "match/match_minimap.h"

// �����ǲ�������
class AutoTrack {
public:
	AutoTrack(void);
	~AutoTrack(void);


	bool init();
	bool uninit();
	
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
	int GetLastErrMsg(char* msg_buff, int buff_size);
	bool startServe();
	bool stopServe();

	bool DebugCapture();

private:
	Resources& res = Resources::getInstance();
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
	int minHessian = 400;
	double ratio_thresh = 0.66;
	double mapScale = 1.3;//1.3;
	int someSizeR = 106;
	double MatchMatScale = 2.0;

	//�û�����ӳ���ϵ����
	double UserWorldOrigin_X = 0;
	double UserWorldOrigin_Y = 0;
	double UserWorldScale = 1.0;
	
#define MAP_3_1


#ifdef MAP_3_0
	// ������������ ������������ϵ��
	//World Center on AbsAllMap Coor
	double WorldCenter_X = 7400; //Abs 2871   * MapAbsScale = 7392.2870
	double WorldCenter_Y = 9432; //Abs 3015.5 * MapAbsScale = 7710.6335
	double WorldScale = 1.0; //Abs

	//��Ծ��Կռ�ԭ������
	//Map Origin Point on AbsAllMap Coor.
	double MapWorldAbsOffset_X = 7.7130; // 7400 - 7392.2870
	double MapWorldAbsOffset_Y = 1721.3665; // 9432 - 7710.6335

	//��Ծ��Կռ�����ϵ��
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%
	
	//2021-09-07 �Ǹ��̶�ֵ�����ñ�
	//2022-09-26 ���캯���г�ʼ��
	cv::Point2d MapWorldOffset; 
#endif
	
#ifdef MAP_3_1
	// ������������ ��������ϵԭ����ʵ�ʻ�����λ��
	// World Center on AbsAllMap Coor
	double WorldCenter_X = 13544; //Abs 4480   * MapAbsScale = 11455.36
	double WorldCenter_Y = 11480; //Abs 3015.5 * MapAbsScale = 7710.6335
	// ������������ϵ��
	double WorldScale = 1.0; //Abs
	
	// ��ͼ���ĵ��ʵ������λ��
	double MapPixelCenter_X = 4480;
	double MapPixelCenter_Y = 3015.5;

	// ��ͼ���ĵ�ת�������Կռ��е�����
	// Map Origin Point on AbsAllMap Coor.
	// MapWorldAbsCenter = MapPixelCenter * MapAbsScale;
	double MapWorldAbsCenter_X = 11455.36;
	double MapWorldAbsCenter_Y = 7710.6335;

	//��Ծ��Կռ�ԭ��ƫ��
	//Map Origin Point on AbsAllMap Coor.
	// MapWorldAbsOrigin = WorldCenter - MapWorldOffset
	double MapWorldAbsOffset_X = 2088.64; // 13544 - 11455.36
	double MapWorldAbsOffset_Y = 3769.3665; // 11480 - 7710.6335

	//��Ծ��Կռ�����ϵ��
	//Map and AbsAllMap Scale Value, Map * MapAbsScale = AbsAllMap.
	double MapAbsScale = 2.557; //from diff Image 67.40%

	//2021-09-07 �Ǹ��̶�ֵ�����ñ�
	//2022-09-26 ���캯���г�ʼ��
	// MapWorldOffset = MapWorldAbsOffset - WorldCenter
	cv::Point2d MapWorldOffset = cv::Point2d(2096.083, 3769.3665);
#endif
	//2022-07-13 ����û������
	double MapWorldScale = 1.0;

	// ???
	double screen_scale = 1;


private:
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
	
	Capture* capture = nullptr;

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
	cv::Rect Area_Paimon_mayArea;
	cv::Rect Area_Minimap_mayArea;
	cv::Rect Area_UID_mayArea;
	cv::Rect Area_Avatar_mayArea;

	GenshinHandle genshin_handle;
	GenshinScreen genshin_screen;
	
private:
	bool getAutoTrackIsInit();
	bool getGengshinImpactWnd();
	bool getGengshinImpactRect();
	bool getGengshinImpactScale();
	bool getGengshinImpactScreen();

	bool getPaimonRefMat();
	bool getMiniMapRefMat();
	bool getMiniMapRefMat_Bitblt();
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
