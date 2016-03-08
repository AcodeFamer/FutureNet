#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<list>
#include<set>
using namespace std;
/**********************读取csv数据并解析****************
*topo_file：输入参数，topo.csv文件名
demand_file：输入参数，demand.csv文件名
graph_data：输出参数,二维数组，图信息，每一行为每一条边的信息
edgeNum：输出参数，有向图中边的数目
start：输出参数，demand中起点编号
end：输出参数，demand中终点编号
demand_vertex：输出参数，一维数组，demang中必须经过的顶点编号
vertexNum:输出参数 demang中必须经过的顶点数目
**/
void readCsvFile(string topo_file,string demand_file,vector<vector<int>>& graph_data,int& edgeNum,int& start,int& end,vector<int>& demand_vertex,int& vertexNum)
{
	ifstream topoData(topo_file);
	ifstream demandData(demand_file);

	string rowData;
	int rowIndex = 0;
	while (getline(topoData,rowData))
	{
		//cout << rowData << endl;
		int pre_comma_index = -1;
		int colIndex = 0;
		for (size_t i = 0; i < rowData.size(); i++)
		{
			if (rowData[i] == ',')
			{
				int num=atoi(rowData.substr(pre_comma_index + 1, i - pre_comma_index - 1).c_str());
				graph_data[rowIndex][colIndex] = num; 
				pre_comma_index = i;
				colIndex++;
			}
		}
		graph_data[rowIndex][colIndex] = atoi(rowData.substr(pre_comma_index + 1, rowData.size() - pre_comma_index - 1).c_str());
		rowIndex++;
	}
	//edgeNum
	edgeNum = rowIndex;

	getline(demandData, rowData); 
	int pre_comma_index = -1;
	int comma_count = 0;
	for (size_t i = 0; i < rowData.size(); i++)
	{
		if (rowData[i] == ',')
		{
			comma_count++;
			if (comma_count == 1)
			{
				start = atoi(rowData.substr(pre_comma_index + 1, i - pre_comma_index - 1).c_str());
				pre_comma_index = i;
			}
			if (comma_count == 2)
			{
				end = atoi(rowData.substr(pre_comma_index + 1, i - pre_comma_index - 1).c_str());
				pre_comma_index = i;
				break;
			}
		}
	}
	string vertex_set = rowData.substr(pre_comma_index + 1, rowData.size() - pre_comma_index - 1);
	int pre_gap_index = -1;
	int vertexIndex = 0;
	for (size_t i = 0; i < vertex_set.size(); i++)
	{
		if (vertex_set[i] == '|')
		{
			demand_vertex[vertexIndex] = atoi(vertex_set.substr(pre_gap_index + 1, i - pre_gap_index - 1).c_str());
			vertexIndex++;
			pre_gap_index = i;
		}
	}
	demand_vertex[vertexIndex] = atoi(vertex_set.substr(pre_gap_index + 1, rowData.size() - pre_gap_index - 1).c_str());
	vertexNum = vertexIndex + 1;
}

struct edge 
{
	int start;
	int end;
};

class Gragh
{
public:
	int vertexNum;   //顶点数目
	int edgeNum;    //有向边数目
	int vertexId[600];//图中顶点索引
	
	vector<vector<int>> cost;//邻接矩阵，即有向边的权值，如果该边不存在，则赋一个很大的值（60000）

	vector<list<int>> adj;//每个顶点维护一个链表，链表中存储该编号顶点相连的顶点编号
	//默认构造函数
	Gragh(){};
	//构造函数，完成建图
	Gragh(const vector<vector<int>>& graph_data,const int& edgeNum);

	//深度优先搜索出所有经过所有顶点的路径
	void searchDFS();
private:
	//从节点v开始进行深度优先搜索
	void DFS_visit(int v);
	//标志位，访问过的节点为1，未访问过为0
	int flag[600];
	//已经访问过的节点数目
	int DFS_visit_count;
	//每找到一条路径，就加入allpath中
	vector<list<int>> allPath;
	//对应于每一条path的权值和
	vector<int> allPathCost;
	//路径记录
	list<int> path;
public:
	void displayPath();
};

//构造函数，先建大图  再间小图
Gragh::Gragh(const vector<vector<int>>& graph_data, const int& edge_num)
{
	vertexNum = 0;
	edgeNum = edge_num;
	//初始化邻接矩阵的权值都为60000
	vector<int> a(600, 60000);
	cost = vector<vector<int>>(600, a);
	//初始化为每一个顶点设置一个了链表
	list<int> link;
	adj = vector<list<int>>(600, link);

	set<int> vertexId_tmp;   //存储已经出现的顶点编号
	//遍历所有的边
	for (int i = 0; i < edgeNum; i++)
	{
		if (vertexId_tmp.find(graph_data[i][1]) == vertexId_tmp.end())  //该顶点编号不存在于vertexId_tmp中
		{
			vertexId_tmp.insert(graph_data[i][1]);
			vertexId[vertexNum] = graph_data[i][1];
			vertexNum++;
		}
		if (vertexId_tmp.find(graph_data[i][2]) == vertexId_tmp.end())  //该顶点编号不存在于vertexId_tmp中
		{
			vertexId_tmp.insert(graph_data[i][2]);
			vertexId[vertexNum] = graph_data[i][2];
			vertexNum++;
		}
		//权值存入邻接矩阵中
		cost[graph_data[i][1]][graph_data[i][2]] = graph_data[i][3];
		//每个顶点维护一个链表，表示与该顶点相连的顶点编号
		adj[graph_data[i][1]].push_back(graph_data[i][2]);
	}

}


void Gragh::searchDFS()
{
	//每一个顶点作为起点，进行深度优先搜索
	for (int i = 0; i < vertexNum; i++)
	{
		memset(flag, 0, 600*sizeof(int));//初始化标志位为0，节点没有被访问过
		DFS_visit_count = 0;
		path.clear();//清空路径
		//开启DFS算法搜连接所有顶点的路径
		DFS_visit(vertexId[i]);
	}
}

void Gragh::DFS_visit(int v)
{
	//访问该节点
	flag[v] = 1;
	//加入路径
	path.push_back(v);
	//访问节点数加1
	DFS_visit_count++;
	//已经访问图中所有节点
	if (DFS_visit_count == vertexNum)
	{
		//保存路径
		allPath.push_back(path);
		//计算权值和
	
		int sum_cost = 0; 
		list<int>::iterator it = path.begin(),it_before;
		it++;
		for (; it  != path.end(); it++)
		{
			it_before = it;
			it_before--;
			sum_cost += cost[*it_before][*it];
			
		}
		allPathCost.push_back(sum_cost);
	}
	//遍历该节点的邻接链表
	for (list<int>::iterator it = adj[v].begin(); it != adj[v].end(); it++)
	{
		if (flag[*it] == 0)  //该节点未被访问
		{
			DFS_visit(*it);
		}
	}
	//
	flag[v] = 0;
	path.pop_back();
	DFS_visit_count--;
}

void Gragh::displayPath()
{
	for (size_t i = 0; i < allPath.size();i++)
	{
		cout << "路径" << i + 1 << ":";
		for (list<int>::iterator it = allPath[i].begin(); it != allPath[i].end(); it++)
		{
			cout << *it << " ";
		}
		cout <<"权值和："<< allPathCost[i] << endl;
	}
}

int main()
{
	vector<int> row(4, 0);
	vector<vector<int>> graph(600,row);
	vector<int> demand_vertex(50, 0);
	int edgeNum, demandVertexNum;
	int start, end;
	//读取csv文件获得数据
	readCsvFile("topo.csv", "demand.csv", graph, edgeNum, start, end, demand_vertex, demandVertexNum);
	//根据数据构建图
	Gragh g(graph, edgeNum);
	//
	g.searchDFS();
	g.displayPath();
	int a = 1;
	return 0;
}