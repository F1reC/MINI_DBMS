#include <fstream>
#include <iostream>
#include <ctime>

#include "Interpreter.h"
#include "CatalogManager.h"
#include "RecordManager.h"	
#include "IndexManager.h"
#include "API.h"

using namespace std;

clock_t start; //����һ�β�����ʱ��

int main(){
    //�����������ļ�
	FILE *fp;
	fp = fopen("INDEX_FILE", "r");
	if (fp == NULL) {
		fp = fopen("INDEX_FILE", "a+");
	}
	fclose(fp);
    
    API api;
    CatalogManager cm;
    RecordManager rm;
    
    //����API������ģ��
    api.setRecordManager(&rm);
    api.setCatalogManager(&cm);
    IndexManager im(&api);
    api.setIndexManager(&im);
    rm.setAPI(&api);
    int fileExec = 0;
    ifstream file;
    Interpreter interpreter;
    interpreter.setAPI(&api);
    string s;
    int status = 0;

	start = 0; //��ʼ��ʱ

	srand(time(NULL));

    ////��ӡminisql�Ļ�����Ϣ
    cout << "\nCopyright 2018, Made by ORACLE���Ǽİ�. All rights reserved." << endl;
	cout << "\nType 'help' for help." << endl;

    while(true){
        if(fileExec){
            //ִ��sql�ļ�
            file.open(interpreter.getFilename());
            if(!file.is_open()){
                cout<<"Fail to open "<<interpreter.getFilename()<<endl;
                fileExec = 0;
                continue;
            }
            while(getline(file,s,';')){
                interpreter.interpret(s);
            }
            file.close();
            fileExec = 0;
        }
        else{
            cout<<"\nminisql>";
            getline(cin,s,';');
			start = clock();
            status =  interpreter.interpret(s);
            if(status==2){
                //ִ���ļ�
                fileExec = 1;
            }
			else if (status == -2) {
                //������Ϣ
				cout << "��֧�� int \ float \ char(n)." << endl;
				cout << "����ʹ��create, drop, delete, insert, select����." << endl;
				cout << "���� 'quit' �� 'exit'���˳�." << endl;
			}
            else if(status==-1){
                //�˳�SQL
                break;
            }
        }
    }

    return 0;
}
