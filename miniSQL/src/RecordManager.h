#ifndef RECORDMANAGER_H_
#define RECORDMANAGER_H_

#include <string>
#include <vector>
#include <iostream>

#include "IndexManager.h"
#include "RecordManager.h"
#include "BufferManager.h"

using namespace std;

class Condition;
class API;

class RecordManager{
public:
	RecordManager();
	~RecordManager();
    
    int createTable(string tableName); // ����������ʼ����¼
    int dropTable(string tableName); //ɾ��һ��������м�¼
    int dropIndex(string indexName); //��������ɾ������
    int createIndex(string indexName); //�������ϴ�������
	int insertRecord(string tableName, char* record, int recordSize); //����в����¼
    int recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector); //��ʾ����ѡ��ļ�¼
    int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset); //��ʾ����Ƶļ�¼�Թ�ѡ��
    int recordAllFind(string tableName, vector<Condition>* conditionVector); //���ұ�ļ�¼
    int recordAllDelete(string tableName, vector<Condition>* conditionVector); //ɾ��һ��������м�¼
    int recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset); //ɾ���ڴ��еļ�¼��
    int indexRecordAllAlreadyInsert(string tableName,string indexName); //Ϊ�����Ѿ����ڵļ�¼��������
    string getTableFileName(string tableName); //���ر���Ϣ���ļ���
    string getIndexFileName(string indexName); //���ر���Ϣ��������
	int recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset); //���������¼���ȼ�
	int recordRowLength(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector); //�������һ�е�����¼����
	void setAPI(API* apiInput);

private:
	BufferManager bm;
	API *api;

    int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block); //��ʾ���еļ�¼
    int recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block); //�ҵ���¼���λ��
    int recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block); //���ڴ���ɾ��һ����¼��
    int indexRecordBlockAlreadyInsert(string tableName,string indexName, blockNode* block); //Ϊ�鴴������
    bool recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector); //���Ͽ������
    void recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector); //��ӡ��¼��Ϣ
    bool contentConditionFit(char* content, int type, Condition* condition); // �������ݵ�����
    void contentPrint(char * content, int type); //�������ʹ�ӡ����
};

#endif
