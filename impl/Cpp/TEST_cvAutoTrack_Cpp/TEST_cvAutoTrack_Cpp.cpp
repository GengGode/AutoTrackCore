#include <iostream>
//#include <cvAutoTrack.h>
#include "../../../cvAutoTrack/include/cvAutoTrack.h"

#include <Windows.h>

#include <vector>

void Test_video()
{
	// ��̬��������
	// ��ʼ��
	//init();
	// ׼������
	float x = 0;
	float y = 0;
	float a = 0;
	double x2 = 0;
	double y2 = 0;
	double a2 = 0;
	double aa2 = 0;
	int uid = 0;
	// ����ѭ��

	std::vector<std::vector<double>> his;
	char path[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Picture/001.png" };
	char pathV[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.mp4" };

	char pathTxt[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.json" };

	//char pathTxt[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.txt" };
	/*GetInfoLoadVideo(pathV, pathTxt);
	std::cout << "������       : " << " " << GetLastErr() << " " << "\n";*/

	if (init())
	{
		//	Sleep(2000);
	}
	//uninit();
	//Sleep(1000);

	FILE* fptr = NULL;
	fopen_s(&fptr, "./Output.txt", "w+");


	//SetWorldScale(0.666667);
	if (GetInfoLoadPicture(path, uid, x2, y2, a2))
	{
		std::cout << "Now Coor and Angle: " << " " << uid << " " << " " << x2 << " " << y2 << " " << a2 << "\n";
	}
	else
	{
		std::cout << "������       : " << " " << GetLastErr() << " " << "\n";
	}
	//SetWorldScale(1.0);
	if (GetInfoLoadPicture(path, uid, x2, y2, a2))
	{
		std::cout << "Now Coor and Angle: " << " " << uid << " " << " " << x2 << " " << y2 << " " << a2 << "\n";
	}
	else
	{
		std::cout << "������       : " << " " << GetLastErr() << " " << "\n";
	}
	char buff[1024] = { 0 };
#ifdef _DEBUG
	if (GetStarJson(buff))
	{
		//������Ҫӳ�� p * 1.33 + AvatarPos
		std::cout << buff << "\n";
	}
#endif

}


int TEST()
{
	char version_buff[256] = { 0 };

	if (GetCompileVersion(version_buff,256))
	{
		std::cout << "�汾��       : " << " " << version_buff << " " << "\n";
	}
	else
	{
		std::cout << "������       : " << " " << GetLastErr() << " " << "\n";
	}
	
	char version_time_buff[256] = {0};
	
	if (GetCompileTime(version_time_buff, 256))
	{
		std::cout << "����ʱ��     : " << " " << version_time_buff << " " << "\n";
	}
	else
	{
		std::cout << "������       : " << " " << GetLastErr() << " " << "\n";
	}
	
	std::cout << "�������\n";
	return 0;
}
int TEST_init_and_uninit()
{
	std::cout << "���� init �� uninit\n";

	init();

	Sleep(1000);

	uninit();

	Sleep(1000);

	init();

	Sleep(1000);

	uninit();

	Sleep(1000);

	init();

	Sleep(1000);

	uninit();

	Sleep(1000);

	std::cout << "�������\n";
	return 0;
}
int Run()
{
	std::ios::sync_with_stdio(false);

	FILE* fptr = NULL;
	fopen_s(&fptr, "./Output.txt", "w+");

	// ׼������
	double x = 0;
	double y = 0;
	double a = 0;
	int map_id = 0;
	double x2 = 0;
	double y2 = 0;
	double a2 = 0;
	double aa2 = 0;
	int uid = 0;
	char buff[1024] = { 0 };

	

	// ����ѭ��
	while (1)
	{
		// ����Dx��ͼ
		/*if (SetUseDx11CaptureMode())
		{
			std::cout << "����Dx��ͼ�ɹ�" << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}*/
#pragma warning(suppress : 4996)
		if (GetTransformOfMap(x, y, a, map_id))
		{
			std::cout << "����ͽǶ�   : " << " " << map_id << x << " " << y << " " << a << "\n";
		}
		else
		{
			char buff[1024] = { 0 };
			GetLastErrMsg(buff, 1024);
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
			std::cout << "������Ϣ    : " << buff << " " << "\n";
		}
		//#pragma warning(suppress : 4996)
		//		if (GetPosition(x2, y2))
		//		{
		//			int mapid = 0;
		//			GetPositionOfMap(x2, y2, mapid);
		//			std::cout << "����         : " << " " << x2 << " " << y2 << " " << mapid << "\n";
		//		}
		//		else
		//		{
		//			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		//		}
		//		if (GetDirection(a2))
		//		{
		//			std::cout << "�Ƕ�         : " << " " << a2 << "\n";
		//		}
		//		else
		//		{
		//			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		//		}
		//		if (GetRotation(aa2))
		//		{
		//			std::cout << "�ӽǳ���     : " << " " << aa2 << "\n";
		//		}
		//		else
		//		{
		//			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		//		}
		if (GetUID(uid))
		{
			std::cout << "��ǰUID      : " << " " << uid << " " << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
		if (GetStarJson(buff))
		{
			//������Ҫӳ�� p + AvatarPos
			std::cout << "��ǰ��ͫJson : " << buff << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
		// ����Dx��ͼ
		if (DebugCapture())
		{
			std::cout << "��ͼ�ɹ�" << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}

		// ����io������
		std::cout << std::flush;

		Sleep(30);
		system("cls");
	}
	fclose(fptr);
	return 0;
}

int main(int argc, char* argv[])
{
	// ���������� -test ��ִ�в���
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
	{
		return TEST();
	}
	else
	{
		// ����ִ�������ĳ���
		return Run();
	}
}