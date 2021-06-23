#include "AutoTrack.h"

AutoTrack::AutoTrack()
{
	_detectorAllMap = new cv::Ptr<cv::xfeatures2d::SURF>;
	_detectorSomeMap = new cv::Ptr<cv::xfeatures2d::SURF>;
	_KeyPointAllMap = new std::vector<cv::KeyPoint>;
	_KeyPointSomeMap = new std::vector<cv::KeyPoint>;
	_KeyPointMiniMap = new std::vector<cv::KeyPoint>;
	_DataPointAllMap = new cv::Mat;
	_DataPointSomeMap = new cv::Mat;
	_DataPointMiniMap = new cv::Mat;
}

AutoTrack::~AutoTrack(void)
{
	delete _detectorAllMap;
	delete _detectorSomeMap;
	delete _KeyPointAllMap;
	delete _KeyPointSomeMap;
	delete _KeyPointMiniMap;
	delete _DataPointAllMap;
	delete _DataPointSomeMap;
	delete _DataPointMiniMap;
}

bool AutoTrack::init()
{
	if (!is_init_end)
	{
		cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*) _detectorAllMap;
		std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
		cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;

		detectorAllMap = cv::xfeatures2d::SURF::create(minHessian);
		detectorAllMap->detectAndCompute(giMatchResource.MapTemplate, cv::noArray(), KeyPointAllMap, DataPointAllMap);

		is_init_end = true;
	}
	return is_init_end;
}

bool AutoTrack::uninit()
{
	if (is_init_end)
	{
		delete _detectorAllMap;
		_detectorAllMap = nullptr;
		delete _KeyPointAllMap;
		_KeyPointAllMap = nullptr;
		delete _DataPointAllMap;
		_DataPointAllMap = nullptr;

		is_init_end = false;
	}
	return !is_init_end;
}

bool AutoTrack::GetTransform(float & x, float & y, float & a)
{
	if (!is_init_end)
	{
		error_code = 1;//未初始化
		return false;
	}
	// 判断原神窗口不存在直接返回false，不对参数做任何修改
	if (getGengshinImpactWnd())
	{
		getGengshinImpactRect();
		getGengshinImpactScreen();


		if (!giFrame.empty())
		{
			getPaimonRefMat();

			cv::Mat paimonTemplate;

			cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

			cv::Mat tmp;

#ifdef _DEBUG
#define Mode1
#ifdef Mode1
			giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));
#endif // Mode1

#ifdef Mode2
			cv::Ptr<cv::xfeatures2d::SURF> detectorPaimon = cv::xfeatures2d::SURF::create(minHessian);
			std::vector<cv::KeyPoint> KeyPointPaimonTemplate, KeyPointPaimonRef;
			cv::Mat DataPointPaimonTemplate, DataPointPaimonRef;

			detectorPaimon->detectAndCompute(giPaimonRef, cv::noArray(), KeyPointPaimonRef, DataPointPaimonRef);
			detectorPaimon->detectAndCompute(paimonTemplate, cv::noArray(), KeyPointPaimonTemplate, DataPointPaimonTemplate);
			cv::Ptr<cv::DescriptorMatcher> matcherPaimonTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
			std::vector< std::vector<cv::DMatch> > KNN_PaimonmTmp;
			std::vector<cv::DMatch> good_matchesPaimonTmp;

			for (size_t i = 0; i < KNN_PaimonmTmp.size(); i++)
			{
				if (KNN_PaimonmTmp[i][0].distance < ratio_thresh * KNN_PaimonmTmp[i][1].distance)
				{
					good_matchesPaimonTmp.push_back(KNN_PaimonmTmp[i][0]);
				}
			}

			cv::Mat img_matchesA, imgmapA, imgminmapA;
			drawKeypoints(giPaimonRef, KeyPointPaimonRef, imgmapA, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawKeypoints(paimonTemplate, KeyPointPaimonTemplate, imgminmapA, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			drawMatches(paimonTemplate, KeyPointPaimonTemplate, giPaimonRef, KeyPointPaimonRef, good_matchesPaimonTmp, img_matchesA, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

			cv::imshow("asda", img_matchesA);

			if (good_matchesPaimonTmp.size() < 7)
			{
				error_code = 6;//未能匹配到派蒙
				return false;
			}
#endif // Mode2

#endif

#ifdef _DEBUG
			cv::namedWindow("test", cv::WINDOW_FREERATIO);
			cv::imshow("test", giPaimonRef);
#endif

#ifdef Mode1

			cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

			double minVal, maxVal;
			cv::Point minLoc, maxLoc;
			cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);

#ifdef _DEBUG
			cv::namedWindow("test2", cv::WINDOW_FREERATIO);
			cv::imshow("test2", tmp);
			std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
#endif

			if (maxVal < 0.36 || maxVal == 1)
			{
				error_code = 6;//未能匹配到派蒙
				return false;
			}
#endif

			getMiniMapRefMat();

			cv::Mat img_scene(giMatchResource.MapTemplate);
			cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

			cv::cvtColor(img_scene, img_scene, CV_RGBA2RGB);

			if (img_object.empty())
			{
				error_code = 5;//原神小地图区域为空或者区域长宽小于60px
				return false;
			}

			isContinuity = false;

			cv::Point2f *hisP = _TransformHistory;

			cv::Point2f pos;

			if ((dis(hisP[1] - hisP[0]) + dis(hisP[2] - hisP[1])) < 2000)
			{
				if (hisP[2].x > someSizeR && hisP[2].x < img_scene.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < img_scene.rows - someSizeR)
				{
					isContinuity = true;
					if (isContinuity)
					{
						cv::Mat someMap(img_scene(cv::Rect(cvRound(hisP[2].x - someSizeR), cvRound(hisP[2].y - someSizeR), cvRound(someSizeR * 2), cvRound(someSizeR * 2))));
						cv::Mat minMap(img_object);

						//resize(someMap, someMap, Size(), MatchMatScale, MatchMatScale, 1);
						//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

						cv::Ptr<cv::xfeatures2d::SURF>& detectorSomeMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorSomeMap;
						std::vector<cv::KeyPoint>& KeyPointSomeMap = *(std::vector<cv::KeyPoint>*)_KeyPointSomeMap;
						cv::Mat& DataPointSomeMap = *(cv::Mat*)_DataPointSomeMap;
						std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
						cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

						detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
						detectorSomeMap->detectAndCompute(someMap, cv::noArray(), KeyPointSomeMap, DataPointSomeMap);
						detectorSomeMap->detectAndCompute(minMap, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
						cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
						std::vector< std::vector<cv::DMatch> > KNN_mTmp;
						std::vector<cv::DMatch> good_matchesTmp;
						matcherTmp->knnMatch(DataPointMiniMap, DataPointSomeMap, KNN_mTmp, 2);
						std::vector<double> lisx;
						std::vector<double> lisy;
						double sumx = 0;
						double sumy = 0;
						for (size_t i = 0; i < KNN_mTmp.size(); i++)
						{
							if (KNN_mTmp[i][0].distance < ratio_thresh * KNN_mTmp[i][1].distance)
							{
								good_matchesTmp.push_back(KNN_mTmp[i][0]);
								try
								{
									// 这里有个bug回卡进来，进入副本或者切换放大招时偶尔触发
									lisx.push_back(((minMap.cols / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.x)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.x));
									lisy.push_back(((minMap.rows / 2 - KeyPointMiniMap[KNN_mTmp[i][0].queryIdx].pt.y)*mapScale + KeyPointSomeMap[KNN_mTmp[i][0].trainIdx].pt.y));

								}
								catch (...)
								{
									error_code = 7;//特征点数组访问越界，是个bug
									return false;
								}
								sumx += lisx.back();
								sumy += lisy.back();
							}
						}

#ifdef _DEBUG
						cv::Mat img_matches, imgmap, imgminmap;
						drawKeypoints(someMap, KeyPointSomeMap, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawKeypoints(img_object, KeyPointMiniMap, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
						drawMatches(img_object, KeyPointMiniMap, someMap, KeyPointSomeMap, good_matchesTmp, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
#endif

						if (lisx.size() <= 4 || lisy.size() <= 4)
						{
							isContinuity = false;
						}
						else
						{
							double meanx = sumx / lisx.size(); //均值
							double meany = sumy / lisy.size(); //均值
							cv::Point2f p = SPC(lisx, sumx, lisy, sumy);

							float x = (float)meanx;
							float y = (float)meany;

							x = p.x;
							y = p.y;

							pos = cv::Point2f(x + hisP[2].x - someSizeR, y + hisP[2].y - someSizeR);
						}
					}
				}
			}
			if (!isContinuity)
			{
				cv::Ptr<cv::xfeatures2d::SURF>& detectorAllMap = *(cv::Ptr<cv::xfeatures2d::SURF>*)_detectorAllMap;
				std::vector<cv::KeyPoint>& KeyPointAllMap = *(std::vector<cv::KeyPoint>*)_KeyPointAllMap;
				cv::Mat& DataPointAllMap = *(cv::Mat*)_DataPointAllMap;
				std::vector<cv::KeyPoint>& KeyPointMiniMap = *(std::vector<cv::KeyPoint>*)_KeyPointMiniMap;
				cv::Mat& DataPointMiniMap = *(cv::Mat*)_DataPointMiniMap;

				detectorAllMap->detectAndCompute(img_object, cv::noArray(), KeyPointMiniMap, DataPointMiniMap);
				cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
				std::vector< std::vector<cv::DMatch> > KNN_m;
				//std::vector<DMatch> good_matches;
				matcher->knnMatch(DataPointMiniMap, DataPointAllMap, KNN_m, 2);

				std::vector<double> lisx;
				std::vector<double> lisy;
				double sumx = 0;
				double sumy = 0;
				for (size_t i = 0; i < KNN_m.size(); i++)
				{
					if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
					{
						//good_matches.push_back(KNN_m[i][0]);
						lisx.push_back(((img_object.cols / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.x)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.x));
						lisy.push_back(((img_object.rows / 2 - KeyPointMiniMap[KNN_m[i][0].queryIdx].pt.y)*mapScale + KeyPointAllMap[KNN_m[i][0].trainIdx].pt.y));
						sumx += lisx.back();
						sumy += lisy.back();
					}
				}
				if (lisx.size() == 0 || lisy.size() == 0)
				{
					error_code = 4;//未能匹配到特征点
					return false;
				}
				else
				{
					pos = SPC(lisx, sumx, lisy, sumy);
				}
			}
			hisP[0] = hisP[1];
			hisP[1] = hisP[2];
			hisP[2] = pos;

			/******************************/

			x = (float)(pos.x);
			y = (float)(pos.y);

			error_code = 0;
			return true;
		}
		else
		{
			error_code = 3;//窗口画面为空
			return false;
		}
	}
	else
	{
		error_code = 2;//未能找到原神窗口句柄
		return false;
	}
}

bool AutoTrack::GetUID(int &uid)
{
	// 判断原神窗口不存在直接返回false，不对参数做任何修改
	if (getGengshinImpactWnd())
	{
		getGengshinImpactRect();
		getGengshinImpactScreen();

		if (!giFrame.empty())
		{
			getUIDRefMat();

			std::vector<cv::Mat> channels;

			split(giUIDRef, channels);
			giUIDRef = channels[3];

			int _uid = 0;
			int _NumBit[9] = { 0 };

			int bitCount = 1;
			cv::Mat matchTmp;
			cv::Mat Roi(giUIDRef);
			cv::Mat checkUID = giMatchResource.UID;
#ifdef _DEBUG
			//if (checkUID.rows > Roi.rows)
			//{
			//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
			//}
#endif
			cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

			double minVal, maxVal;
			cv::Point minLoc, maxLoc;
			//寻找最佳匹配位置
			cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
			if (maxVal > 0.75)
			{
				int x = maxLoc.x + checkUID.cols + 7;
				int y = 0;
				double tmplis[10] = { 0 };
				int tmplisx[10] = { 0 };
				for (int p = 8; p >= 0; p--)
				{
					_NumBit[p] = 0;
					for (int i = 0; i < 10; i++)
					{
						cv::Rect r(x, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
						if (x + r.width > giUIDRef.cols)
						{
							r = cv::Rect(giUIDRef.cols - giMatchResource.UIDnumber[i].cols - 2, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);
						}

						cv::Mat numCheckUID = giMatchResource.UIDnumber[i];
						Roi = giUIDRef(r);

						cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

						double minVali, maxVali;
						cv::Point minLoci, maxLoci;
						//寻找最佳匹配位置
						cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

						tmplis[i] = maxVali;
						tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
						if (maxVali > 0.91)
						{
							_NumBit[p] = i;
							x = x + maxLoci.x + numCheckUID.cols - 1;
							break;
						}
						if (i == 10 - 1)
						{
							_NumBit[p] = getMaxID(tmplis, 10);
							x = x + tmplisx[_NumBit[p]];
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
				error_code = 8;//未能在UID区域检测到有效UID
				return false;
			}
			uid = _uid;
			error_code = 0;
			return true;
		}
		else
		{
			error_code = 3;//窗口画面为空
			return false;
		}
	}
	else
	{
		error_code = 2;//未能找到原神窗口句柄
		return false;
	}
}

int AutoTrack::GetLastError()
{
	return error_code;
}

bool AutoTrack::getGengshinImpactWnd()
{
	giHandle = FindWindowA("UnityWndClass", "原神");/* 对原神窗口的操作 */

#ifdef _DEBUG
	std::cout << "GI 原神 Windows Handle Find is " << giHandle << std::endl;
#endif

	return (giHandle != NULL ? true : false);
}

void AutoTrack::getGengshinImpactRect()
{
	GetWindowRect(giHandle, &giRect);
	GetClientRect(giHandle, &giClientRect);

	int x_offset = GetSystemMetrics(SM_CXDLGFRAME);
	int y_offset = GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYCAPTION);

	giClientSize.width = (int)(screen_scale * (giClientRect.right - giClientRect.left));// -x_offset;
	giClientSize.height = (int)(screen_scale * (giClientRect.bottom - giClientRect.top));// -y_offset;

#ifdef _DEBUG
	std::cout << "GI Windows Size: " << giClientSize.width << "," << giClientSize.height << std::endl;
	std::cout << "GI Windows Scale: " << screen_scale << std::endl;
#endif

}

void AutoTrack::getGengshinImpactScreen()
{
	static HBITMAP	hBmp;
	BITMAP bmp;

	DeleteObject(hBmp);

	if (giHandle == NULL)return;

	//获取目标句柄的窗口大小RECT
	GetWindowRect(giHandle, &giRect);/* 对原神窗口的操作 */

	//获取目标句柄的DC
	HDC hScreen = GetDC(giHandle);/* 对原神窗口的操作 */
	HDC hCompDC = CreateCompatibleDC(hScreen);

	//获取目标句柄的宽度和高度
	int	nWidth = (int)((screen_scale) * (giRect.right - giRect.left));
	int	nHeight = (int)((screen_scale) * (giRect.bottom - giRect.top));

	//创建Bitmap对象
	hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);//得到位图

	SelectObject(hCompDC, hBmp); //不写就全黑
	BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);

	//释放对象
	DeleteDC(hScreen);
	DeleteDC(hCompDC);

	//类型转换
	//这里获取位图的大小信息,事实上也是兼容DC绘图输出的范围
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	//mat操作
	giFrame.create(cv::Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));

	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, giFrame.data);

	giFrame = giFrame(cv::Rect(giClientRect.left, giClientRect.top, giClientSize.width, giClientSize.height));

	//getScreenScale(); 
}

void AutoTrack::getPaimonRefMat()
{
	int Paimon_Rect_x = cvRound(giFrame.cols*0.0135);
	int Paimon_Rect_y = cvRound(giFrame.cols*0.006075);
	int Paimon_Rect_w = cvRound(giFrame.cols*0.035);
	int Paimon_Rect_h = cvRound(giFrame.cols*0.0406);

	giPaimonRef = giFrame(cv::Rect(Paimon_Rect_x, Paimon_Rect_y, Paimon_Rect_w, Paimon_Rect_h));

#ifdef _DEBUG
	cv::namedWindow("Paimon", cv::WINDOW_FREERATIO);
	cv::imshow("Paimon", giPaimonRef);
	cv::waitKey(AUTO_TRACK_DEBUG_DELAY);
	std::cout << "Show Paimon" << std::endl;
#endif

}

void AutoTrack::getMiniMapRefMat()
{
	int MiniMap_Rect_x = cvRound(giFrame.cols*0.03125);
	int MiniMap_Rect_y = cvRound(giFrame.cols*0.009);
	int MiniMap_Rect_w = cvRound(giFrame.cols*0.1125);
	int MiniMap_Rect_h = cvRound(giFrame.cols*0.1125);

	giMiniMapRef = giFrame(cv::Rect(MiniMap_Rect_x, MiniMap_Rect_y, MiniMap_Rect_w, MiniMap_Rect_h));

#ifdef _DEBUG
	cv::namedWindow("MiniMap", cv::WINDOW_FREERATIO);
	cv::imshow("MiniMap", giMiniMapRef);
	cv::waitKey(AUTO_TRACK_DEBUG_DELAY);
	std::cout << "Show MiniMap" << std::endl;
#endif

}

void AutoTrack::getUIDRefMat()
{
	int UID_Rect_x = cvRound(giFrame.cols*0.875);
	int UID_Rect_y = cvRound(giFrame.rows*0.9755);
	int UID_Rect_w = cvRound(giFrame.cols* 0.0938);
	int UID_Rect_h = cvRound(UID_Rect_w*0.11);

	giUIDRef = giFrame(cv::Rect(UID_Rect_x, UID_Rect_y, UID_Rect_w, UID_Rect_h));

#ifdef _DEBUG
	cv::namedWindow("UID", cv::WINDOW_FREERATIO);
	cv::imshow("UID", giUIDRef);
	cv::waitKey(AUTO_TRACK_DEBUG_DELAY);
	std::cout << "Show UID" << std::endl;
#endif

}

void AutoTrack::getAvatarRefMat()
{
	int Avatar_Rect_x = cvRound(giMiniMapRef.cols*0.4);
	int Avatar_Rect_y = cvRound(giMiniMapRef.rows*0.4);
	int Avatar_Rect_w = cvRound(giMiniMapRef.cols*0.2);
	int Avatar_Rect_h = cvRound(giMiniMapRef.rows*0.2);

	giAvatarRef = giMiniMapRef(cv::Rect(Avatar_Rect_x, Avatar_Rect_y, Avatar_Rect_w, Avatar_Rect_h));

#ifdef _DEBUG
	cv::namedWindow("Avatar", cv::WINDOW_FREERATIO);
	cv::imshow("Avatar", giAvatarRef);
	cv::waitKey(AUTO_TRACK_DEBUG_DELAY);
	std::cout << "Show Avatar" << std::endl;
#endif

}

void AutoTrack::getScreenScale()
{
	HWND hWnd = GetDesktopWindow();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	// 获取监视器逻辑宽度与高度
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

	// 获取监视器物理宽度与高度
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	int cyPhysical = dm.dmPelsHeight;

	double horzScale = ((double)cxPhysical / (double)cxLogical);
	screen_scale = horzScale;
}

#ifdef _DEBUG
void AutoTrack::testLocalImg(std::string path)
{
	giFrame = cv::imread(path, cv::IMREAD_UNCHANGED);

	static bool is_frist_start = false;
	if (!is_frist_start)
	{
		is_frist_start=getGengshinImpactWnd();
	}

	getGengshinImpactRect();
	getGengshinImpactScreen();

	if (!giFrame.empty())
	{
		getPaimonRefMat();

		cv::Mat paimonTemplate;

		cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

		cv::Mat tmp;
		giPaimonRef = giFrame(cv::Rect(0, 0, cvRound(giFrame.cols / 20), cvRound(giFrame.rows / 10)));

		cv::namedWindow("test", cv::WINDOW_FREERATIO);
		cv::imshow("test", giPaimonRef);


		cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);

		cv::namedWindow("test2", cv::WINDOW_FREERATIO);
		cv::imshow("test2", tmp);

		std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;

		if (maxVal < 0.36 || maxVal == 1)
		{
			error_code = 6;//未能匹配到派蒙
		}

		getMiniMapRefMat();

		getAvatarRefMat();

		cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2,2);

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
		cv::resize(and12, and12, cv::Size(), 1.2, 1.2,3);
		cv::Canny(and12, and12, 20, 3*20, 3);
		cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), 24, cv::Scalar(0, 0, 0), -1);
		cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
		cv::dilate(and12, and12, dilate_element);
		cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
		cv::erode(and12, and12, erode_element);
		cv::Mat dstImage(and12.size(), CV_8UC3, cv::Scalar(128, 128, 128));
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarcy;
		cv::findContours(and12, contours, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
		std::vector<cv::RotatedRect> box(contours.size()); //定义最小外接矩形集合
		cv::Point2f rect[4];
		std::vector<cv::Point> AvatarKeyPoint;
		if (contours.size() > 3)
		{
			error_code = 9;
			return;
		}

		for (int i = 0; i < contours.size(); i++)
		{
			box[i] = cv::minAreaRect(cv::Mat(contours[i]));  //计算每个轮廓最小外接矩形
			boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
#ifdef _DEBUG
			cv::circle(dstImage, cv::Point(box[i].center.x, box[i].center.y), 5, cv::Scalar(0, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
			box[i].points(rect);  //把最小外接矩形四个端点复制给rect数组
			cv::rectangle(dstImage, cv::Point(boundRect[i].x, boundRect[i].y), cv::Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), cv::Scalar(0, 255, 0), 2, 8);
			for (int j = 0; j < 4; j++)
			{
				line(dstImage, rect[j], rect[(j + 1) % 4], cv::Scalar(0, 0, 255), 2, 8);  //绘制最小外接矩形每条边
			}
#endif
			AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));

		}
//#define mod1
#ifndef mod1






		
		
		double AvatarKeyPointLine[3] = { 0 };
		cv::Point KeyLine;
		if (AvatarKeyPoint.size() == 3)
		{
			AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
			AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
			AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

			//ab
			//bc
			//ca

			//abc
			//acb
			//bac
			//bca
			//cab
			//cba
			int keyMode = 0;
			if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1])
			{
				//a>b
				if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
				{
					//a>b>c
					keyMode = 3;
					KeyLine = AvatarKeyPoint[1] - AvatarKeyPoint[0];
				}
				else
				{
					//a>b,b<c
					if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2])
					{
						//a>c>b
						keyMode = 2;
						KeyLine = AvatarKeyPoint[2] - AvatarKeyPoint[0];
					}
					else
					{
						//c>a>b
						keyMode = 2;
						KeyLine = AvatarKeyPoint[2] - AvatarKeyPoint[0];
					}
				}
			}
			else
			{
				//b>a
				if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
				{
					//b>a,b>c
					if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2])
					{
						//b>a>c
						keyMode = 3;
						KeyLine = AvatarKeyPoint[1] - AvatarKeyPoint[0];
					}
					else
					{
						//b>c>a
						keyMode = 1;
						KeyLine = AvatarKeyPoint[2] - AvatarKeyPoint[1];
					}
				}
				else
				{
					//c>b>a
					keyMode = 1;
					KeyLine = AvatarKeyPoint[1] - AvatarKeyPoint[1];
				}
			}
			switch (keyMode)
			{
			case 0:
			{
				break;
			}
			case 1:
			{
				cv::circle(dstImage, AvatarKeyPoint[0], 5, cv::Scalar(255, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
				if ((AvatarKeyPoint[2].x + AvatarKeyPoint[1].x - AvatarKeyPoint[0].x * 2) > 0)
				{
					KeyLine = AvatarKeyPoint[2] - AvatarKeyPoint[1];

				}
				else
				{
					KeyLine = AvatarKeyPoint[1] - AvatarKeyPoint[2];
				}
				break;
			}
			case 2:
			{
				cv::circle(dstImage, AvatarKeyPoint[1], 5, cv::Scalar(255, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
				if ((AvatarKeyPoint[0].x + AvatarKeyPoint[2].x - AvatarKeyPoint[1].x * 2) > 0)
				{
					KeyLine = AvatarKeyPoint[0] - AvatarKeyPoint[2];

				}
				else
				{
					KeyLine = AvatarKeyPoint[2] - AvatarKeyPoint[0];
				}
				break;
			}
			case 3:
			{				
				cv::circle(dstImage, AvatarKeyPoint[2], 5, cv::Scalar(255, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
				if ((AvatarKeyPoint[1].x + AvatarKeyPoint[0].x - AvatarKeyPoint[2].x * 2) > 0)
				{
					KeyLine = AvatarKeyPoint[1] - AvatarKeyPoint[0];

				}
				else
				{
					KeyLine = AvatarKeyPoint[0] - AvatarKeyPoint[1];
				}
				break;
			}
			}
			cv::circle(dstImage, KeyLine+cv::Point(50,50), 2, cv::Scalar(255, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
			double angle = Line2Angle(KeyLine);
			std::cout <<"angle point 3: "<< angle << std::endl;
		}
		else if (AvatarKeyPoint.size() == 2)
		{
			double angle = Line2Angle(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
			std::cout << "angle point 2: " << angle << std::endl;
		}
		else
		{
			error_code = 10;
			return;
		}



		cv::namedWindow("test4", cv::WINDOW_FREERATIO);
		cv::imshow("test4", dstImage);
#endif // !mod1


#ifdef mod1
		std::vector<cv::Vec4i> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合
		cv::HoughLinesP(and12, lines, 1, CV_PI / 180, 15, 3, 100);

		cv::Mat dstImage(and12.size(), CV_8UC3, cv::Scalar(128, 128, 128));
		for (size_t i = 0; i < lines.size(); i++)
		{
			cv::Vec4i l = lines[i];
			cv::line(dstImage, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, 1);
		}

		cv::circle(dstImage, cv::Point(cvCeil(dstImage.cols / 2), cvCeil(dstImage.rows / 2)), 24, cv::Scalar(0, 255, 0));
		cv::circle(dstImage, cv::Point(cvCeil(dstImage.cols / 2), cvCeil(dstImage.rows / 2)), 2, cv::Scalar(0, 255, 0));
		cv::namedWindow("test4", cv::WINDOW_FREERATIO);
		cv::imshow("test4", dstImage);


		cv::Mat color(and12.size(), CV_8UC3, cv::Scalar(128, 128, 128));
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(and12, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		cv::drawContours(color, contours, -1, cv::Scalar(0, 0, 255), 3, 8, hierarchy);

		cv::Mat img_scene(giMatchResource.MapTemplate);
		cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

		cv::cvtColor(img_scene, img_scene, CV_RGBA2RGB);

		if (img_object.empty())
		{
			error_code = 5;//原神小地图区域为空或者区域长宽小于60px
		}
#endif // mod1

		


		error_code = 0;
	}
	else
	{
		error_code = 3;//窗口画面为空
		is_frist_start = false;
	}
}

void AutoTrack::testSaveGiScreen(std::string path)
{
	getGengshinImpactScreen();
	if (!giFrame.empty())
	{
		cv::imwrite(path, giFrame);
		std::cout<<"Save path: "<<path<<std::endl;
	}
	else
	{
		std::cout << "Save Faile!" << path << std::endl;
	}

}

void AutoTrack::sleep(int s)
{
	cv::waitKey(s*1000);
}
#endif