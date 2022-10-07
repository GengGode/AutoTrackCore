#include "pch.h"
#include "AutoTrack.h"
#include "ErrorCode.h"
#include "capture/dxgi/Dxgi.h"
#include "capture/bitblt/Bitblt.h"
#include "utils/Utils.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace TianLi::Utils;

AutoTrack::AutoTrack()
{
	giHandle = 0;

	MapWorldOffset.x = MapWorldAbsOffset_X - WorldCenter_X;
	MapWorldOffset.y = MapWorldAbsOffset_Y - WorldCenter_Y;
	MapWorldScale = WorldScale;

	capture = new Bitblt();
	capture->init();

	wForAfter.append(this, &AutoTrack::clear_error_logs, 0);
	wForAfter.append(this, &AutoTrack::getGengshinImpactWnd, 101);
	wForAfter.append(this, &AutoTrack::getGengshinImpactRect, 102);
	wForAfter.append(this, &AutoTrack::getGengshinImpactScreen, 103);

	wPaimon.append(this, &AutoTrack::clear_error_logs, 0);
	wPaimon.append(this, &AutoTrack::getGengshinImpactWnd, 104);
	wPaimon.append(this, &AutoTrack::getGengshinImpactRect, 105);
	wPaimon.append(this, &AutoTrack::getGengshinImpactScreen, 106);
	wPaimon.append(this, &AutoTrack::getPaimonRefMat, 107);

	wMiniMap.append(this, &AutoTrack::clear_error_logs, 0);
	wMiniMap.append(this, &AutoTrack::getAutoTrackIsInit, 108);
	wMiniMap.append(this, &AutoTrack::getGengshinImpactWnd, 109);
	wMiniMap.append(this, &AutoTrack::getGengshinImpactRect, 110);
	wMiniMap.append(this, &AutoTrack::getGengshinImpactScreen, 111);
	wMiniMap.append(this, &AutoTrack::getMiniMapRefMat, 112);

	wAvatar.append(this, &AutoTrack::clear_error_logs, 0);
	wAvatar.append(this, &AutoTrack::getGengshinImpactWnd, 113);
	wAvatar.append(this, &AutoTrack::getGengshinImpactRect, 114);
	wAvatar.append(this, &AutoTrack::getGengshinImpactScreen, 115);
	wAvatar.append(this, &AutoTrack::getAvatarRefMat, 116);

	wRotating.append(this, &AutoTrack::clear_error_logs, 0);
	wRotating.append(this, &AutoTrack::getGengshinImpactWnd, 117);
	wRotating.append(this, &AutoTrack::getGengshinImpactRect, 118);
	wRotating.append(this, &AutoTrack::getGengshinImpactScreen, 119);
	wRotating.append(this, &AutoTrack::getMiniMapRefMat, 120);

	wStar.append(this, &AutoTrack::clear_error_logs, 0);
	wStar.append(this, &AutoTrack::getGengshinImpactWnd, 121);
	wStar.append(this, &AutoTrack::getGengshinImpactRect, 122);
	wStar.append(this, &AutoTrack::getGengshinImpactScreen, 123);
	wStar.append(this, &AutoTrack::getMiniMapRefMat, 124);

	wUID.append(this, &AutoTrack::clear_error_logs, 0);
	wUID.append(this, &AutoTrack::getGengshinImpactWnd, 125);
	wUID.append(this, &AutoTrack::getGengshinImpactRect, 126);
	wUID.append(this, &AutoTrack::getGengshinImpactScreen, 127);
	wUID.append(this, &AutoTrack::getUIDRefMat, 128);

}

AutoTrack::~AutoTrack(void)
{
	delete capture;
}

bool AutoTrack::init()
{
	if (!is_init_end)
	{
		res.install();

		_detectorAllMap = cv::xfeatures2d::SURF::create(minHessian);
		
		std::string xml_string(res.xmlPtr.ptr);
		cv::FileStorage fs(xml_string, cv::FileStorage::MEMORY | cv::FileStorage::READ);
		fs["keypoints"] >> _KeyPointAllMap;
		fs["descriptors"] >> _DataPointAllMap;

		is_init_end = true;
	}
	return is_init_end;
}

bool AutoTrack::uninit()
{
	if (is_init_end)
	{
		_detectorAllMap.release();
		_KeyPointAllMap.resize(0);
		_KeyPointAllMap.reserve(0);
		_DataPointAllMap.release();

		res.release();

		is_init_end = false;
	}
	return !is_init_end;
}

bool AutoTrack::SetUseBitbltCaptureMode()
{
	if (capture == nullptr)
	{
		capture = new Bitblt();
		return true;
	}
	if (capture->mode == Capture::Mode_Bitblt)
	{
		return true;
	}

	delete capture;
	capture = new Bitblt();

	return true;
}

bool AutoTrack::SetUseDx11CaptureMode()
{
	if (capture == nullptr)
	{
		capture = new Dxgi();
		return true;
	}

	if (capture->mode == Capture::Mode_DirectX)
	{
		return true;
	}

	delete capture;
	capture = new Dxgi();

	return true;
}


bool AutoTrack::SetHandle(long long int handle)
{
	if (handle == 0)
	{
		is_Auto_getHandle = true;
		return true;
	}
	else
	{
		is_Auto_getHandle = false;
		giHandle = (HWND)handle;
	}

	return IsWindow(giHandle);
}

bool AutoTrack::SetWorldCenter(double x, double y)
{
	UserWorldOrigin_X = x;
	UserWorldOrigin_Y = y;
	return true;
}

bool AutoTrack::SetWorldScale(double scale)
{
	UserWorldScale = scale;
	return true;
}

bool AutoTrack::startServe()
{
	return false;
}

bool AutoTrack::stopServe()
{
	return false;
}

bool AutoTrack::DebugCapture()
{
	if (giFrame.empty())
	{
		err = { 501,"����Ϊ��" };
		return false;
	}
	cv::Mat out_info_img = giFrame.clone();
	switch (capture->mode)
	{
	case Capture::Mode_Bitblt:
	{
		// ����paimon Rect
		cv::rectangle(out_info_img, genshin_screen.config.rect_paimon, cv::Scalar(0, 0, 255), 2);
		// ����miniMap Rect
		cv::rectangle(out_info_img, genshin_screen.config.rect_minimap, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = Area_Avatar_mayArea;
		Avatar.x += genshin_screen.config.rect_minimap.x;
		Avatar.y += genshin_screen.config.rect_minimap.y;

		// ����avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// ����UID Rect
		cv::rectangle(out_info_img, Area_UID_mayArea, cv::Scalar(0, 0, 255), 2);
		break;
	}
	case Capture::Mode_DirectX:
	{
		// ����paimon Rect
		cv::rectangle(out_info_img, Area_Paimon_mayArea, cv::Scalar(0, 0, 255), 2);
		// ����miniMap Rect
		cv::rectangle(out_info_img, Area_Minimap_mayArea, cv::Scalar(0, 0, 255), 2);
		cv::Rect Avatar = Area_Avatar_mayArea;
		Avatar.x += Area_Minimap_mayArea.x;
		Avatar.y += Area_Minimap_mayArea.y;

		// ����avatar Rect
		cv::rectangle(out_info_img, Avatar, cv::Scalar(0, 0, 255), 2);
		// ����UID Rect
		cv::rectangle(out_info_img, Area_UID_mayArea, cv::Scalar(0, 0, 255), 2);
	}
	}
	
	bool rel = cv::imwrite("Capture.png", out_info_img);

	if (!rel)
	{
		err = { 502,"���滭��ʧ�� " };
		return false;
	}
	err = 0;
	return true;
}

bool AutoTrack::GetTransform(double& x, double& y, double& a)
{
	double x2 = 0, y2 = 0, a2 = 0;
	if (!is_init_end)
	{
		init();//��ʼ��
	}

	/*
	�ֱ��ж��Ƿ�ɹ���ȡ������ǰһ��error_code����һ��error_code����
	�����±���������false����ʾʧ�ܣ���error_codeΪ0����ʾ�ɹ�����
	*/
	if (!GetPosition(x2, y2))
	{
		return false;
	}
	if (!GetDirection(a2))
	{
		return false;
	}
	x = x2;
	y = y2;
	a = a2;
	return true;
}

bool AutoTrack::GetTransformOfMap(double& x, double& y, double& a, int& mapId)
{
	double x2 = 0, y2 = 0, a2 = 0;
	int mapId2 = 0;
	if (!is_init_end)
	{
		init();//��ʼ��
	}

	/*
	�ֱ��ж��Ƿ�ɹ���ȡ������ǰһ��error_code����һ��error_code����
	�����±���������false����ʾʧ�ܣ���error_codeΪ0����ʾ�ɹ�����
	*/
	if (!GetPositionOfMap(x2, y2, mapId2))
	{
		return false;
	}
	if (!GetDirection(a2))
	{
		return false;
	}
	x = (float)x2;
	y = (float)y2;
	a = (float)a2;
	mapId = mapId2;
	return true;
}

bool AutoTrack::GetPosition(double& x, double& y)
{
	static cv::Mat img_scene(res.MapTemplate);
	static bool is_frist = true;
	if (is_frist)
	{
		cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);
		is_frist = false;
	}

	if (wForAfter.run() == false)
	{
		return false;
	}
	if (!is_init_end)
	{
		err = { 1, "û�г�ʼ��" };
		return false;
	}
	if (capture->mode == Capture::Mode_Bitblt)
	{
		
		if (getMiniMapRefMat_Bitblt()==false)
		{
			err = { 1000, "Bitbltģʽ�»�ȡ����ʱ��û��ʶ��paimon" };
			return false;
		}
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 1000, "��ȡ����ʱ��û��ʶ��paimon" };
			return false;
		}

		getMiniMapRefMat();
	}

	if (giMiniMapRef.empty())
	{
		err = { 5, "ԭ��С��ͼ����Ϊ��" };
		return false;
	}

	cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));
	cv::cvtColor(img_object, img_object, cv::COLOR_RGBA2RGB);


	isContinuity = false;
	isConveying = false;

	cv::Point2d* hisP = _TransformHistory;

	cv::Point2d pos;

	if (dis(hisP[2] - hisP[1]) < 1000)
	{
		if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
		{
			isContinuity = true;
		}
	}

	if (isContinuity)
	{
		if (isOnCity == false)
		{
			cv::Mat someMap = img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).clone();
			//cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
			cv::Mat minMap(img_object);


			cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
			std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
			cv::Mat& DataPointSomeMap = _DataPointSomeMap;
			std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
			cv::Mat& DataPointMiniMap = _DataPointMiniMap;

			detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
			detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
			detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);



			if (KeyPointMiniMap.size() == 0 || KeyPointSomeMap.size() <= 2)
			{
				isContinuity = false;
			}
			else
			{
				cv::Ptr<cv::DescriptorMatcher> matcher_on_city_not = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
				std::vector< std::vector<cv::DMatch> > KNN_m_on_city_not;

				matcher_on_city_not->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city_not, 2);

				std::vector<double> lisx;
				std::vector<double> lisy;
				double sumx = 0;
				double sumy = 0;

				calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city_not, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);

				if (min(lisx.size(), lisy.size()) <= 4)
				{
					//�п��ܴ��ڳ�����

					/***********************/
					//���´������е�ͼȡ����ɫ��Χ���ֵ�ͼ
					cv::Mat someMap = img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).clone();
					//img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);
					//Mat minMap(img_object);

					resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
					//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

					detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
					detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
					//detectorSomeMap->detectAndCompute(minMap, noArray(), KeyPointMinMap, DataPointMinMap);
					if (KeyPointSomeMap.size() == 0 || KeyPointMiniMap.size() == 0)
					{
						isContinuity = false;
					}
					else
					{
						cv::Ptr<cv::DescriptorMatcher> matcher_on_city_maybe = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
						std::vector< std::vector<cv::DMatch> > KNN_m_on_city_maybe;

						matcher_on_city_maybe->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city_maybe, 2);
						//std::vector<double> lisx;
						//std::vector<double> lisy;
						lisx.clear();
						lisy.clear();
						//double sumx = 0;
						//double sumy = 0;
						sumx = 0;
						sumy = 0;

						calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city_maybe, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);

						if (min(lisx.size(), lisy.size()) <= 4)
						{
							isContinuity = false;
						}
						else
						{
							if (min(lisx.size(), lisy.size()) >= 10)
							{
								isOnCity = true;
							}
							else
							{
								isOnCity = false;
							}

							cv::Point2d pos_on_city_maybe = SPC(lisx, sumx, lisy, sumy);

							double x_on_city_maybe = (pos_on_city_maybe.x - someMap.cols / 2.0) / 2.0;
							double y_on_city_maybe = (pos_on_city_maybe.y - someMap.rows / 2.0) / 2.0;

							pos = cv::Point2d(x_on_city_maybe + hisP[2].x, y_on_city_maybe + hisP[2].y);
						}

					}
				}
				else
				{
					isOnCity = false;

					cv::Point2d p_on_city_not = SPC(lisx, sumx, lisy, sumy);

					pos = cv::Point2d(p_on_city_not.x + hisP[2].x - someSizeR, p_on_city_not.y + hisP[2].y - someSizeR);
				}
			}
		}
		else
		{
			//�ڳ�����
				/***********************/
				//���´������е�ͼȡ����ɫ��Χ���ֵ�ͼ
			cv::Mat someMap = img_scene(cv::Rect(cvCeil(hisP[2].x - someSizeR), cvCeil(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).clone();
			cv::Mat minMap(img_object);


			cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = _detectorSomeMap;
			std::vector<cv::KeyPoint>& KeyPointSomeMap = _KeyPointSomeMap;
			cv::Mat& DataPointSomeMap = _DataPointSomeMap;
			std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
			cv::Mat& DataPointMiniMap = _DataPointMiniMap;


			resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
			//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

			detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
			detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
			detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

			if (KeyPointSomeMap.size() != 0 && KeyPointMiniMap.size() != 0)
			{
				cv::Ptr<cv::DescriptorMatcher> matcher_on_city = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
				std::vector< std::vector<cv::DMatch> > KNN_m_on_city;

				matcher_on_city->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_m_on_city, 2);
				std::vector<double> lisx;
				std::vector<double> lisy;
				double sumx = 0;
				double sumy = 0;

				calc_good_matches(someMap, KeyPointSomeMap, img_object, KeyPointMiniMap, KNN_m_on_city, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);

				if (max(lisx.size(), lisy.size()) > 4)
				{
					if (min(lisx.size(), lisy.size()) >= 10)
					{
						isOnCity = true;
					}
					else
					{
						isOnCity = false;
					}

					cv::Point2d pos_on_city = SPC(lisx, sumx, lisy, sumy);

					double x_on_city = (pos_on_city.x - someMap.cols / 2.0) / 2.0;
					double y_on_city = (pos_on_city.y - someMap.rows / 2.0) / 2.0;

					pos = cv::Point2d(x_on_city + hisP[2].x, y_on_city + hisP[2].y);
				}
				else
				{
					isContinuity = false;
				}//if (max(lisx.size(), lisy.size()) > 4)
			}
			else
			{
				isContinuity = false;
			}//if (KeyPointSomeMap.size() != 0 && KeyPointMinMap.size() != 0)
		}
	}
	else
	{
		isConveying = true;
	}

	if (!isContinuity)
	{
		cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = _detectorAllMap;
		std::vector<cv::KeyPoint>& KeyPointAllMap = _KeyPointAllMap;
		cv::Mat& DataPointAllMap = _DataPointAllMap;
		std::vector<cv::KeyPoint>& KeyPointMiniMap = _KeyPointMiniMap;
		cv::Mat& DataPointMiniMap = _DataPointMiniMap;

		detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);

		if (KeyPointMiniMap.size() == 0)
		{
			err = { 4, "С��ͼδ�ܼ������ʶ��������" };//δ��ƥ�䵽������
			return false;
		}
		else
		{
			cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
			std::vector< std::vector<cv::DMatch> > KNN_m;

			matcher->knnMatch(DataPointMiniMap, DataPointAllMap, KNN_m, 2);

			std::vector<double> lisx;
			std::vector<double> lisy;
			double sumx = 0;
			double sumy = 0;

			calc_good_matches(img_scene, KeyPointAllMap, img_object, KeyPointMiniMap, KNN_m, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);



			if (lisx.size() == 0 || lisy.size() == 0)
			{
				err = { 42, "δ��ƥ�䵽������" };
				return false;
			}
			else
			{
				pos = SPC(lisx, sumx, lisy, sumy);
				isConveying = true;
			}
		}
	}

	cv::Point2d filt_pos;

#define USE_Filt
#ifdef USE_Filt
	if (isConveying || !isContinuity)
	{
		filt_pos = posFilter.reinitfilterting(pos);
		//isConveying = false;
	}
	else
	{
		filt_pos = posFilter.filterting(pos);
	}
#else
	filt_pos = pos;
#endif // USE_Filt

	hisP[0] = hisP[1];
	hisP[1] = hisP[2];
	hisP[2] = filt_pos;

	cv::Point2d abs_pos = TransferTianLiAxes(filt_pos * MapAbsScale, MapWorldOffset, MapWorldScale);

	cv::Point2d user_pos = TransferUserAxes(abs_pos, UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);

	x = (float)(user_pos.x);
	y = (float)(user_pos.y);

	err = 0;
	return true;
}

bool AutoTrack::GetPositionOfMap(double& x, double& y, int& mapId)
{
	mapId = 0;
	cv::Point2d pos_tr;
	bool res_pos = GetPosition(x, y);
	if (res_pos != true)
	{
		return false;
	}

	pos_tr = TransferUserAxes_Tr(cv::Point2d(x, y), UserWorldOrigin_X, UserWorldOrigin_Y, UserWorldScale);
	pos_tr = TransferTianLiAxes_Tr(pos_tr, MapWorldOffset, MapWorldScale);
	pos_tr = pos_tr / MapAbsScale;

	//cv::Size size_DiXiaCengYan(1250, 1016);
	//cv::Size size_YuanXiaGong(2400, 2401); 5544 
	//cv::Size size_YuanXiaGong_Un(800, 450);
	cv::Rect rect_DiXiaCengYan(0, 0, 1250, 1016);
	cv::Rect rect_YuanXiaGong(0, 5543, 2400, 2401);
	{
		double _x = pos_tr.x;
		double _y = pos_tr.y;
		// Ԩ�¹�
		if (_x > 0 && _x <= 0 + 2400 && _y > 5543 && _y <= 5543 + 2401)
		{
			mapId = 1;
		}
		// ���²���
		if (_x > 0 && _x <= 0 + 1250 && _y > 0 && _y <= 0 + 1016)
		{
			mapId = 2;
		}

		switch (mapId)
		{
		case 0:
		{
			break;
		}
		case 1:
		{
			_x = _x - 0;
			_y = _y - 5543;
			cv::Point2d pos = TransferTianLiAxes(cv::Point2d(_x, _y), cv::Point2d(0, 0), MapWorldScale);
			pos = TransferUserAxes(pos, 0, 0, 1);
			x = pos.x;
			y = pos.y;
			break;
		}
		case 2:
		{
			_x = _x - 0;
			_y = _y - 0;
			cv::Point2d pos = TransferTianLiAxes(cv::Point2d(_x, _y), cv::Point2d(0, 0), MapWorldScale);
			pos = TransferUserAxes(pos, 0, 0, 1);
			x = pos.x;
			y = pos.y;
			break;
		}
		default:
			break;
		}
	}
	return true;
}

bool AutoTrack::GetDirection(double& a)
{
	if (wAvatar.run() == false)
	{
		return false;
	}
	

	if (capture->mode == Capture::Mode_Bitblt)
	{
		getMiniMapRefMat_Bitblt();
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 2000 ,"��ȡ��ɫ����ʱ��û��ʶ��Paimon" };
			return false;
		}

		getMiniMapRefMat();
	}

	if (giMiniMapRef.empty())
	{
		err = { 5,"ԭ��С��ͼ����Ϊ��" };
		return false;
	}

	getAvatarRefMat();

	if (giAvatarRef.empty())
	{
		err = { 11,"ԭ���ɫС��ͷ����Ϊ��" };
		return false;
	}

	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	cv::Mat gray0;
	cv::Mat gray1;
	cv::Mat gray2;

	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

	cv::Mat and12;
	cv::bitwise_and(gray1, gray2, and12, gray0);
	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
	cv::Canny(and12, and12, 20, 3 * 20, 3);
	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(and12, and12, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //������Ӿ��μ���
	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() != 3)
	{
		err = { 9,"��ȡС��ͷ����������" };
		return false;
	}

	for (int i = 0; i < 3; i++)
	{
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
		AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
	}

	AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
	AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	}
	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
	}
	if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
	}

	AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
	KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

	a = Line2Angle(KeyLine);

	err = 0;
	return true;
}

bool AutoTrack::GetRotation(double& a)
{
	if (wForAfter.run() == false)
	{
		return false;
	}

	if (capture->mode == Capture::Mode_Bitblt)
	{
		getMiniMapRefMat_Bitblt();
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 2000 ,"��ȡ�ӽǳ���ʱ��û��ʶ��Paimon" };
			return false;
		}

		getMiniMapRefMat();
	}

	//cv::Mat img_scene(res.MapTemplate);
	cv::Mat img_object(giMiniMapRef(cv::Rect(40, 40, giMiniMapRef.cols - 80, giMiniMapRef.rows - 80)));

	if (img_object.channels() != 4)
	{
		err = { 401,"��ȡ�ӽǳ���ʱ��ԭ��С��ͼ����û��ȡ��͸��ͨ��" };
		return false;
	}

	if (capture->mode == Capture::Mode_DirectX)
	{
		err = { 402,"DXģʽ�£�ԭ��С��ͼ�����޷�ȡ��͸��ͨ��" };
		return false;
	}

	std::vector<cv::Mat>scr_channels;

	split(img_object, scr_channels);

	cv::Mat Alpha = scr_channels[3];

	Alpha = 255.0 - Alpha;

	Alpha = Alpha * 2;

	cv::threshold(Alpha, Alpha, 150, 0, cv::THRESH_TOZERO_INV);
	cv::threshold(Alpha, Alpha, 50, 0, cv::THRESH_TOZERO);
	cv::threshold(Alpha, Alpha, 50, 255, cv::THRESH_BINARY);

	cv::circle(Alpha, cv::Point(Alpha.cols / 2, Alpha.rows / 2), static_cast<int>(min(Alpha.cols / 2, Alpha.rows / 2) * 1.21), cv::Scalar(0, 0, 0), static_cast<int>(min(Alpha.cols / 2, Alpha.rows / 2) * 0.42));
	cv::circle(Alpha, cv::Point(Alpha.cols / 2, Alpha.rows / 2), static_cast<int>(min(Alpha.cols / 2, Alpha.rows / 2) * 0.3), cv::Scalar(0, 0, 0), -1);


	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::dilate(Alpha, Alpha, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::erode(Alpha, Alpha, erode_element);

	erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::erode(Alpha, Alpha, erode_element);
	dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	cv::dilate(Alpha, Alpha, dilate_element);


	//����ڰ�ͼ
	//���ݰ׿鲿�ּ����ӽ���������
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(Alpha, contours, hierarcy, 0, 1);

	std::vector<cv::Rect> boundRect(contours.size());  //������Ӿ��μ���

	if (contours.size() == 0)
	{
		err = { 402 ,"��ȡ�ӽǳ���ʱ��û����ȡ���ӽ���������" };
		return false;
	}

	cv::Point p;
	int maxBlack = 0;
	int maxId = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > maxBlack)
		{
			maxBlack = static_cast<int>(contours[i].size());
			maxId = i;
		}
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));

	}

	p = cv::Point(boundRect[maxId].x + boundRect[maxId].width / 2, boundRect[maxId].y + boundRect[maxId].height / 2);



	// double res;
#ifdef _DEBUG
	circle(Alpha, p, 3, cv::Scalar(255, 0, 0));
	line(Alpha, p, cv::Point(img_object.cols / 2, img_object.rows / 2), cv::Scalar(0, 255, 0));
	cv::imshow("Img", Alpha);
#endif
	p = p - cv::Point(img_object.cols / 2, img_object.rows / 2);

	a = Line2Angle(p);

	return true;
}

bool AutoTrack::GetStar(double& x, double& y, bool& isEnd)
{
	static bool isNotSee = false;
	static vector<cv::Point2d> pos;
	static int seeId = 0;

	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	double scale = 1.3;

	if (isNotSee)
	{
		if (isOnCity)
		{
			scale = 0.8667;
		}
		x = pos[seeId].x * scale;
		y = pos[seeId].y * scale;
		seeId++;
		if (seeId == pos.size())
		{
			isEnd = true;
			isNotSee = false;
		}
		else
		{
			isEnd = false;
			isNotSee = true;
		}
		return true;
	}
	else
	{
		pos.clear();
		seeId = 0;

		if (wStar.run() == false)
		{
			return false;
		}

		getPaimonRefMat();
		
		if (capture->mode == Capture::Mode_Bitblt)
		{
			getMiniMapRefMat_Bitblt();
		}
		else
		{
			cv::Rect paimon_rect;
			if (!check_paimon(paimon_rect))
			{
				err = { 1000, "��ȡ����ʱ��û��ʶ��paimon" };
				return false;
			}

			getMiniMapRefMat();
		}

		cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
			giStarRef, cv::COLOR_RGBA2GRAY);


		matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isStarVisible = false;
		}
		else
		{
			isLoopMatch = true;
			isStarVisible = true;
			pos.push_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
		}

		while (isLoopMatch)
		{
			giStarRef(cv::Rect(maxLoc.x, maxLoc.y, res.StarTemplate.cols, res.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
			matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
			minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
			//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif

			if (maxVal < 0.66)
			{
				isLoopMatch = false;
			}
			else
			{
				pos.push_back(cv::Point2d(maxLoc) -
					cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
					cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
			}

			MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
		}


		if (isStarVisible == true)
		{
			if (isOnCity)
			{
				scale = 0.8667;
			}
			x = pos[seeId].x * scale;
			y = pos[seeId].y * scale;
			seeId++;
			if (seeId == pos.size())
			{
				isEnd = true;
				isNotSee = false;
			}
			else
			{
				isEnd = false;
				isNotSee = true;
			}
			return true;
		}
		err = 601;
		return false;
	}

	return false;
}

bool AutoTrack::GetStarJson(char* jsonBuff)
{
	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	vector<cv::Point2d> pos;
	double scale = 1.3;


	if (wStar.run() == false)
	{
		return false;
	}

	getPaimonRefMat();
	
	if (capture->mode == Capture::Mode_Bitblt)
	{
		getMiniMapRefMat_Bitblt();
	}
	else
	{
		cv::Rect paimon_rect;
		if (!check_paimon(paimon_rect))
		{
			err = { 1000, "��ȡ����ʱ��û��ʶ��paimon" };
			return false;
		}

		getMiniMapRefMat();
	}
	//һ��bug δ����Ϸ���ȿ�Ӧ�ã�����Ϸʱ����
	cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
		giStarRef, cv::COLOR_RGBA2GRAY);


	matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
	if (maxVal < 0.66)
	{
		isStarVisible = false;
	}
	else
	{
		isLoopMatch = true;
		isStarVisible = true;
		pos.push_back(cv::Point2d(maxLoc) -
			cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
			cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
	}

	while (isLoopMatch)
	{
		giStarRef(cv::Rect(maxLoc.x, maxLoc.y, res.StarTemplate.cols, res.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
		matchTemplate(res.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		//cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isLoopMatch = false;
		}
		else
		{
			pos.push_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(res.StarTemplate.cols / 2, res.StarTemplate.rows / 2));
		}

		MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
	}

	if (isOnCity)
	{
		scale = 0.8667;
	}

	if (isStarVisible == true)
	{

		sprintf_s(jsonBuff, 1024, "{\"n\": %d ,\"list\":[", static_cast<int>(pos.size()));//[123,12],[123,53]]}")
		for (int i = 0; i < pos.size(); i++)
		{
			char buff[99];
			if (i == 0)
			{
				sprintf_s(buff, 99, "[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			else
			{
				sprintf_s(buff, 99, ",[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			strncat_s(jsonBuff, 1024, buff, 99);
		}
		strncat_s(jsonBuff, 1024, "]}", 3);
		err = 0;
		return true;
	}
	sprintf_s(jsonBuff, 99, "{\"n\": 0 ,\"list\":[]}");
	err = 0;
	return true;
}

bool AutoTrack::GetUID(int& uid)
{
	if (wForAfter.run() == false)
	{
		err = 300;
		return false;
	}
	if (getUIDRefMat() == false)
	{
		return false;
	}

	std::vector<cv::Mat> channels;

	split(giUIDRef, channels);

	if (capture->mode == Capture::Mode_DirectX)
	{
		cv::cvtColor(giUIDRef, giUIDRef, cv::COLOR_RGBA2GRAY);
	}
	else
	{
		giUIDRef = channels[3];
	}

	int _uid = 0;
	int _NumBit[9] = { 0 };

	int bitCount = 1;
	cv::Mat matchTmp;
	cv::Mat Roi;
	cv::Mat checkUID = res.UID;

#ifdef _DEBUG
	//if (checkUID.rows > Roi.rows)
	//{
	//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
	//}
#endif
	giUIDRef.copyTo(Roi);

	if (checkUID.rows > Roi.rows)
	{
		cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
	}

	cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	//Ѱ�����ƥ��λ��
	cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
	if (maxVal > 0.75)
	{
		int x_uid_ = maxLoc.x + checkUID.cols + 7;
		int y_uid_ = 0;
		double tmplis[10] = { 0 };
		int tmplisx[10] = { 0 };
		for (int p = 8; p >= 0; p--)
		{
			_NumBit[p] = 0;
			for (int i = 0; i < 10; i++)
			{
				cv::Rect r(x_uid_, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
				if (x_uid_ + r.width > giUIDRef.cols)
				{
					r = cv::Rect(giUIDRef.cols - res.UIDnumber[i].cols - 2, y_uid_, res.UIDnumber[i].cols + 2, giUIDRef.rows);
				}

				cv::Mat numCheckUID = res.UIDnumber[i];
				Roi = giUIDRef(r);

				cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

				double minVali, maxVali;
				cv::Point minLoci, maxLoci;
				//Ѱ�����ƥ��λ��
				cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

				tmplis[i] = maxVali;
				tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
				if (maxVali > 0.91)
				{
					_NumBit[p] = i;
					x_uid_ = x_uid_ + maxLoci.x + numCheckUID.cols - 1;
					break;
				}
				if (i == 10 - 1)
				{
					_NumBit[p] = getMaxID(tmplis, 10);
					x_uid_ = x_uid_ + tmplisx[_NumBit[p]];
				}
			}
		}
	}
	_uid = 0;
	for (int i = 0; i < 9; i++)
	{
		_uid += _NumBit[i] * bitCount;
		bitCount = bitCount * 10;
	}
	if (_uid == 0)
	{
		err = { 8,"δ����UID�����⵽��ЧUID" };
		return false;
	}
	uid = _uid;
	err = 0;
	return true;

}
bool AutoTrack::GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
{
	// warning C4100 path uid x y a
	UNREFERENCED_PARAMETER(path);
	UNREFERENCED_PARAMETER(uid);
	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);
	UNREFERENCED_PARAMETER(a);

	return false;
}
bool AutoTrack::GetInfoLoadVideo(char* path, char* pathOutFile)
{
	UNREFERENCED_PARAMETER(path);
	UNREFERENCED_PARAMETER(pathOutFile);
	return true;
}

int AutoTrack::GetLastError()
{
#ifdef _DEBUG
	std::cout << err;
#endif
	return err;
}

bool AutoTrack::getAutoTrackIsInit()
{
	if (is_init_end)
	{
		err = { 1,"δ��ʼ��" };
		return false;
	}
	else
	{
		return true;
	}
}

const char* AutoTrack::GetLastErrorStr()
{
	return nullptr;
}

int AutoTrack::GetLastErrMsg(char* msg_buff, int buff_size)
{
	std::string msg = err.getLastErrorMsg();
	if (msg.size() > buff_size)
	{
		return -1;
	}
	else
	{
		strcpy_s(msg_buff, msg.size(), msg.c_str());
		return 0;
	}
}

bool AutoTrack::getGengshinImpactWnd()
{
	if (is_Auto_getHandle)
	{
		LPCWSTR giWindowName = { L"ԭ��" };
		/* ��ԭ�񴰿ڵĲ��� */
		giWindowName = L"ԭ��";
		giHandle = FindWindowW(L"UnityWndClass", giWindowName);
		if (giHandle == NULL)
		{
			giWindowName = L"Genshin Impact";
			giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* ƥ�����ƣ�ԭ�� */
		}
		if (giHandle == NULL)
		{
			giWindowName = L"??";
			giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* ƥ�����ƣ�?? */
		}
		if (giHandle == NULL)
		{
			giWindowName = L"\u539F\u795E";
			giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* ƥ�����ƣ�ԭ�� */
		}
		if (giHandle == NULL)
		{
			giWindowName = L"\uC6D0\uC2E0";
			giHandle = FindWindowW(L"UnityWndClass", giWindowName); /* ƥ�����ƣ�?? */
		}

		if (giHandle == NULL)
		{
			err = 10; //��Ч�����ָ�������ָ�򴰿ڲ�����
			return false;
		}
	}
	else
	{
		if (IsWindow(giHandle))
		{
			return true;
		}
		else
		{
			err = 10; //��Ч�����ָ�������ָ�򴰿ڲ�����
			return false;
		}
	}

	capture->setHandle(giHandle);

	return (giHandle != NULL ? true : false);
}

bool AutoTrack::getGengshinImpactRect()
{
	if (!GetWindowRect(giHandle, &giRect))
	{
		err = 12;//���ھ��ʧЧ
		return false;
	}
	if (!GetClientRect(giHandle, &giClientRect))
	{
		err = 12;//���ھ��ʧЧ
		return false;
	}

	//��ȡ��Ļ���ű���
	getGengshinImpactScale();

	giClientSize.width = (int)(screen_scale * (giClientRect.right - giClientRect.left));
	giClientSize.height = (int)(screen_scale * (giClientRect.bottom - giClientRect.top));
	int x = giClientSize.width;
	int y = giClientSize.height;
	
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{
		genshin_handle.size_frame = cv::Size(1920, 1080);
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//���ͣ��Կ�Ϊ����

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//��giFrame���ŵ�1920*1080�ı���
		fx = x / f;
		// ��ͼƬ����
		genshin_handle.size_frame = cv::Size(static_cast<int>(fx), 1080);

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//���ͣ��Ը�Ϊ����

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//��giFrame���ŵ�1920*1080�ı���
		fy = y / f;
		// ��ͼƬ����
		genshin_handle.size_frame = cv::Size(1920, static_cast<int>(fy));
	}
	x = genshin_handle.size_frame.width;
	y = genshin_handle.size_frame.height;
	// ���ɿ���������������
	int paimon_mayArea_left = 0;
	int paimon_mayArea_top = 0;
	int paimon_mayArea_width = static_cast<int>(x * 0.10);
	int paimon_mayArea_height = static_cast<int>(y * 0.10);
	// ���ɿ���������
	//cv::Rect Area_Paimon_mayArea(
	genshin_handle.rect_paimon_maybe=cv::Rect(
		paimon_mayArea_left,
		paimon_mayArea_top,
		paimon_mayArea_width,
		paimon_mayArea_height);
	//genshin_handle.rect_paimon_maybe = Area_Paimon_mayArea;

	// С��ͼ�궨����������������
	int miniMap_Cailb_mayArea_left = static_cast<int>(x * 0.10);
	int miniMap_Cailb_mayArea_top = 0;
	int miniMap_Cailb_mayArea_width = static_cast<int>(x * 0.10);
	int miniMap_Cailb_mayArea_height = static_cast<int>(y * 0.10);
	// С��ͼ�궨����������
	cv::Rect Area_MiniMap_Cailb_mayArea(
		miniMap_Cailb_mayArea_left,
		miniMap_Cailb_mayArea_top,
		miniMap_Cailb_mayArea_width,
		miniMap_Cailb_mayArea_height);
	genshin_handle.rect_minimap_cailb_maybe = Area_MiniMap_Cailb_mayArea;

	// С��ͼ����������������
	int miniMap_mayArea_left = 0;
	int miniMap_mayArea_top = 0;
	int miniMap_mayArea_width = static_cast<int>(x * 0.18);
	int miniMap_mayArea_height = static_cast<int>(y * 0.22);
	// С��ͼ����������
	cv::Rect Area_MiniMap_mayArea(
		miniMap_mayArea_left,
		miniMap_mayArea_top,
		miniMap_mayArea_width,
		miniMap_mayArea_height);
	genshin_handle.rect_minimap_maybe = Area_MiniMap_mayArea;

	// UID����������������
	int UID_mayArea_left = static_cast<int>(x * 0.88);
	int UID_mayArea_top = static_cast<int>(y * 0.97);
	int UID_mayArea_width = x - UID_mayArea_left;
	int UID_mayArea_height = y - UID_mayArea_top;
	// UID����������
	//cv::Rect Area_UID_mayArea(
	genshin_handle.rect_uid_maybe =cv::Rect(
		UID_mayArea_left,
		UID_mayArea_top,
		UID_mayArea_width,
		UID_mayArea_height);
	//genshin_handle.rect_uid_maybe = Area_UID_mayArea;
	
	int UID_Rect_x = cvCeil(x - x * (1.0 - 0.865));
	int UID_Rect_y = cvCeil(y - 1080.0 * (1.0 - 0.9755));
	int UID_Rect_w = cvCeil(1920 * 0.11);
	int UID_Rect_h = cvCeil(1920 * 0.0938 * 0.11);


	genshin_handle.rect_uid = cv::Rect(UID_Rect_x, UID_Rect_y, UID_Rect_w, UID_Rect_h);

	// ����ѻ�ȡ��Ʒ����������������
	int leftGetItems_mayArea_left = static_cast<int>(x * 0.570);
	int leftGetItems_mayArea_top = static_cast<int>(y * 0.250);
	int leftGetItems_mayArea_width = static_cast<int>(x * 0.225);
	int leftGetItems_mayArea_height = static_cast<int>(y * 0.500);
	// ����ѻ�ȡ��Ʒ����������
	cv::Rect Area_LeftGetItems_mayArea(
		leftGetItems_mayArea_left,
		leftGetItems_mayArea_top,
		leftGetItems_mayArea_width,
		leftGetItems_mayArea_height);
	genshin_handle.rect_left_give_items_maybe = Area_LeftGetItems_mayArea;

	// �Ҳ�ɼ�ȡ��Ʒ����������������
	int rightGetItems_mayArea_left = static_cast<int>(x * 0.050);
	int rightGetItems_mayArea_top = static_cast<int>(y * 0.460);
	int rightGetItems_mayArea_width = static_cast<int>(x * 0.160);
	int rightGetItems_mayArea_height = static_cast<int>(y * 0.480);
	// �Ҳ�ɼ�ȡ��Ʒ����������
	cv::Rect Area_RightGetItems_mayArea(
		rightGetItems_mayArea_left,
		rightGetItems_mayArea_top,
		rightGetItems_mayArea_width,
		rightGetItems_mayArea_height);
	genshin_handle.rect_right_pick_items_maybe = Area_RightGetItems_mayArea;

	return true;
}

bool AutoTrack::getGengshinImpactScale()
{
#ifdef _DEBUG
	//std::cout << "-> getGengshinImpactScale()" << std::endl;
#endif
	HWND hWnd = GetDesktopWindow();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	// ��ȡ�������߼������߶�
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	//int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

	// ��ȡ��������������߶�
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	//int cyPhysical = dm.dmPelsHeight;

	double horzScale = ((double)cxPhysical / (double)cxLogical);
	screen_scale = horzScale;

	return true;
}

bool AutoTrack::getGengshinImpactScreen()
{
	cv::Mat Frame;
	if (capture->capture(Frame))
	{
		giFrame = Frame;
		if (capture->mode == Capture::Mode_DirectX)
		{
			int w = giClientRect.right - giClientRect.left;
			int h = giClientRect.bottom - giClientRect.top;
			int x = 0;
			int y = giFrame.rows - h;
			giFrame = giFrame(cv::Rect(x, y, w, h));
		}

		cv::resize(giFrame, genshin_screen.img_screen, genshin_handle.size_frame);
		
		giFrame = genshin_screen.img_screen;
		
		genshin_screen.rect_client = cv::Rect(giRect.left, giRect.top, giClientRect.right - giClientRect.left, giClientRect.bottom - giClientRect.top);

		// ��ȡmaybe����
		genshin_screen.img_paimon_maybe = giFrame(genshin_handle.rect_paimon_maybe);
		genshin_screen.img_minimap_cailb_maybe = giFrame(genshin_handle.rect_minimap_cailb_maybe);
		genshin_screen.img_minimap_maybe = giFrame(genshin_handle.rect_minimap_maybe);
		genshin_screen.img_uid_maybe = giFrame(genshin_handle.rect_uid_maybe);
		genshin_screen.img_left_give_items_maybe = giFrame(genshin_handle.rect_left_give_items_maybe);
		genshin_screen.img_right_pick_items_maybe = giFrame(genshin_handle.rect_right_pick_items_maybe);

		genshin_screen.config.rect_paimon_maybe = genshin_handle.rect_paimon_maybe;
		genshin_screen.config.rect_minimap_cailb_maybe = genshin_handle.rect_minimap_cailb_maybe;
		genshin_screen.config.rect_minimap_maybe = genshin_handle.rect_minimap_maybe;


		genshin_screen.img_uid = giFrame(genshin_handle.rect_uid);
		
		return true;
	}
	else
	{
		err = { 433, "��ͼʧ��" };
		return false;
	}
}
bool AutoTrack::getPaimonRefMat()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//��������������
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//���ͣ��Կ�Ϊ����

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//��giFrame���ŵ�1920*1080�ı���
		fx = x / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//���ͣ��Ը�Ϊ����

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//��giFrame���ŵ�1920*1080�ı���
		fy = y / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//����

	}

	int Paimon_Rect_x = cvCeil(1920 * 0.0135);
	int Paimon_Rect_y = cvCeil(1920 * 0.006075);
	int Paimon_Rect_w = cvCeil(1920 * 0.035);
	int Paimon_Rect_h = cvCeil(1920 * 0.0406);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		Paimon_Rect_x = cvCeil(1920 * 0.0135 + 72);
		Paimon_Rect_y = cvCeil(1920 * 0.006075);
		Paimon_Rect_w = cvCeil(1920 * 0.035);
		Paimon_Rect_h = cvCeil(1920 * 0.0406);
	}

	// ���ɿ���������������
	int paimon_mayArea_left = 0;
	int paimon_mayArea_top = 0;
	int paimon_mayArea_width = static_cast<int>(x * 0.10);
	int paimon_mayArea_height = static_cast<int>(y * 0.10);
	// ���ɿ���������
	Area_Paimon_mayArea = cv::Rect(
		paimon_mayArea_left,
		paimon_mayArea_top,
		paimon_mayArea_width,
		paimon_mayArea_height);

	//giPaimonRef = giFrame(cv::Rect(Paimon_Rect_x, Paimon_Rect_y, Paimon_Rect_w, Paimon_Rect_h));
	giPaimonRef = giFrame(Area_Paimon_mayArea);

#ifdef _DEBUG
	cv::namedWindow("Paimon", cv::WINDOW_FREERATIO);
	cv::imshow("Paimon", giPaimonRef);
	cv::waitKey(1);
	//std::cout << "Show Paimon" << std::endl;
#endif
	return true;
}

bool AutoTrack::getMiniMapRefMat()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//��������������
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//���ͣ��Կ�Ϊ����

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//��giFrame���ŵ�1920*1080�ı���
		fx = x / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//���ͣ��Ը�Ϊ����

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//��giFrame���ŵ�1920*1080�ı���
		fy = y / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//����

	}

	int MiniMap_Rect_x = cvRound(1920 * 0.033);
	int MiniMap_Rect_y = cvRound(1920 * 0.01);
	int MiniMap_Rect_w = cvRound(1920 * 0.11);
	int MiniMap_Rect_h = cvRound(1920 * 0.11);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		MiniMap_Rect_x = cvRound(1920 * 0.033 + 72);
		MiniMap_Rect_y = cvRound(1920 * 0.01);
		MiniMap_Rect_w = cvRound(1920 * 0.11);
		MiniMap_Rect_h = cvRound(1920 * 0.11);
	}
	Area_Minimap_mayArea = cv::Rect(
		MiniMap_Rect_x,
		MiniMap_Rect_y,
		MiniMap_Rect_w,
		MiniMap_Rect_h);

	giMiniMapRef = giFrame(Area_Minimap_mayArea);

#ifdef _DEBUG
		cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
		cv::imshow("MiniMap", giMiniMapRef);
	cv::waitKey(1);
	//std::cout << "Show MiniMap" << std::endl;
#endif
	return true;
}

bool AutoTrack::getMiniMapRefMat_Bitblt()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//��������������
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//���ͣ��Կ�Ϊ����

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//��giFrame���ŵ�1920*1080�ı���
		fx = x / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//���ͣ��Ը�Ϊ����

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//��giFrame���ŵ�1920*1080�ı���
		fy = y / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//����

	}

	int MiniMap_Rect_x = cvRound(1920 * 0.033);
	int MiniMap_Rect_y = cvRound(1920 * 0.01);
	int MiniMap_Rect_w = cvRound(1920 * 0.11);
	int MiniMap_Rect_h = cvRound(1920 * 0.11);

	if (giFrame.cols == 3440 && giFrame.rows == 1440)
	{
		cv::resize(giFrame, giFrame, cv::Size(2560, 1080));
	}

	if (giFrame.cols == 2560 && giFrame.rows == 1080)
	{
		MiniMap_Rect_x = cvRound(1920 * 0.033 + 72);
		MiniMap_Rect_y = cvRound(1920 * 0.01);
		MiniMap_Rect_w = cvRound(1920 * 0.11);
		MiniMap_Rect_h = cvRound(1920 * 0.11);
	}
	genshin_minimap.rect_minimap = cv::Rect(
		MiniMap_Rect_x,
		MiniMap_Rect_y,
		MiniMap_Rect_w,
		MiniMap_Rect_h);
	genshin_minimap.img_minimap = giFrame(genshin_minimap.rect_minimap);
	giMiniMapRef = giFrame(genshin_minimap.rect_minimap);


	// ������� -> ���С��ͼ�궨 -> ����С��ͼ����

	if (TianLi::Match::check_paimon(genshin_screen, genshin_paimon) == false)
	{
		err = { 40001,"Bitbltģʽ�¼������ʧ��" };
		return false;
	}
	if (genshin_paimon.is_visial == false)
	{
		err = { 50001,"Bitbltģʽ��δ�ܼ�⵽����" };
		return false;
	}

	genshin_screen.config.rect_paimon = genshin_paimon.rect_paimon;
	genshin_screen.config.is_handle_mode = genshin_paimon.is_handle_mode;

	if (TianLi::Match::match_minimap_cailb(genshin_screen, genshin_minimap_cailb) == false)
	{
		err = { 40002,"Bitbltģʽ�¼��С��ͼ�궨ʧ��" };
		return false;
	}
	if (genshin_minimap_cailb.is_visial == false)
	{
		err = { 50002,"Bitbltģʽ��δ�ܼ�⵽С��ͼ�궨" };
		return false;
	}

	genshin_screen.config.rect_minimap_cailb = genshin_minimap_cailb.rect_minimap_cailb;

	if (TianLi::Match::splite_minimap(genshin_screen, genshin_minimap) == false)
	{
		err = { 40003, "Bitbltģʽ�¼���С��ͼ����ʧ��" };
		return false;
	}

	genshin_screen.config.rect_minimap = genshin_minimap.rect_minimap;
	giMiniMapRef = genshin_minimap.img_minimap;

#ifdef _DEBUG
	cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
	cv::imshow("MiniMap", giMiniMapRef);
	cv::waitKey(1);
#endif
	return true;
}

bool AutoTrack::getUIDRefMat()
{
	int& x = giFrame.cols, & y = giFrame.rows;
	double f = 1, fx = 1, fy = 1;

	if (static_cast<double>(x) / static_cast<double>(y) == 16.0 / 9.0)
	{

		//��������������
		if (x != 1920 && y != 1080)
		{
			cv::resize(giFrame, giFrame, cv::Size(1920, 1080));
		}
	}
	else if (static_cast<double>(x) / static_cast<double>(y) > 16.0 / 9.0)
	{

		//���ͣ��Կ�Ϊ����

		// x = (y * 16) / 9;
		f = y / 1080.0;
		//��giFrame���ŵ�1920*1080�ı���
		fx = x / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(static_cast<int>(fx), 1080));

	}
	else if (static_cast<double>(x) / static_cast<double>(y) < 16.0 / 9.0)
	{

		//���ͣ��Ը�Ϊ����

		// x = (y * 16) / 9;
		f = x / 1920.0;
		//��giFrame���ŵ�1920*1080�ı���
		fy = y / f;
		// ��ͼƬ����
		cv::resize(giFrame, giFrame, cv::Size(1920, static_cast<int>(fy)));
	}
	else
	{
		//����

	}

	int UID_Rect_x = cvCeil(giFrame.cols - giFrame.cols * (1.0 - 0.865));
	int UID_Rect_y = cvCeil(giFrame.rows - 1080.0 * (1.0 - 0.9755));
	int UID_Rect_w = cvCeil(1920 * 0.11);
	int UID_Rect_h = cvCeil(1920 * 0.0938 * 0.11);

	Area_UID_mayArea = cv::Rect(
		UID_Rect_x,
		UID_Rect_y,
		UID_Rect_w,
		UID_Rect_h);

	giUIDRef = giFrame(Area_UID_mayArea);

#ifdef _DEBUG
	cv::namedWindow("UID", cv::WINDOW_FREERATIO);
	cv::imshow("UID", giUIDRef);
	cv::waitKey(1);
	//std::cout << "Show UID" << std::endl;
#endif
	return true;
}

bool AutoTrack::getAvatarRefMat()
{
	if (giMiniMapRef.empty())
	{
		err = 21;
		return false;
	}
	int Avatar_Rect_x = cvRound(giMiniMapRef.cols * 0.4);
	int Avatar_Rect_y = cvRound(giMiniMapRef.rows * 0.4);
	int Avatar_Rect_w = cvRound(giMiniMapRef.cols * 0.2);
	int Avatar_Rect_h = cvRound(giMiniMapRef.rows * 0.2);

	Area_Avatar_mayArea = cv::Rect(
		Avatar_Rect_x,
		Avatar_Rect_y,
		Avatar_Rect_w,
		Avatar_Rect_h);

	giAvatarRef = giMiniMapRef(Area_Avatar_mayArea);

#ifdef _DEBUG
	cv::namedWindow("Avatar", cv::WINDOW_FREERATIO);
	cv::imshow("Avatar", giAvatarRef);
	cv::waitKey(1);
#endif
	return true;
}

bool AutoTrack::clear_error_logs()
{
	err = 0;
	return true;
}

bool AutoTrack::check_paimon(cv::Rect& paimon_rect)
{
	static cv::Mat paimon_template;
	static std::vector<cv::Mat> split_paimon_template;
	static bool is_first = true;
	if (is_first)
	{
		res.PaimonTemplate.copyTo(paimon_template);
		cv::split(paimon_template, split_paimon_template);
		is_first = false;
	}

	getPaimonRefMat();

	std::vector<cv::Mat>  split_paimon;
	cv::split(giPaimonRef, split_paimon);

	cv::Mat template_result;
	cv::Mat object = split_paimon[3];

	const double check_match_paimon_params_dx = 0.60;
	static double check_match_paimon_param = check_match_paimon_params;
	if (capture->mode == Capture::Mode_DirectX)
	{
		cv::cvtColor(giPaimonRef, object, cv::COLOR_RGBA2GRAY);
		check_match_paimon_param = check_match_paimon_params_dx;
	}
	else
	{
		check_match_paimon_param = check_match_paimon_params;
	}

	cv::matchTemplate(object, split_paimon_template[3], template_result, cv::TM_CCOEFF_NORMED);

	double paimon_match_minVal, paimon_match_maxVal;
	cv::Point paimon_match_minLoc, paimon_match_maxLoc;
	cv::minMaxLoc(template_result, &paimon_match_minVal, &paimon_match_maxVal, &paimon_match_minLoc, &paimon_match_maxLoc);

#ifdef _DEBUG
	cv::namedWindow("paimon match result", cv::WINDOW_FREERATIO);
	cv::imshow("paimon match result", template_result);
#endif

	if (paimon_match_maxVal < check_match_paimon_param || paimon_match_maxVal == 1)
	{
		err = 6;//δ��ƥ�䵽����
		return false;
	}
	paimon_rect = cv::Rect(paimon_match_maxLoc, paimon_template.size());
	return true;
}

