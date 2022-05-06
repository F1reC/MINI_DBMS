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

    //���ļ�, 'r' = ֻ��
	fp = fopen("INDEX_FILE", "r");
	if (fp == NULL) {
        //���ļ�, 'a+' = ������ļ������ļ������ڣ��򴴽����ļ�
		fp = fopen("INDEX_FILE", "a+");
	}
	fclose(fp);
    
    //����API, CatalogManager�� RecordManagerʵ��
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
    cout << "\nCopyright 2022, Made by Group 10. All rights reserved." << endl;
	cout << "\nType 'help' for help." << endl;

    //����ѭ��
    while(true){
        if(fileExec){

            //ִ��sql�ļ�
            file.open(interpreter.getFilename());
            if(!file.is_open()){
                cout<<"Fail to open "<<interpreter.getFilename()<<endl;
                fileExec = 0;
                continue;
            }
            //���ļ����ݰ�';'���
            while(getline(file,s,';')){
                interpreter.interpret(s);
            }
            file.close();
            fileExec = 0;
        }
        else{
            //����ִ��sql�ļ�

            cout<<"\nminisql>";
            //��';'��ȡ��������
            getline(cin,s,';');

            //���㿪ʼʱ��
			start = clock();

            //���������������interpreter���н���
            status =  interpreter.interpret(s);

            //status == 2ʱ
            //����Ϊִ��sql
            if(status==2){
                //ִ���ļ�
                fileExec = 1;
            }

            //status == -2ʱ
            //����Ϊ������Ϣ
			else if (status == -2) {
                //������Ϣ
				cout << "��֧�� int \ float \ char(n)." << endl;
				cout << "����ʹ��create, drop, delete, insert, select����." << endl;
				cout << "���� 'quit' �� 'exit'���˳�." << endl;
			}
            //status == -1ʱ
            //����Ϊ�˳�sql
            else if(status==-1){
                //�˳�sql
                break;
            }
        }
    }

    return 0;
}
