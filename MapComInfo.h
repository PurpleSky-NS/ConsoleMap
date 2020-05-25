#pragma once

#include "NodesManager.h"
#include "MapRenderer.h"

struct MapComInfo
{
	bool isBack = false; //是否返回原UI组件
	bool isJumpMode = false;
	size_t curX = 0, curY = 0; //屏幕光标位置
	size_t moveStep = 1; //一次跳几格
	NodesManager nodeManager;
	MapRenderer mapRenderer;
};

class MapUIBase :public UIComponent
{
public:
	MapUIBase(MapComInfo& map) :
		map(map) {}
protected:
	MapComInfo& map;
};
