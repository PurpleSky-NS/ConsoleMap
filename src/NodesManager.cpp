#include "NodesManager.h"

NodesManager::~NodesManager()
{
	Clear();
}

bool NodesManager::InsertVertex(const VertexNode& node)
{
	return vertexes.insert({ node.id, node }).second;
}

bool NodesManager::DeleteVertex(VertexID id)
{
	//需要删除该节点和他相关联的路径节点(麻烦)
	auto fd = vertexes.find(id);
	if (fd == vertexes.end())
		return false;
	auto& vertex = fd->second;
	std::vector<VertexID> delPath(vertex.neighbors.size());
	vertex.Foreach([&](auto to, auto node) //查找所有连接节点并删除
		{
			delPath.push_back(to);
		});
	for (auto& i : delPath)
		DeletePath(id, i);
	vertexes.erase(id);//删掉节点
	return true;
}

VertexNode* NodesManager::GetVertex(VertexID id)const
{
	auto fd = vertexes.find(id);
	return (fd == vertexes.end() ? nullptr : (VertexNode*)&fd->second);
}

void NodesManager::ForeachVertex(std::function<void(VertexID, VertexNode&)> func)
{
	for (auto& i : vertexes)
		func(i.first, i.second);
}

void NodesManager::ForeachVertexWithCond(std::function<bool(VertexID, VertexNode&)> func)
{
	for (auto& i : vertexes)
		if (!func(i.first, i.second))
			return;
}

LinkNode* NodesManager::MakePath(LinkNode* node, size_t x, size_t y)
{
	LinkNode* newNode;
	if (node == nullptr)
		newNode = new LinkNode(x, y);
	else
	{
		newNode = new LinkNode(x, y, nullptr, node);
		node->next = newNode;
	}
	nodePool.push_back(newNode);
	return newNode;
}

bool NodesManager::InsertNext(LinkNode* which, LinkNode* node)
{
	if (which->IsVertex())
		return false;
	node->prev = which;
	node->next = which->next;
	which->next->prev = node;
	which->next = node;
	return true;
}

bool NodesManager::InsertPrev(LinkNode* which, LinkNode* node)
{
	if (which->IsVertex())
		return false;
	node->next = which;
	node->prev = which->prev;
	which->prev->next = node;
	which->prev = node;
	return true;
}

bool NodesManager::SetPath(VertexID fromID, VertexID toID, LinkNode* toPath)
{
	if (fromID == toID)
		return false;
	auto fd = vertexes.find(fromID);
	if (fd == vertexes.end())
		return false;
	auto& fromNode = fd->second;
	fd = vertexes.find(toID);
	if (fd == vertexes.end())
		return false;
	auto& toNode = fd->second;
	if (toPath == nullptr) //如果没有路径节点，直链就完事了
	{
		fromNode.SetPath(toID, toNode);
		toNode.SetPath(fromID, fromNode);
		return true;
	}
	//找到node的开头
	LinkNode* fromPath = toPath;
	if (toPath->next == nullptr) //node是尾
	{
		for (; fromPath->prev != nullptr; fromPath = fromPath->prev);
		fromPath->prev = &fromNode;
		toPath->next = &toNode;
	}
	else
	{
		for (; fromPath->next != nullptr; fromPath = fromPath->next);
		fromPath->next = &fromNode;
		toPath->prev = &toNode;
	}
	fromNode.SetPath(toID, *fromPath);
	toNode.SetPath(fromID, *toPath);
	return true;
}

bool NodesManager::DeletePath(VertexID v1, VertexID v2)
{
	auto fd = vertexes.find(v1);
	if (fd == vertexes.end())
		return false;
	auto& vertex1 = fd->second;
	fd = vertexes.find(v2);
	if (fd == vertexes.end())
		return false;
	auto& vertex2 = fd->second;
	auto node = vertex1.GetPath(v2);
	if (node == nullptr)
		return false;
	if (node->next == &vertex1) //如果自己是尾
		for (; node->prev != nullptr; )
		{
			if (node->IsVertex())
				break;
			auto des = node;
			node = node->prev;
			delete des;
			nodePool.erase(std::find(nodePool.begin(), nodePool.end(), des));
		}
	else
		for (; node->next != nullptr; )
		{
			if (node->IsVertex())
				break;
			auto des = node;
			node = node->next;
			delete des;
			nodePool.erase(std::find(nodePool.begin(), nodePool.end(), des));
		}
	vertex1.DeletePath(v2);
	vertex2.DeletePath(v1);
	return true;
}

void NodesManager::Clear()
{
	vertexes.clear();
	for (auto& i : nodePool)
		delete i;
	nodePool.clear();
}

bool NodesManager::Save(const std::string& path)
{
	/*
	文件结构
		CMAP作为文件标志
		vertexs.size -> 顶点信息(id,name,x,y)
		路径信息(fromID,toID,nodeSize) -> 路径节点信息(x,y)
	*/
	/*先写头信息*/
	File f;
	if (!f.OpenWithOut(path))
		return false;
	f.Write('C').Write('M').Write('A').Write('P'); //写入4个字节头信息
	f.Write((uint32_t)vertexes.size()); //写入顶点数量
	for (auto& i : vertexes) //开始写入各个顶点
		f.Write((uint32_t)i.second.id).Write(i.second.name)
		.Write((uint32_t)i.second.x).Write((uint32_t)i.second.y);//写入顶点基本信息
	//开始写入所有的路径信息
	for (auto& i : vertexes) //开始写入各个顶点
	{
		i.second.Foreach([&](auto id, auto entry)
			{
				if (i.first > id) //只写from.id<to.id的路径信息，节省空间
					return;
				size_t nodeNum = 0;
				for (auto node = entry; !node->IsVertex(); ++nodeNum, node = node->next);
				f.Write((uint32_t)i.first).Write((uint32_t)id);//写入from与to的id
				f.Write((uint32_t)nodeNum); //写入路径节点个数
				for (; !entry->IsVertex(); entry = entry->next)
					f.Write((uint32_t)(entry->x)).Write((uint32_t)(entry->y));//写入顶点坐标
			});
	}
	return true;
}

bool NodesManager::Load(const std::string& path)
{
	File f;
	if (!f.OpenWithIn(path))
		return false;
	try //这段内容不可能出eof，除非错误
	{
		if (f.Read<char>() != 'C' || f.Read<char>() != 'M' || f.Read<char>() != 'A' || f.Read<char>() != 'P') //读出4个字节头信息
			return false;
		Clear();
		size_t size = f.Read<uint32_t>(); //读出顶点数量
		{
			VertexNode v(0);
			for (size_t i = 0; i < size; ++i) //开始读出各个顶点
			{
				//读出顶点基本信息
				v.id = f.Read<uint32_t>();
				v.name = f.Read<std::string>();
				v.x = f.Read<uint32_t>();
				v.y = f.Read<uint32_t>();
				InsertVertex(v);
			}
		}
		//开始读出所有的路径信息，变长内容
		VertexID from, to;
		LinkNode* path;
		while (f)
		{
			from = f.Read<uint32_t>();
			if (!f)//若读完了就返回true
				return true;
			to = f.Read<uint32_t>();
			path = nullptr;
			for (size_t i = 0, num = f.Read<uint32_t>(), x, y; i < num; ++i)
			{
				x = (size_t)f.Read<uint32_t>();
				y = (size_t)f.Read<uint32_t>();
				path = MakePath(path, x, y);
			}
			SetPath(from, to, path);
		}
	}
	catch (File::FileEnd)
	{
		return false;
	}
	return true;
}
