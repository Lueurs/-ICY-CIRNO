#pragma once
#include<vector>

using ushort = unsigned short;

enum icy_nodetype_t:ushort
{
	OBJECT,
	CONST_OBJECT,


	MOV,

	CALL,

	ADD,
	MINUS,
	MUL,
	DIV,
	POW,


}

struct icyAstNode
{
	std::vector<icyAstNode*>  sub_nodes; // 这里储存了子节点的指针，默认两个子节点。
	ict_nodetype_t            node_type;		//该节点的类型
	uint                      source;			//该节点的资源编号
	
};
