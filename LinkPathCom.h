#pragma once

#include "MapWatcherCom.h"

class LinkPathCom :public MapWatcherCom
{
public:

	LinkPathCom(MapComInfo& map) :
		MapWatcherCom(map) {}

	virtual void OnPrint()
	{
		/*开始渲染*/
		map.mapRenderer.buffer.Reset();
		OnRender();
		if (!isSelectMode)
		{
			auto onNode = GetCurOnVertex();
			tmpNode.x = map.curX;
			tmpNode.y = map.curY;
			map.mapRenderer.camera.WorldPositon(tmpNode.x, tmpNode.y);
			//三部绘制：v1->x->v2
			//绘制初始顶点到第一个路径节点/光标
			if (pathHead == nullptr) //暂时没有路径，直接绘制到光标
			{
				if (onNode == nullptr)
					map.mapRenderer.RenderVertex2LinkNode(*selectNode, tmpNode);
				else if (onNode != selectNode) //光标在另一个顶点上，画v2v
					map.mapRenderer.RenderVertex2VertexNode(*selectNode, *onNode);
			}
			else
				map.mapRenderer.RenderVertex2LinkNode(*selectNode, *pathHead);
			//绘制
			if (pathHead != nullptr) //绘制路径节点间的路径
				map.mapRenderer.RenderNextPath(*pathHead);
			if (onNode != nullptr && onNode != selectNode) //如果光标在另一个顶点上
				if (pathTail == nullptr)
					map.mapRenderer.RenderVertex2VertexNode(*onNode, *selectNode);
				else
					map.mapRenderer.RenderVertex2LinkNode(*onNode, *pathTail);
			else if (pathTail != nullptr) //否则如果有最后一个结点的话，绘制最后一个顶点到光标
				map.mapRenderer.RenderLink2LinkNode(*pathTail, tmpNode);
		}
		cp.DisplayLine(map.mapRenderer.buffer.Data());
		OnMessage();
		cp.DisplayLine();
		if (isSelectMode)
			cp.DisplayLine(" # 请将光标放在初始顶点上并按回车进行选择");
		else
			cp.DisplayLine(" # 移动光标在空地按下回车就可以设置路径节点啦")
			.DisplayLine(" # 将光标放在一个顶点节点上以结束路径的绘制");
	}

	virtual void OnEvent(char input)
	{
		if (input == '\r')
		{
			if (isSelectMode)
			{
				selectNode = GetCurOnVertex();
				if (selectNode != nullptr)
					isSelectMode = false;
			}
			else
			{
				auto toNode = GetCurOnVertex();
				if (toNode == nullptr) //点到了空地
				{
					size_t wx = map.curX, wy = map.curY;
					map.mapRenderer.camera.WorldPositon(wx, wy);
					pathTail = map.nodeManager.MakePath(pathTail, wx, wy);
					if (pathHead == nullptr)
						pathHead = pathTail;
				}
				else if (toNode != selectNode) //点到了另一个节点上，完成路径绘制
				{
					if (selectNode->GetPath(toNode->id) != nullptr)
						cp.DisplayError("该路径已经存在了，请先删除之前的路径再进行绘制！");
					else
					{
						map.nodeManager.SetPath(selectNode->id, toNode->id, pathTail);
						cp.DisplayOK("路径绘制完成啦！");
					}
					pathHead = pathTail = selectNode = nullptr;
					isSelectMode = true;
					map.isBack = true;
				}
				else
					cp.DisplayError("不能建立从自己到自己的路径！");
			}
		}
		MapWatcherCom::OnEvent(input);
	}

private:

	LinkNode tmpNode;
	bool isSelectMode = true;
	LinkNode* pathHead = nullptr;
	LinkNode* pathTail = nullptr;
	VertexNode* selectNode = nullptr;
};
