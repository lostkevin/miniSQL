### IndexManager模块说明

#### 一些说明

一个IndexManager实例与磁盘上的一个索引文件对应

#### How To Create a Index

首先用一个空文件 实例化Index Manager
这时 该实例的setIndexInfo()可以调用，设置Index的基本信息。设置成功将会返回true，此时能对该Manager使用insert、erase
、find方法了。


