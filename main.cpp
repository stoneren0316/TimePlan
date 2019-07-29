// main.cpp : 定义控制台应用程序的入口点。
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

