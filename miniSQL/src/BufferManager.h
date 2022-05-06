#ifndef __Minisql__BufferManager__
#define __Minisql__BufferManager__

#include <queue>
#include <string>
#include <iostream>
#include <ctime>

#define MAX_FILE_NUM 100
#define MAX_BLOCK_NUM 500
#define MAX_FILE_NAME 100

using namespace std;

static int replaced_block = -1;

struct blockNode{
	int offsetNum; // ���б��е�ƫ����
	bool pin;  // �˿鱻�����ı�־
	bool ifbottom; // �ļ��ڵ�����ı�־
	char* fileName; // ��ڵ��������ļ���

	char *address; // ���ݵ�ַ
	blockNode * preBlock;
	blockNode * nextBlock;
	bool reference; // LRU������־
	bool dirty; // ��ʾ�˿�����ı�־���Ժ���Ҫ����д�ش���
	size_t usingSize; // ��ʹ�õ��ֽڴ�С������ܴ�СΪBLOCK_SIZE�����ֵ�洢�ڿ�ͷ����
};

//�ļ��ڵ�B+��
struct fileNode{
	char *fileName;
	bool pin; // ���ļ��������ı�־
	blockNode *blockHead;
	fileNode * nextFile;
	fileNode * preFile;
};


class BufferManager{
public:
	BufferManager();
	~BufferManager();
	void deleteFileNode(const char * fileName); //ɾ���������е��ļ�
	fileNode* getFile(const char* fileName, bool if_pin = false); //�ӻ������л�ȡ�ļ�
	void setDirty(blockNode & block); //����������Ѵ��̻�д
	void setPin(blockNode & block, bool pin); //�����������ļ�����Ϊ��
	void setPin(fileNode & file, bool pin);  //�����������ļ�����Ϊ�ļ�
	void setUsingSize(blockNode & block, size_t usage);
	size_t getUsingSize(blockNode & block);
	char* getContent(blockNode& block);
	static int getBlockSize();

	blockNode* getNextBlock(fileNode * file, blockNode* block);
	blockNode* getBlockHead(fileNode* file);
	blockNode* getBlockByOffset(fileNode* file, int offestNumber);

private:
    fileNode *fileHead;
    fileNode file_pool[MAX_FILE_NUM];
    blockNode block_pool[MAX_BLOCK_NUM];
    int totalBlock; // ��ʹ�õĿ������������ζ�Ÿÿ����б��С�
    int totalFile; // ��ʹ�õ��ļ���������ζ�Ÿ��ļ����б��С�
    void initBlock(blockNode & block); //��ʼ����
    void initFile(fileNode & file); //��ʼ���ļ�
    blockNode* getBlock(fileNode * file,blockNode* position,bool if_pin = false);
    void writtenBackToDiskAll(); //���ڴ��е����п�д�ش���
    void writtenBackToDisk(const char* fileName, blockNode* block); //����ƺõĿ�д�����
    void cleanDirty(blockNode &block); //ͨ��д�ش�����������
    size_t getUsingSize(blockNode* block);
    static const int BLOCK_SIZE = 4096;

};

extern clock_t start;

#endif
