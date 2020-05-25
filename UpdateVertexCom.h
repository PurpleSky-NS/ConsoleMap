#pragma once

#include "MapWatcherCom.h"

class UpdateVertexCom :public MapWatcherCom
{
public:

	UpdateVertexCom(MapComInfo& map) :
		MapWatcherCom(map) {}

	virtual void OnPrint()
	{
		/*给iNode设置坐标*/
		if (node != nullptr && updatePos)
		{
			node->x = map.curX;
			node->y = map.curY;
			map.mapRenderer.camera.WorldPositon(node->x, node->y);
			/*开始渲染*/
			map.mapRenderer.buffer.Reset();
			OnRender();
			map.mapRenderer.RenderVertexNode(*node);
			cp.DisplayLine(map.mapRenderer.buffer.Data());
			OnMessage();
			cp.DisplayLine().DisplayLine(" # 选好位置后按下回车就可以更新该顶点坐标了");
		}
		else
		{
			MapWatcherCom::OnPrint();
			cp.DisplayLine();
			if (!updatePos && node != nullptr)
				cp.DisplayLine(" # 是否更新除位置外顶点的信息？(回车键进行更新)~");
			else
				cp.DisplayLine(" # 请选择你要修改的顶点~");
		}
	}

	virtual void OnEvent(char input)
	{
		if (input == '\r')
		{
			if (node != nullptr)
			{
				if (!updatePos)
				{
					updatePos = true;
					int id = GetLineVal("为该节点更新一个唯一的编号(直接回车键则不更新)：", node->id);
					if (id < 0)
					{
						cp.DisplayError("编号输入有误，更新失败");
						map.isBack = true;
						return;
					}
					else if (id != node->id && map.nodeManager.GetVertex(id) != nullptr)
					{
						cp.DisplayError("编号已经被其他顶点占用啦，更新失败");
						map.isBack = true;
						return;
					}
					node->id = (size_t)id;
					std::string name = GetLine("为该节点更新名字(直接回车键则不更新)：");
					if (!name.empty())
						node->name = name;
				}
				else
				{
					cp.DisplayOK("更新顶点完成！");
					updatePos = false;
					map.isBack = true;
					node = nullptr;
				}
			}
			else if ((node = GetCurOnVertex()) != nullptr)
			{
				//将光标设置到顶点上
				map.curX = node->x;
				map.curY = node->y;
				map.mapRenderer.camera.ScreenPosition(map.curX, map.curY);
			}
		}
		else if (node != nullptr && !updatePos)
			updatePos = true;
		else
			MapWatcherCom::OnEvent(input);
	}

private:

	bool updatePos = false; //是否正在更新位置
	VertexNode* node = nullptr;
};
