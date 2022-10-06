﻿using System;
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

            // 如果输入参数 -test 就执行测试
            if (args.Length > 0 && args[0] == "-test")
            {
                // 测试版本号
                byte[] version_buff = new byte[256];
                cvAutoTrack.verison(version_buff);
                string version_str = Encoding.UTF8.GetString(version_buff);
                Console.WriteLine("version: " + version_str);
                return 0;
            }
            else
            {
                // 获取版本号
                byte[] versionBuff = new byte[1024];
                //string version = "       ";

                cvAutoTrackCSharp.cvAutoTrack.verison(versionBuff);
                string version = Encoding.UTF8.GetString(versionBuff);
                //cvAutoTrack.cvAutoTrack.verison( version);

                Console.WriteLine("version: " + version);



                // 申请内存
                byte[] jsonBuffer = new byte[1024];

                // 调用 cvAutoTrack.dll
                bool ret = cvAutoTrackCSharp.cvAutoTrack.GetStarJson(jsonBuffer);

                // 输出结果
                Console.WriteLine("ret = " + ret);
                Console.WriteLine("jsonBuffer = " + Encoding.UTF8.GetString(jsonBuffer));

                int count = 0;
                while (true)
                {
                    if (count == 1)
                    {
                        cvAutoTrackCSharp.cvAutoTrack.SetUseDx11CaptureMode();
                        Console.WriteLine("启用Dx模式");
                    }
                    if (count == 15)
                    {
                        cvAutoTrackCSharp.cvAutoTrack.SetUseBitbltCaptureMode();
                        Console.WriteLine("启用Bitblt模式");
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

                    // 获取错误码及错误信息
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