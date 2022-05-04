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
	int addIndex(string indexName, string tableName, string attributeName, int type); //��ӱ�����
	int revokeIndexOnAttribute(string tableName, string AttributeName, string indexName); //����������
	int findTable(string tableName); //�ҵ���Ʊ����ı�
	int findIndex(string indexName); //�ҵ����������������
	int dropTable(string tableName); //ɾ��һ����ƺõı�
	int dropIndex(string index); //ɾ��һ����ƺõ�����
	int deleteValue(string tableName, int deleteNum);// ɾ����¼����
	int insertRecord(string tableName, int recordNum);  // ���Ӽ�¼������
	int getRecordNum(string tableName); //��ȡ��ļ�¼��
	int getIndexNameList(string tableName, vector<string>* indexNameVector); //��ȡ����������б�
	int getAllIndex(vector<IndexInfo> * indexs); //��ȡһ�������������
	int setIndexOnAttribute(string tableName, string AttributeName, string indexName); //�ڱ����������������
	int addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation); //������Ա�
	int getIndexType(string indexName);
	int getAttribute(string tableName, vector<Attribute>* attributeVector);
	int calcuteLenth(string tableName); //�����ĳ���
	int calcuteLenth(int type); //�������͵ĳ���
	void getRecordString(string tableName, vector<string>* recordContent, char* recordResult); //���ַ����л�ȡ��¼
};

#endif
