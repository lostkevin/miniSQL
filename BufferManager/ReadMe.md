### BufferManagerģ��˵��

#### һЩ˵��

Ϊ��ʹ���򾡿��ܼ�࣬��ģ�������һ��ʵ����

ÿ��ģ��ʵ����ʱ���һ�����ģ��ʵ����ָ�뼴��

#### ��Ա�б�

public �ӿ�:

+ **void readRawData (const string& fileName ,const IndexInfo &info, BYTE * result)** : ��ȡinfoָ�����Ϣ��
������Щ����д��resultָ��ָ����ڴ���
+ **void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData��** : ��infoָ����ļ�д�����ݣ�
��pageδ��ȡ���ȶ�ȡpage
+ **const IndexInfo createBlock (const string & fileName, uint size)** : ���ļ����½�һ�����ݿ飬���������� ���ݿ��С������
���ݴ���ʱ�Ĵ�С��page������buffer�ڲ���ɣ����ļ������ڣ��½��ļ������ļ����ڣ�����ļ�ͷ�е�size��Ϣ������Ĳ����Ƿ�
һ�£���һ�����쳣
+ **void erase (const IndexInfo &info, const string& fileName)** : ����file��infoָ���Block
+ **void drop (const string & fileName)** : ɾ���ļ������ͷ����и��ļ���Page

### �����ļ�����

@SK catalog����Ϣ���Լ������ȡ������Ҫͨ��buffer

**ͨ�ø�ʽ**��data�ļ���index�ļ�

ÿ���ļ�����page��С����������������0x??�ĸ�ʽ��ʾpage�ĵ�ַ

0x00 �ļ�ͷ������ÿ�����ݿ�Ĵ�С�Լ�freelist����Ϣ

0x01~ ������

+ index�ļ� : ����ʱҪ��һ��node�����ܽӽ�pagesize����֤һ��IO����һ��node��ʹ��order�����ܴ�ʹB+�������ܴ�׳����ÿ��node����
��key-indexinfo�ԣ�Ϊ�����ݰ�ȫ������indexinfo���ǵ�����offset
+ data�ļ� : ����page�ڴ�����һ��������tuple�������Ŀ��BufferManager�����������tuple��size��

### Buffer�ṹ

BufferManager ������������Ա:Buffer\BufferIndex,Buffer��������������������С���ϣ���֤���滻�Ӿ�����������page��ʼ

Buffer���˱�����������ص�����������⣬������һ��Page *Array

BufferIndexʹ��STL�⣬��������Node�ĸ��Ӷȣ����ڿ����жϸ�����IndexInfo�����Ŀ��Ƿ��Ѷ����ڴ棬����������Ҳ�ܴ�
BufferIndex�л��һ��Pageָ��

Page�����ŴӴ��̶�������ݣ���ֱ������Щ�����Ͻ��ж�д����Page������ʱ��Pageά�������ݽ����Զ����µ��ļ���



