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
	string value; //Ҫ�Ƚϵ�ֵ
	int operate; //Ҫ�Ƚϵ�����
};


//API������
class API{
public:
    API();
	~API();
    void dropTable(string tableName); //ɾ����API����
    void createTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName,int primaryKeyLocation); //������API����
    void dropIndex(string indexName); //ɾ������API����
	void createIndex(string indexName, string tableName, string attributeName); //��������API����
    void showRecord(string tableName, vector<string>* attributeNameVector = NULL); //��ʾ��ѯ��¼
	void showRecord(string tableName,  vector<string>* attributeNameVector, vector<Condition>* conditionVector); //��ʾ���״̬�ļ�¼
	void insertRecord(string tableName,vector<string>* recordContent); //����в����¼
	void deleteRecord(string tableName); //ɾ�����еļ�¼
	void deleteRecord(string tableName, vector<Condition>* conditionVector); //ɾ����������µļ�¼
	int getRecordNum(string tableName); //���ؼ�¼���
	int getRecordSize(string tableName); //��ȡ��¼��С
	int getTypeSize(int type); //��ȡ�������ʹ�С
    void getAllIndexAddressInfo(vector<IndexInfo> *indexNameVector); //��ȡ���������ļ���
    int getAttribute(string tableName, vector<Attribute>* attributeVector); //��ȡ�������
    void insertIndex(string indexName, char* value, int type, int blockOffset); //��������API����
    void deleteRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //ɾ�������еļ�¼
    void insertRecordIndex(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset); //�������в����¼
    void setRecordManager(RecordManager *rmInput);
    void setCatalogManager(CatalogManager *cmInput);
    void setIndexManager(IndexManager *imInput);
	int getLength();

private:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	int length; //�������ֵ�����Եĳ���

    int tableExist(string tableName); //�жϱ��Ƿ����
    int getIndexNameList(string tableName, vector<string>* indexNameVector); //��ȡ����������
    string getPrimaryIndex(string tableName); //��ȡ�����Ե�����
    void tableAttributePrint(vector<string>* name); //��ӡ�������
};

#endif
