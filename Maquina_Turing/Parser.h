#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED
#include "stdafx.h"
class Parser{
public:
	Parser(){}
	~Parser(){}
	std::vector<wchar_t>characters;
	void parseString(HWND hwnd,std::wstring str);
};
#endif