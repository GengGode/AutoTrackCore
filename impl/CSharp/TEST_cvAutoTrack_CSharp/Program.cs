using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using System.Text;
using cvAutoTrackCSharp;

namespace TEST_cvAutoTrack_CSharp
{
    class Program
    {
        static int Main(string[] args)
        {

            // ���������� -test ��ִ�в���
            if (args.Length > 0 && args[0] == "-test")
            {
                // ���԰汾��
                byte[] version_buff = new byte[256];
                if (cvAutoTrack.GetCompileVersion(version_buff,256))
                {
                    string version_str = Encoding.UTF8.GetString(version_buff);
                    Console.WriteLine("�汾��       : " + version_str);
                }
                else
                {
                    Console.WriteLine("������       : " + cvAutoTrack.GetLastErr().ToString());
                }
                byte[] version_time_buff = new byte[256];
                if (cvAutoTrack.GetCompileTime(version_time_buff, 256))
                {
                    string version_time_str = Encoding.UTF8.GetString(version_time_buff);
                    Console.WriteLine("����ʱ��     : " + version_time_str);
                }
                else
                {
                    Console.WriteLine("������       : " + cvAutoTrack.GetLastErr().ToString());
                }

                Console.WriteLine("�������");
                return 0;
            }
            else
            {
                // ��ȡ�汾��
                byte[] versionBuff = new byte[1024];
                //string version = "       ";

                cvAutoTrackCSharp.cvAutoTrack.verison(versionBuff);
                string version = Encoding.UTF8.GetString(versionBuff);
                //cvAutoTrack.cvAutoTrack.verison( version);

                Console.WriteLine("version: " + version);



                // �����ڴ�
                byte[] jsonBuffer = new byte[1024];

                // ���� cvAutoTrack.dll
                bool ret = cvAutoTrackCSharp.cvAutoTrack.GetStarJson(jsonBuffer);

                // ������
                Console.WriteLine("ret = " + ret);
                Console.WriteLine("jsonBuffer = " + Encoding.UTF8.GetString(jsonBuffer));

                int count = 0;
                while (true)
                {
                    if (count == 1)
                    {
                        cvAutoTrackCSharp.cvAutoTrack.SetUseDx11CaptureMode();
                        Console.WriteLine("����Dxģʽ");
                    }
                    if (count == 15)
                    {
                        cvAutoTrackCSharp.cvAutoTrack.SetUseBitbltCaptureMode();
                        Console.WriteLine("����Bitbltģʽ");
                    }

                    if (count >= 30)
                    {
                        count = 0;
                    }

                    count = count + 1;

                    double x = 0;
                    double y = 0;
                    double a = 0;
                    int mapid = 0;
                    cvAutoTrackCSharp.cvAutoTrack.GetTransformOfMap(ref x, ref y, ref a, ref mapid);
                    Console.WriteLine("x = " + x + " y = " + y + " a = " + a + " mapid = " + mapid);

                    // ��ȡ�����뼰������Ϣ
                    byte[] errorBuff = new byte[1024];
                    for (int i = 0; i < 1024; i++)
                    {
                        errorBuff[i] = 0;
                    }
                    int res = cvAutoTrackCSharp.cvAutoTrack.GetLastErrMsg(errorBuff, 1024);
                    string error = Encoding.UTF8.GetString(errorBuff);
                    Console.WriteLine("error: " + error);

                }

                return 0;
            }
        }
    }
}