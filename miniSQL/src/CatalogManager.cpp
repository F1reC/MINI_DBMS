#include "CatalogManager.h"

using namespace std;

#define UNKNOWN_FILE -10
#define TABLE_FILE -20
#define INDEX_FILE -30
#define DATABASE_FILE -40

string useDatabase = "";

CatalogManager::CatalogManager() {
	// TODO Auto-generated constructor stub

}

CatalogManager::~CatalogManager() {
	// TODO Auto-generated destructor stub
}

//ɾ����,�ɹ�->����1,ʧ��->����0
int CatalogManager::dropTable(string tableName) {
	string tableFileName = "��Ŀ¼" + tableName;
	//����bufferManagerɾ��ָ�����ļ�
	bm.deleteFileNode(tableFileName.c_str());
	if (remove(tableFileName.c_str()))
		return 0;
	return 1;
}

//��������λ��,��û���ҵ��򷵻�-2
int CatalogManager::getIndexType(string indexName) {
	//��ʼ��fileNode ��blockNode
	fileNode *ftmp = bm.getFile("�ļ�����");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin;
		//����bufferManager��ȡ��ַ����
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//ѭ����������λ��
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getIndexName() == indexName) {
				return i->getType();
			}
			i++;
		}
		return -2;
	}
	return -2;
}

//������������,�������洢
int CatalogManager::getAllIndex(vector<IndexInfo> * indexs) {
	//��ʼ��fileNode��blockNode
	fileNode *ftmp = bm.getFile("�ļ�����");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//ѭ���洢����
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			indexs->push_back((*i));
			i++;
		}
	}
	return 1;
}

//�������,���ɹ����->����,��ʧ��->����0
int CatalogManager::addIndex(string indexName, string tableName, string Attribute, int type) {
	//��ʼ��fileNode��blockNode
	fileNode *ftmp = bm.getFile("�ļ�����");
	blockNode *btmp = bm.getBlockHead(ftmp);
	IndexInfo i(indexName, tableName, Attribute, type);
	while (true) {
		//�ݴ�:btmpΪ��
		if (btmp == NULL) {
			return 0;
		}
		//��bufferManager�Ŀ���ʹ�ô�С��block���ʹ�ô�С - ��������Ĵ�С
		//�������Ϊ��ʱ�黹��û�пռ�,�����������
		if (bm.getUsingSize(*btmp) <= bm.getBlockSize() - sizeof(IndexInfo)) {
			char* addressBegin;
			//�ڴ��ַ��ʼ = ��ʹ�õ�ַ���� + ����õĵ�ַ
			addressBegin = bm.getContent(*btmp) + bm.getUsingSize(*btmp);
			//��������С���Ƶ�addressBegin
			memcpy(addressBegin, &i, sizeof(IndexInfo));
			//������ʹ�ô�С(+���������С)
			bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + sizeof(IndexInfo));
			//����Ϊ���
			bm.setDirty(*btmp);
			//��������
			return this->setIndexOnAttribute(tableName, Attribute, indexName);
		}
		else {
			//��ȡ��һ����,ֱ������
			btmp = bm.getNextBlock(ftmp, btmp);
		}
	}
	return 0;
}

//���ұ�,���ҵ�->����TABLE_FILE,ʧ��->����0
int CatalogManager::findTable(string tableName) {
	FILE *fp;
	string tableFileName = "��Ŀ¼" + tableName;
	//ֻ���򿪱�Ŀ¼
	fp = fopen(tableFileName.c_str(), "r");
	if (fp == NULL) {
		return 0;
	}
	else {
		fclose(fp);
		return TABLE_FILE;
	}
}

//�������ݿ��ļ����Ƿ����
int CatalogManager::findDatabase(string databaseName)
{
	//�鿴�ļ����Ƿ����
	if (access(databaseName.c_str(), 0) == -1)
		return 0;
	else
		return DATABASE_FILE;
	return 0;
}

//��������,���ҵ�->����INDEX_FILE,û�ҵ�->����UNKNOWN_FILE,�˿鲻����->����0
int CatalogManager::findIndex(string fileName) {
	//��ʼ��ftmp��btmp
	fileNode *ftmp = bm.getFile("�ļ�����");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//�Ƚ�flag����ΪUNKNOWN_FILE
		int flag = UNKNOWN_FILE;
		//ѭ����������,���ҵ���flagΪINDEX_FILE
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getIndexName() == fileName) {
				flag = INDEX_FILE;
				break;
			}
			i++;
		}
		return flag;
	}
	return 0;
}

//ɾ������,�ɹ�->����1.ʧ��->����0
int CatalogManager::dropIndex(string index) {
	//��ʼ��ftmp��btmp
	fileNode *ftmp = bm.getFile("�ļ�����");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		int j = 0;
		//ѭ����������
		for (j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getIndexName() == index)
				break;
			i++;
		}
		//ɾ������
		this->revokeIndexOnAttribute((*i).getTableName(), (*i).getAttribute(), (*i).getIndexName());
		//����������ȫ����Ϊ��һ������(ɾ��������ȥ����)
		for (; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo) - 1); j++) {
			(*i) = *(i + sizeof(IndexInfo));
			i++;
		}
		//����bufferManager����ʹ�ô�С
		//����Ϊ���
		bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) - sizeof(IndexInfo));
		bm.setDirty(*btmp);

		return 1;
	}
	return 0;
}

//ɾ����������ʵ��,Ϊ��һ��dropIndex����,���ɹ�->����1,ʧ��->����0
int CatalogManager::revokeIndexOnAttribute(string tableName, string AttributeName, string indexName) {
	//��ʼ��tableFileName,fileNode,blockNode
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		//addressBegin = ���ݵ�ַ + 1+int��С + 1
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += (1 + sizeof(int));
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		int i;
		//ѭ�����ҵ�ַ
		for (i = 0; i<size; i++) {
			//�������Ƿ�ƥ��
			if (a->getName() == AttributeName) {
				//�������Ƿ�ƥ��
				if (a->getIndex() == indexName) {
					//��������Ϊ��
					a->getIndex() = "";
					//����bufferManager���˿���Ϊ��
					bm.setDirty(*btmp);
				}
				else {
					//�쳣����:����λ������
					cout << "����δ֪����: " << indexName << " on " << tableName << "!" << endl;
					cout << "����: " << AttributeName << " on �� " << tableName << " ������: " << a->getIndex() << "!" << endl;
				}
				break;
			}
			a++;
		}
		if (i<size)
			return 1;
		else
			return 0;
	}
	return 0;
}

//��ȡ�������б�,�ɹ�->����1,ʧ��->����0
int CatalogManager::getIndexNameList(string tableName, vector<string>* indexNameVector) {
	//��ʼ��ftmp��btmp
	fileNode *ftmp = bm.getFile("�ļ�����");
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin;
		addressBegin = bm.getContent(*btmp);
		IndexInfo * i = (IndexInfo *)addressBegin;
		//ѭ����ȡ,����string����
		for (int j = 0; j<(bm.getUsingSize(*btmp) / sizeof(IndexInfo)); j++) {
			if ((*i).getTableName() == tableName) {
				(*indexNameVector).push_back((*i).getIndexName());
			}
			i++;
		}
		return 1;
	}
	return 0;
}

//ɾ������,�ɹ�->���� *recordNum,ʧ��->����0
int CatalogManager::deleteValue(string tableName, int deleteNum) {
	//��ʼ��tableFileName,fileNode,blockNode
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * recordNum = (int*)addressBegin;
		//�洢�������裾ɾ������
		if ((*recordNum) <deleteNum) {
			cout << "CatalogManager���ִ���::ɾ������" << endl;
			return 0;
		}
		else
			//ɾ���˿���Ҫɾ��������
			(*recordNum) -= deleteNum;
		//�������
		bm.setDirty(*btmp);
		return *recordNum;
	}
	return 0;
}

//��������,�ɹ�->����originalRecordNum,ʧ��->����0
int CatalogManager::insertRecord(string tableName, int recordNum) {
	//��ʼ��tableFileName,fileNode,blockNode
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * originalRecordNum = (int*)addressBegin;
		//����originalRecordNum���ӱ���Ĵ洢�Ĵ�С
		*originalRecordNum += recordNum;
		bm.setDirty(*btmp);
		return *originalRecordNum;
	}
	return 0;
}

//��ȡ���ݵ�����,�ɹ�->����recordNum,ʧ��->����0
int CatalogManager::getRecordNum(string tableName) {
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);

	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * recordNum = (int*)addressBegin;
		return *recordNum;
	}
	return 0;
}

//��ӱ�,�ɹ�->����1,ʧ��->����0
int CatalogManager::addTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName = "", int primaryKeyLocation = 0) {
	//��ʼ��fp,tableFileName
	FILE *fp;
	string tableFileName = "��Ŀ¼" + tableName;
	//д���ļ�
	fp = fopen(tableFileName.c_str(), "w+");
	//�ݴ�:fpΪ��
	if (fp == NULL)
		return 0;
	fclose(fp);
	//��ʼ��ftmp,btmp
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		int * size = (int*)addressBegin;
		//addressBegin�ճ�һ��int�Ĵ�С
		*size = 0;
		addressBegin += sizeof(int);
		//����ռ���ڴ�
		*addressBegin = primaryKeyLocation;
		addressBegin++;
		//����ռ���ڴ�
		*addressBegin = (*attributeVector).size();
		addressBegin++;
		//memcpy(addressBegin, attributeVector, (*attributeVector).size()*sizeof(Attribute));
		//ѭ�������ڴ��ַ
		for (int i = 0; i<(*attributeVector).size(); i++) {
			memcpy(addressBegin, &((*attributeVector)[i]), sizeof(Attribute));
			addressBegin += sizeof(Attribute);
		}
		//���Ŀ��ÿռ�,��Ϊ���
		bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + (*attributeVector).size() * sizeof(Attribute) + 2 + sizeof(int));
		bm.setDirty(*btmp);
		return 1;
	}
	return 0;
}
//������ݿ�,�ɹ�->����1,ʧ��->����0
int CatalogManager::addDatabase(string databaseName) {
	useDatabase = databaseName;
	return 1;
}

//�����������ʵ��,ΪaddIndex����,�ɹ�->����1,ʧ��->����0
int CatalogManager::setIndexOnAttribute(string tableName, string AttributeName, string indexName) {
	//��ʼ��tableFileName,fileNode,blockNode
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		//���㿪ʼ��ַ
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += 1 + sizeof(int);
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		int i;
		//ѭ������������
		for (i = 0; i<size; i++) {
			//�ж�������
			if (a->getName() == AttributeName) {
				a->setIndex(indexName);
				//����Ϊ���
				bm.setDirty(*btmp);
				break;
			}
			a++;
		}
		if (i<size)
			return 1;
		else
			return 0;
	}
	return 0;
}

//��ȡ����,�����������attributeVector,�ɹ�->����1,ʧ��->����0
int CatalogManager::getAttribute(string tableName, vector<Attribute>* attributeVector) {
	//��ʼ��tableFileName,fileNode,blockNode
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += 1 + sizeof(int);
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		for (int i = 0; i<size; i++) {
			//�������������������
			attributeVector->push_back(*a);
			a++;
		}

		return 1;
	}
	return 0;
}

//����length,�ɹ�->����singleRecordSize,ʧ��->����0
int CatalogManager::calcuteLenth(string tableName) {
	//��ʼ��tableFileName,fileNode,blockNode
	string tableFileName = "��Ŀ¼" + tableName;
	fileNode *ftmp = bm.getFile(tableFileName.c_str());
	blockNode *btmp = bm.getBlockHead(ftmp);
	//�ݴ�:btmpΪ��
	if (btmp) {
		int singleRecordSize = 0;
		char* addressBegin = bm.getContent(*btmp);
		addressBegin += 1 + sizeof(int);
		int size = *addressBegin;
		addressBegin++;
		Attribute *a = (Attribute *)addressBegin;
		for (int i = 0; i<size; i++) {//�����������ͷֱ���㳤��
			if ((*a).getType() == -1) {
				singleRecordSize += sizeof(float);
			}
			else if ((*a).getType() == 0) {
				singleRecordSize += sizeof(int);
			}
			else if ((*a).getType()>0) {
				singleRecordSize += (*a).getType() * sizeof(char);
			}
			else {
				//�ݴ�:
				cout << "Ŀ¼�����𻵣�" << endl;
				return 0;
			}
			a++;
		}
		return singleRecordSize;
	}
	return 0;
}

//����length(��������Ϊint)�ɹ�->���ظ��������ݳ���,ʧ��->����0
int CatalogManager::calcuteLenth(int type) {
	if (type == Attribute::TYPE_INT) {
		return sizeof(int);
	}
	else if (type == Attribute::TYPE_FLOAT)
		return sizeof(float);
	else
		return (int)type * sizeof(char); // �����ʹ洢�� Attribute.h
}

// ͨ��������recordContent��ȡ��ļ�¼�ַ������������д��recordResult���á�
void CatalogManager::getRecordString(string tableName, vector<string>* recordContent, char* recordResult) {
	//��ʼ��attributeVector,tableFileName,contentBegin
	vector<Attribute> attributeVector;
	string tableFileName = "��Ŀ¼" + tableName;
	getAttribute(tableName, &attributeVector);
	char * contentBegin = recordResult;

	//ѭ���������������ӵ��ڴ�ռ�
	for (int i = 0; i < attributeVector.size(); i++) {
		Attribute attribute = attributeVector[i];
		string content = (*recordContent)[i];
		int type = attribute.getType();
		int typeSize = calcuteLenth(type);
		stringstream ss;
		ss << content;
		if (type == Attribute::TYPE_INT) {
			//������ int
			int intTmp;
			ss >> intTmp;
			memcpy(contentBegin, ((char*)&intTmp), typeSize);
		}
		else if (type == Attribute::TYPE_FLOAT) {
			//������ float
			float floatTmp;
			ss >> floatTmp;
			memcpy(contentBegin, ((char*)&floatTmp), typeSize);
		}
		else {
			//������ string
			memcpy(contentBegin, content.c_str(), typeSize);
		}

		contentBegin += typeSize;
	}
	return;
}
