// main.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "TPGragh.h"

int main(int argc, char* argv[])
{
	TPGragh Gragh;

	Gragh.InitNodes();
	Gragh.GenerateEdge();
	Gragh.PrintGragh();

	Gragh.GreedyPlan_Test();

	return 0;
}

