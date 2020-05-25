#pragma once

#include "MapWatcherCom.h"

class DeleteVertexCom :public MapWatcherCom
{
public:

	DeleteVertexCom(MapComInfo& map) :
		MapWatcherCom(map) {}


	virtual void OnPrint()
	{
		MapWatcherCom::OnPrint();
		rmNode = GetCurOnVertex();
		cp.DisplayLine();
		if (rmNode != nullptr)
			cp.DisplayLine(" # 确认要删除吗(回车键确定，q键取消)？");
		else
			cp.DisplayLine(" # 请将光标移动到需要删除的顶点上(q键取消)");
	}

	virtual void OnEvent(char inputText)
	{
		if (inputText == '\r' && rmNode != nullptr)
		{
			map.nodeManager.DeleteVertex(rmNode->id);
			cp.DisplayOK("好的，已经删除啦");
			map.isBack = true;
			rmNode = nullptr;
		}
		else if (inputText == 'q')
			map.isBack = true;
		MapWatcherCom::OnEvent(inputText);
	}

private:
	VertexNode* rmNode = nullptr;
};
