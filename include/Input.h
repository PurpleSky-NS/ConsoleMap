#pragma once

#include <string>
#include <sstream>
#include <iostream>

static std::stringstream ss;
static std::string input;

template<class T>
T GetVal(const std::string& tip, T def)
{
	T v;
	ss.str("");
	ss.clear();
	input.clear();
	std::cout << tip;
	std::cin >> input;
	getchar();
	ss << input;
	ss >> v;
	if (ss.eof() && !ss.fail())
		return v;
	else
		return def;
}

template<class T>
T GetLineVal(const std::string& tip, T def)
{
	T v;
	ss.str("");
	ss.clear();
	input.clear();
	std::cout << tip;
	std::getline(std::cin, input);
	ss << input;
	ss >> v;
	if (ss.eof() && !ss.fail())
		return v;
	else
		return def;
}

std::string GetStr(const std::string& tip)
{
	std::cout << tip;
	std::cin >> input;
	getchar();
	return input;
}

std::string GetLine(const std::string& tip)
{
	std::cout << tip;
	std::getline(std::cin, input);
	return input;
}
