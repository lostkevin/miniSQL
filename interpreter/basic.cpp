#include "./basic.h"
#include <ostream>
using namespace std;

Tuple_s::Tuple_s(const Tuple_s &tuple_in) {
	for (int index = 0; index < tuple_in.data_.size(); index++)
	{
		this->data_.push_back(tuple_in.data_[index]);
	}
}

inline int Tuple_s::getSize() {
	return (int)data_.size();
}

//新增数据
void Tuple_s::addData(Data data_in) {
	this->data_.push_back(data_in);
}

bool Tuple_s::isDeleted() {
	return isDeleted_;
}

void Tuple_s::setDeleted() {
	isDeleted_ = true;
}

//得到元组中的数据
std::vector<Data> Tuple_s::getData() const {
	return this->data_;
}

//ostream & operator << (ostream &out, const string &s)
//{
//	out << s;
//	return out;
//} //对<< 进行重载。

void Tuple_s::showTuple(vector<Attribute>attr_info) {
	for (int index = 0; index < getSize(); index++) {
		if (data_[index].type == -1)throw exception ();
		if (index > 0 && attr_info[index - 1].attr_name.length > 8) {
			std::cout << '\t';
		}
		if (data_[index].type == 0)
			std::cout << data_[index].datai << '\t';
		else if (data_[index].type == 1) {
			std::cout << data_[index].dataf << '\t';
		}
		else {
			cout << data_[index].datas << '\t';
		}
			
	}
	std::cout << std::endl;
}
void Tuple_s::showTuple() {
	for (int index = 0; index < getSize(); index++) {
		if (data_[index].type == -1)throw exception();
		if (data_[index].type == 0)
			std::cout << data_[index].datai << '\t';
		else if (data_[index].type == 1) {
			std::cout << data_[index].dataf << '\t';
		}
		else {
			cout << data_[index].datas << '\t';
		}

	}
	std::cout << std::endl;
}

//table构造函数
Table_s::Table_s(std::string title, Attribute_s attr) {
	this->title_ = title;
	this->attr_ = attr;
	this->index_.num = 0;
}

//table的构造函数，拷贝用
Table_s::Table_s(const Table_s &table_in) {
	this->attr_ = table_in.attr_;
	this->index_ = table_in.index_;
	this->title_ = table_in.title_;
	for (int index = 0; index < tuple_.size(); index++)
		this->tuple_.push_back(table_in.tuple_[index]);
}

//插入元组
// int Table::addTuple(Tuple tuple_in){
//     if(tuple_in.getSize()!=attr_.num)
//     {
//         std::cout<<"Illegal Tuple Insert: The size of column is unequal."<<std::endl;
//         return 0;
//     }
//     for(int index=0;index<attr_.num;index++){
//         //当输入的tuple的type为int或者float时，必须与对应的attr_.type相同。如果为string的话，则需要不大于attr_.type
//         if(tuple_in.getData()[index].type>attr_.type[index]||(tuple_in.getData()[index].type<=0&&tuple_in.getData()[index].type!=attr_.type[index]))
//         {
//             std::cout<<"Illegal Tuple Insert: The types of attributes are unequal."<<std::endl;
//             return 0;
//         }
//     }
//     tuple_.push_back(tuple_in);
//     return 1;
// }

//插入索引
int Table_s::setIndex(short index, std::string index_name) {
	short tmpIndex;
	for (tmpIndex = 0; tmpIndex < index_.num; tmpIndex++) {
		if (index == index_.location[tmpIndex])  //当该元素已经有索引时，报错
		{
			std::cout << "Illegal Set Index: The index has been in the table." << std::endl;
			return 0;
		}
	}
	for (tmpIndex = 0; tmpIndex < index_.num; tmpIndex++) {
		if (index_name == index_.indexname[tmpIndex])  //当该元素已经有索引时，报错
		{
			std::cout << "Illegal Set Index: The name has been used." << std::endl;
			return 0;
		}
	}
	index_.location[index_.num] = index;  //插入索引位置和索引名字，最后把索引个数加一
	index_.indexname[index_.num] = index_name;
	index_.num++;
	return 1;
}

int Table_s::dropIndex(std::string index_name) {
	short tmpIndex;
	for (tmpIndex = 0; tmpIndex < index_.num; tmpIndex++) {
		if (index_name == index_.indexname[tmpIndex])  //当该元素已经有索引时，跳出
			break;
	}
	if (tmpIndex == index_.num)
	{
		std::cout << "Illegal Drop Index: No such a index in the table." << std::endl;
		return 0;
	}

	//交换最后的索引位置和名字，来达到删除的效果
	index_.indexname[tmpIndex] = index_.indexname[index_.num - 1];
	index_.location[tmpIndex] = index_.location[index_.num - 1];
	index_.num--;
	return 1;
}

//计算每个attribute的大小
// int Table::DataSize(){
//     int result=0;
//     for(int index=0;index<attr_.num;index++){
//         switch (attr_.type[index]) {
//             case -1:
//                 result+=sizeof(int);
//                 break;
//             case 0:
//                 result+=sizeof(float);
//                 break;
//             default:
//                 result+=attr_.type[index];
//                 break;
//         }
//     }
//     return result;
// }

//返回一些private的值
std::string Table_s::getTitle() {
	return title_;
}
Attribute_s Table_s::getAttr() {
	return attr_;
}
std::vector<Tuple_s>& Table_s::getTuple() {
	return tuple_;
}
Index_s Table_s::getIndex() {
	return index_;
}


void Table_s::showTable() {
	for (int index = 0; index < attr_.num; index++) {
		std::cout << attr_.name[index] << '\t';
	}
		
	std::cout << std::endl;
	for (int index = 0; index < tuple_.size(); index++)
		tuple_[index].showTuple();
}

void Table_s::showTable(int limit) {
	for (int index = 0; index < attr_.num; index++) {
		std::cout << attr_.name[index] << '\t';
	}
	std::cout << std::endl;
	for (int index = 0; index < limit&&index < tuple_.size(); index++)
		tuple_[index].showTuple();
}