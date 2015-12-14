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
	Message << "Error in " << line << ":";
	error_count++;
	switch (errortype)
	{
		//the function have defined before.
	case 41: Message << "��������ȱ�ٱ�ʶ��" << endl; break;
	case 50: Message << "����û����.��β" << endl; break;
	case 42: Message << "�� = �� := ʹ��" << endl; break;
	case 60: Message << "�ֳ���û����������" << endl; break;
	case 61: Message << "���������ʽ����ȷ" << endl; break;
	default:
		break;
	}
	error_messages.push_back(Message.str());
}

void Error::errorMessage(int errortype, int line,string message1) {
	stringstream Message;
	Message << "�����ڵ�" << line << "��:";
	error_count++;
	switch (errortype)
	{
		//the function have defined before.
	case 13:
	case 16: Message << message1 << "����һ���Ϸ��ĳ������ͣ�" << endl; break;
	case 9: Message << "ȱ���ַ�" << message1 << endl; break;
	case 19: Message << message1 << "��Ӧ���ڴ˴����֣�" << endl; break;
	case 40: Message << "��ʶ��" << message1 << "δ���壡" << endl; break;
	case 42: Message << "����" << message1 << "�Ѿ��������!" << endl; break;
	case 44: Message << "����" << message1 << "�ظ����壡" << endl; break;
	case 45: Message << "�����ķ�������" << message1 << "���ǻ������ͣ�" << endl; break;

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