#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<list>
#include<set>
using namespace std;
/**********************��ȡcsv���ݲ�����****************
*topo_file�����������topo.csv�ļ���
demand_file�����������demand.csv�ļ���
graph_data���������,��ά���飬ͼ��Ϣ��ÿһ��Ϊÿһ���ߵ���Ϣ
edgeNum���������������ͼ�бߵ���Ŀ
start�����������demand�������
end�����������demand���յ���
demand_vertex�����������һά���飬demang�б��뾭���Ķ�����
vertexNum:������� demang�б��뾭���Ķ�����Ŀ
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
	int vertexNum;   //������Ŀ
	int edgeNum;    //�������Ŀ
	int vertexId[600];//ͼ�ж�������
	
	vector<vector<int>> cost;//�ڽӾ��󣬼�����ߵ�Ȩֵ������ñ߲����ڣ���һ���ܴ��ֵ��60000��

	vector<list<int>> adj;//ÿ������ά��һ�����������д洢�ñ�Ŷ��������Ķ�����
	//Ĭ�Ϲ��캯��
	Gragh(){};
	//���캯������ɽ�ͼ
	Gragh(const vector<vector<int>>& graph_data,const int& edgeNum);

	//����������������о������ж����·��
	void searchDFS();
private:
	//�ӽڵ�v��ʼ���������������
	void DFS_visit(int v);
	//��־λ�����ʹ��Ľڵ�Ϊ1��δ���ʹ�Ϊ0
	int flag[600];
	//�Ѿ����ʹ��Ľڵ���Ŀ
	int DFS_visit_count;
	//ÿ�ҵ�һ��·�����ͼ���allpath��
	vector<list<int>> allPath;
	//��Ӧ��ÿһ��path��Ȩֵ��
	vector<int> allPathCost;
	//·����¼
	list<int> path;
public:
	void displayPath();
};

//���캯�����Ƚ���ͼ  �ټ�Сͼ
Gragh::Gragh(const vector<vector<int>>& graph_data, const int& edge_num)
{
	vertexNum = 0;
	edgeNum = edge_num;
	//��ʼ���ڽӾ����Ȩֵ��Ϊ60000
	vector<int> a(600, 60000);
	cost = vector<vector<int>>(600, a);
	//��ʼ��Ϊÿһ����������һ��������
	list<int> link;
	adj = vector<list<int>>(600, link);

	set<int> vertexId_tmp;   //�洢�Ѿ����ֵĶ�����
	//�������еı�
	for (int i = 0; i < edgeNum; i++)
	{
		if (vertexId_tmp.find(graph_data[i][1]) == vertexId_tmp.end())  //�ö����Ų�������vertexId_tmp��
		{
			vertexId_tmp.insert(graph_data[i][1]);
			vertexId[vertexNum] = graph_data[i][1];
			vertexNum++;
		}
		if (vertexId_tmp.find(graph_data[i][2]) == vertexId_tmp.end())  //�ö����Ų�������vertexId_tmp��
		{
			vertexId_tmp.insert(graph_data[i][2]);
			vertexId[vertexNum] = graph_data[i][2];
			vertexNum++;
		}
		//Ȩֵ�����ڽӾ�����
		cost[graph_data[i][1]][graph_data[i][2]] = graph_data[i][3];
		//ÿ������ά��һ��������ʾ��ö��������Ķ�����
		adj[graph_data[i][1]].push_back(graph_data[i][2]);
	}

}


void Gragh::searchDFS()
{
	//ÿһ��������Ϊ��㣬���������������
	for (int i = 0; i < vertexNum; i++)
	{
		memset(flag, 0, 600*sizeof(int));//��ʼ����־λΪ0���ڵ�û�б����ʹ�
		DFS_visit_count = 0;
		path.clear();//���·��
		//����DFS�㷨���������ж����·��
		DFS_visit(vertexId[i]);
	}
}

void Gragh::DFS_visit(int v)
{
	//���ʸýڵ�
	flag[v] = 1;
	//����·��
	path.push_back(v);
	//���ʽڵ�����1
	DFS_visit_count++;
	//�Ѿ�����ͼ�����нڵ�
	if (DFS_visit_count == vertexNum)
	{
		//����·��
		allPath.push_back(path);
		//����Ȩֵ��
	
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
	//�����ýڵ���ڽ�����
	for (list<int>::iterator it = adj[v].begin(); it != adj[v].end(); it++)
	{
		if (flag[*it] == 0)  //�ýڵ�δ������
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
		cout << "·��" << i + 1 << ":";
		for (list<int>::iterator it = allPath[i].begin(); it != allPath[i].end(); it++)
		{
			cout << *it << " ";
		}
		cout <<"Ȩֵ�ͣ�"<< allPathCost[i] << endl;
	}
}

int main()
{
	vector<int> row(4, 0);
	vector<vector<int>> graph(600,row);
	vector<int> demand_vertex(50, 0);
	int edgeNum, demandVertexNum;
	int start, end;
	//��ȡcsv�ļ��������
	readCsvFile("topo.csv", "demand.csv", graph, edgeNum, start, end, demand_vertex, demandVertexNum);
	//�������ݹ���ͼ
	Gragh g(graph, edgeNum);
	//
	g.searchDFS();
	g.displayPath();
	int a = 1;
	return 0;
}