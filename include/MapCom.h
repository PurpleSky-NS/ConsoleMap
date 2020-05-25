#pragma once

#include "Input.h"
#include "ConsoleUI.h"
#include "MapComInfo.h"
#include "MapWatcherCom.h"
#include "InsertVertexCom.h"
#include "LinkPathCom.h"
#include "DeleteVertexCom.h"
#include "DeletePathCom.h"
#include "UpdateVertexCom.h"
#include "MapPathWatcher.h"

/*采用委托模式，主Com委托各种功能不同的Com协同工作*/
class MapCom :public UIComponent
{
	class TipsCom :public MapUIBase
	{
	public:
		TipsCom(MapComInfo& map) :
			MapUIBase(map) {}

		virtual void OnPrint()
		{
			cp.DisplayLine("命令按键说明，一个说明书哦~")
				.DisplayTab().DisplayLine("全部命令都可以在另一个命令的基础上使用，比如你可以在制作路径的时候使用命令跳转到另一个顶点上")
				.DisplayLine("全局命令按键：")
				.DisplayTab().DisplayLine("i 重新设定屏幕的长宽")
				.DisplayTab().DisplayLine("q 退出")
				.DisplayLine("浏览模式相关命令按键:")
				.DisplayTab().DisplayLine("c 切换为普通移动模式(默认)，wasd可以控制光标的移动")
				.DisplayTab().DisplayLine("p 设置普通模式移动的步数(默认为1)，嫌跑得慢可以设置这个哦~")
				.DisplayTab().DisplayLine("j 切换为顶点跳转模式，wasd控制光标跳转到下一个离光标所指方向距离最近的顶点")
				.DisplayTab().DisplayLine("f 查找一个顶点的编号并跳转")
				.DisplayTab().DisplayLine("t 立刻跳转到某一个坐标上")
				.DisplayLine("地图制作相关命令按键:")
				.DisplayTab().DisplayLine("v 生成并插入一个顶点")
				.DisplayTab().DisplayLine("u 更新顶点信息以及位置")
				.DisplayTab().DisplayLine("r 删除一个顶点")
				.DisplayTab().DisplayLine("m 开始制作路径")
				.DisplayTab().DisplayLine("e 删除一条路径")
				.DisplayLine("地图存储相关命令按键:")
				.DisplayTab().DisplayLine("z 加载地图")
				.DisplayTab().DisplayLine("x 保存地图")
				.DisplayOK("按回车键返回哦~");

		}
		virtual void OnEvent(char input)
		{
			if (input == '\r')
				map.isBack = true;
		}
	};

public:

	MapCom() :
		workUI(&watcherCom),
		tipsCom(map),
		watcherCom(map),
		insertVCom(map),
		linkPathCom(map),
		deleteVCom(map),
		deletePathCom(map),
		updateVCom(map),
		pathWatcher(map)
	{
		map.mapRenderer.buffer.Init(60, 20);
	}

	virtual void OnPrint()
	{
		workUI->OnPrint();
	}

	virtual void OnEvent(char inputText)
	{
		/*处理输入*/
		switch (inputText)
		{
		case 'i':
		{
			cp.DisplayLine(" # 开始重新设置屏幕大小...").Flush();
			int w = GetVal("请输入宽度：", 0);
			int h = GetVal("请输入高度：", 0);
			if (w == 0 || h == 0)
			{
				cp.DisplayError("输入数值无效，你可以重新设置呀");
				return;
			}
			map.mapRenderer.buffer.Init(w, h);
			cp.DisplayOK("设置成功，继续使用吧~");
			break;
		}
		case 'q':
			SurfaceManager::GetInstance().CloseTop();
			return;
		case '?':
			SwitchToUI(&tipsCom);
			break;
		case 'v':
			SwitchToUI(&insertVCom);
			insertVCom.Work();
			break;
		case 'm':
			SwitchToUI(&linkPathCom);
			break;
		case 'r':
			SwitchToUI(&deleteVCom);
			break;
		case 'e':
			SwitchToUI(&deletePathCom);
			break;
		case 'u':
			SwitchToUI(&updateVCom);
			break;
		case 'z':
			LoadMap();
			break;
		case 'x':
			SaveMap();
			break;
		default:
			workUI->OnEvent(inputText);
			break;
		}
		/*处理返回*/
		ProcessBack();
		GetContext()->Refresh();
	}

	virtual bool WillRegisterEvnet()const
	{
		return true;
	}

	bool UpdateVertex(VertexID id, const std::string& name)
	{
		auto node = map.nodeManager.GetVertex(id);
		if (node != nullptr)
		{
			node->name = name;
			return true;
		}
		return false;
	}

	/*渲染某一条路径，传入经过的顶点*/
	void RenderPath(const std::vector<VertexID>& vids)
	{
		SwitchToUI(&pathWatcher);
		pathWatcher.Work(vids);
	}

	bool Save(const std::string& path)
	{
		return map.nodeManager.Save(path);
	}

	bool Load(const std::string& path)
	{
		return map.nodeManager.Load(path);
	}

private:
	MapComInfo map;

	UIComponent* workUI;
	UIComponent* lastUI = nullptr;

	TipsCom tipsCom;
	MapWatcherCom watcherCom;
	InsertVertexCom insertVCom;
	LinkPathCom linkPathCom;
	DeleteVertexCom deleteVCom;
	DeletePathCom deletePathCom;
	UpdateVertexCom updateVCom;
	MapPathWatcher pathWatcher;


	void ProcessBack()
	{
		if (map.isBack)
		{
			if (lastUI != nullptr)
			{
				workUI = lastUI;
				lastUI = nullptr;
			}
			else
				workUI = &watcherCom;
			map.isBack = false;
		}
	}

	void SwitchToUI(UIComponent* ui)
	{
		if (ui != nullptr)
		{
			lastUI = workUI;
			workUI = ui;
		}
	}

	void LoadMap()
	{
		auto path = GetLine(" # 请输入完整地图文件路径(什么也不输入表示不读取)：");
		if (path.empty())
			return;
		if (map.nodeManager.Load(path))
			cp.DisplayOK("读取地图成功！");
		else
			cp.DisplayError("读取地图失败，可能是因为地图文件损坏或者 " + path + " 不存在...");
	}

	void SaveMap()
	{
		std::string path = GetLine(" # 请输入要保存的地图文件名/路径(什么也不输入表示不保存)：");
		if (path.empty())
			return;
		if (path.size() < 6 || path.substr(path.size() - 6, 6) != ".clmap") //判断用户写文件名带不带后缀
			path += ".clmap";
		if (map.nodeManager.Save(path))
			cp.DisplayOK("保存地图成功，地图已保存在：" + path);
		else
			cp.DisplayError("存储地图失败，请检查是否存在同名文件：" + path);
	}
};
