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
	//�����ֵܵ�����
	IndexInfo LIndex;
	IndexInfo RIndex;
	bool IsDirty;
	uint _size;
	uint _order;
public:
	class Pair {
		_KTy key;
		IndexInfo info;
	};
	Pair * Index;
	IndexInfo Parent;
	//����ָ��key�Ľڵ㣬�������ʧ�ܣ�����false�����򷵻�true
	void insert (_KTy key, const IndexInfo &);
	//ɾ��ָ��key�Ľڵ㣬���key�����ڣ�����false���������쳣
	bool erase (_KTy key);
	~BPlusNode ();
	//����ӵ�м�ֵkey��ĳ�ڵ�
	IndexInfo find (_KTy);
	uint getSize () { return _size };
	uint getOrder () { return _order; }
	//��Χ��ѯ
	void find (_KTy min, _KTy max, vector<IndexInfo>& result);
	bool getDirtyState () {
		return IsDirty;
	}
	BPlusNode (uint order, BPlusNode* (&GetNodePtr)(const IndexInfo&) , const IndexInfo (&GetNewNode)() , bool IsDirty = false)
		: GetNodePtr(GetNodePtr), GetNewNode(GetNewNode){
		this->order = order;
		this->IsDirty = IsDirty;
	}
#ifdef DEBUG
	void debug();
#endif
};
