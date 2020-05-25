#pragma once

#include "Camera.h"
#include "MapBuffer.h"
#include "Node.h"

/*生成的变量名为_+传入变量名*/
#define TransPositionVar(x,y,xName,yName) int xName=x,yName=y;camera.ScreenPosition(xName,yName);

/*地图渲染器，负责渲染地图*/
class MapRenderer
{
public:
	Camera camera;
	MapBuffer buffer;

	//传入坐标全都是世界坐标
	/*渲染文字*/
	inline void RenderText(const std::string& text, size_t x, size_t y)noexcept;

	/*渲染一个顶点节点*/
	inline void RenderVertexNode(const VertexNode& node)noexcept;

	/*渲染一个顶点节点的指定的路径，不渲染顶点节点*/
	inline void RenderVertexPath(const VertexNode& node, const LinkNode& entry)noexcept;

	/*渲染next路径，不渲染顶点*/
	inline void RenderNextPath(const LinkNode& node)noexcept;

	/*渲染Prev路径，不渲染顶点*/
	inline void RenderPrevPath(const LinkNode& node)noexcept;

	/*渲染一条路径*/
	inline void RenderPath(size_t fromX, size_t fromY, size_t toX, size_t toY)noexcept;

	/*渲染从顶点节点到路径节点的路径*/
	inline void RenderVertex2LinkNode(const VertexNode& v, const LinkNode& l)noexcept;

	/*渲染从顶点节点到顶点节点的路径*/
	inline void RenderVertex2VertexNode(const VertexNode& v1, const VertexNode& v2)noexcept;

	/*渲染从路径节点到路径节点的路径*/
	inline void RenderLink2LinkNode(const LinkNode& l1, const LinkNode& l2)noexcept;

	/*该点是否在屏幕中*/
	inline bool InCamera(size_t x, size_t y)noexcept;

	/*获取一个节点渲染后的长宽*/
	inline size_t GetNodeWidth(const VertexNode& node)noexcept;
	inline constexpr size_t GetNodeHeight(const VertexNode& node)noexcept;

private:

	/*两个路径字符冲突，将他们糅合*/
	inline char Mix(char c1, char c2)noexcept;

	//基础渲染函数，参数为CameraPosition
	/*渲染一条竖线*/
	inline void RenderVertical(int x, int from, int to)noexcept;
	/*渲染一条横线*/
	inline void RenderHorizontal(int y, int from, int to)noexcept;
	/*渲染一条左倾线 \ */
	inline void RenderLeft(int fromX, int toX, int fromY)noexcept;
	/*渲染一条右倾线 / */
	inline void RenderRight(int fromX, int toX, int fromY)noexcept;

};
inline void MapRenderer::RenderText(const std::string& text, size_t x, size_t y) noexcept
{
	TransPositionVar(x, y, _x, _y);
	if (_y < 0)
		return;
	if (_x < 0 && text.size()>(size_t) - _x)
		buffer.SetString(text.c_str(), (size_t)-_x, text.size() + _x, 0, _y);
	else
		buffer.SetString(text, _x, _y);
}
inline void MapRenderer::RenderVertexNode(const VertexNode& node) noexcept
{
	TransPositionVar(node.x, node.y, vx, vy);
	RenderText(node.name, node.x + 1, node.y + 1); //渲染名称
	//渲染边框
	RenderHorizontal(vy, vx, vx + node.name.size() + 1);
	RenderHorizontal(vy + 2, vx, vx + node.name.size() + 1);
	RenderVertical(vx, vy, vy + 2);
	RenderVertical(vx + node.name.size() + 1, vy, vy + 2);
}
inline void MapRenderer::RenderVertexPath(const VertexNode& node, const LinkNode& entry) noexcept
{
	if (entry.IsVertex())
	{
		RenderVertex2VertexNode(node, (const VertexNode&)entry);
		return;
	}
	RenderVertex2LinkNode(node, entry);
	if (entry.next != &node)
		RenderNextPath(entry);
	else
		RenderPrevPath(entry);
}
inline void MapRenderer::RenderNextPath(const LinkNode& node) noexcept
{
	LinkNode const* from = &node; //如果进来一个VertexNode
	LinkNode const* to = node.next; //这个就是null，不会执行循环
	while (to != nullptr)
	{
		if (to->IsVertex())
			RenderVertex2LinkNode((const VertexNode&)*to, *from);
		else
			RenderLink2LinkNode(*from, *to);
		from = to, to = from->next;//切换下一个节点
	}
}
inline void MapRenderer::RenderPrevPath(const LinkNode& node) noexcept
{
	LinkNode const* from = &node; //如果进来一个VertexNode
	LinkNode const* to = node.prev; //这个就是null，不会执行循环
	while (to != nullptr)
	{
		if (to->IsVertex())
			RenderVertex2LinkNode((const VertexNode&)*to, *from);
		else
			RenderLink2LinkNode(*from, *to);
		from = to, to = from->prev;//切换下一个节点
	}
}
inline void MapRenderer::RenderPath(size_t fromX, size_t fromY, size_t toX, size_t toY) noexcept
{
	//因为无法绘制一条任意斜率的直线，所以只能通过一个转折点来绘制一条折线
#define ABS_SUB(x,y) ((x)>(y)?(x)-(y):(y)-(x))
#define ABS(x) ((x)>0?(x):-(x))
	TransPositionVar(fromX, fromY, _fromX, _fromY);
	TransPositionVar(toX, toY, _toX, _toY);
	if (_fromX == _toX) //竖线
		RenderVertical(_fromX, _fromY, _toY);
	else if (_fromY == _toY) //横线
		RenderHorizontal(_fromY, _fromX, _toX);
	else
	{
		size_t dx = ABS_SUB(_toX, _fromX); //获取dx
		size_t dy = ABS_SUB(_toY, _fromY); //获取dy
		double k = (double)dy / dx * (_toX < _fromX ? -1 : 1) * (_toY < _fromY ? -1 : 1); //计算斜率
		if (dx == dy) //斜线
		{
			if (k > 0) //左斜线
				RenderLeft(_fromX, _toX, _fromY);
			else //右斜线 
				RenderRight(_fromX, _toX, _fromY);
			return;
		}
		//斜率不为1
		//规则：如果|斜率|>1，绘制竖线->斜线
		//		如果|斜率|<1，绘制斜线->横线
		size_t len = ABS_SUB(dx, dy); //直线的长度
		if (ABS(k) > 1)
		{
			//代码优化过，所以才这么几行，一大堆判断优化成了三目，临时变量去掉了
			RenderVertical(_fromX, _fromY, _toY > _fromY ? _fromY + len : _fromY - len); //从自己向一个临时点绘制竖线
			//从终点向一个临时点(因为那个点已经绘制过了，所以有1点的偏移)绘制斜线
			if (k > 0) //左斜线
				RenderLeft(_toX, _toX > _fromX ? _fromX + 1 : _fromX - 1, _toY);
			else //右斜线
				RenderRight(_toX, _toX > _fromX ? _fromX + 1 : _fromX - 1, _toY);
		}
		else
		{
			if (k > 0) //左斜线
				RenderLeft(_fromX, _toX > _fromX ? _fromX + dy - 1 : _fromX - dy + 1, _fromY);
			else
				RenderRight(_fromX, _toX > _fromX ? _fromX + dy - 1 : _fromX - dy + 1, _fromY);
			RenderHorizontal(_toY, _toX > _fromX ? _toX - len : _toX + len, _toX);
		}
	}
#undef ABS_SUB
#undef ABS
}
inline bool MapRenderer::InCamera(size_t x, size_t y) noexcept
{
	return IsInRect_WH(x, y, camera.x, camera.y, buffer.Width(), buffer.Height());
}
inline size_t MapRenderer::GetNodeWidth(const VertexNode& node) noexcept
{
	return node.name.size() + 2;
}
inline constexpr size_t MapRenderer::GetNodeHeight(const VertexNode& node) noexcept
{
	return 3;
}
inline char MapRenderer::Mix(char c1, char c2) noexcept
{
	/*Ascii排序为：
	 * + - / X \ |
	*/
	if (c1 == c2)
		return c1;
	else if (c1 > c2)
		return Mix(c2, c1);
	if (c1 == '*' || c2 == '*')
		return '*';
	if (c1 == ' ')
		return c2;
	if (c2 == ' ')
		return c1;
	switch (c1)
	{
	case '+':
		switch (c2)
		{
		case '-':
		case '|':
			return '+';
		case '/':
		case '\\':
		case 'X':
			return '*';
		}
	case '-':
		switch (c2)
		{
		case '|':
			return '+';
		case '/':
		case '\\':
		case 'X':
			return '*';
		}
	case '/':
		switch (c2)
		{
		case '|':
			return '*';
		case '\\':
		case 'X':
			return 'X';
		}
	case 'X':
		switch (c2)
		{
		case '|':
			return '*';
		case '\\':
			return 'X';
		}
	case '\\':
		switch (c2)
		{
		case '|':
			return '*';
		}
	default:
		return c2;
	}
	return c1;
}

inline void MapRenderer::RenderVertical(int x, int from, int to) noexcept
{
	if (from > to)//使from<to
		from ^= to, to ^= from, from ^= to; //交换
	//截取需要绘制的部分
	if (x >= (int)buffer.Width() || x < 0 || from >= (int)buffer.Height())
		return;
	if (to >= (int)buffer.Height())
		to = (int)buffer.Height() - 1;
	if (from < 0)
		from = 0;
	//开始绘制
	for (char* c = &buffer(x, from); from <= to; ++from, c = &buffer(x, from))
		*c = Mix(*c, '|');
}

inline void MapRenderer::RenderHorizontal(int y, int from, int to) noexcept
{
	if (from > to)//使from<to
		from ^= to, to ^= from, from ^= to; //交换
	//截取需要绘制的部分
	if (y >= (int)buffer.Height() || y < 0 || from >= (int)buffer.Width())
		return;
	if (to >= (int)buffer.Width())
		to = (int)buffer.Width() - 1;
	if (from < 0)
		from = 0;
	//开始绘制
	for (char* c = &buffer(from, y); from <= to; ++from, c = &buffer(from, y))
		*c = Mix(*c, '-');
}

inline void MapRenderer::RenderLeft(int fromX, int toX, int fromY) noexcept
{
	if (fromX > toX)//使from<to
	{
		fromX ^= toX, toX ^= fromX, fromX ^= toX; //交换
		fromY -= toX - fromX; //计算fromY
	}
	//截取需要绘制的部分
	if (fromX < 0) //斜移X
	{
		fromY -= fromX;
		fromX = 0;
	}
	if (fromY < 0) //斜移Y
	{
		fromX -= fromY;
		fromY = 0;
	}
	if (fromY >= (int)buffer.Height() || fromX >= (int)buffer.Width() || toX < 0)
		return;
	if (toX >= (int)buffer.Width())
		toX = (int)buffer.Width() - 1;
	//开始绘制
	for (char* c = &buffer(fromX, fromY); fromX <= toX && fromY < (int)buffer.Height(); ++fromX, ++fromY, c = &buffer(fromX, fromY))
		*c = Mix(*c, '\\');
}

inline void MapRenderer::RenderRight(int fromX, int toX, int fromY) noexcept
{
	if (fromX > toX)//使from<to
	{
		fromX ^= toX, toX ^= fromX, fromX ^= toX; //交换
		fromY += toX - fromX; //计算fromY
	}
	//截取需要绘制的部分

	/*限制from处于方框内*/
	if (fromX < 0) //限制x>=0
	{
		fromY += fromX;
		fromX = 0;
	}
	if (fromY >= (int)buffer.Height()) //限制y<H
		fromX += fromY - (int)buffer.Height() + 1, fromY = (int)buffer.Height() - 1;
	if (fromX >= (int)buffer.Width() || fromY < 0 || toX < 0) //限制y>=0,x<W,toX>=0
		return;
	//toX需要限制>=0和<W，>=0上面限制过了
	if (toX >= (int)buffer.Width()) //限制toX<W
		toX = (int)buffer.Width() - 1;

	for (char* c = &buffer(fromX, fromY); fromX <= toX && fromY >= 0; ++fromX, --fromY, c = &buffer(fromX, fromY))
		*c = Mix(*c, '/');
}

inline void MapRenderer::RenderVertex2LinkNode(const VertexNode& v, const LinkNode& l) noexcept
{
	//保证l不在v之上
	if (l.x >= v.x && l.x <= v.x + v.name.size() + 1 && //x在边框里
		l.y >= v.y && l.y <= v.y + 2)
		return;
	//查找边框适合连线的位置，分为上下左右与四个角
	size_t linkerX = (v.name.size() + 2) / 2 + v.x; //中间X轴连线位置
	if (v.y - 1 >= l.y) //上段
		if (v.x > l.x) //左
			RenderPath(v.x - 1, v.y - 1, l.x, l.y); //1区
		else if (v.x + v.name.size() + 1 >= l.x) //中
			RenderPath(linkerX, v.y - 1, l.x, l.y); //2区
		else //右
			RenderPath(v.x + v.name.size() + 2, v.y - 1, l.x, l.y); //3区
	else if (v.y + 2 >= l.y) //中段
		RenderPath(v.x > l.x ? v.x - 1 : v.x + v.name.size() + 2, v.y + 1, l.x, l.y); //4，5区
	else //下段
	{
		if (v.x > l.x) //左
			RenderPath(v.x - 1, v.y + 3, l.x, l.y); //6区
		else if (v.x + v.name.size() + 1 >= l.x) //中
			RenderPath(linkerX, v.y + 3, l.x, l.y); //7区
		else //右
			RenderPath(v.x + v.name.size() + 2, v.y + 3, l.x, l.y); //8区
	}
}

inline void MapRenderer::RenderVertex2VertexNode(const VertexNode& v1, const VertexNode& v2) noexcept
{
	//借助一个跳转点跳转
	LinkNode node;
	node.x = (v1.x + v1.name.size() + 1) / 4 + (v2.x + v2.name.size() + 1) / 4;
	node.y = (v1.y + v2.y) / 2 + 1;
	RenderVertex2LinkNode(v1, node);
	RenderVertex2LinkNode(v2, node);
}

inline void MapRenderer::RenderLink2LinkNode(const LinkNode& l1, const LinkNode& l2) noexcept
{
	RenderPath(l1.x, l1.y, l2.x, l2.y);
}
#undef TransPositionVar
