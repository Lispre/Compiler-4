#include "error.h"
#include <sstream>

Error::Error() {
	is_success = true;
};

int Error::getErrorCount() {
	return error_count;
};

bool Error::IsSuccess() {
	return is_success;
}

void Error::errorMessage(int errortype,int line) {
	stringstream Message;
	is_success = false;
	Message << "错误在第" << line << "行:";
	error_count++;
	switch (errortype)
	{
		//the function have defined before.
	case 6: Message << "程序中有无法识别的单词" << endl; break;
	case 14: Message << "程序意外中断" << endl; break;
	case 15:Message << "在语句中出现了无意义字符串" << endl; break;
	case 24: Message << "for语句中出现了非to或downto标识" << endl; break;
	case 26: Message << "条件出现非法关系运算符" << endl; break;
	case 41: Message << "常量定义缺少标识符" << endl; break;
	case 50: Message << "程序没有以.结尾" << endl; break;
	case 42: Message << "把 = 当 := 使用" << endl; break;
	case 60: Message << "分程序没有正常结束" << endl; break;
	case 61: Message << "常量定义格式不正确" << endl; break;
	case 62: Message << "block语句块前有不正确的字符串" << endl; break;
	case 63: Message << "factor语句块前有语义不正确的字符串" << endl; break;
	case 64: Message << "const语句块前有语义不正确的字符串" << endl; break;
	case 65: Message << "var语句块前有语义不正确的字符串" << endl; break;
	case 66: Message << "基本类型前有语义不正确的字符串" << endl; break;
	case 67: Message << "语句块前有语义不正确的字符串" << endl; break;
	default:
		break;
	}
	error_messages.push_back(Message.str());
}

void Error::errorMessage(int errortype, int line,string message1) {
	stringstream Message;
	Message << "错误在第" << line << "行:";
	is_success = false;
	error_count++;
	switch (errortype)
	{
		//the function have defined before.
	case 11: Message << message1 << "不是一个合法的过程声明！" << endl; break;
	case 12: Message << message1 << "不是一个合法的基本类型！" << endl; break;
	case 13: Message << "在函数" << message1 << "声明后缺少分号！" << endl; break;
	case 16: Message << message1 << "不是一个合法的常量类型！" << endl; break;
	case 9: Message << "缺少字符" << message1 << endl; break;
	case 19: Message << message1 << "不应该在此处出现！" << endl; break;
	case 40: Message << "标识符" << message1 << "未定义！" << endl; break;
	case 41: Message << "过程" << message1 << "已经定义过了!" << endl; break;
	case 42: Message << "常量" << message1 << "已经定义过了!" << endl; break;
	case 43: Message << "变量" << message1 << "已经定义过了！" << endl; break;
	case 44: Message << "函数" << message1 << "已经定义过了！" << endl; break;
	case 45: Message << "函数的返回类型" << message1 << "不是基本类型！" << endl; break;
	case 46: Message << message1 << "不是数组！" << endl; break;
	case 55: Message << message1 << "形参与实参个数不匹配！" << endl; break;
	case 56: Message << message1 << "形参与实参的类型不匹配！" << endl; break;
	case 57: Message << message1 << "不是过程！" << endl; break;
	default:
		break;
	}
	error_messages.push_back(Message.str());
}


void Error::errorMessage(int errortype, int line, string message1, string message2) {
	stringstream Message;
	Message << "错误在第" << line << "行:";
	is_success = false;
	error_count++;
	switch (errortype)
	{
		//the function have defined before.
	case 47: Message << message1 << "与" << message2 << "类型不匹配，不可以直接赋值！" << endl; break;

	default:
		break;
	}
	error_messages.push_back(Message.str());
}

void Error::print() {
	vector<string>::iterator it = error_messages.begin();
	while (it != error_messages.end()) {
		cout << (*it);
		it++;
	}
}