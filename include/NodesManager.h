#pragma once

#include <list>
#include "File.h"
#include "Node.h"

class NodesManager
{
public:

	~NodesManager();

	//节点相关函数

	/*插入一个新节点，若已存在则插入失败*/
	bool InsertVertex(const VertexNode& node);

	/*删除节点*/
	bool DeleteVertex(VertexID id);

	/*获取一个节点*/
	VertexNode* GetVertex(VertexID id)const;

	/*遍历所有顶点*/
	void ForeachVertex(std::function<void(VertexID, VertexNode&)> func);

	/*遍历所有顶点，回调函数返回false则停止遍历*/
	void ForeachVertexWithCond(std::function<bool(VertexID, VertexNode&)> func);

	//路径相关函数

	/*制作路径，从node到下一个节点，返回新节点，node可以为null*/
	LinkNode* MakePath(LinkNode* node, size_t x, size_t y);

	/*将node插到which的next上，若which为顶点，返回false*/
	bool InsertNext(LinkNode* which, LinkNode* node);

	/*将node插到which的next上，若which为顶点，返回false*/
	bool InsertPrev(LinkNode* which, LinkNode* node);

	/*设置v1与v2之间的通路路径，node若为null表示直链*/
	bool SetPath(VertexID from, VertexID to, LinkNode* node);

	/*设置v1与v2之间的通路路径，node若为null表示直链*/
	bool DeletePath(VertexID v1, VertexID v2);

	/*清空*/
	void Clear();

	/*保存地图*/
	bool Save(const std::string& path);

	/*读取地图*/
	bool Load(const std::string& path);

private:
	std::unordered_map<VertexID, VertexNode> vertexes; //存放所有节点
	std::list<LinkNode*> nodePool; //所有节点放在这里，方便析构统一管理
};
