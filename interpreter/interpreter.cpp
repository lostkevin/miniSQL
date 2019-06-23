#include "interpreter.h"
#include"../CatalogManager/catalog.h"
Interpreter::Interpreter() {
}

//将query进行赋值
void Interpreter::getQuery() {
	std::string tmp;
	bool flag = true;
	//std::cout << "mysql > ";
	//得到一行的所有字符，当最后一个字符为分号时结束
	do {
		if (flag) {
			std::cout << "mysql > ";
			flag = false;
		}
		else {
			std::cout << "      > ";
		}
		getline(std::cin, tmp);
		query += tmp;
		query += ' ';
	} while (tmp[tmp.length() - 1] != ';');
	//在最后补一个结尾标识符
	query[query.length() - 2] = '\0';
	//调用Normalize进行字符串的规范化
	Normalize();
}

void Interpreter::Normalize() {
	for (int pos = 0; pos < query.length(); pos++) {
		if (query[pos] == '*' || query[pos] == '=' || query[pos] == ',' || query[pos] == '(' || query[pos] == ')' || query[pos] == '<' || query[pos] == '>') {
			//假如语句未包含空格，则前后增加空格
			if (query[pos - 1] != ' ') {
				query.insert(pos++, " ");
			}
			if (query[pos + 1] != ' ') {
				query.insert(++pos, " ");
			}
		}
	}
	//在结尾补一个空格以规范化
	query.insert(query.length() - 2, " ");
	//删除段落中的多余空格
	std::string::iterator it;
	int flag = 0;
	for (it = query.begin(); it < query.end(); it++) {
		if (flag == 0 && (*it == ' ' || *it == '\t')) {
			flag = 1;
			continue;
		}
		if (flag == 1 && (*it == ' ' || *it == '\t')) {
			query.erase(it);
			//去除空格,指针前移
			if (it != query.begin())
				it--;
			continue;
		}
		if (*it != ' '&&*it != '\t') {
			flag = 0;
			continue;
		}
	}
	//如果段落开始有空格，就删除多余空格以规范化
	if (query[0] == ' ')
		query.erase(query.begin());
	//将query的第一个词全部转换为小写，方便之后对字符串的解析
	query = getLower(query, 0);
}
//分解指令到各个函数
void Interpreter::EXEC() {
	try {
		//根据字符串的第一个单词来对所进行的操作解析
		if (query.substr(0, 6) == "select") {
			EXEC_SELECT();
		}
		//由于drop有两种情况，所以需要进行进一步的解析
		else if (query.substr(0, 4) == "drop") {
			query = getLower(query, 5);
			if (query.substr(5, 5) == "table")
				EXEC_DROP_TABLE();
			else if (query.substr(5, 5) == "index")
				EXEC_DROP_INDEX();
		}
		else if (query.substr(0, 6) == "insert") {
			EXEC_INSERT();
		}
		//create也有两种情况
		else if (query.substr(0, 6) == "create") {
			query = getLower(query, 7);
			if (query.substr(7, 5) == "table") {
				EXEC_CREATE_TABLE();
			}
			else if (query.substr(7, 5) == "index") {
				EXEC_CREATE_INDEX();
			}
		}
		else if (query.substr(0, 6) == "delete") {
			EXEC_DELETE();
		}
		else if (query.substr(0, 4) == "exit"&&query[5] == '\0') {
			EXEC_EXIT();
		}
		//如果所有指令都不能对应，则抛出输入格式错误
		else {
			throw input_format_error();
		}
		query = string();
	}

	catch (table_exist error) {
		std::cout << ">>> Error: Table has existed!" << std::endl;
	}
	catch (table_not_exist error) {
		std::cout << ">>> Error: Table not exist!" << std::endl;
	}
	catch (attribute_not_exist error) {
		std::cout << ">>> Error: Attribute not exist!" << std::endl;
	}
	catch (index_exist error) {
		std::cout << ">>> Error: Index has existed!" << std::endl;
	}
	catch (index_not_exist error) {
		std::cout << ">>> Error: Index not existed!" << std::endl;
	}
	catch (tuple_type_conflict error) {
		std::cout << ">>> Error: Tuple type conflict!" << std::endl;
	}
	catch (primary_key_conflict error) {
		std::cout << ">>> Error: Primary key conflict!" << std::endl;
	}
	catch (data_type_conflict error) {
		std::cout << ">>> Error: data type conflict!" << std::endl;
	}
	catch (index_full error) {
		std::cout << ">>> Error: Index full!" << std::endl;
	}
	catch (unique_conflict error) {
		std::cout << ">>> Error: unique conflict!" << std::endl;
	}
	catch (primary_index_drop_fail error) {
		std::cout << ">>> Error: cant drop a primary key!" << std::endl;
	}
	catch (exit_command error) {
		std::cout << ">>> Bye bye~" << std::endl;
		exit(0);
	}
	catch (...) {
		std::cout << ">>> Error: Input format error!" << std::endl;
	}
	query = string();
}

void Interpreter::EXEC_CREATE_INDEX() {
	CatalogManager CM;
	API API;
	std::string index_name;
	std::string table_name;
	std::string attr_name;
	int check_index;
	index_name = getWord(13, check_index);
	check_index++;
	if (getLower(query, check_index).substr(check_index, 2) != "on")
		throw 1;//格式错误
	table_name = getWord(check_index + 3, check_index);
	if (!CM.CheckTableExist(table_name))
		throw table_not_exist();//table not exist
	if (query[check_index + 1] != '(')
		throw 1;//格式错误
	attr_name = getWord(check_index + 3, check_index);
	if (query[check_index + 1] != ')' || query[check_index + 3] != '\0')
		throw 1;//格式错误
	API.createIndex(table_name, index_name, attr_name);
	std::cout << "      > SUCCESS" << std::endl;
}

void Interpreter::EXEC_FILE() {
	int check_index = 0;
	int start_index = 0;
	std::string tmp_query;
	std::string file_path = getWord(9, check_index);
	if (query[check_index + 1] != '\0')
		throw 1;
	std::string::iterator it;
	std::fstream fs(file_path);
	std::stringstream ss;
	ss << fs.rdbuf();
	tmp_query = ss.str();
	check_index = 0;
	do {
		while (tmp_query[check_index] != '\n')
			check_index++;
		query = tmp_query.substr(start_index, check_index - start_index);
		check_index++;
		start_index = check_index;
		Normalize();
		EXEC();
	} while (tmp_query[check_index] != '\0');
}

void Interpreter::EXEC_DROP_INDEX() {
	API API;
	std::string table_name;
	std::string index_name;
	int check_index;
	//得到index的名字
	index_name = getWord(11, check_index);
	check_index++;
	//格式错误的情况
	if (query[check_index + 1] != '\0')
		throw 1;//输入错误
	API.dropIndex(table_name, index_name);
	std::cout << "      > SUCCESS" << std::endl;
}

void Interpreter::EXEC_EXIT() {
	//如果需要退出，直接抛出一个exit command
	throw exit_command();
}

void Interpreter::EXEC_DELETE() {
	API API;
	CatalogManager CM;
	Where where_delete;
	int check_index;
	std::string table_name;
	std::string attr_name;
	std::string relation;
	if (getLower(query, 7).substr(7, 4) != "from")
		throw 1;
	table_name = getWord(12, check_index);
	if (!CM.CheckTableExist(table_name))
		throw table_not_exist();


	//处理删除所有信息的情况
	//删除整张表
	if (query[check_index + 1] == '\0') {
		attr_name = "";
		vector<Where> tmp;
		tmp.push_back (where_delete);
		API.deleteRecord (table_name, tmp);
		std::cout << "      > SUCCESS" << std::endl;
		return;
	}
	//有where子句
	if (getLower(query, check_index + 1).substr(check_index + 1, 5) != "where")
		throw 1;//格式错误
	attr_name = getWord(check_index + 7, check_index);
	if (!CM.CheckAttrExist(table_name, attr_name))
		throw attribute_not_exist();
	relation = getRelation(check_index + 1, check_index);
	if (relation == "<")
		where_delete.relation_character = LESS;
	else if (relation == "< =")
		where_delete.relation_character = LESS_OR_EQUAL;
	else if (relation == "=")
		where_delete.relation_character = EQUAL;
	else if (relation == "> =")
		where_delete.relation_character = GREATER_OR_EQUAL;
	else if (relation == ">")
		where_delete.relation_character = GREATER;
	else if (relation == "! =")
		where_delete.relation_character = NOT_EQUAL;
	else
		throw 1;//格式错误
	std::string value_delete = getWord(check_index + 1, check_index);

	vector<Attribute> tmp_attr;
	CM.getAttrInfo(table_name,tmp_attr);
	for (int i = 0; i < tmp_attr.size(); i++)
	{
		//where子句中的属性名与table中的属性名对应，添加到where条件
		if (attr_name == tmp_attr[i].attr_name) {
			where_delete.data.type = tmp_attr[i].attr_type;
			where_delete.attr_name = attr_name;
			switch (where_delete.data.type) {
			case 0:
				try {
					//string data = value_delete.substr (1, value_delete.length () - 2);

					where_delete.data.datai = stringToNum<int>(value_delete);
				}
				catch (...) {
					throw data_type_conflict();//转换失败
				}
				break;
			case 1:
				try {
					//string data = value_delete.substr (1, value_delete.length () - 2);
					where_delete.data.dataf = stringToNum<float> (value_delete);
				}
				catch (...) {
					throw data_type_conflict();//转换失败
				}
				break;
			default:
				try {
					if (!(value_delete[0] == '\''&&value_delete[value_delete.length() - 1] == '\'') && !(value_delete[0] == '"'&&value_delete[value_delete.length() - 1] == '"'))
						throw 1;//格式不正确
					where_delete.data.datas = value_delete.substr(1, value_delete.length() - 2);
				}
				catch (...) {
					throw data_type_conflict();//转换失败
				}
				break;
			}
			break;
		}
	}
	vector<Where> tmp;
	tmp.push_back (where_delete);
	API.deleteRecord(table_name, tmp);
	std::cout << "      > SUCCESS" << std::endl;
}

void Interpreter::EXEC_INSERT() {
	API API;
	std::string table_name;
	int check_index;
	Tuple_s tuple_insert;
	vector<Attribute> attr_exist;
	if (getLower(query, 7).substr(7, 4) != "into")
		throw input_format_error();
	table_name = getWord(12, check_index);
	if (getLower(query, check_index + 1).substr(check_index + 1, 6) != "values")
		throw input_format_error();
	check_index += 8;
	if (query[check_index] != '(')
		throw input_format_error();
	CatalogManager cMgr;
	if (!cMgr.CheckTableExist(table_name)) {
		throw table_not_exist();
	}
	cMgr.getAttrInfo(table_name, attr_exist);
	check_index--;
	int num_of_insert = 0;
	//对括号内的所有元素进行遍历
	while (query[check_index + 1] != '\0'&&query[check_index + 1] != ')') {
		if (num_of_insert >= attr_exist.size())
			throw 1;//属性数不匹配
		check_index += 3;
		std::string value_insert = getWord(check_index, check_index);
		Data insert_data;
		if (value_insert == "") {
			insert_data.type = -1;
			tuple_insert.addData(insert_data);
			num_of_insert++;
			continue;
		}
		insert_data.type = attr_exist[num_of_insert].attr_type;
		switch (attr_exist[num_of_insert].attr_type) {
		case 0:
			try {
				//string data = value_insert.substr (1, value_insert.length () - 2);
				//insert_data.datai = stringToNum<int>(data);
				insert_data.datai = stringToNum<float> (value_insert);
			}
			catch (...) {
				throw data_type_conflict();//转换失败
			}
			break;
		case 1:
			try {
				//string data = value_insert.substr (1, value_insert.length () - 2);
				insert_data.dataf = stringToNum<float> (value_insert);
			}
			catch (...) {
				throw data_type_conflict();//转换失败
			}
			break;
		default:
			try {
				if (!(value_insert[0] == '\''&&value_insert[value_insert.length() - 1] == '\'') && !(value_insert[0] == '"'&&value_insert[value_insert.length() - 1] == '"'))
					throw input_format_error();//格式不正确
				if (value_insert.length() - 1 > attr_exist[num_of_insert].attr_type)
					throw input_format_error();//长度超过限制
				insert_data.datas = value_insert.substr(1, value_insert.length() - 2);
			}
			catch (input_format_error error) {
				throw input_format_error();
			}
			catch (...) {
				throw data_type_conflict();//转换失败
			}
			break;
		}
		tuple_insert.addData(insert_data);
		num_of_insert++;
	}
	if (query[check_index + 1] == '\0')
		throw input_format_error();//格式错误
	if (num_of_insert != attr_exist.size())
		throw input_format_error();//插入的数量不正确
	API.insertRecord(table_name, tuple_insert);
	std::cout << "      > SUCCESS" << std::endl;
}

void Interpreter::EXEC_SELECT() {
	API API;
	CatalogManager CM;
	std::string table_name;
	std::vector<std::string> attr_name;		//select部分字段值时，记录这些字段名
	std::vector<std::string> target_name;	//where子句后面，字段名，用数组记录
	std::vector<Where> where_select;		
	std::string tmp_target_name;			//where子句后面， 暂时记录单个字段名
	std::string tmp_value;					//值
	Where tmp_where;						//记录当前条件的运算符、属性类型
	std::string relation;
	Table output_table;
	char op = 0;
	int check_index;
	int flag = 0;//判断是否为select *
	if (getWord(7, check_index) == "*")
	{
		flag = 1;
		check_index++;
	}
	else {
		check_index = 7;
		while (1) {
			attr_name.push_back(getWord(check_index, check_index));
			if (query[++check_index] != ',')
				break;
			else
				check_index += 2;
		}
	}
	if (getLower(query, check_index).substr(check_index, 4) != "from")
		throw input_format_error();//格式错误
	check_index += 5;
	table_name = getWord(check_index, check_index);
	if (!CM.CheckTableExist(table_name))
		throw table_not_exist();
	vector<Attribute> tmp_attr;
	CM.getAttrInfo(table_name, tmp_attr);
	if (!flag) {
		//查找部分属性
		for (int index = 0; index < attr_name.size(); index++) {
			if (!CM.CheckAttrExist(table_name, attr_name[index]))
				throw attribute_not_exist();
			target_name.push_back(attr_name[index]);
		}
	}
	else {
		for (int index = 0; index < tmp_attr.size(); index++) {
			attr_name.push_back(tmp_attr[index].attr_name);
			target_name.push_back (tmp_attr[index].attr_name);
		}
	}
	check_index++;
	vector<bool> Operations;
	Operations.push_back (op);
	if (query[check_index] == '\0')
		API.selectRecord(table_name, target_name, where_select, Operations);
	//带有where子句
	else {
		if (getLower(query, check_index).substr(check_index, 5) != "where")
			throw input_format_error();//格式错误
		check_index += 6;
		while (1) {
			tmp_target_name = getWord(check_index, check_index); //字段名
			//if (!CM.hasAttribute(table_name, tmp_target_name))
			//	throw attribute_not_exist();
			//target_name.push_back(tmp_target_name);
			relation = getRelation(check_index + 1, check_index);
			if (relation == "<")
				tmp_where.relation_character = LESS;
			else if (relation == "< =")
				tmp_where.relation_character = LESS_OR_EQUAL;
			else if (relation == "=")
				tmp_where.relation_character = EQUAL;
			else if (relation == "> =")
				tmp_where.relation_character = GREATER_OR_EQUAL;
			else if (relation == ">")
				tmp_where.relation_character = GREATER;
			else if (relation == "! =")
				tmp_where.relation_character = NOT_EQUAL;
			else
				throw input_format_error();//格式错误
			tmp_value = getWord(check_index + 1, check_index);	//属性值
			//逐个条件检验类型等是否满足
			for (int i = 0; i < tmp_attr.size(); i++)
			{
				if (tmp_target_name == tmp_attr[i].attr_name) {
					tmp_where.data.type = tmp_attr[i].attr_type;
					tmp_where.attr_name = tmp_attr[i].attr_name;
					switch (tmp_where.data.type) {
					case 0:
						try {
							tmp_where.data.datai = stringToNum<int>(tmp_value);
						}
						catch (...) {
							throw data_type_conflict();//转换失败
						}
						break;
					case 1:
						try {
							tmp_where.data.dataf = stringToNum<float>(tmp_value);
						}
						catch (...) {
							throw data_type_conflict();//转换失败
						}
						break;
					default:
						try {
							//字符型，缺少''报错
							if (!(tmp_value[0] != '\''&&tmp_value[tmp_value.length() - 1] != '\'') && !(tmp_value[0] != '"'&&tmp_value[tmp_value.length() - 1] != '"'))
								throw input_format_error();//格式不正确
							tmp_where.data.datas = tmp_value.substr(1, tmp_value.length() - 2);
						}
						catch (input_format_error error) {
							throw input_format_error();
						}
						catch (...) {
							throw data_type_conflict();//转换失败
						}
					}
					break;
				}
			}
			where_select.push_back(tmp_where);
			if (query[check_index + 1] == '\0')
				break;
			else if (getLower(query, check_index + 1).substr(check_index + 1, 3) == "and")//假设关系链接是and
				op = 1;
			else if (getLower(query, check_index + 1).substr(check_index + 1, 2) == "or")//假设关系链接是or
				op = 0;
			else
				throw 1;
			getWord(check_index + 1, check_index);
			check_index++;
		}
		vector<bool> Operations;
		Operations.push_back (op);
		API.selectRecord(table_name, target_name, where_select, Operations);
	}
}

void Interpreter::EXEC_CREATE_TABLE() {
	//输入表名
	std::string table_name;
	//定位属性位置
	int check_index;
	table_name = getWord(13, check_index);
	//表的索引初始化
	Table table;
	string table_filename = ".\\";
	table_filename += table_name;
	table_filename += ".dat";
	table.tablename = table_name;
	table.table_fileName = table_filename;
	//设置属性
	std::string attr_name;
	int primary = -1;
	int attr_num = 0;
	while (1) {
		check_index += 3;
		//如果已经遍历完string了，就退出
		if (query[check_index] == '\0') {
			if (query[check_index - 2] == '\0')
				throw 1;
			else
				break;
		}
		//得到一个属性的名字
		attr_name = getWord(check_index, check_index);
		//检测这个属性是不是primary
		std::string check_primary(attr_name);
		check_primary = getLower(check_primary, 0);
		if (check_primary == "primary") {
			int tmp_end = check_index;
			std::string check_key = getWord(tmp_end + 1, tmp_end);
			//再检查接下来的关键字是不是key，如果不是，则将primary看作属性名字
			if (check_key != "key") {
				table.attr[attr_num].attr_name = attr_name;
				break;
			}
			//设置属性的primary
			else {
				check_index = tmp_end + 3;
				std::string unique_name = getWord(check_index, check_index);
				int hasset = 1;
				for (int find_name = 0; find_name < table.attr_num; find_name++) {
					if (table.attr[find_name].attr_name == unique_name) {
						hasset = 0;
						primary = find_name;
						table.attr[find_name].primary = true;
						table.attr[find_name].unique = true;
						check_index += 2;
						break;
					}
				}
				if (hasset)
					throw 1;
				continue;
			}
		}
		//如果不是primary key，就直接把这个属性名字插入进去
		else
			table.attr[attr_num].attr_name = attr_name;
		check_index++;
		//得到一个属性的类型
		table.attr[attr_num].attr_type = getType(check_index, check_index);
		table.attr[attr_num].unique = false;
		//判断之后有没有unique关键词
		if (query[check_index + 1] == 'u' || query[check_index + 1] == 'U') {
			query = getLower(query, 0);
			//如果有，则设为unique，如果开头为u，但是不为unique，那就是一个错误
			if (getWord(check_index + 1, check_index) == "unique") {
				table.attr[attr_num].unique = true;
			}
			else
				throw 1;
		}
		//更新属性的数量
		attr_num++;
		table.attr_num = attr_num;
	}
	//调用CatalogManager，将表的信息插入进去
	API API;
	API.createTable(table);
	std::cout << "      > SUCCESS" << std::endl;
}



void Interpreter::EXEC_DROP_TABLE() {
	API API;
	std::string table_name;
	int check_index;
	//得到table的名字
	table_name = getWord(11, check_index);
	//如果table的名字之后有多余字符串，则是格式错误
	if (query[check_index + 1] != '\0')
		throw 1;//输入错误
	API.dropTable(table_name);
	std::cout << "      > SUCCESS" << std::endl;
}

//得到一个位置的属性类型
int Interpreter::getType(int pos, int &end_pos) {
	std::string type = getWord(pos, end_pos);
	if (type == "int")
		return 0;
	else if (type == "float")
		return 1;
	else if (type == "char") {
		end_pos += 3;
		std::string length = getWord(end_pos, end_pos);
		end_pos += 2;
		return atoi(length.c_str()) + 1;
	}
	throw 1;
}

//取出一个单词，同时将end_pos更新到单词后的那个空格的位置处
std::string Interpreter::getWord(int pos, int &end_pos) {
	std::string PartWord = "";
	for (int pos1 = pos; pos1 < (int)query.length(); pos1++) {
		if (query[pos1] == ' ' || query[pos1] == '\0')
		{
			PartWord = query.substr(pos, pos1 - pos);
			//std::cout<<PartWord<<std::endl;
			end_pos = pos1;
			return PartWord;
		}
	}
	return PartWord;
}

//得到一个字符的小写形式
std::string Interpreter::getLower(std::string str, int pos) {
	for (int index = pos;; index++) {
		if (str[index] == ' ' || str[index] == '\0')
			break;
		else if (str[index] >= 'A'&&str[index] <= 'Z')
			str[index] += 32;
	}
	return str;
}

//得到一个关系符号的字符串，如果关系符号有两个字符，则字符中会产生一个空格，如"< ="
std::string Interpreter::getRelation(int pos, int &end_pos) {
	std::string PartWord = "";
	for (int pos1 = pos; pos1 < (int)query.length(); pos1++) {
		if (query[pos1] == ' ')
			continue;
		if (query[pos1] != '<'&&query[pos1] != '>'&&query[pos1] != '='&&query[pos1] != '!')
		{
			PartWord = query.substr(pos, pos1 - pos - 1);
			//std::cout<<PartWord<<std::endl;
			end_pos = pos1 - 1;
			return PartWord;
		}
	}
	return PartWord;
}

//根据除法得到整形的数字的长度
int Interpreter::getBits(int num) {
	int bit = 0;
	if (num == 0)
		return 1;
	if (num < 0) {
		bit++;
		num = -num;
	}
	while (num != 0) {
		num /= 10;
		bit++;
	}
	return bit;
}

//根据除法得到小数的数字的长度
int Interpreter::getBits(float num) {
	int bit = 0;
	if ((int)num == 0)
		return 4;
	if (num < 0) {
		bit++;
		num = -num;
	}
	int integer_part = (int)num;
	while (integer_part != 0) {
		bit++;
		integer_part /= 10;
	}
	return bit + 3;//为了保留小数点的后几位
}