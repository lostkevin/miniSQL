#pragma once
#include <vector>
#include <string>
using namespace std;

//接受一条命令(无尾部分号)对它进行分割，并检查语法
vector<string> parser (string command);

//句法分析，检查是否存在未定义token等, 若正确，后处理command，
bool 