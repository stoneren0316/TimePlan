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
	
	int NodeID;//ΨһID
	TPNODE_TYPE NodeType;//���ͣ����ڱ��tunnel
	int TUCost;//����ʱ��

	//��¼��æʱ�䣬�б����¼���Ǳ�ռ�õĿ�ʼʱ�䣬����TUCost�ɼ������æ����ʱ��
	//�����tunnel����æʱ�䲻���ص�������æʱ����ص�����ʱ����Ϊ�ڴ˷�����ϻ���ײ
	std::list<int> BusyTimes;

	//Ϊ�򵥣���ַ����������ʾ
	int Addr1;
	TPNODE_TYPE Addr1Type;
	int Addr2;
	TPNODE_TYPE Addr2Type;

	//���˵���ʱ���Ƿ������ͻ��Ҫ�ȴ�
	//@return 0:����ȴ�������0����Ҫ�ȴ���ʱ��
	int CheckBusy(int StartTime)
	{
		std::list<int>::iterator it = BusyTimes.begin();
		for (; it != BusyTimes.end(); ++it)
		{
			int TimeDiff = StartTime - (*it);
			if (TimeDiff < TUCost && (-TimeDiff) < TUCost)//abs(TimeDiff) < TUCost, �г�ͻ����
			{
				int AdjustTime = TUCost - TimeDiff;//�����������ʱ�䣬ʹ����ȴ�
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
	//·��������Nodes
	std::vector<TPNode*> m_Nodes;
	//���ÿ��Node����ʱ��ʱ��㣬Ԫ�ظ�����m_Nodes��ͬ
	std::vector<int> m_EndTimes;
	//����ʱ��
	int m_RetDepartTime;

	//�ݹ�����޳�ͻ�޵ȴ������µ�ʱ��	
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
				if (AdjustTime > 0)//�з��ʳ�ͻ����ȴ�����ʱ�޸ĳ���ʱ�䣬���¹滮
				{
					break;
				}
			}
			int NewTime = LastTime + (*it)->TUCost;//��Node����ʱ�䣬��Ϊ��һNode��ʼʱ��
			LastTime = NewTime;
			
			m_EndTimes.push_back(NewTime);
		}

		if (AdjustTime > 0)//�з��ʳ�ͻ����ȴ�����ʱ�޸ĳ���ʱ�䣬���¹滮
		{
			DepartTime += AdjustTime;
			m_EndTimes.clear();
			PlanDepartTime(DepartTime);
		}
		else//����ȴ�������ʱ��ȷ��
		{
			m_RetDepartTime = DepartTime;
			//����tunnel busytime			
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
	//Ϊ�������Node��ʹ�������ַ�ʽ���Nodes
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
		if (Node1.NodeType == TPNODE_TUNNEL)//Node1����Ϊtunnel
		{
			if (Node1.Addr1 == Node2.Addr1 || Node1.Addr1 == Node2.Addr2)//����
			{
				return true;
			}
		}
		else if (Node2.NodeType == TPNODE_TUNNEL)//Node2����Ϊtunnel
		{
			if (Node2.Addr1 == Node1.Addr1 || Node2.Addr1 == Node1.Addr2)//����
			{
				return true;
			}
		}
		//����Ϊ���
		//�����費Ӧ�ó��ֶ�Ϊtunnel���������Ϊ�����Ϊtunnel���Ϳ��Ժϲ�Ϊһ��tunnel��������tunnel֮��Ӧ�û���һ��Node��
		// (�����������tunnel���ڣ���ôֻ�����֮���Node��TU��Ϊ0)
		else 
		{
			//�����ַ��ͬ������,�Ҳ�Ϊtunnel��������
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
	//����ʵ�ʵ���λ�ó�ʼ��Nodes
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

	//����Node�������ԣ�����edge
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

	//����·���Ĳ���ֱ�Ӹ������������˺���Ӧ����gragh����Node�����·������ʹ�õؽ�˹����
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

	//ʹ��̰���㷨�滮
	virtual void GreedyPlan_Test()
	{
		//��ȡ����·��
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

	//ʹ��̰���㷨�滮һ���˶���λ
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

