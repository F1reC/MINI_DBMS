#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include<iostream>
#include <string>
#include <vector>
#include "API.h"
#include "IndexManager.h"

using namespace std;
class Interpreter{
public:
	Interpreter();
	~Interpreter();
<<<<<<< HEAD
	int interpret(string s); //��������͸�API
=======
	int interpret(string s); //����API
>>>>>>> 35cd068e5018e4c6a538ed8168b735d28635db80
	string getFilename();
	void setAPI(API* apiInput);

private:
    API* api;
	string fileName;
<<<<<<< HEAD
	string getCommand(string s, int &st); //�õ������е�ÿ������
=======
	string getCommand(string s, int &st); //��ȡָ��
>>>>>>> 35cd068e5018e4c6a538ed8168b735d28635db80
};

#endif
