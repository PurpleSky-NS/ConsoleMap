#pragma once

#include "MapWatcherCom.h"

class MapPathWatcher :public MapWatcherCom
{
public:

	MapPathWatcher(MapComInfo& map) :
		MapWatcherCom(map) {}

	void Work(const std::vector<VertexID>& vids)
	{
		path = vids;
	}

	virtual void OnPrint()
	{
		map.mapRenderer.buffer.Reset();
		map.mapRenderer.buffer(map.curX, map.curY) = '#';
		if (path.empty())
			return;
		bool isInPath = false;
		VertexNode* onCur = GetCurOnVertex();
		VertexNode* from;
		LinkNode* entry;
		for (size_t i = 1; i < path.size(); ++i)
		{
			from = map.nodeManager.GetVertex(path[i]);
			if (from == nullptr)
			{
				cp.DisplayError(cp.SwitchConcat().Display("不存在编号为 ").Display(path[i]).Display(" 的顶点").FlushOut()).SwitchDefault();
				break;
			}
			if (from == onCur) //测试光标在的顶点是不是路径节点
				isInPath = true;
			entry = from->GetPath(path[i - 1]);
			if (entry == nullptr)
			{
				cp.DisplayError(cp.SwitchConcat().Display("不存在从编号 ").Display(path[i]).Display(" 到").Display("编号 ").Display(path[i]).Display(" 的路径").FlushOut()).SwitchDefault();
				break;
			}
			map.mapRenderer.RenderVertexNode(*from);
			map.mapRenderer.RenderVertexPath(*from, *entry);
		}
		from = map.nodeManager.GetVertex(path[0]);
		if (from == nullptr)
			cp.DisplayError(cp.SwitchConcat().Display("不存在编号为 ").Display(path[0]).Display(" 的顶点").FlushOut()).SwitchDefault();
		else
			map.mapRenderer.RenderVertexNode(*from);
		if (from == onCur)
			isInPath = true;
		cp.DisplayLine(map.mapRenderer.buffer.Data());
		PrintCurMsg();
		if (onCur) //光标在顶点上
			PrintVertexMsg(*onCur);//输出顶点信息
	}

	virtual void OnEvent(char inputText)
	{
		if (inputText == '\r')
			map.isBack = true;
		else
			MapWatcherCom::OnEvent(inputText);
	}

private:
	std::vector<VertexID> path;
};
