#include "RecordManager.h"
#include "API.h"
#include <cstring>

using namespace std;

RecordManager::RecordManager() {
    //default ctor
}

RecordManager::~RecordManager() {
    //default dtor
}

int RecordManager::createTable(string tableName){
    string tableFileName = getTableFileName(tableName); //��ȡҪ�򿪵ı��ļ���
    
    FILE *fp;
    fp = fopen(tableFileName.c_str(), "a+");//�򿪱��ļ�
    if (fp == NULL)
        return 0;
    fclose(fp);//�ر��ļ�
    return 1;
}

int RecordManager::dropTable(string tableName){
    string tableFileName = getTableFileName(tableName);//��ȡҪɾ���ı��ļ���
    bm.deleteFileNode(tableFileName.c_str());//����deleteFileNode

    if (remove(tableFileName.c_str())) //��ɾ���ɹ�
		return 0;
    return 1;
}

int RecordManager::createIndex(string indexName){
    string indexFileName = getIndexFileName(indexName);//��ȡҪ������������
  
    FILE *fp;
    fp = fopen(indexFileName.c_str(), "a+");//�������ļ�
    if (fp == NULL)
        return 0;
    fclose(fp);
    return 1;
}

int RecordManager::dropIndex(string indexName){
    string indexFileName = getIndexFileName(indexName);//��ȡҪɾ����������
    bm.deleteFileNode(indexFileName.c_str());//����deleteFileNode
    if (remove(indexFileName.c_str()))//��ɾ���ɹ�
        return 0;
    return 1;
}

int RecordManager::insertRecord(string tableName,char* record, int recordSize){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    while (true){
        if (btmp == NULL)//�����Ϊ�գ��򷵻�-1
            return -1;

        if (bm.getUsingSize(*btmp) <= bm.getBlockSize() - recordSize){//��ʹ�õ��ֽڴ�СС�ڵ��ڿ�ʹ�õ��ֽڴ�С
            char* addressBegin;
            addressBegin = bm.getContent(*btmp) + bm.getUsingSize(*btmp);//ȷ����ʼλ��
            memcpy(addressBegin, record, recordSize);//����¼���Ƶ�addressBegin
            bm.setUsingSize(*btmp, bm.getUsingSize(*btmp) + recordSize);
            bm.setDirty(*btmp);
            return btmp->offsetNum;
        }
        else
            btmp = bm.getNextBlock(ftmp, btmp);
    }
    
    return -1;
}

int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector,  vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        if (btmp == NULL)//�����Ϊ�գ��򷵻�-1
            return -1;

        if (btmp->ifbottom){//���ļ��ڵ����
            int recordBlockNum = recordBlockShow(tableName,attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            return count;//���ؼ�¼����
        }
        else{
            int recordBlockNum = recordBlockShow(tableName, attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);//����ָ��ָ����һ����
        }
    }
    
    return -1;
}

int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//�����Ϊ�գ��򷵻�-1
        return -1;
    else//��ǿ�
        return  recordBlockShow(tableName, attributeNameVector, conditionVector, block);
}

int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//��ȡ��¼��ʼ��
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);

    api->getAttribute(tableName, &attributeVector);
    char* blockBegin = bm.getContent(*block);//��ȡ�����ʼ��
    size_t usingSize = bm.getUsingSize(*block);//���ʹ�ô�С
    
	if (usingSize == 0)
		return count;
	cout << "* ";
    while (recordBegin - blockBegin  < usingSize){//����¼��ʼ�������ʼ��С�ڿ������С
        //���recordBeginָ��һ����¼
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)){
            count ++;//��¼����1
            recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);//��ӡ����
            cout <<"\n* ";
        }
        
        recordBegin += recordSize;//ָ����һ����¼
    }
	for (int i = 0; i < (*attributeNameVector).size(); i++) {
		for (int j = 0; j < api->getLength() + 4; j += 2)
			cout << "* ";
	}
	cout << "\n";
    
    return count;
}

int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;

    while (true){
        if (btmp == NULL)//�����Ϊ�գ��򷵻�-1
            return -1;
        if (btmp->ifbottom){//���ļ��ڵ����
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;//���س�ͻ��¼����
        }
        else{
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);//����ָ��ָ����һ����
        }
    }
    
    return -1;
}

int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//��ȡ��¼��ʼ��
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);//��ȡ��¼��С
    
    api->getAttribute(tableName, &attributeVector);
    
    while (recordBegin - bm.getContent(*block)  < bm.getUsingSize(*block)){
        //���recordBeginָ��һ����¼
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
            count++;//��¼������1
        
        recordBegin += recordSize;//ָ����һ����¼
    }
    
    return count;
}

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

int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

    if (block == NULL)//�����Ϊ�գ��򷵻�-1
        return -1;
    else
        return  recordBlockDelete(tableName, conditionVector, block);
}

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
            memset(recordBegin + i, 0, recordSize);//��ռ�¼
            bm.setUsingSize(*block, bm.getUsingSize(*block) - recordSize);
            bm.setDirty(*block); //��ʾ�˿�����ģ��Ժ���Ҫ����д�ش���
        }
        else
            recordBegin += recordSize;//ָ����һ����¼
    }
    
    return count;
}

int RecordManager::indexRecordAllAlreadyInsert(string tableName,string indexName){
    fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true){
        if (btmp == NULL)
            return -1;
        if (btmp->ifbottom){//���ļ��ڵ����
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

int RecordManager::indexRecordBlockAlreadyInsert(string tableName,string indexName,  blockNode* block){
    //�����Ϊ�գ��򷵻�-1
    if (block == NULL)
        return -1;
    int count = 0;
    
    char* recordBegin = bm.getContent(*block);//��ȡ��¼��ʼ��
    vector<Attribute> attributeVector;
    int recordSize = api->getRecordSize(tableName);//��ȡ��¼��С
    
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
        recordBegin += recordSize;//ָ����һ����¼
    }
    
    return count;
}

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
            if ((*conditionVector)[j].getAttributeName() == attributeName){
                //������������Ҫ��������
                if(!contentConditionFit(content, type, &(*conditionVector)[j])){
                    //����˼�¼����������
                    return false;
                }
            }
        }

        contentBegin += typeSize;
    }
    return true;
}

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

void RecordManager::contentPrint(char * content, int type){
    if (type == Attribute::TYPE_INT){
        //���������int����
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
    else if (type == Attribute::TYPE_FLOAT){
        //���������float���� 
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
    else{
        //���������string����
        string tmp = content;
		cout << tmp;
		for (int j = 0; j < api->getLength() + 2 - tmp.size(); j++)
			cout << " ";
		cout << "* ";
    }
}

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

string RecordManager::getIndexFileName(string indexName){
    string tmp = "";
    return "INDEX_RECORD_" + indexName;
}

string RecordManager::getTableFileName(string tableName){
    string tmp = "";
    return tmp + "TABLE_RECORD_" + tableName;
}

void RecordManager::setAPI(API* apiInput) {
	api = apiInput;
}


int RecordManager::recordLength(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset) {
	fileNode *ftmp = bm.getFile(getTableFileName(tableName).c_str());
	blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);

	//�����Ϊ�գ��򷵻�-1
	if (block == NULL)
		return -1;
	int length = 0;

	char* recordBegin = bm.getContent(*block);//��ȡ��¼��ʼ��
	vector<Attribute> attributeVector;
	int recordSize = api->getRecordSize(tableName);//��ȡ��¼��С

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
				if (type == Attribute::TYPE_INT) {
					//���������int����
					int tmp = *((int *)content);   //�����ȡ����ֵ
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
					length = stmp.size();
				}
				else if (type == Attribute::TYPE_FLOAT) {
					//���������float���� 
					float tmp = *((float *)content);   //�����ȡ����ֵ
					string stmp;
					stringstream ss;
					ss << tmp;
					ss >> stmp;
					if (stmp.size() > length)
						length = stmp.size();
				}
				else {
					//���������string���� 
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
