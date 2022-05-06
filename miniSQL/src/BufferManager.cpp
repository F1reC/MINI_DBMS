#include "BufferManager.h"
#include <cstring>

using namespace std;

BufferManager::BufferManager():totalBlock(0),totalFile(0),fileHead(NULL){
    for (int i = 0; i < MAX_FILE_NUM; i ++){
        file_pool[i].fileName = new char[MAX_FILE_NAME];
        if(file_pool[i].fileName == NULL){
            cout << "�ڳ�ʼ���ļ���ʱ���ܷ����ڴ�!" <<endl;
            exit (1);
        }
        initFile(file_pool[i]);//��ʼ���ļ���
    }
    for (int i = 0; i < MAX_BLOCK_NUM; i ++) {
        block_pool[i].address = new char[BLOCK_SIZE];
        if(block_pool[i].address == NULL){
            cout <<"�ڳ�ʼ�����ʱ���ܷ����ڴ�!"<<endl;
            exit (1);
        }
        block_pool[i].fileName = new char[MAX_FILE_NAME];
        if(block_pool[i].fileName == NULL){
            cout<<"�ڳ�ʼ�����ʱ���ܷ����ڴ�!"<<endl;
            exit (1);
        }
        initBlock(block_pool[i]);//��ʼ�����
    }
}

BufferManager::~BufferManager(){
    writtenBackToDiskAll();//д���ļ�
    for (int i = 0; i < MAX_FILE_NUM; i ++){
        delete [] file_pool[i].fileName;
    }
    for (int i = 0; i < MAX_BLOCK_NUM; i ++){
        delete [] block_pool[i].address;
    }
}

//��ʼ���ļ�
void BufferManager::initFile(fileNode &file){
    file.nextFile = NULL;
    file.preFile = NULL;
    file.blockHead = NULL;
    file.pin = false;
    memset(file.fileName,0,MAX_FILE_NAME);
}

//��ʼ����
void BufferManager::initBlock(blockNode &block){
    //��ʼ���ڴ���ܴ�С4096
    memset(block.address,0,BLOCK_SIZE);
    size_t init_usage = 0;
    //size_t��С���Ա�֤�洢�ڴ��ж���Ĵ�С
    memcpy(block.address, (char*)&init_usage, sizeof(size_t)); //���ÿ�ͷ
    block.usingSize = sizeof(size_t);
    block.dirty = false;
    block.nextBlock = NULL;
    block.preBlock = NULL;
    block.offsetNum = -1;
    block.pin = false;
    block.reference = false;
    block.ifbottom = false;
    memset(block.fileName,0,MAX_FILE_NAME);
}

//�����ļ�fileNode
fileNode* BufferManager::getFile(const char * fileName, bool if_pin){
    blockNode * btmp = NULL;
    fileNode * ftmp = NULL;
    if(fileHead != NULL){
        //�ļ�����
        for(ftmp = fileHead;ftmp != NULL;ftmp = ftmp->nextFile){
            //�ȶ��ļ���
            if(!strcmp(fileName, ftmp->fileName)){ 
				//fileNode�Ѿ����б���
                ftmp->pin = if_pin;
                return ftmp;
            }
        }
    }
    // fileNode�����б���
    if(totalFile == 0){ 
		// �����б���û���ļ�
        ftmp = &file_pool[totalFile];
        totalFile ++;
        fileHead = ftmp;
    }
    else if(totalFile < MAX_FILE_NUM){ 
		// �ļ������пյ��ļ��ڵ�
        ftmp = &file_pool[totalFile];
        // �����fileNode��ӵ��б��β��
        file_pool[totalFile-1].nextFile = ftmp;
        ftmp->preFile = &file_pool[totalFile-1];//����ǰ���ڵ�
        totalFile ++;
    }
    else {
		// ��totalFile >= MAX_FILE_NUM,�ҵ�һ��Ҫ�滻��fileNode��д�����ڸ�fileNode�Ŀ�ڵ�
        ftmp = fileHead;
        while(ftmp->pin){
            if(ftmp -> nextFile)
                ftmp = ftmp->nextFile;
            else {
				//����û���㹻���ļ��ڵ�
                cout<<"�ļ�����û���㹻���ļ��ڵ�!";
                exit(2);
            }
        }
        for(btmp = ftmp->blockHead;btmp != NULL;btmp = btmp->nextBlock){
            if(btmp->preBlock){
                initBlock(*(btmp->preBlock));
                totalBlock --;//�ܿ�����1
            }
            writtenBackToDisk(btmp->fileName,btmp);//д���ļ�
        }
        initFile(*ftmp);//��ʼ���ļ�
    }
    if(strlen(fileName) + 1 > MAX_FILE_NAME){
        cout <<"�ļ����������ֵ " << MAX_FILE_NAME<<endl;
        exit(3);
    }
    strncpy(ftmp->fileName, fileName,MAX_FILE_NAME);//�����ļ���
    setPin(*ftmp, if_pin);//ͬ������״̬
    return ftmp;
}

//����BlockNode
blockNode* BufferManager::getBlock(fileNode * file,blockNode *position, bool if_pin){
    const char * fileName = file->fileName;
    blockNode * btmp = NULL;
    if(totalBlock == 0){// �����б���û�п�
        btmp = &block_pool[0];
        totalBlock ++;//��������1
    }
    else if(totalBlock < MAX_BLOCK_NUM){ // ����д��ڿտ�
        for(int i = 0 ;i < MAX_BLOCK_NUM;i ++){
            if(block_pool[i].offsetNum == -1){
                btmp = &block_pool[i];
                totalBlock ++;//��������1
                break;
            }
            else
                continue;
        }
    }
    else{ // totalBlock >= MAX_BLOCK_NUM,����ζ��û�пտ飬�������Ǳ����滻һ����
        int i = replaced_block;
        while (true){
            i ++;
            if(i >= totalBlock) i = 0;
            if(!block_pool[i].pin){//���ÿ�û������
                if(block_pool[i].reference == true)
                    block_pool[i].reference = false;
                else {
					//ѡ��˿���и���
                    btmp = &block_pool[i];
                    if(btmp->nextBlock) btmp -> nextBlock -> preBlock = btmp -> preBlock;
                    if(btmp->preBlock) btmp -> preBlock -> nextBlock = btmp -> nextBlock;
                    if(file->blockHead == btmp) file->blockHead = btmp->nextBlock;
                    replaced_block = i; //��¼���滻�Ŀ飬�´δ���һ���鿪ʼ��
                    
                    writtenBackToDisk(btmp->fileName, btmp);//д���ļ�
                    initBlock(*btmp);//��ʼ����
                    break;
                }
            }
            else // �˿��ѱ�����
                continue;
        }
    }
    //������ӵ����б���
    if(position != NULL && position->nextBlock == NULL){//���б�ǿգ�����һ��Ϊ�տ�
        btmp -> preBlock = position;
        position -> nextBlock = btmp;
        btmp -> offsetNum = position -> offsetNum + 1;
    }
    else if (position !=NULL && position->nextBlock != NULL){//���б�ǿգ�����һ��Ϊ�ǿտ�
        btmp->preBlock=position;
        btmp->nextBlock=position->nextBlock;
        position->nextBlock->preBlock=btmp;
        position->nextBlock=btmp;
        btmp -> offsetNum = position -> offsetNum + 1;
    }
    else {
		// �齫���б��ͷ
        btmp -> offsetNum = 0;
        if(file->blockHead){
			// ����ļ���һ������Ŀ�ͷ,�������ͷ
            file->blockHead -> preBlock = btmp;
            btmp->nextBlock = file->blockHead;
        }
        file->blockHead = btmp;
    }
    setPin(*btmp, if_pin);//��������״̬
    if(strlen(fileName) + 1 > MAX_FILE_NAME){
        cout << "�ļ����������ֵ "<<MAX_FILE_NAME<<endl;
        exit(3);
    }
    strncpy(btmp->fileName, fileName, MAX_FILE_NAME);//�����ļ���
    
    //��ȡ�ļ����ݵ���
    FILE * fileHandle;
    if((fileHandle = fopen(fileName, "ab+")) != NULL){
        if(fseek(fileHandle, btmp->offsetNum*BLOCK_SIZE, 0) == 0){
            if(fread(btmp->address, 1, BLOCK_SIZE, fileHandle)==0)
                btmp->ifbottom = true;
            btmp ->usingSize = getUsingSize(btmp);
        }
        else{
            cout<<"�����ļ� "<<fileName<<" ���ֶ�ȡ����"<<endl;
            exit(1);
        }
        fclose(fileHandle);
    }
    else{
        cout<<"�����ļ� "<<fileName<<" ʱ��������"<<endl;
        exit(1);
    }
    return btmp;
}

//д���ļ���Ӳ��,(ָ���ļ���)
void BufferManager::writtenBackToDisk(const char* fileName,blockNode* block){
    if(!block->dirty) // �����û�б��޸Ĺ�������������Ҫд���ļ�
        return;
    else{ 
		// д���ļ�
        FILE * fileHandle = NULL;
        if((fileHandle = fopen(fileName, "rb+")) != NULL){
            if(fseek(fileHandle, block->offsetNum*BLOCK_SIZE, 0) == 0){
                if(fwrite(block->address, block->usingSize+sizeof(size_t), 1, fileHandle) != 1){
                    cout<<"д���ļ� "<<fileName<<" ʱ��������";
                    exit(1);
                }
            }
            else{
                cout<<"�����ļ� "<<fileName<<" ʱ��������"<<endl;
                exit(1);
            }
            fclose(fileHandle);
        }
        else{
            cout << "���ļ� "<<fileName<<" ʱ��������"<<endl;
            exit(1);
        }
    }
}

//д��ȫ���ļ���Ӳ��
void BufferManager::writtenBackToDiskAll(){
    blockNode *btmp = NULL;
    fileNode *ftmp = NULL;
    if(fileHead){//�ļ�ͷ��Ϊ��
        for(ftmp = fileHead;ftmp != NULL;ftmp = ftmp ->nextFile){
            if(ftmp->blockHead){//��ͷ��Ϊ��
                for(btmp = ftmp->blockHead;btmp != NULL;btmp = btmp->nextBlock){
                    if(btmp->preBlock)initBlock(*(btmp -> preBlock));//��ʼ����
                    writtenBackToDisk(btmp->fileName, btmp);//д���ļ�
                }
            }
        }
    }
}

//��ȡ��һ����BLOCK
blockNode* BufferManager::getNextBlock(fileNode* file,blockNode* block){
    if(block->nextBlock == NULL){//�������ڵ�Ϊ�տ�
        if(block->ifbottom) block->ifbottom = false;// �ļ��ڵ�����ı�־��Ϊfalse
        return getBlock(file, block);
    }
    else{ 
		//�������ڵ�Ϊ�ǿտ�
        if(block->offsetNum == block->nextBlock->offsetNum - 1)
            return block->nextBlock;
        else //��ֹ�б��˳�򲻶�
            return getBlock(file, block);
    }
}

//��ȡ��ͷBLockHead
blockNode* BufferManager::getBlockHead(fileNode* file){
    blockNode* btmp = NULL;
    if(file->blockHead != NULL){//���ļ��п�ͷ
        if(file->blockHead->offsetNum == 0){ 
			//��һ�������ƫ����
            btmp = file->blockHead;
        }
        else
            btmp = getBlock(file, NULL);
    }
    else // ����ļ�û�п�ͷ����Ϊ���ȡһ���µĿ�ڵ�
        btmp = getBlock(file,NULL);
    return btmp;
}

//��ȡ��(ָ��offset)
blockNode* BufferManager::getBlockByOffset(fileNode* file, int offsetNumber){
    blockNode* btmp = NULL;
    if(offsetNumber == 0) return getBlockHead(file);//��ƫ����Ϊ0
    else{
        btmp = getBlockHead(file);
        while( offsetNumber > 0){
            btmp = getNextBlock(file, btmp);//��Ϊ��һ��
            offsetNumber --;//ƫ������1
        }
        return btmp;
    }
}

//ɾ���ļ��ڵ�
void BufferManager::deleteFileNode(const char * fileName){
    fileNode* ftmp = getFile(fileName);
    blockNode* btmp = getBlockHead(ftmp);
    queue<blockNode*> blockQ;
    while (true) {
        if(btmp == NULL) return;
        blockQ.push(btmp);//ѹ�����
        if(btmp->ifbottom) break;//���ļ��ڵ����
        btmp = getNextBlock(ftmp,btmp);//ָ����һ����
    }
    totalBlock -= blockQ.size();//�����ܿ���
    while(!blockQ.empty()){//�����зǿ�
        initBlock(*blockQ.back());//��ʼ����
        blockQ.pop();//�Ӷ����е���
    }
    if(ftmp->preFile) ftmp->preFile->nextFile = ftmp->nextFile;
    if(ftmp->nextFile) ftmp->nextFile->preFile = ftmp->preFile;
    if(fileHead == ftmp) fileHead = ftmp->nextFile;
    initFile(*ftmp);
    totalFile --;
}

//���ÿ�PIN��,��û������reference,LRU������־Ϊtrue
void BufferManager::setPin(blockNode &block,bool pin){
    block.pin = pin;
    if(!pin)
        block.reference = true;
}

//�����ļ�PIN��
void BufferManager::setPin(fileNode &file,bool pin){
    file.pin = pin;
}

//���ô˿�Ϊ��
void BufferManager::setDirty(blockNode &block){
    block.dirty = true;
}

//���ô˿�Ϊ����
void BufferManager::cleanDirty(blockNode &block){
    block.dirty = false;
}

//���ؿ�ʹ�õ�ַ�ռ��С
size_t BufferManager::getUsingSize(blockNode* block){
    return *(size_t*)block->address;
}

//���ÿ�ʹ�ÿռ�
void BufferManager::setUsingSize(blockNode & block,size_t usage){
    block.usingSize = usage;
    memcpy(block.address,(char*)&usage,sizeof(size_t));
}

//���ؿ�ʹ�õ��ֽڴ�С
size_t BufferManager::getUsingSize(blockNode & block){
    return block.usingSize;
}

//���ص�ַ����
char* BufferManager::getContent(blockNode& block){
    return block.address + sizeof(size_t);
}

//��ȡ��������ʹ�õĿ�Ĵ�С����������ʹ�ÿ�ͷ
int BufferManager::getBlockSize() {
	return BLOCK_SIZE - sizeof(size_t);
}
