#include "Interpreter.h"
#include <stdexcept>
#include <cstring>

using namespace std;

Interpreter::Interpreter(){
	//����interpreter
}

Interpreter::~Interpreter(){
	//ɾ��interpreter
}

int Interpreter::interpret(string s){
    int tmp = 0;
	string command;

	//��command��ֵ��һ��ָ��(�����ո�ֹͣ)
	command = getCommand(s, tmp);

	//��������
	if(command == "create"){
		//��command��ֵ�ڶ���ָ��
        command = getCommand(s, tmp);

		//������
		if(command == "table"){
			//��ʼ������������
			string primaryKey = "";
			string tableName = "";

			//��command��ֵ����
			command = getCommand(s, tmp);
			if(!command.empty())	
				//��������
				tableName = command;
			else{
				//�쳣����:û���������
				cout << "Syntax Error! No table name" << endl;
				return 0;
			}
			//��ȡ�������,������ȡһλ��command��ֵ
			command = getCommand(s, tmp);
			if(command.empty() || command != "("){
				//�쳣����:����������,�﷨����
				cout << "Syntax Error! Miss '('" << endl;
				return 0;
			}
			else{
				//��������,������ȡ
				//��ʱ������Ϊ��һ��������
				command = getCommand(s, tmp);
				//����attribute����
				vector<Attribute> attributeVector;
				//����ѭ����ȡ
				while (!command.empty() && command != "primary"
                        && command != ")"){

					//��ʼ�� attributeName,type,ifUnique
					string attributeName = command;
					int type = 0;
					bool ifUnique = false;

					//������������
					//��ȡһλ��ֵ��command
					//int 0;float -1; char >0;
					command = getCommand(s, tmp);
					if(command == "int"){
						type = 0;
                    }
					else if(command == "float"){
						type = -1;
                    }
					else if(command == "char"){
						command = getCommand(s, tmp);
						//char��Ӧ����'('��ʾchar�Ĵ�С
						//�쳣����:û��������
						if (command != "("){
							cout << "Syntax Error: unknown data type" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						istringstream convert(command);
						//type = covert
						if (!(convert >> type)){
							cout << "Syntax error : illegal number in char()" << endl;
							return 0;
						}
						//�쳣����:û��������
						command = getCommand(s, tmp);
						if (command != ")"){
							cout << "Syntax Error: unknown data type" << endl;
							return 0;
						}
					}
					else{
						//��ʱֻ֧��int,float,char������������
						cout << "Syntax Error! Unknown data type" << endl;
						return 0;
					}
					//������ȡ
					command = getCommand(s, tmp);
					//������unique�ؼ���
					//ifUniqueΪ��
					if(command == "unique"){
						ifUnique = true;
                        command = getCommand(s, tmp);
					}
					//B+������
					Attribute attribute(attributeName,type,ifUnique);
					attributeVector.push_back(attribute);
					if(command != ","){
						if(command != ")"){
							cout << "Syntax Error!" << endl;
							return 0;
						}
						else break;
					}
					//ѭ������
					command = getCommand(s, tmp);
				}

				int primaryKeyLocation = 0;
				//������primary
				if(command == "primary"){
					command = getCommand(s, tmp);
					//�쳣����:primary��û��key
					if(command != "key"){
						cout << "Error in syntax!" << endl;
						return 0;
					}
					//��������
					else{
						command = getCommand(s, tmp);
						if(command == "("){
							command = getCommand(s, tmp);
							primaryKey = command;
							int i = 0;
							//����������������unique
							for( i = 0; i < attributeVector.size(); i++){
								if(primaryKey == attributeVector[i].getName()){
									attributeVector[i].setUnique(true);
									break;
								}

							}
							//�쳣����:û�в�ѯ������
							if(i == attributeVector.size()){
								cout << "Syntax Error! primary key does not exist" << endl;
								return 0;
							}
							primaryKeyLocation = i;
							command = getCommand(s, tmp);
							//�쳣����:û��')'
							if(command != ")"){
								cout << "Syntax Error!" << endl;
								return 0;
							}
						}
						//�쳣����:�﷨����
						else{
							cout << "Syntax Error!" << endl;
							return 0;
						}
						command = getCommand(s, tmp);
						//�쳣����:û��������
						if(command != ")"){
							cout << "Syntax Error" << endl;
							return 0;
						}
					}
				}
				//�쳣����:û��������
				else if(command.empty()){
					cout << "Syntax Error!" << endl;
					return 0;
				}
				//����API������
				api->createTable(tableName, &attributeVector, primaryKey, primaryKeyLocation);
				return 1;
			}
		}
		//��������
		else if(command == "index")
		{
			//��ʼ��indexName,tableName,attributeName
			string indexName = "";
			string tableName = "";
			string attributeName = "";

			command = getCommand(s, tmp);
			if(!command.empty()){			
				//��������ֵ
				indexName = command;
            }
			else{
				//�쳣����:��������
				cout << "Syntax Error!" << endl;
				return 0;
			}

			command = getCommand(s, tmp);
			try {
				//�쳣����:�﷨����
				logic_error ex("Syntax Error!");
				if(command != "on")
					throw exception(ex);
				command = getCommand(s, tmp);
				if(command.empty())
					throw exception(ex);
				//��������
				tableName = command;
				command = getCommand(s, tmp);
				//�쳣����:������ȱʧ
				if(command != "(")
					throw exception(ex);
				command = getCommand(s, tmp);
				//�쳣����:��ֵ
				if(command.empty())
					throw exception(ex);
				//��������ֵ
				attributeName = command;
				command = getCommand(s, tmp);
				//�쳣����:��������
				if(command != ")")
					throw exception(ex);
				//����API,��������
				api->createIndex(indexName, tableName, attributeName);
				return 1;
			} 
        catch(exception&) {
				//�쳣����:�﷨����
				cout << "Syntax Error!" << endl;
				return 0;
			}
		}
		//�������ݿ�
		else if(command == "database"){
			string databaseName = "";
			command = getCommand(s,tmp);

			if (!command.empty())
				//��������
				databaseName = command;
			else {
				//�쳣����:û���������ݿ���
				cout << "Syntax Error! No database name" << endl;
				return 0;
			}
			//����API�������ݿ�
			api->createDatabase(databaseName);
		}
		else{
			//�쳣����:�﷨����
			cout << "Syntax Error!" << endl;
			return 0;
		}
		return 0;
	}

	//ѡ������
	else if(command == "select"){
		//��ʼ��string�����ͱ���
		vector<string> attrSelected;
		string tableName = "";
		command = getCommand(s, tmp);

		//�Ƿ�ѡ��ȫ��
		if(command != "*"){
			while(command != "from"){
				attrSelected.push_back(command);
				command = getCommand(s, tmp);
			}
		}
		else{
			command = getCommand(s, tmp);
		}
		//�쳣����:û��from�﷨����
		if(command != "from"){
			cout << "Syntax Error!" << endl;
			return 0;
		}
		
		command = getCommand(s, tmp);
		//��ȡ���,������ֵ
		if(!command.empty())
			tableName = command;
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}

		
		command = getCommand(s, tmp);
		//����API,ѡ����е���
		if(command.empty()){
			if(attrSelected.size() == 0){
				api-> showRecord(tableName);
			}
			else{
				api-> showRecord(tableName, &attrSelected);
			}
			return 1;
		}
		//Where�ؼ��ִ���
		else if(command == "where"){
			//��ʼ��attributeName,value,operateΪequal���
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			//��ʼ��condition����
			std::vector<Condition> conditionVector;
			command = getCommand(s, tmp);
			//ѭ������
			while(1){
				try {
					//�쳣����:ex:�﷨����, ex2:û�бȽ������, ex3:û��ֵ
					logic_error ex("Syntax Error!");
					logic_error ex2("Syntax Error! No operator!");
					logic_error ex3("Syntax Error! No value!");
					if(command.empty())
						throw exception(ex);
					attributeName = command ;
					command = getCommand(s, tmp);
					//���ֱȽ�����,��ҪΪvector�������ݱȽϷ���
					if(command == "<=")
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if(command == ">=")
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if(command == "<")
						operate = Condition::OPERATOR_LESS;
					else if(command == ">")
						operate = Condition::OPERATOR_MORE;
					else if(command == "=")
						operate = Condition::OPERATOR_EQUAL;
					else if(command == "<>")
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw exception(ex2);
					command = getCommand(s, tmp);
					//�쳣����:û��ֵ
					if(command.empty())
						throw exception(ex3);
					value = command;
					//Condition c�Ƚ�
					Condition c(attributeName, value, operate);
					conditionVector.push_back(c);
					command = getCommand(s, tmp);
					//��ȡ���,break
					if(command.empty())
						break;
					//and �ؼ���,�������ȡ
					if(command != "and")
						throw exception(ex);
					command = getCommand(s, tmp);
				} 	
				catch (exception& e) {
					cout << e.what() << endl;
					return 0;
				}
			}
			//����API,�г��������������
			if(attrSelected.size()==0)
				api-> showRecord(tableName, NULL, &conditionVector);
			else
				api->showRecord(tableName, &attrSelected, &conditionVector);
			
			return 1;
		}
	}

	//DROP
	else if(command == "drop"){
		command = getCommand(s, tmp);

		//ɾ����
		if(command == "table"){
			command = getCommand(s, tmp);
			if(!command.empty()){
				api->dropTable(command);
				return 1;
			}
			else{
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		//ɾ������
		else if(command == "index"){
			command = getCommand(s, tmp);
			if(!command.empty()){
				api->dropIndex(command);
				return 1;
			}
			else{
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		//ɾ�����ݿ�
		else if (command == "database") {
			command = getCommand(s, tmp);
			if (!command.empty()) {
				api->dropDatabase(command);
				return 1;
			}
			else {
				cout << "Syntax Error!" << endl;
				return 1;
			}
		}
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}
	}

	//DELETE
	else if(command == "delete"){
		string tableName = "";
		command = getCommand(s, tmp);
		if(command != "from"){
			cout << "Syntax Error!" << endl;
			return 0;
		}

		command = getCommand(s, tmp);
		if(!command.empty())
			tableName = command;
		else{
			cout << "Syntax Error!" << endl;
			return 0;
		}

		// condition extricate
		command = getCommand(s, tmp);
		if(command.empty()){
			api->deleteRecord(tableName);
			return 1;
		}
		else if(command == "where"){
			string attributeName = "";
			string value = "";
			int operate = Condition::OPERATOR_EQUAL;
			std::vector<Condition> conditionVector;
			command = getCommand(s, tmp);		//col1
			while(1){
				try {
					logic_error ex("Syntax Error!");
					if(command.empty())
						throw exception(ex);
					attributeName = command ;
					command = getCommand(s, tmp);
					if(command == "<=")
						operate = Condition::OPERATOR_LESS_EQUAL;
					else if(command == ">=")
						operate = Condition::OPERATOR_MORE_EQUAL;
					else if(command == "<")
						operate = Condition::OPERATOR_LESS;
					else if(command == ">")
						operate = Condition::OPERATOR_MORE;
					else if(command == "=")
						operate = Condition::OPERATOR_EQUAL;
					else if(command == "<>")
						operate = Condition::OPERATOR_NOT_EQUAL;
					else
						throw exception(ex);
					command = getCommand(s, tmp);
					if(command.empty()) // no condition
						throw exception(ex);
					value = command;
					command = getCommand(s, tmp);
					Condition c(attributeName, value, operate);
					conditionVector.push_back(c);
					if(command.empty()) // no condition
						break;
					if(command != "and")
						throw  exception(ex);;
					command = getCommand(s, tmp);
				} 	catch (exception&) {
					cout << "Syntax Error!" << endl;
					return 0;
				}
			}
			api->deleteRecord(tableName, &conditionVector);
			return 1;
		}
	}

	//INSERT
	else if(command == "insert"){
		string tableName = "";
		std::vector<string> valueVector;
		command = getCommand(s, tmp);
		try {
			logic_error ex("Syntax Error!");
			if(strcmp(command.c_str(),"into") != 0)
				throw  exception(ex);
			command = getCommand(s, tmp);
			if(command.empty())
				throw  exception(ex);
			tableName = command;
			command = getCommand(s, tmp);
			if(command != "values")
				throw  exception(ex);
			command = getCommand(s, tmp);
			if(command != "(")
				throw  exception(ex);
			command = getCommand(s, tmp);
			while (!command.empty() && command != ")"){
				valueVector.push_back(command);
				command = getCommand(s, tmp);
				if(command == ",")  // bug here
					command = getCommand(s, tmp);
			}
			if(command != ")")
				throw  exception(ex);
		} catch (exception&){
			cout << "Syntax Error!" << endl;
			return 0;
		}
		api->insertRecord(tableName, &valueVector);
		return 1;
	}

	//ʹ�����ݿ�
	else if (command == "use") {
		command = getCommand(s, tmp);
		if (!command.empty()) {
			//����APIʹ�����ݿ�
			api->useDatabase(command);
			return 1;
		}
		else {
			cout << "Syntax Error!" << endl;
			return 1;
		}

	}

	//�˳�
	else if(command == "quit" || command == "exit"){ 
		return -1;
	}

	//HELP����
	else if (command == "help") {
		return -2;
	}

	//���г���
	else if(command == "execfile"){
		fileName = getCommand(s, tmp);
		cout << "try to open file " << fileName << "." << endl;
		return 2;
	}

	else{
		if(command != "")
			cout << "Error, command " << command << " not found" << endl;
		return 0;
	}
	return 0;
}

string Interpreter::getCommand(string s, int &tmp){
    string command;
    int index1, index2;

	//��ȡ���ʵĵ�һ���±�
	while ((s[tmp] == ' ' || s[tmp] == '\n'  || s[tmp] == '\t') && s[tmp] != 0)
		tmp++;
	index1 = tmp;

	//��ȡ��һ�����ʵĵ�һ���±�
	if(s[tmp] == '(' || s[tmp] == ',' || s[tmp] == ')'){
		tmp++;
		index2 = tmp;
		command = s.substr(index1, index2 - index1);
	}

	else if(s[tmp] == '\''){
		tmp++;
		while (s[tmp] != '\'' && s[tmp] !=0)
			tmp++;

		if(s[tmp] == '\''){
			index1++;
			index2 = tmp;
			tmp++;
			command = s.substr(index1, index2 - index1);
		}
		else
			command = "";
	}

	else{
		while (s[tmp] != ' ' &&s[tmp] != '(' && s[tmp] != '\n' 
                && s[tmp] != 0 && s[tmp] != ')' && s[tmp] != ',')
			tmp++;

		index2 = tmp;
		if(index1 != index2)
			command = s.substr(index1, index2 - index1);

        else
			command = "";
	}

    return command;
}

string Interpreter::getFilename() {
	return fileName;
}

void Interpreter::setAPI(API* apiInput) {
	api = apiInput;
}
