#include "RecordManager.h"
#include "API.h"
#include <cstring>
#include <direct.h>

using namespace std;

RecordManager::RecordManager() {
    //default ctor
}

RecordManager::~RecordManager() {
    //default dtor
}

//������
int RecordManager::createTable(string tableName){
    //��ȡҪ�򿪵ı��ļ���
    string tableFileName = getTableFileName(tableName); 
    
    FILE *fp;
    //�򿪱��ļ�
    fp = fopen(tableFileName.c_str(), "a+");
    if (fp == NULL)
        return 0;
    fclose(fp);//�ر��ļ�
    return 1;
}

//�������ݿ�
int RecordManager::createDatabase(string databaseName){
    //�����ļ���
    if (_mkdir(databaseName.c_str()) == -1)
        return 0;
    //�ɹ���������1
    return 1;
}



//ɾ����
int RecordManager::dropTable(string tableName){
    //��ȡҪɾ���ı��ļ���
    string tableFileName = getTableFileName(tableName);
    //����deleteFileNode
    bm.deleteFileNode(tableFileName.c_str());

    //��ɾ���ɹ�
    if (remove(tableFileName.c_str())) 
		return 0;
    return 1;
}

//��������
int RecordManager::createIndex(string indexName){
    //��ȡҪ������������
    string indexFileName = getIndexFileName(indexName);
  
    FILE *fp;
    //�������ļ�
    fp = fopen(indexFileName.c_str(), "a+");
    if (fp == NULL)
        return 0;
    fclose(fp);
    return 1;
}

//ɾ������
int RecordManager::dropIndex(string indexName){
    //��ȡҪɾ����������
    string indexFileName = getIndexFileName(indexName);
    //����bufferManagerɾ������
    bm.deleteFileNode(indexFileName.c_str());
    //��ɾ���ɹ�
    if (remove(indexFileName.c_str()))
        return 0;
    return 1;
}

//�����¼
int RecordManager::insertRecord(string tableName,char* record, int recordSize){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    while (true){
        //�����Ϊ�գ��򷵻�-1
        if (btmp == NULL)
            return -1;

        //��ʹ�õ��ֽڴ�СС�ڵ��ڿ�ʹ�õ��ֽڴ�С
        if (bm.getUsingSize(*btmp) <= bm.getBlockSize() - recordSize){
            char* addressBegin;
            //ȷ����ʼλ��
            addressBegin = bm.getContent(*btmp) + bm.getUsingSize(*btmp);
            //����¼���Ƶ�addressBegin
            memcpy(addressBegin, record, recordSize);
            bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + recordSize);
            bm.setDirty(*btmp);
            return btmp->offsetNum;
        }
        else
            btmp = bm.getNextBlock(ftmp, btmp);
    }
    
    return -1;
}

//��ʾ����ѡ��ļ�¼
int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector,  vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        //�����Ϊ�գ��򷵻�-1
        if (btmp == NULL)
            return -1;

        //���ļ��ڵ����
        if (btmp->ifbottom){
            int recordBlockNum = recordBlockShow(tableName,attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            //���ؼ�¼����
            return count;
        }
        else{
            int recordBlockNum = recordBlockShow(tableName, attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            //����ָ��ָ����һ����
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

//��ʾѡ��ļ�¼��WHERE(����ΪblockOffset)
int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//�����Ϊ�գ��򷵻�-1
        return -1;
    else//��ǿ�
        return  recordBlockShow(tableName, attributeNameVector, conditionVector, block);
}

//��ʾѡ��ļ�¼��WHERE(����ΪblockNode)
int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    
    int count = 0;
    //��ȡ��¼��ʼ��
    char* recordBegin = bm.getContent(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);

    api->getAttribute(tableName, &attributeVector);
    //��ȡ�����ʼ��
    char* blockBegin = bm.getContent(*block);
    //���ʹ�ô�С
    size_t usingSize = bm.getUsingSize(*block);
    
	if (usingSize == 0)
		return count;
	cout << "* ";
    //����¼��ʼ�������ʼ��С�ڿ������С
    while (recordBegin - blockBegin  < usingSize){
        //���recordBeginָ��һ����¼
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)){
            count ++;//��¼����1
            //��ӡ��¼
            recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);
            cout <<"\n* ";
        }

        //ָ����һ����¼
        recordBegin += recordSize;
    }
	for (int i = 0; i < (*attributeNameVector).size(); i++) {
		for (int j = 0; j < api->getLength() + 4; j += 2)
			cout << "* ";
	}
	cout << "\n";
    
    return count;
}

//��ʾȫ����,��WHERE
int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;

    while (true){
        //�����Ϊ�գ��򷵻�-1
        if (btmp == NULL)
            return -1;
        //���ļ��ڵ����
        if (btmp->ifbottom){
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            //���س�ͻ��¼����
            return count;
        }
        else{
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            //����ָ��ָ����һ����
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}
//�ҵ���¼���λ��
int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    int count = 0;
    //��ȡ��¼��ʼ��
    char* recordBegin = bm.getContent(*block);
    vector<Attribute> attributeVector;
    //��ȡ��¼��С
    int recordSize = api->getRecordSize(tableName);
    
    api->getAttribute(tableName, &attributeVector);
    
    while (recordBegin - bm.getContent(*block)  < bm.getUsingSize(*block)){
        //���recordBeginָ��һ����¼
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
            count++;//��¼������1
        //ָ����һ����¼
        recordBegin += recordSize;
    }
    
    return count;
}

//ɾ��һ��������м�¼
int RecordManager::recordAllDelete(string tableName, vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);

    int count = 0;
    while (true){
        if (btmp == NULL)//�����Ϊ�գ��򷵻�-1
            return -1;
        if (btmp->ifbottom){//���ļ��ڵ����
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;//����ɾ���ļ�¼����
        }
        else{
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);//����ָ��ָ����һ����
        }
    }
    
    return -1;
}

//���ڴ���ɾ��һ����¼��(����ΪblockNum)
int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//�����Ϊ�գ��򷵻�-1
        return -1;
    else
        return  recordBlockDelete(tableName, conditionVector, block);
}

//���ڴ���ɾ��һ����¼��(����ΪblockNode)
int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//��ȡ��¼��ʼ��
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);//��ȡ��¼��С
    
    api->getAttribute(tableName, &attributeVector);
    
    while (recordBegin - bm.getContent(*block) < bm.getUsingSize(*block)){
        //���recordBeginָ��һ����¼
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)){
            count ++;//ɾ����¼����1
            
            api->deleteRecordIndex(recordBegin, recordSize, &attributeVector, block->offsetNum);//ͬʱɾ����¼������
            int i = 0;
            for (i = 0; i + recordSize + recordBegin - bm.getContent(*block) < bm.getUsingSize(*block); i++){
                recordBegin[i] = recordBegin[i + recordSize];
            }
            //��ռ�¼
            memset(recordBegin + i, 0, recordSize);
            bm.setUsingSize(*block, bm.getUsingSize(*block) - recordSize);
            //��ʾ�˿�����ģ��Ժ���Ҫ����д�ش���
            bm.setDirty(*block); 
        }
        else
            //ָ����һ����¼
            recordBegin += recordSize;
    }
    
    return count;
}

//Ϊ�����Ѿ����ڵļ�¼��������
int RecordManager::indexRecordAllAlreadyInsert(string tableName,string indexName){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        if (btmp == NULL)
            return -1;
        //���ļ��ڵ����
        if (btmp->ifbottom){
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            return count;
        }
        else{
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

//Ϊ�����Ѿ����ڵļ�¼��������(������blockNode)
int RecordManager::indexRecordBlockAlreadyInsert(string tableName,string indexName,  blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    //��ȡ��¼��ʼ��
    char* recordBegin = bm.getContent(*block);
    vector<Attribute> attributeVector;
    //��ȡ��¼��С
    int recordSize = api->getRecordSize(tableName);
    
    api->getAttribute(tableName, &attributeVector);
    
    int type;
    int typeSize;
    char * contentBegin;
    
    while (recordBegin - bm.getContent(*block)  < bm.getUsingSize(*block)){
        contentBegin = recordBegin;
        //���recordBeginָ��һ����¼
        for (int i = 0; i < attributeVector.size(); i++){
            type = attributeVector[i].getType();
            typeSize = api->getTypeSize(type);
            
            //�ҵ���¼�����������������������
            if (attributeVector[i].getIndex()==indexName){
                api->insertIndex(indexName, contentBegin, type, block->offsetNum);
                count++;
            }
            
            contentBegin += typeSize;
        }
        //ָ����һ����¼
        recordBegin += recordSize;
    }
    
    return count;
}

//���������Ŀ�
bool RecordManager::recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector){
    if (conditionVector == NULL)
        return true;
    
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++){
        type = (*attributeVector)[i].getType();
        attributeName = (*attributeVector)[i].getName();
        typeSize = api->getTypeSize(type);
        
        //��ʼ������(���������ַ���ʱ�����ǿ��Ժ����׵صõ�һ���ַ���)
        memset(content, 0, 255);
        memcpy(content, contentBegin, typeSize);
        for(int j = 0; j < (*conditionVector).size(); j++){
            //������������Ҫ��������
            if ((*conditionVector)[j].getAttributeName() == attributeName){
                //����˼�¼����������
                if(!contentConditionFit(content, type, &(*conditionVector)[j])){
                    return false;
                }
            }
        }

        contentBegin += typeSize;
    }
    return true;
}

//��ӡ��¼��Ϣ
void RecordManager::recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector){
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++){
        type = (*attributeVector)[i].getType();
        typeSize = api->getTypeSize(type);
        
        //��ʼ������(���������ַ���ʱ�����ǿ��Ժ����׵صõ�һ���ַ���)
        memset(content, 0, 255);
        memcpy(content, contentBegin, typeSize);

        for(int j = 0; j < (*attributeNameVector).size(); j++){
            if ((*attributeNameVector)[j] == (*attributeVector)[i].getName()){
                contentPrint(content, type);
                break;
            }
        }
        
        contentBegin += typeSize;
    }
}

//�������ʹ�ӡ����
void RecordManager::contentPrint(char * content, int type){
    //���������int����
    if (type == Attribute::TYPE_INT){
        int tmp = *((int *) content);   //�����ȡ����ֵ
		cout << tmp;
		stringstream ss;
		ss << tmp;
		string stmp;
		ss >> stmp;
		for (int j = 0; j < api->getLength() + 2 - stmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
    //���������float����
    else if (type == Attribute::TYPE_FLOAT){
        float tmp = *((float *) content);   //�����ȡ����ֵ
		cout << tmp;
		stringstream ss;
		ss << tmp;
		string stmp;
		ss >> stmp;
		for (int j = 0; j < api->getLength() + 2 - stmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
    //���������string����
    else{
        string tmp = content;
		cout << tmp;
		for (int j = 0; j < api->getLength() + 2 - tmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
}

// �������ݵ�����
bool RecordManager::contentConditionFit(char* content,int type,Condition* condition){
    if (type == Attribute::TYPE_INT){
        //���������int����
        int tmp = *((int *) content);   //�����ȡ����ֵ
        return condition->FitAttribute(tmp);
    }
    else if (type == Attribute::TYPE_FLOAT){
        //���������float����
        float tmp = *((float *) content);   //�����ȡ����ֵ
        return condition->FitAttribute(tmp);
    }
    else{
        //���������string����
        return condition->FitAttribute(content);
    }
    return true;
}

//���������ļ���
string RecordManager::getIndexFileName(string indexName){
    string tmp = "";
    return "INDEX_RECORD_" + indexName;
}

//���ر��ļ���
string RecordManager::getTableFileName(string tableName){
    string tmp = "";
    return tmp + "TABLE_RECORD_" + tableName;
}

//����API
void RecordManager::setAPI(API* apiInput) {
	api = apiInput;
}

//����������¼����
int RecordManager::recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset) {
	fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
	blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

	//�����Ϊ�գ��򷵻�-1
	if (block == NULL)
		return -1;
	int length = 0;
    //��ȡ��¼��ʼ��
	char* recordBegin = bm.getContent(*block);
	vector<Attribute> attributeVector;
    //��ȡ��¼��С
	int recordSize = api->getRecordSize(tableName);

	api->getAttribute(tableName, &attributeVector);
	char* blockBegin = bm.getContent(*block);
	size_t usingSize = bm.getUsingSize(*block);

	while (recordBegin - blockBegin  < usingSize) {
		//���recordBeginָ��һ����¼
		if (recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)) {
			int lentmp = recordRowLength(recordBegin, recordSize, &attributeVector, attributeNameVector);
			if (lentmp > length)//���³���
				length = lentmp;
		}
		recordBegin += recordSize;
	}

	return length;
}

//�������һ�е�����¼����
int RecordManager::recordRowLength(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector) {
	int type;
	string attributeName;
	int typeSize;
	char content[255];
	int length = 0;

	char *contentBegin = recordBegin;
	for (int i = 0; i < attributeVector->size(); i++) {
		type = (*attributeVector)[i].getType();
		typeSize = api->getTypeSize(type);

		//��ʼ������
		memset(content, 0, 255);
		memcpy(content, contentBegin, typeSize);

		for (int j = 0; j < (*attributeNameVector).size(); j++) {
			if ((*attributeNameVector)[j] == (*attributeVector)[i].getName()) {
                //���������int����
                if (type == Attribute::TYPE_INT) {
					int tmp = *((int *)content);   //�����ȡ����ֵ
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
					length = stmp.size();
				}
                //���������float���� 
				else if (type == Attribute::TYPE_FLOAT) {
					float tmp = *((float *)content);   //�����ȡ����ֵ
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
						length = stmp.size();
				}
                //���������string����
				else {
					string tmp = content;
					if (tmp.size() > length)
						length = tmp.size();
				}
				break;
			}
		}

		contentBegin += typeSize;
	}
	return length;
}
