### IndexManager模块说明

#### 一些说明

一个IndexManager实例与磁盘上的一个索引文件对应

Key是string时需要传入大小，包括字符串的终止符

#### How To Create a Index

> 首先用一个空文件 实例化Index Manager
这时 该实例的setIndexInfo()可以调用，设置Index的基本信息。设置成功将会返回true，此时能对该Manager使用insert、erase
、find方法了。


#### IndexNode格式

文件头中保存以下信息：
+ blocksize (bufferMgr) | 0x00 - 0x07
+ Freelist  (bufferMgr) | 0x08 - 0x0F
+ IndexType (IndexMgr ) | sizeof(TreeType)
+ KeySize   (IndexMgr ) | sizeof(uint) [int, float]:4 [string]:length
+ order     (IngexMgr ) | sizeof(uint) 为了减少计算
+ rootPos   (IndexMgr ) | sizeof(IndexInfo)

每个Node则保存以下信息 (if not deleted) 
+ Parent sizeof(IndexInfo)
+ LSib, RSib 2 * sizeof(IndexInfo)
+ KeySize [int, float]:4 [string]:size() sizeof(uint)
+ 0x30 - Key - Index对：sizeof(IndexInfo) + value(KeySize)

#### 模块功能

IndexManager 负责新建索引文件，删除索引文件，并提供索引查询、索引修改接口，确定索引的类型

子模块Index将文件中的Node数据转换成B+Node，在内存中对它进行相关操作，子模块能修改rootPos，
但不能修改索引文件的基本信息