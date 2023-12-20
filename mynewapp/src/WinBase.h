#ifndef UTSTRING_H
#define UTSTRING_H

#include "file.h"

// 定义结构体存储CSV行的数据
struct CSVRow {
	char_t name[40];
	char_t val[40];
	char_t sig[40];
};

#endif /* WIN_BASE*/
