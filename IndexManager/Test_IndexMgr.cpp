#include <iostream>
#include "IndexManager.h"

int main () {
	//比如一个table，它的attribute如下
	//{string Name, int code, float weight, string introduction}
	//现在要用code建立一个索引
	//首先，自定义一个文件名，文件必须不存在，假设是.//a.index，这个index名字为test
	//catalog记录索引信息，test索引的key类型是int
	
	//1.实例化buffer
	BufferManager bMgr;
	string fileName = "DB1.bin";
	//2.实例化indexmanager
	IndexManager iMgr (fileName, bMgr);
	//3.设置表的类型，可以检查返回值是否为true
	iMgr.setIndexInfo (INT);
	
	//使用buffer存入数据时，将返回一个indexinfo，对于主键的自动建立，必须马上把这个indexinfo加到indexmanager里
	//例如
	IndexInfo info = bMgr.createBlock ("table.dat" , 400);
	iMgr.insert (3170103618, info);
	//当主键已经建立，给unique键手动建立索引时，使用find查找需要的indexinfo
	//假设以primary key ——code为key的 index已经建好，文件名是p.index
	IndexManager Finder(".\\p.index", bMgr);
	iMgr.insert ("Kevin", Finder.find (3170103618));




	//iMgr.insert (100001, IndexInfo (4096, 0x23456789));
	IndexInfo tmp = iMgr.find (100001);
	std::cout << tmp._size << endl;
	system ("pause");
	return 0;
}