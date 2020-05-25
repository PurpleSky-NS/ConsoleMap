#pragma once

#include <unordered_map>
#include <functional>

using VertexID = size_t;

/*空间上的一个路径节点，用来做路径使用*/
struct LinkNode
{
	/*节点坐标*/
	size_t x;
	size_t y;
	/*双向链表结点指针*/
	LinkNode* next;
	LinkNode* prev;

	LinkNode()noexcept :
		LinkNode(0, 0, nullptr, nullptr) {}
	LinkNode(size_t x, size_t y)noexcept :
		x(x), y(y), next(nullptr), prev(nullptr) {}
	LinkNode(size_t x, size_t y, LinkNode* next, LinkNode* prev)noexcept :
		x(x), y(y), next(next), prev(prev) {}

	/*是否是个顶点*/
	virtual bool IsVertex()const
	{
		return false;
	}
};

/*表示一个顶点节点*/
struct VertexNode :public LinkNode
{
public:
	VertexID id;//ID
	std::string name; //名称

	inline VertexNode(VertexID id)noexcept;
	inline VertexNode(VertexID id, size_t x, size_t y, const std::string& name)noexcept;

	/*获取到达某一邻接节点的路径(头/尾)*/
	inline LinkNode* GetPath(VertexID id)const;

	/*遍历所有邻接节点*/
	inline void Foreach(std::function<void(VertexID, LinkNode*)> func)const;

	/*遍历一条通路*/
	inline void ForeachPath(VertexID id, std::function<void(LinkNode*)> func)const;

	/*是否是个顶点*/
	virtual bool IsVertex()const override;

private:
	friend class NodesManager;

	std::unordered_map<VertexID, LinkNode*> neighbors; //存储能到哪个顶点，以及它的空间跳转节点 

	/*为该节点添加/更新邻接节点与路径*/
	inline void SetPath(VertexID id, LinkNode& node);

	/*删除邻接节点与路径*/
	inline void DeletePath(VertexID id);
};

inline VertexNode::VertexNode(VertexID id) noexcept :
	id(id)
{
	next = prev = nullptr;
}

inline VertexNode::VertexNode(VertexID id, size_t x, size_t y, const std::string& name) noexcept :
	LinkNode(x, y),
	id(id),
	name(name)
{}

inline LinkNode* VertexNode::GetPath(VertexID id) const
{
	auto fd = neighbors.find(id);
	return (fd == neighbors.end() ? nullptr : fd->second);
}

inline void VertexNode::SetPath(VertexID id, LinkNode& node)
{
	neighbors[id] = &node;
}

inline void VertexNode::DeletePath(VertexID id)
{
	neighbors.erase(id);
}

inline void VertexNode::Foreach(std::function<void(VertexID, LinkNode*)> func) const
{
	for (const auto& i : neighbors)
		func(i.first, i.second);
}

inline void VertexNode::ForeachPath(VertexID id, std::function<void(LinkNode*)> func)const
{
	LinkNode* node = GetPath(id);
	if (node == nullptr)
		return;
	if (node->next == this) //如果自己是尾
		for (; node->prev != nullptr; node = node->prev)
			func(node);
	else
		for (; node->next != nullptr; node = node->next)
			func(node);
}

inline bool VertexNode::IsVertex() const
{
	return true;
}
