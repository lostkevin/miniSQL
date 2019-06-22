//#ifndef API_H
//#define API_H
//#include "../interpreter/basic.h"
//#include "../catalog/catalog.h"
//#include "../IndexManager/IndexManager.h"
//using namespace std;
////API�ӿڡ���ΪInterpreter������͵ײ�Manager���ӵĽӿ�
////������������й��ܽӿ�
////APIֻ�������ĳ������жϣ�������쳣�׳����ɸ��ײ�Manager���
//class API {
//public:
//	//���캯��
//	API();
//	//��������
//	~API();
//	//���ذ�������Ŀ����������Where�����ļ�¼�ı�
//	Table selectRecord(string table_name, vector<string> target_attr, vector<Where> where, char operation);
//	//ɾ����Ӧ�����µ�Table�ڼ�¼(��ɾ�����ļ�)
//	int deleteRecord(string table_name, string target_attr, Where where);
//	//���Ӧ���ڲ���һ����¼
//	void insertRecord(string table_name, Tuple& tuple);
//	//�����ݿ��в���һ�����Ԫ��Ϣ
//	bool createTable(string table_name, Attribute attribute, int primary, Index index);
//	//�����ݿ���ɾ��һ�����Ԫ��Ϣ�����������м�¼(ɾ�����ļ�)
//	bool dropTable(string table_name);
//	//�����ݿ��д���������Ϣ
//	bool createIndex(string table_name, string index_name, string attr_name);
//	//ɾ����Ӧ��Ķ�Ӧ�����ϵ�����
//	bool dropIndex(string table_name, string index_name);
//private:
//	//˽�к��������ڶ�������ѯʱ��and�����ϲ�
//	Table unionTable(Table &table1, Table &table2, string target_attr, Where where);
//	//˽�к��������ڶ�������ѯʱ��or�����ϲ�
//	Table joinTable(Table &table1, Table &table2, string target_attr, Where where);
//
//protected:
//	//Table table;
//	//index iindex;
//	//IndexManager index;
//	//BufferManager buffer;
//	RecordManager record;
//	CatalogManager catalog;
//};
//
////���ڶ�vector��sortʱ����
//bool sortcmp(const Tuple &tuple1, const Tuple &tuple2);
////���ڶ�vector�Ժϲ�ʱ������
//bool calcmp(const Tuple &tuple1, const Tuple &tuple2);
//bool isSatisfied(Tuple& tuple, int target_attr, Where where);
//#endif // !API_H
