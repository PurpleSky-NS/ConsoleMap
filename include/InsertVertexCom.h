#pragma once

#include "MapWatcherCom.h"

class InsertVertexCom :public MapWatcherCom
{
public:

	InsertVertexCom(MapComInfo& map) :
		MapWatcherCom(map), iNode(0) {}

	void Work()
	{
		int id = GetVal("为该节点设置一个唯一的编号：", -1);
		if (id < 0)
		{
			cp.DisplayError("编号输入有误，顶点生成失败");
			map.isBack = true;
			return;
		}
		if (map.nodeManager.GetVertex(id) != nullptr)
		{
			cp.DisplayError("编号已经存在啦，顶点生成失败");
			map.isBack = true;
			return;
		}
		iNode.id = (size_t)id;
		iNode.name = GetStr("为该节点起个名字吧：");
	}

	virtual void OnPrint()
	{
		/*给iNode设置坐标*/
		iNode.x = map.curX;
		iNode.y = map.curY;
		map.mapRenderer.camera.WorldPositon(iNode.x, iNode.y);
		/*开始渲染*/
		map.mapRenderer.buffer.Reset();
		OnRender();
		map.mapRenderer.RenderVertexNode(iNode);
		cp.DisplayLine(map.mapRenderer.buffer.Data());
		OnMessage();
		cp.DisplayLine().DisplayLine(" # 选好位置后按下回车就可以安放该顶点了");
	}

	virtual void OnEvent(char input)
	{
		if (input == '\r')
		{
			iNode.x = map.curX;
			iNode.y = map.curY;
			map.mapRenderer.camera.WorldPositon(iNode.x, iNode.y);
			map.nodeManager.InsertVertex(iNode);
			map.isBack = true;
			return;
		}
		MapWatcherCom::OnEvent(input);
	}

private:

	VertexNode iNode;
};
