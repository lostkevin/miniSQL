#pragma once
#ifndef uint
#define uint unsigned int
#endif // !uint
#define DEBUG

#include <vector>
#include <string>
using namespace std;
enum NodeType { LEAF, NONLEAF };

class IndexInfo {

};


//����������
//ÿ���ڵ����һ��������ʾ�ýڵ��Ƿ�Ϊleaf
template<typename _KTy>
class BPluseNode {
	uint order;
	IndexInfo LIndex;
	IndexInfo RIndex;
	//���ָ��ĳһ���ڵ��ָ�룬�ýڵ���������Ϣȷ��
	BPluseNode* (*GetNodePtr)(const IndexInfo&); 
	//���һ����node
	const IndexInfo (*GetNewNode)();
	//������
	Pair *index;
	//�����ֵܵ�����
public:
	class Pair {
		_KTy key;
		IndexInfo info;
	};
	//����ָ��key�Ľڵ㣬�������ʧ�ܣ�����false�����򷵻�true
	bool insert (_KTy key, const IndexInfo &);
	//ɾ��ָ��key�Ľڵ㣬���key�����ڣ�����false���������쳣
	bool erase (_KTy key);
	~BPluseNode ();
	//����ӵ�м�ֵkey��ĳ�ڵ�
	IndexInfo find (_KTy);
	//��Χ��ѯ
	vector<IndexInfo> find (_KTy min, _KTy max);
	BPluseNode (uint order, BPluseNode* (*)(const IndexInfo&) GetNodePtr, const IndexInfo (*)() GetNewNode);
#ifdef DEBUG
	void debug();
#endif
};
