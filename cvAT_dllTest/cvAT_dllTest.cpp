#include <iostream>
#include <cvAutoTrack.h>

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
	

int main()
{
	//system("chcp 65001");
	//system("chcp 936");
	std::ios::sync_with_stdio(false);

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
	char buff[1024] = { 0 };
	// ����ѭ��
	
	//if (init())
	{
	//	Sleep(2000);
	}
	//uninit();
	//Sleep(1000);

	FILE *fptr = NULL;
	fopen_s(&fptr,"./Output.txt","w+");

	while (1)
	{
		//���û�ȡ����
		
		// ����Dx��ͼ
		if (SetUseDx11CaptureMode())
		{
			std::cout << "����Dx��ͼ�ɹ�" << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
#pragma warning(suppress : 4996)
		if (GetTransform(x, y, a))
		{
			std::cout << "����ͽǶ�   : " << " " << x << " " << y << " " << a << "\n";
			fprintf(fptr, "%lf %lf %lf", x, y, a);
			fflush(fptr);
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
#pragma warning(suppress : 4996)
		if (GetPosition(x2, y2))
		{			int mapid = 0;
			GetPositionOfMap(x2, y2, mapid);
			std::cout << "����         : " << " " << x2 << " " << y2 <<" " << mapid<< "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
		if (GetDirection(a2))
		{
			std::cout << "�Ƕ�         : " << " " << a2 << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
		if (GetRotation(aa2))
		{
			std::cout << "�ӽǳ���     : " << " " << aa2 << "\n";
		}
		else
		{
			std::cout << "������       : " << " \n" << GetLastErr() << " " << "\n";
		}
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
		
		Sleep(30);
		system("cls");
		//std::cout << "\r\r\r\r\r";

	}
	fclose(fptr);
}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
