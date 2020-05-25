#pragma once

#include <string>
#include <fstream>
#include <cstdint>

/*很简单的二进制文件操作器*/
class File
{
public:

	/*两个异常类，见名知意*/
	class FileEnd {};

	inline bool OpenWithInOut(const std::string& path);

	inline bool OpenWithIn(const std::string& path);

	inline bool OpenWithOut(const std::string& path);

	/*写系列*/
	template<class T>
	inline File& Write(const T& v);
	template<>
	inline File& Write(const std::string& v);

	/*读系列*/
	template<class T>
	inline T Read();
	template<>
	inline std::string Read();

	inline operator bool()const;

private:
	std::fstream file;
};
inline bool File::OpenWithInOut(const std::string& path)
{
	file.open(path, std::ios::in | std::ios::out | std::ios::binary);
	return file.is_open();
}

inline bool File::OpenWithIn(const std::string& path)
{
	file.open(path, std::ios::in | std::ios::binary);
	return file.is_open();
}

inline bool File::OpenWithOut(const std::string& path)
{
	file.open(path, std::ios::trunc | std::ios::out | std::ios::binary);
	return file.is_open();
}

template<>
inline File& File::Write(const std::string& v)
{
	Write((uint32_t)v.size());
	file.write(v.c_str(), v.size());
	return *this;
}

template<class T>
inline File& File::Write(const T& v)
{
	file.write((const char*)&v, sizeof(T));
	return *this;
}

template<class T>
inline T File::Read()
{
	if (!(*this))
		throw FileEnd();
	T tmp;
	file.read((char*)&tmp, sizeof(T));
	return tmp;
}
template<>
inline std::string File::Read()
{
	if (!(*this))
		throw FileEnd();
	std::string str;
	uint32_t size = Read<uint32_t>();
	char* buf = new char[size + 1];
	buf[size] = 0;
	file.read(buf, size);
	str.assign(buf);
	delete[] buf;
	return str;
}

inline File::operator bool() const
{
	return !file.eof();
}
