### IndexManagerģ��˵��

#### һЩ˵��

һ��IndexManagerʵ��������ϵ�һ�������ļ���Ӧ

Key��stringʱ��Ҫ�����С�������ַ�������ֹ��

#### How To Create a Index

> ������һ�����ļ� ʵ����Index Manager
��ʱ ��ʵ����setIndexInfo()���Ե��ã�����Index�Ļ�����Ϣ�����óɹ����᷵��true����ʱ�ܶԸ�Managerʹ��insert��erase
��find�����ˡ�


#### IndexNode��ʽ

�ļ�ͷ�б���������Ϣ��
+ blocksize (bufferMgr) | 0x00 - 0x07
+ Freelist  (bufferMgr) | 0x08 - 0x0F
+ IndexType (IndexMgr ) | sizeof(TreeType)
+ KeySize   (IndexMgr ) | sizeof(uint) [int, float]:4 [string]:length
+ order     (IngexMgr ) | sizeof(uint) Ϊ�˼��ټ���
+ rootPos   (IndexMgr ) | sizeof(IndexInfo)

ÿ��Node�򱣴�������Ϣ (if not deleted) 
+ Parent sizeof(IndexInfo)
+ LSib, RSib 2 * sizeof(IndexInfo)
+ KeySize [int, float]:4 [string]:size() sizeof(uint)
+ 0x30 - Key - Index�ԣ�sizeof(IndexInfo) + value(KeySize)

#### ģ�鹦��

IndexManager �����½������ļ���ɾ�������ļ������ṩ������ѯ�������޸Ľӿڣ�ȷ������������

��ģ��Index���ļ��е�Node����ת����B+Node�����ڴ��ж���������ز�������ģ�����޸�rootPos��
�������޸������ļ��Ļ�����Ϣ