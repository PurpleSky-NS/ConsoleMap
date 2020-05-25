#pragma once

#include "MapWatcherCom.h"

class DeletePathCom :public MapWatcherCom
{
public:

	DeletePathCom(MapComInfo& map) :
		MapWatcherCom(map) {}


	virtual void OnPrint()
	{
		MapWatcherCom::OnPrint();
		cp.DisplayLine();
		if (v1 == nullptr)
			cp.DisplayLine(" # 请将光标移动到第一个顶点上(q键取消)");
		else
			cp.DisplayLine(" # 请将光标移动到第二个顶点上(q键取消)");
	}

	virtual void OnEvent(char inputText)
	{
		if (inputText == '\r')
		{
			if (v1 == nullptr) //选v1
				v1 = GetCurOnVertex();
			else //选v2
				v2 = GetCurOnVertex();
			if (v1 != nullptr && v2 != nullptr) //开始删除
			{
				if (v1 == v2)
				{
					v2 = nullptr;
					cp.DisplayError("自己和自己之间没有路径啦，请重新选择第二个顶点！");
				}
				else if (map.nodeManager.DeletePath(v1->id, v2->id))
				{
					map.isBack = true;
					v1 = nullptr;
					v2 = nullptr;
					cp.DisplayOK("删除路径成功~");
				}
				else
				{
					v2 = nullptr;
					cp.DisplayError("这俩顶点不存在路径哦，请重新选择第二个顶点~");
				}
			}
		}
		else if (inputText == 'q')
		{
			v1 = nullptr;
			v2 = nullptr;
			map.isBack = true;
		}
		else
			MapWatcherCom::OnEvent(inputText);
	}

private:
	VertexNode* v1 = nullptr;
	VertexNode* v2 = nullptr;
};
