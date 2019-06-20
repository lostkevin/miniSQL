#include <iostream>
#include "IndexManager.h"

int main () {
	//����һ��table������attribute����
	//{string Name, int code, float weight, string introduction}
	//����Ҫ��code����һ������
	//���ȣ��Զ���һ���ļ������ļ����벻���ڣ�������.//a.index�����index����Ϊtest
	//catalog��¼������Ϣ��test������key������int
	
	//1.ʵ����buffer
	BufferManager bMgr;
	string fileName = "DB1.bin";
	//2.ʵ����indexmanager
	IndexManager iMgr (fileName, bMgr);
	//3.���ñ�����ͣ����Լ�鷵��ֵ�Ƿ�Ϊtrue
	iMgr.setIndexInfo (INT);
	
	//ʹ��buffer��������ʱ��������һ��indexinfo�������������Զ��������������ϰ����indexinfo�ӵ�indexmanager��
	//����
	IndexInfo info = bMgr.createBlock ("table.dat" , 400);
	iMgr.insert (3170103618, info);
	//�������Ѿ���������unique���ֶ���������ʱ��ʹ��find������Ҫ��indexinfo
	//������primary key ����codeΪkey�� index�Ѿ����ã��ļ�����p.index
	IndexManager Finder(".\\p.index", bMgr);
	iMgr.insert ("Kevin", Finder.find (3170103618));




	//iMgr.insert (100001, IndexInfo (4096, 0x23456789));
	IndexInfo tmp = iMgr.find (100001);
	std::cout << tmp._size << endl;
	system ("pause");
	return 0;
}