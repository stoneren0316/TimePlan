// bintong.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TPGragh.h"

int _tmain(int argc, _TCHAR* argv[])
{
	TPGragh Gragh;

	Gragh.InitNodes();
	Gragh.GenerateEdge();
	Gragh.PrintGragh();

	Gragh.GreedyPlan_Test();

	return 0;
}

