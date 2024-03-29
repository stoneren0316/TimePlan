### 问题描述如下：
![问题](https://github.com/stoneren0316/TimePlan/blob/master/problem/problem.JPG)

### 初步的思路：
首先这是一张图，有5个node，中间一个代表隧道，时间5个TU，按照题目的要求，另外4个Note，分别表示两个起点终点与隧道的路线对象，有不等的TU。边（edge）代表note之间的连接关系，按照相邻性，可以创建出4条边。Humpty和Dumpty分别由各自起点出发到达终点，求他们的出发时间，我感觉是要求总的TU最小。
人到达目标所经过的路径感觉可以用地杰斯特拉或者A*来求最短
题目里只有一个隧道，算出他们到达隧道的时间差，和隧道花费时间比较，判断是否会需要等待，如果需要等待，则反推后到的人的出发时间。

### 问题补充：

![补充](https://github.com/stoneren0316/TimePlan/blob/master/problem/%E8%A1%A5%E5%85%85.png)

### 由此：
采用三个人，三个起点，三个终点，三个tunnel的数据做测试，示意图如下：
一个人从s1前往d7，
一个人从s2前往d8，
一个人从s3前往d9，
每个人都会经过两个tunnel

![图示](https://github.com/stoneren0316/TimePlan/blob/master/gragh/gragh.jpg)

### 方法和说明：

本题目求每个人的出发时间，因为没有对总的工作时间做约束，所以最优的情况下是没有等待发生，即当有等待发生时，根据等待时间计算需要调整的出发时间，从而使不发生等待。
使用贪心方法，以人为处理对象，先对一个人进行规划，并记录下他到达所有Node的时间，对于tunnel，记录下的就是它被占用的时间。规划下一个人时，根据tunnel的占用情况，可知是否发生等待，若有等待发生，则根据等待时间调整出发时间，并重新计算。直至所有人规划完成，任务结束。
因为是贪心方法，所得结果不一定是最优，可根据具体业务考虑一些方法优化，如处理人的顺序，可考虑无冲突条件下先到达tunnel的人优先处理。

### 编译,运行：

g++ main.cpp

./a.out

### 测试结果：

![图示](https://github.com/stoneren0316/TimePlan/blob/master/gragh/RetPrint.JPG)