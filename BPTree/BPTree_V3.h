#pragma once
#define DEBUG
#include <vector>
using namespace std;

//����������
//ÿ���ڵ����һ��������ʾ�ýڵ��Ƿ�Ϊleaf
template<typename _KTy>
class BPlusNode {
	uint order;
	//���ָ��ĳһ���ڵ��ָ�룬�ýڵ���������Ϣȷ��
	BPlusNode* (*GetNodePtr)(const IndexInfo&); 
	//���һ����node
	const IndexInfo (*GetNewNode)();
	//������
	Pair *index;
	//�����ֵܵ�����
	IndexInfo LIndex;
	IndexInfo RIndex;
public:
	class Pair {
		_KTy key;
		IndexInfo info;
	};
	//����ָ��key�Ľڵ㣬�������ʧ�ܣ�����false�����򷵻�true
	bool insert (_KTy key, const IndexInfo &);
	//ɾ��ָ��key�Ľڵ㣬���key�����ڣ�����false���������쳣
	bool erase (_KTy key);
	~BPlusNode ();
	//����ӵ�м�ֵkey��ĳ�ڵ�
	IndexInfo find (_KTy);
	//��Χ��ѯ
	vector<IndexInfo> find (_KTy min, _KTy max);
	BPlusNode (uint order, BPlusNode* (*)(const IndexInfo&) GetNodePtr, const IndexInfo (*)() GetNewNode);
#ifdef DEBUG
	void debug();
#endif
};
