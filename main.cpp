// bintong.cpp : 定义控制台应用程序的入口点。
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

