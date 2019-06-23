#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "..\miniSQL\Commonheader.h"
#include "..\interpreter\exception.h"

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
	bool IsAttrExist (string attr_name);
};

class CatalogManager {
	map<string, Table> DatabaseInfo;
	//��֪����ʲô�ã����ĳ�����Ƿ����

	Table* getTableinfo (string table_name);
public:
	//��ȡ����table��attribute��Ϣ
	void getAttrInfo (string table_name, vector<Attribute>& attributsInfo);
	//��ȡtable��index��Ϣ
	void getIndex (string table_name, vector<index>& index);
	//��ȡkey�Ƿ�Ϊunique����������ȷ����false
	bool getUniqueState (string attr_name, string table_name);
	//��ȡkeyID�������Ͻ�����ʱ������˳��ΪkeyID����Ϊ�˷�ֹ�������ṩһ���ӿ�
	int getKeyID (string attr_name, string table_name);
	//�ڱ��ϴ���������ʧ�ܷ��ش�����Ϣ
	void CreateIndex (index index, string table_name);
	//ɾ���ض����������ļ���ʧ�ܷ��ش�����Ϣ
	void Dropindex (string index_name);
	//�����±�ʧ�ܷ��ش�����Ϣ
	void CreateTable (Table info);
	//ɾ��ĳ����������ļ���ʧ�ܷ��ش�����Ϣ
	void DropTable (string table_name);
	//����ʱ���ļ��ж�ȡ���ݿ���Ϣ
	CatalogManager ();
	//����ʱ�����ݿ���Ϣ���浽Ӳ����
	~CatalogManager ();
	bool CheckTableExist (string table_name);
	bool CheckAttrExist (string table_name, string attrname);
	bool CheckIndexExist (string index_name, string table_name);
};

