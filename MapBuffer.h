#pragma once

#include <cstring>

/*为了高效，该类内联*/
class MapBuffer
{
public:

	inline MapBuffer()noexcept = default;
	inline MapBuffer(size_t w, size_t h);
	inline MapBuffer(const MapBuffer&)noexcept;
	inline MapBuffer(MapBuffer&&)noexcept;
	inline ~MapBuffer()noexcept;

	/*初始化一个w*h大小的buffer*/
	inline void Init(size_t w, size_t h);

	/*清除buffer*/
	inline void Clear()noexcept;
	/*重置buffer的内容*/
	inline void Reset()noexcept;

	/*获取源缓冲区*/
	inline char* Data()noexcept;
	inline const char* Data()const noexcept;

	/*通过坐标获取某个字符*/
	char& operator()(size_t x, size_t y)noexcept;
	const char& operator()(size_t x, size_t y)const noexcept;

	/*设置字符串，超出部分截掉*/
	inline void SetString(const std::string& str, size_t x, size_t y)noexcept;
	/*字符串，offset为七十位置，size是截取多少个字符，会在内部处理中文的情况*/
	inline void SetString(const char* str, size_t offset, size_t size, size_t x, size_t y)noexcept;

	/*设置一行连续的多个字符，超出部分截掉*/
	inline void SetChar(char c, size_t x, size_t y, size_t len)noexcept;

	/*获取长宽*/
	inline size_t Width()const noexcept;
	inline size_t Height()const noexcept;

private:
	char* buffer = nullptr;
	size_t w = 0;
	size_t h = 0;
};
#include <string>
inline MapBuffer::MapBuffer(size_t w, size_t h)
{
	Init(w, h);
}
inline MapBuffer::MapBuffer(const MapBuffer& m) noexcept
{
	if (m.buffer == nullptr)
		return;
	size_t size = (w + 1) * h;
	buffer = new char[size];
	memcpy(buffer, m.buffer, size);
}
inline MapBuffer::MapBuffer(MapBuffer&& m) noexcept
{
	buffer = m.buffer;
	m.buffer = nullptr;
	w = m.w;
	h = m.h;
	m.w = m.h = 0;
}
inline MapBuffer::~MapBuffer() noexcept
{
	Clear();
}
inline void MapBuffer::Init(size_t w, size_t h)
{
	Clear();
	if (w == 0 && h == 0)
		return;
	this->w = w;
	this->h = h;
	buffer = new char[(w + 1) * h];
	Reset();
}

inline void MapBuffer::Clear() noexcept
{
	if (buffer != nullptr)
		delete[] buffer;
	buffer = nullptr;
	w = h = 0;
}

inline void MapBuffer::Reset() noexcept
{
	memset(buffer, ' ', (w + 1) * h);
	for (size_t i = 0; i < h; ++i)
		buffer[i * (w + 1) + w] = '\n';
	buffer[(w + 1) * h - 1] = 0;
}

inline char* MapBuffer::Data() noexcept
{
	return buffer;
}

inline const char* MapBuffer::Data() const noexcept
{
	return buffer;
}

inline char& MapBuffer::operator()(size_t x, size_t y) noexcept
{
	return buffer[y * (w + 1) + x];
}

inline const char& MapBuffer::operator()(size_t x, size_t y) const noexcept
{
	return buffer[y * (w + 1) + x];
}

inline void MapBuffer::SetString(const std::string& str, size_t x, size_t y) noexcept
{
	SetString(str.c_str(), 0, str.size(), x, y);
}

inline void MapBuffer::SetString(const char* str, size_t offset, size_t size, size_t x, size_t y) noexcept
{
	if (x >= w || y >= h)
		return;
	size_t i = 0;
	if (offset == 0)
		memcpy(&(*this)(x, y), str, size + x > w ? w - x : size);
	else
	{
		//需要截断字符串，那么问题就来了，一个占俩字节中文不能被截掉一半，不然还会影响后序字符串的输出
		//判断这个字符串起始字节是一个中文字符的初始字节还是结束字节
		for (; i < offset;)
			if (str[i] > 0 && str[i] < 127)
				++i;
			else
				i += 2; //占俩字节
		if (i != offset)
			(*this)(x, y) = ' ';
		memcpy(&(*this)(x, y) + (i != offset),
			str + offset + (i != offset), //如果是初始字节则再向后偏移一个字节
			(size + x > w ? w - x : size) - (i != offset));
	}
	if (size + x > w) //结尾也有可能被截断
	{
		size_t afterLastByte = offset + w - x;
		for (; i < afterLastByte;)
			if (str[i] > 0 && str[i] < 127)
				++i;
			else
				i += 2; //占俩字节
		if (i != afterLastByte) //最后一个字节是乱码
			(*this)(w - 1, y) = ' ';
	}
}

inline void MapBuffer::SetChar(char c, size_t x, size_t y, size_t len) noexcept
{
	len = x + len > w ? w - x : len;
	memset(&(*this)(x, y), c, len);
}

inline size_t MapBuffer::Width() const noexcept
{
	return w;
}

inline size_t MapBuffer::Height() const noexcept
{
	return h;
}
