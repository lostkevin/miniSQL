### BufferManager模块说明

#### 一些说明

为了使程序尽可能简洁，该模块仅允许一个实例。

每个模块实例化时获得一个这个模块实例的指针即可

### Usage

+ 向文件中追加内容、新建文件时调用createblock方法，若文件不存在将自动新建并生成文件头（需要提供块大小）
+ 修改文件使用Read/Write即可
+ 删除某个块调用erase
+ 删除整个文件调用drop
+ 需要锁定某个文件时使用setPageState，令它的PIN=true，这样直到被用户释放前都不会写入到磁盘内

#### 成员列表

public 接口:

+ **void readRawData (const string& fileName ,const IndexInfo &info, BYTE * result)** : 读取info指向的信息，
并将这些数据写到result指针指向的内存中
+ **void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData）** : 向info指向的文件写入数据，
若page未读取，先读取page
+ **const IndexInfo createBlock (const string & fileName, uint size)** : 在文件中新建一个数据块，返回索引， 数据块大小必须是
数据储存时的大小，page对齐由buffer内部完成！若文件不存在，新建文件；若文件存在，检查文件头中的size信息与给定的参数是否
一致，不一致抛异常
+ **void erase (const IndexInfo &info, const string& fileName)** : 擦除file中info指向的Block
+ **void drop (const string & fileName)** : 删除文件，并释放所有该文件的Page
+ **bool IsFileExist(const string& fileName)** : 判断该文件是否存在
+ **void setPageState (const string &fileName, const IndexInfo &info)** ：锁定某个索引指向的数据块，避免这个数据块被释放。
### 关于文件储存

@SK catalog的信息请自己保存读取，不需要通过buffer

**通用格式**：data文件，index文件

每个文件均是page大小的整数倍，下面用0x??的格式表示page的地址

0x00 文件头，储存每个数据块的大小以及freelist的信息

读取文件头数据可以按照下列格式调用

bufferMgr.readRawData (fileName, IndexInfo (), header);

修改文件头时，前十六字节不会被写入

在文件头中（以下是字节地址）
> 0x00 - 0x07 文件的blocksize（Buffer使用）
> 0x08 - 0x0F 首个freelist的地址（buffer使用）
> 0x10 - 0x2000 客户区，各模块可以自行分配

0x01~ 数据区
+ index文件 : 建立时要求一个node尽可能接近pagesize，保证一次IO读入一个node并使得order尽可能大（使B+树尽可能粗壮），每个node保存
着key-indexinfo对，为了数据安全，储存indexinfo而非单纯的offset
+ data文件 : 数据page内储存了一个到数个tuple，这个数目由BufferManager决定（需给出tuple的size）

### Buffer结构

BufferManager 仅包含两个成员:Buffer\BufferIndex,Buffer将整个缓冲区建立在最小堆上，保证了替换从尽可能早读入的page开始

Buffer除了保存整个缓冲池的相关属性以外，核心是一个Page *Array

BufferIndex使用STL库，降低索引Node的复杂度，便于快速判断给定的IndexInfo包含的块是否已读入内存，两次索引后也能从
BufferIndex中获得一个Page指针

Page保存着从磁盘读入的数据，并直接在这些数据上进行读写，当Page被销毁时，Page维护的数据将会自动更新到文件中



