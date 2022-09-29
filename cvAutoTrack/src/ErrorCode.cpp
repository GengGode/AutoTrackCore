#include "pch.h"
#include "ErrorCode.h"

ErrorCode::ErrorCode()
{
	fopen_s(&fptr, "./autoTrack.log", "w+");

	errCodeMsg.push_back(u8"ִ�гɹ�");
	errCodeMsg.push_back(u8"δ��ʼ��");
	errCodeMsg.push_back(u8"δ���ҵ�ԭ�񴰿ھ��");
	errCodeMsg.push_back(u8"���ڻ���Ϊ��");
	errCodeMsg.push_back(u8"δ��ƥ�䵽������");
	errCodeMsg.push_back(u8"ԭ��С��ͼ����Ϊ�ջ������򳤿�С��60px");
	errCodeMsg.push_back(u8"δ��ƥ�䵽����");
	errCodeMsg.push_back(u8"�������������Խ��");
	errCodeMsg.push_back(u8"δ����UID�����⵽��ЧUID");
	errCodeMsg.push_back(u8"��ȡС��ͷ����������");
	errCodeMsg.push_back(u8"��Ч�����ָ�������ָ�򴰿ڲ�����");
	errCodeMsg.push_back(u8"δ��ȡ��С��ͷ����");
	errCodeMsg.push_back(u8"���ھ��Ϊ��");
	errCodeMsg.push_back(u8"���ھ��ʧЧ");
}

ErrorCode::~ErrorCode()
{
	fclose(fptr);
}

ErrorCode & ErrorCode::getInstance()
{
	static ErrorCode instance;
	return instance;
}

ErrorCode & ErrorCode::operator=(const int & code)
{
	string msg = u8"δ���������Ϣ";
	if (code < errCodeMsg.size())
	{
		msg = errCodeMsg[code];
	}
	(* this) = {code,msg};
	return *this;
}

ErrorCode& ErrorCode::operator=(const std::pair<int, string>& err_code_msg)
{
	const int& code = err_code_msg.first;
	const string& msg = err_code_msg.second;
	
	this->errorCode = code;
	if (code == 0)
	{
		//fprintf_s(fptr, "��մ����ջ\n");
		error_code_msg_list.clear();
	}
	else
	{
		// ��ȡ��ǰϵͳʱ�� 
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		// ת��Ϊϵͳʱ��
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		// ת��Ϊ����ʱ��
		std::tm* now_tm = std::localtime(&now_c);
		// ת��Ϊ�ַ���
		char time_str[100];
		std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);
		
		fprintf_s(fptr, u8"%s | ������룺%d��������Ϣ��%s\n", time_str, code, msg.c_str());
		push(code,msg);
	}
	
	fflush(fptr);

	return *this;
}


ErrorCode::operator int()
{
	return this->errorCode;
}

int ErrorCode::getLastError()
{
	return this->errorCode;
}

string ErrorCode::getLastErrorMsg()
{
	if (this->errorCode != 0)
	{
		return to_string(error_code_msg_list.back().first) +": "+ error_code_msg_list.back().second;
	}
	else
	{
		return "0: SUCCESS";
	}
}

void ErrorCode::push(int code, string msg)
{
	error_code_msg_list.push_back({ code,msg });
	
	if (error_code_msg_list.size() > 9)
	{
		vector<pair<int, string>>::iterator index = error_code_msg_list.begin();
		error_code_msg_list.erase(index);
	}
}

ostream & operator<<(ostream & os, const ErrorCode & err)
{
	for (int i = static_cast<int>(err.error_code_msg_list.size()) - 1; i >= 0; i--)
	{
		for (int j = 1; j < err.error_code_msg_list.size() - i; j++)
		{
			os << "  ";
		}
		if (i == 0)
		{
			os << u8"\u2514\u2500\u2500\u2500\u2192";
		}
		else
		{
			os << u8"\u2514\u2500\u252C\u2500\u2192";
		}
		//os << "\u2514 \u2500 \u252C \u2500 \u2192";

		os << to_string(err.error_code_msg_list[i].first) + ": " + err.error_code_msg_list[i].second + '\n';
	}
	return os;
}
