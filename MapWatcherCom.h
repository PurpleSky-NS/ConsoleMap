#pragma once

#include <queue>
#include "ConsoleUI.h"
#include "MapComInfo.h"
#include "Input.h"

//命令：
//c 切换为普通移动模式(默认)，w a s d可以控制光标的移动"
//p 设置普通模式移动的步数(默认为1)，嫌跑得慢可以设置这个哦~"
//j 切换为顶点跳转模式，w a s d控制光标跳转到下一个离光标所指方向距离最近的顶点"
//f 查找一个顶点的编号并跳转"
//t 立刻跳转到某一个坐标上"
/*地图浏览器*/
class MapWatcherCom :public MapUIBase
{
public:

	MapWatcherCom(MapComInfo& map) :
		MapUIBase(map)
	{}

	/*渲染整个地图*/
	void OnRender()
	{
		map.mapRenderer.buffer(map.curX, map.curY) = '#';
		map.nodeManager.ForeachVertex([&](auto fromID, auto& from)
			{
				map.mapRenderer.RenderVertexNode(from);
				from.Foreach([&](auto toID, auto to)
					{
						if (fromID < toID)
							map.mapRenderer.RenderVertexPath(from, *to);
					});
			});
	}

	/*输出该输出的信息*/
	void OnMessage()
	{
		PrintCurMsg();
		ProcessCurOnVertex();
	}

	virtual void OnPrint()
	{
		map.mapRenderer.buffer.Reset();
		OnRender();
		cp.DisplayLine(map.mapRenderer.buffer.Data());
		OnMessage();
	}

	virtual void OnEvent(char input)
	{
		switch (input)
		{
		case 'c':
			if (map.isJumpMode)
			{
				map.isJumpMode = false;
				cp.DisplayOK("已切换为普通移动模式~");
			}
			else
				cp.DisplayOK("已经是该模式啦~");
			break;
		case 'j':
			if (!map.isJumpMode)
			{
				map.isJumpMode = true;
				cp.DisplayOK("已切换为顶点跳转模式~");
			}
			else
				cp.DisplayOK("已经是该模式啦~");
			break;
		case 'p':
		{
			int step = GetVal("请输入普通移动模式的移动步数：", 0);
			if (step > 0)
			{
				map.moveStep = step;
				cp.DisplayOK("设置步数成功~");
			}
			else
				cp.DisplayError("输入步数异常，很遗憾，设置失败...");
			break;
		}
		case 'f':
		{
			int id = GetVal("请输入顶点的编号：", -1);
			if (id < 0)
				cp.DisplayError("请输入正确的编号好吧...");
			else
			{
				auto node = map.nodeManager.GetVertex(id);
				if (node != nullptr)
				{
					JumpToNode(*node);
					cp.DisplayOK("跳转成功啦");
				}
				else
					cp.DisplayError("不存在这个节点呢...");
			}
			break;
		}
		case 't':
		{
			int x = GetVal("请输入横坐标X：", -1);
			if (x < 0)
			{
				cp.DisplayError("请输入正确坐标好吧...");
				return;
			}
			int y = GetVal("请输入纵坐标Y：", -1);
			if (y < 0)
			{
				cp.DisplayError("请输入正确坐标好吧...");
				return;
			}
			map.curX = x;
			map.curY = y;
			map.mapRenderer.camera.SetPosition(map.curX, map.curY);
			map.mapRenderer.camera.Move(-(int)(map.mapRenderer.buffer.Width() / 2), -(int)(map.mapRenderer.buffer.Height() / 2));
			map.mapRenderer.camera.ScreenPosition(map.curX, map.curY);
			cp.DisplayOK("跳转成功啦");
			break;
		}
		default:
			if (map.isJumpMode)
				OnJumpMode(input);
			else
				OnNormalMode(input);
			break;
		}
	}

	void OnNormalMode(char input)
	{
		switch (input)
		{
		case 'w':
			if (map.curY >= map.moveStep)
				map.curY -= map.moveStep;
			else
			{
				map.mapRenderer.camera.Move(0, -(int)(map.moveStep - map.curY));
				map.curY = 0;
			}
			break;
		case 'a':
			if (map.curX >= map.moveStep)
				map.curX -= map.moveStep;
			else
			{
				map.mapRenderer.camera.Move(-(int)(map.moveStep - map.curX), 0);
				map.curX = 0;
			}
			break;
		case 's':
			map.curY += map.moveStep;
			if (map.curY >= map.mapRenderer.buffer.Height())
			{
				map.mapRenderer.camera.Move(0, map.curY - map.mapRenderer.buffer.Height() + 1);
				map.curY = map.mapRenderer.buffer.Height() - 1;
			}
			break;
		case 'd':
			map.curX += map.moveStep;
			if (map.curX >= map.mapRenderer.buffer.Width())
			{
				map.mapRenderer.camera.Move(map.curX - map.mapRenderer.buffer.Width() + 1, 0);
				map.curX = map.mapRenderer.buffer.Width() - 1;
			}
			break;
		}
	}

	void OnJumpMode(char input)
	{
		std::priority_queue<std::pair<double, VertexID>, std::vector<std::pair<double, VertexID>>, std::greater<std::pair<double, VertexID>>> q;
		size_t x = map.curX, y = map.curY;
		map.mapRenderer.camera.WorldPositon(x, y);
		switch (input)
		{
		case 'w':
			map.nodeManager.ForeachVertex([&](auto id, auto& node)
				{
					if (node.y >= y)
						return;
					q.push({ double(node.y - y) * (node.y - y) + double(node.x - x) * (node.x - x),id });
				});
			break;
		case 'a':
			map.nodeManager.ForeachVertex([&](auto id, auto& node)
				{
					if (node.x >= x)
						return;
					q.push({ double(node.y - y) * (node.y - y) + double(node.x - x) * (node.x - x),id });
				});
			break;
		case 's':
			map.nodeManager.ForeachVertex([&](auto id, auto& node)
				{
					if (node.y <= y)
						return;
					q.push({ double(node.y - y) * (node.y - y) + double(node.x - x) * (node.x - x),id });
				});
			break;
		case 'd':
			map.nodeManager.ForeachVertex([&](auto id, auto& node)
				{
					if (node.x <= x)
						return;
					q.push({ double(node.y - y) * (node.y - y) + double(node.x - x) * (node.x - x),id });
				});
			break;
		}
		if (q.empty())
			cp.SwitchMessage().DisplayLine("已经到头啦！").SwitchDefault();
		else
			JumpToNode(*map.nodeManager.GetVertex(q.top().second));
	}

	void JumpToNode(const VertexNode& node)
	{
		size_t nw = map.mapRenderer.GetNodeWidth(node);
		size_t nh = map.mapRenderer.GetNodeHeight(node);
		size_t rx = GetEnd(node.x, nw);
		size_t ry = GetEnd(node.y, nh);
		if (map.mapRenderer.InCamera(node.x, node.y) &&
			map.mapRenderer.InCamera(rx, ry)) //看看是不是完全在屏幕上
		{
			//在屏幕上，直接把光标移过去
			map.curX = node.x;
			map.curY = node.y;
			map.mapRenderer.camera.ScreenPosition(map.curX, map.curY);
		}
		//否则就是从1~8区
		else
		{
			//先移动摄像机，然后确定屏幕光标位置
			if (node.y < map.mapRenderer.camera.y) //上段
				if (node.x < map.mapRenderer.camera.x) //1区，左上角
					map.mapRenderer.camera.SetPosition(node.x, node.y);
				else if (rx <= GetEnd(map.mapRenderer.camera.x, map.mapRenderer.buffer.Width())) //2区
					map.mapRenderer.camera.SetPosition(map.mapRenderer.camera.x, node.y);
				else //3区
					map.mapRenderer.camera.SetPosition(node.x + nw - map.mapRenderer.buffer.Width(), node.y);
			else if (ry <= GetEnd(map.mapRenderer.camera.y, map.mapRenderer.buffer.Height())) //中段
				if (node.x < map.mapRenderer.camera.x) //4区
					map.mapRenderer.camera.SetPosition(node.x, map.mapRenderer.camera.y);
				else //5区
					map.mapRenderer.camera.SetPosition(node.x + nw - map.mapRenderer.buffer.Width(), map.mapRenderer.camera.y);
			else
			{
				if (node.x < map.mapRenderer.camera.x) //6区
					map.mapRenderer.camera.SetPosition(node.x, node.y + nh - map.mapRenderer.buffer.Height());
				else if (rx <= GetEnd(map.mapRenderer.camera.x, map.mapRenderer.buffer.Width())) //7区
					map.mapRenderer.camera.SetPosition(map.mapRenderer.camera.x, node.y + nh - map.mapRenderer.buffer.Height());
				else //8区
					map.mapRenderer.camera.SetPosition(node.x + nw - map.mapRenderer.buffer.Width(), node.y + nh - map.mapRenderer.buffer.Height());
			}
			map.curX = node.x;
			map.curY = node.y;
			map.mapRenderer.camera.ScreenPosition(map.curX, map.curY);
		}
	}

	VertexNode* GetCurOnVertex()
	{
		size_t wx = map.curX, wy = map.curY;
		VertexNode* retNode = nullptr;
		map.mapRenderer.camera.WorldPositon(wx, wy);
		map.nodeManager.ForeachVertexWithCond([&](auto id, auto& node)
			{
				if (IsInRect_WH(wx, wy, node.x, node.y, map.mapRenderer.GetNodeWidth(node), map.mapRenderer.GetNodeHeight(node)))
				{
					retNode = &node;
					return false;
				}
				return true;
			});
		return retNode;
	}

	/*输出光标信息*/
	void PrintCurMsg()
	{
		cp.Display(" -> 坐标 ").Display(map.mapRenderer.camera.WorldX(map.curX)).DisplaySpace().Display(map.mapRenderer.camera.WorldY(map.curY)).DisplayLine(" <- ");
	}

	/*输出顶点信息*/
	void PrintVertexMsg(const VertexNode& node)
	{
		cp.Display(" -> ").Display("顶点名称：").DisplayLine(node.name)
			.Display(" -> ").Display("顶点编号：").DisplayLine(node.id);
	}

	/*处理当前光标所在位置是否在顶点上，是的话会输出顶点信息*/
	void ProcessCurOnVertex()
	{
		auto node = GetCurOnVertex();
		if (node != nullptr)
			PrintVertexMsg(*node);
	}

};
