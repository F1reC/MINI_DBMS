#ifndef API_H_
#define API_H_

#include <string>
#include <vector>
#include <iostream>

#include "BufferManager.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

using namespace std;

class Attribute;

// class for judging query conditions like '=' or '>'.
class Condition {
public:
	Condition();
	~Condition();
	Condition(string attributeInput, string valueInput, int operateInput);
	bool FitAttribute(int content); //judge conditions for int
	bool FitAttribute(float content); //judge conditions for float
	bool FitAttribute(string content); //judge conditions for string
	string getValue();
	string getAttributeName();
	int getOperate();

	const static int OPERATOR_EQUAL = 0; //"="
	const static int OPERATOR_NOT_EQUAL = 1; //"<>"
	const static int OPERATOR_LESS = 2; //"<"
	const static int OPERATOR_MORE = 3; //">"
	const static int OPERATOR_LESS_EQUAL = 4; //"<="
	const static int OPERATOR_MORE_EQUAL = 5; //">="

private:
	string attributeName;
	string value; //the value to be compared
	int operate; //the type to be compared
};


//class for API interefere
class API{
public:
    API();
	~API();
    void dropTable(string tableName); //ɾ����
    void createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation); //������
    void dropIndex(string indexName); //ɾ������
	void createIndex(string indexName, string tableName, string attributeName); //��������
    void showRecord(string tableName, vector<string>* attributeNameVector = NULL); //��ʾ��¼
	void showRecord(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector); //��ʾ��¼����Where
	void insertRecord(string tableName,vector<string>* recordContent); //�����¼
	void deleteRecord(string tableName); //ɾ����¼
	void deleteRecord(string tableName, vector<Condition>* conditionVector); //ɾ����¼����Where
	int getRecordNum(string tableName); //���ؼ�¼����
	int getRecordSize(string tableName); //���ؼ�¼��С
	int getTypeSize(int type); //��ȡ��������
    void getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector); //��ȡ��������������ֵ��������
    int getAttribute(string tableName, vector<Attribute>* attributeVector); //��ȡ�������
    void insertIndex(string indexName, char* value, int type, int blockOffset); //��������
    void deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //ɾ����¼
    void insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //�����¼
    void setRecordManager(RecordManager *rmInput);
    void setCatalogManager(CatalogManager *cmInput);
    void setIndexManager(IndexManager *imInput);
	int getLength();

private:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	int length; //����������Եĳ���

    int tableExist(string tableName); //�жϱ��Ƿ����
    int getIndexNameList(string tableName, vector<string>* indexNameVector); //��ȡ����������
    string getPrimaryIndex(string tableName); //��ȡ����������
    void tableAttributePrint(vector<string>* name); //��ӡ���������е�����
};

#endif
