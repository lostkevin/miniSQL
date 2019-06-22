#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "..\miniSQL\Commonheader.h"
#define Asize 32
using namespace std;

struct Error {
	bool isError;
	string info;
};

struct Attribute {		//��������Ե���Ϣ
	string attr_name;	//������
	//�������ͣ�defined as below: 0 int 1 float 2~256 (char(1) to char(255)
	int attr_type;		
	//��tuple�е�λ��
	uint offset;
	bool primary;
	bool unique;
};

struct index {
	string index_name;
	string index_file;
	//��ʾ�ǵڼ���attribute��Ϊkey
	uint keyID;
	//�������ͣ�defined as below: 0 int 1 float 2~256 (char(1) to char(255)
	int index_type;
};

//Table��洢����Ϣ
//��ָ�룬ʹ��ǳ����
class Table {		
	friend class CatalogManager;
	//��ȡ�ض���������Ϣ
	index* getIndexinfo (string index_name);
	//ɾ�����������ļ��������ļ�
	void release ();
	//������������Ϣ
	Attribute &getPrimaryKeyInfo ();
public:
	//����
	string tablename;
	//���Ӧ���ļ�
	string table_fileName;
	//tuple����
	Attribute attr[Asize];
	//������
	uint attr_num;	
	//��������,����ñ�����index��Ϣ
	vector<index> IndexBasic;
	Table () {
	}
	//���ַ����ж�ȡtable��Ϣ
	Table (string TableData);
	//��table��Ϣ���浽�ַ�����
	string ToString ();
	//��֪����ʲô�ã����ĳ�����Ƿ����
	bool IsAttrExist (string attr_name);
};

class CatalogManager {
	map<string, Table> DatabaseInfo;
	CatalogManager ();
	//��֪����ʲô�ã����ĳ�����Ƿ����
	bool CheckAttrExist (string tablename, string attrname);
	bool CheckIndexExist (string index_name, string table_name);
	Table* getTableinfo (string table_name);
public:
	//��ȡ����table��attribute��Ϣ
	void getAttrInfo (string table_name, vector<Attribute>& attributsInfo);
	//�ڱ��ϴ���������ʧ�ܷ��ش�����Ϣ
	Error CreateIndex (index index, string table_name);
	//ɾ���ض����������ļ���ʧ�ܷ��ش�����Ϣ
	Error Dropindex (string index_name);
	//�����±�ʧ�ܷ��ش�����Ϣ
	Error CreateTable (Table info);
	//ɾ��ĳ����������ļ���ʧ�ܷ��ش�����Ϣ
	Error DropTable (string table_name);
	//����ʱ���ļ��ж�ȡ���ݿ���Ϣ
	CatalogManager ();
	//����ʱ�����ݿ���Ϣ���浽Ӳ����
	~CatalogManager ();
};

