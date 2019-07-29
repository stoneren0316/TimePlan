#pragma once
#include <vector>
#include <list>
#include <map>
#include <iostream>

enum TPNODE_TYPE{
	TPNODE_NORMAL = 0,
	TPNODE_TUNNEL = 1
};

struct TPNode
{
	TPNode(int ID, TPNODE_TYPE type, int TU, int ad1, TPNODE_TYPE adType1, int ad2, TPNODE_TYPE adType2)
		: NodeID(ID), NodeType(type), TUCost(TU), Addr1(ad1), Addr1Type(adType1), Addr2(ad2), Addr2Type(adType2)
	{

	}
	~TPNode()
	{

	}
	
	int NodeID;//唯一ID
	TPNODE_TYPE NodeType;//类型，用于标记tunnel
	int TUCost;//花费时间

	//记录繁忙时间，列表里记录的是被占用的开始时间，加上TUCost可计算出繁忙结束时间
	//如果是tunnel，繁忙时间不可重叠，否则繁忙时间可重叠，此时可认为在此发生汇合或碰撞
	std::list<int> BusyTimes;

	//为简单，地址采用整数表示
	int Addr1;
	TPNODE_TYPE Addr1Type;
	int Addr2;
	TPNODE_TYPE Addr2Type;

	//检查此到达时间是否引起冲突需要等待
	//@return 0:不需等待，大于0：需要等待的时间
	int CheckBusy(int StartTime)
	{
		std::list<int>::iterator it = BusyTimes.begin();
		for (; it != BusyTimes.end(); ++it)
		{
			int TimeDiff = StartTime - (*it);
			if (TimeDiff < TUCost && (-TimeDiff) < TUCost)//abs(TimeDiff) < TUCost, 有冲突发生
			{
				int AdjustTime = TUCost - TimeDiff;//计算调整出发时间，使不需等待
				return AdjustTime;
			}
		}
		return 0;
	}
};

struct TPEdge
{
	TPEdge(int ID1, int ID2)
		:NodeID1(ID1), NodeID2(ID2)
	{
	}
	~TPEdge()
	{

	}

	int NodeID1;
	int NodeID2;
};

class TPPath
{
public:
	TPPath()
		:m_RetDepartTime(0)
	{
	}
	~TPPath()
	{
		std::vector<TPNode*>::iterator it = m_Nodes.begin();
		for (; it != m_Nodes.end(); ++it) 
		{
			delete (*it);
		}
	}
	//路径经过的Nodes
	std::vector<TPNode*> m_Nodes;
	//存放每个Node结束时的时间点，元素个数和m_Nodes相同
	std::vector<int> m_EndTimes;
	//出发时间
	int m_RetDepartTime;

	//递归计算无冲突无等待条件下的时间	
	void PlanDepartTime(int DepartTime = 0)
	{
		int LastTime = DepartTime;
		int AdjustTime = 0;
		std::vector<TPNode*>::iterator it = m_Nodes.begin();
		for (; it != m_Nodes.end(); ++it)
		{
			if ((*it) == NULL)
			{
				//error
				continue;
			}
			if ((*it)->NodeType == TPNODE_TUNNEL)
			{
				AdjustTime = (*it)->CheckBusy(LastTime);
				if (AdjustTime > 0)//有访问冲突，需等待，此时修改出发时间，重新规划
				{
					break;
				}
			}
			int NewTime = LastTime + (*it)->TUCost;//此Node结束时间，作为下一Node开始时间
			LastTime = NewTime;
			
			m_EndTimes.push_back(NewTime);
		}

		if (AdjustTime > 0)//有访问冲突，需等待，此时修改出发时间，重新规划
		{
			DepartTime += AdjustTime;
			m_EndTimes.clear();
			PlanDepartTime(DepartTime);
		}
		else//不需等待，出发时间确定
		{
			m_RetDepartTime = DepartTime;
			//更新tunnel busytime			
			for (int i = 0;i < m_Nodes.size(); ++i)
			{
				if (m_Nodes[i]->NodeType == TPNODE_TUNNEL)
				{
					m_Nodes[i]->BusyTimes.push_back(m_EndTimes[i] - m_Nodes[i]->TUCost);
				}
			}
		}
	}
};

class TPGragh
{
public:
	TPGragh()
	{

	}
	virtual ~TPGragh()
	{

	}

protected:
	//为方便访问Node，使用了两种方式存放Nodes
	std::vector<TPNode*> m_Nodes;
	std::map<int, TPNode*> m_NodeMap;
	std::vector<TPEdge> m_Edges;

	void AddNode(int ID, TPNODE_TYPE type, int TU, int ad1, TPNODE_TYPE ad1type, int ad2, TPNODE_TYPE ad2type)
	{
		TPNode* pNode = new TPNode(ID, type, TU, ad1, ad1type, ad2, ad2type);
		m_Nodes.push_back(pNode);
		m_NodeMap.insert(std::make_pair(pNode->NodeID, pNode));
	}

	void AddEdge(const TPNode& Node1, const TPNode& Node2)
	{
		m_Edges.push_back(TPEdge(Node1.NodeID, Node2.NodeID));
	}

	bool CheckNodeConnect(const TPNode& Node1, const TPNode& Node2)
	{
		if (Node1.NodeType == TPNODE_TUNNEL)//Node1类型为tunnel
		{
			if (Node1.Addr1 == Node2.Addr1 || Node1.Addr1 == Node2.Addr2)//相邻
			{
				return true;
			}
		}
		else if (Node2.NodeType == TPNODE_TUNNEL)//Node2类型为tunnel
		{
			if (Node2.Addr1 == Node1.Addr1 || Node2.Addr1 == Node1.Addr2)//相邻
			{
				return true;
			}
		}
		//都不为隧道
		//（假设不应该出现都为tunnel的情况，因为如果都为tunnel，就可以合并为一条tunnel，所以两tunnel之间应该会有一个Node）
		// (如果允许两个tunnel相邻，那么只需把其之间的Node的TU设为0)
		else 
		{
			//任意地址相同则相邻,且不为tunnel，则相邻
			if ((Node1.Addr1 == Node2.Addr1 || Node1.Addr1 == Node2.Addr2) && Node1.Addr1Type != TPNODE_TUNNEL)
			{
				return true;
			}
			if ((Node1.Addr2 == Node2.Addr1 || Node1.Addr2 == Node2.Addr2) && Node1.Addr2Type != TPNODE_TUNNEL)
			{
				return true;
			}
		}

		return false;
	}

public:
	//根据实际地理位置初始化Nodes
	void InitNodes()
	{
		int GenNodeID = 1;
		AddNode(GenNodeID++, TPNODE_NORMAL, 10, 1, TPNODE_NORMAL, 4, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 12, 2, TPNODE_NORMAL, 4, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 10, 3, TPNODE_NORMAL, 6, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_TUNNEL, 5, 4, TPNODE_NORMAL, 0, TPNODE_NORMAL);
		AddNode(GenNodeID++, TPNODE_TUNNEL, 10, 5, TPNODE_NORMAL, 0, TPNODE_NORMAL);
		AddNode(GenNodeID++, TPNODE_TUNNEL, 5, 6, TPNODE_NORMAL, 0, TPNODE_NORMAL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 5, 7, TPNODE_NORMAL, 5, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 10, 8, TPNODE_NORMAL, 6, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 10, 9, TPNODE_NORMAL, 5, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 5, 4, TPNODE_TUNNEL, 5, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 10, 4, TPNODE_TUNNEL, 6, TPNODE_TUNNEL);
		AddNode(GenNodeID++, TPNODE_NORMAL, 8, 6, TPNODE_TUNNEL, 5, TPNODE_TUNNEL);
	}

	//根据Node的相邻性，生成edge
	void GenerateEdge()
	{
		for (int i = 0; i < m_Nodes.size(); i++)
		{
			for (int j = i + 1; j < m_Nodes.size(); j++)
			{
				if (CheckNodeConnect(*m_Nodes[i], *m_Nodes[j]) == true)
				{
					AddEdge(*m_Nodes[i], *m_Nodes[j]);
				}
			}
		}
	}

	//简化求路径的部分直接给出结果，否则此函数应计算gragh中两Node的最短路径，如使用地杰斯特拉
	virtual void GetPath(int AddrSource, int AddrDest, TPPath& RetPath)
	{
		if (AddrSource == 1 && AddrDest == 7)
		{
			RetPath.m_Nodes.push_back(m_NodeMap[1]);
			RetPath.m_Nodes.push_back(m_NodeMap[4]);
			RetPath.m_Nodes.push_back(m_NodeMap[10]);
			RetPath.m_Nodes.push_back(m_NodeMap[5]);
			RetPath.m_Nodes.push_back(m_NodeMap[7]);
		}
		else if (AddrSource == 2 && AddrDest == 8)
		{
			RetPath.m_Nodes.push_back(m_NodeMap[2]);
			RetPath.m_Nodes.push_back(m_NodeMap[4]);
			RetPath.m_Nodes.push_back(m_NodeMap[11]);
			RetPath.m_Nodes.push_back(m_NodeMap[6]);
			RetPath.m_Nodes.push_back(m_NodeMap[8]);
		}
		else if (AddrSource == 3 && AddrDest == 9)
		{
			RetPath.m_Nodes.push_back(m_NodeMap[3]);
			RetPath.m_Nodes.push_back(m_NodeMap[6]);
			RetPath.m_Nodes.push_back(m_NodeMap[12]);
			RetPath.m_Nodes.push_back(m_NodeMap[5]);
			RetPath.m_Nodes.push_back(m_NodeMap[9]);
		}
	}

	//使用贪心算法规划
	virtual void GreedyPlan_Test()
	{
		//获取三条路径
		TPPath Path1, Path2, Path3;

		GetPath(1, 7, Path1);
		GetPath(2, 8, Path2);
		GetPath(3, 9, Path3);

		PlanOneUnit_Greedy(Path1);
		PlanOneUnit_Greedy(Path2);
		PlanOneUnit_Greedy(Path3);

		PrintPath(Path1);
		PrintPath(Path2);
		PrintPath(Path3);
	}

	//使用贪心算法规划一个运动单位
	virtual void PlanOneUnit_Greedy(TPPath& Path)
	{
		Path.PlanDepartTime();
	}

	void PrintGragh() const
	{
		std::cout << "The total num of nodes of the gragh is " << m_Nodes.size() << std::endl;
		std::cout << "The total num of edges of the gragh is " << m_Edges.size() << std::endl;
	}
	void PrintPath(const TPPath& Path) const
	{
		std::cout << "depart time is " << Path.m_RetDepartTime << std::endl;
	}




};

