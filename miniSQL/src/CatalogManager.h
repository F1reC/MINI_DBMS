#ifndef CATALOGMANAGER_H_
#define CATALOGMANAGER_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "BufferManager.h"
#include "IndexManager.h"

using namespace std;

class CatalogManager {
public:
	BufferManager bm;
	CatalogManager();
	virtual ~CatalogManager();
	//�������,���ɹ����->����,��ʧ��->����0
	int addIndex(string indexName, string tableName, string attributeName, int type); 
	//ɾ����������ʵ��,Ϊ��һ��dropIndex����,���ɹ�->����1,ʧ��->����0
	int revokeIndexOnAttribute(string tableName, string AttributeName, string indexName); 
	//���ұ�,���ҵ�->����TABLE_FILE,ʧ��->����0
	int findTable(string tableName); 
	//��������,���ҵ�->����INDEX_FILE,û�ҵ�->����UNKNOWN_FILE,�˿鲻����->����0
	int findIndex(string indexName); 
	//ɾ����,�ɹ�->����1,ʧ��->����0
	int dropTable(string tableName); 
	//ɾ������,�ɹ�->����1.ʧ��->����0
	int dropIndex(string index); 
	//ɾ������,�ɹ�->���� *recordNum,ʧ��->����0
	int deleteValue(string tableName, int deleteNum);
	//��������,�ɹ�->����originalRecordNum,ʧ��->����0
	int insertRecord(string tableName, int recordNum); 
	//��ȡ���ݵ�����,�ɹ�->����recordNum,ʧ��->����0
	int getRecordNum(string tableName);
	//��ȡ�������б�,�ɹ�->����1,ʧ��->����0
	int getIndexNameList(string tableName, vector<string>* indexNameVector); 
	//������������,�������洢
	int getAllIndex(vector<IndexInfo> * indexs); 
	//�����������ʵ��,ΪaddIndex����,�ɹ�->����1,ʧ��->����0
	int setIndexOnAttribute(string tableName, string AttributeName, string indexName); 
	//��ӱ�,�ɹ�->����1,ʧ��->����0
	int addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation);
	//��������λ��,��û���ҵ��򷵻�-2
	int getIndexType(string indexName);
	//��ȡ����,�����������attributeVector,�ɹ�->����1,ʧ��->����0
	int getAttribute(string tableName, vector<Attribute>* attributeVector);
	//����length,(����Ϊstring)�ɹ�->����singleRecordSize,ʧ��->����0
	int calcuteLenth(string tableName);
	//����length(��������Ϊint)�ɹ�->���ظ��������ݳ���,ʧ��->����0
	int calcuteLenth(int type);
	// ͨ��������recordContent��ȡ��ļ�¼�ַ������������д��recordResult���á�
	void getRecordString(string tableName, vector<string>* recordContent, char* recordResult);
};

#endif
