#include "main.h"
#include "BaseGraph.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Yen.h"

#define TOP_K 5

bool PushRelabel_canPush(std::vector<std::vector<int>> &capacity, int src, int sink, std::vector<int> &parent)
{
	int N = capacity.size();
	std::vector<int> vis(N, 0);
	vis[src] = 1;
	parent[src] = -1;
	std::queue<int> q;
	q.push(src);
	while (!q.empty())
	{
		int cnode = q.front();
		q.pop();
		for (int i = 0; i < N; i++)
		{
			if (vis[i] == 0 && capacity[cnode][i] > 0)
			{
				vis[i] = 1;
				parent[i] = cnode;
				q.push(i);
			}
		}
	}
	if (vis[sink])
		return true;
	return false;
}

int PushRelabel_FIFO(std::vector<std::vector<int>> &capacity, int src, int sink)
{
	int N = capacity.size();
	std::vector<int> parent(N);
	int max_flow = 0;
	while (PushRelabel_canPush(capacity, src, sink, parent))
	{
		int curr_flow = INT_MAX;
		int temp = sink;
		while (temp != src)
		{
			curr_flow = std::min(curr_flow, capacity[parent[temp]][temp]);
			temp = parent[temp];
		}
		temp = sink;
		while (temp != src)
		{
			capacity[parent[temp]][temp] -= curr_flow;
			temp = parent[temp];
		}
		max_flow += curr_flow;
	}
	return max_flow;
}

int PushRelabel_dev(int src, int sink)
{
	std::vector<std::vector<int>> capacity_abs = {
		{0, 16, 13, 0, 0, 0},
		{0, 0, 10, 12, 0, 0},
		{0, 4, 0, 0, 14, 0},
		{0, 0, 9, 0, 0, 20},
		{0, 0, 0, 7, 0, 4},
		{0, 0, 0, 0, 0, 0}};
	return PushRelabel_FIFO(capacity_abs, src, sink);
}

void testDijkstra() // testing only
{
	Graph *my_graph_pt = new Graph("data/danYen");
	Dijkstra shortest_path_alg(my_graph_pt);
	BasePath *result =
		shortest_path_alg.getShortestPath(
			my_graph_pt->getVertex(46), my_graph_pt->getVertex(13));
	result->printOut(std::cout);
}

void runDSC(const std::string &cfg_filename)
{
	// create graph
	Graph my_graph("data/graph_AnSuong_SGZoo.cfg");

	// get input
	int begin_point;
	int end_point;
	std::ifstream input_file(cfg_filename);
	if (!input_file.is_open())
	{
		std::cerr << "Error opening configuration file: " << cfg_filename << std::endl;
		return;
	}
	std::string line;
	if (!std::getline(input_file, line))
	{
		std::cerr << "Configuration file is empty." << std::endl;
		return;
	}
	std::istringstream iss(line);
	int tempX, tempY;
	if (!(iss >> tempX >> tempY))
	{
		std::cerr << "Invalid format in configuration file." << std::endl;
		return;
	}
	if (tempX < 0 || tempX > 50 || tempY < 1 || tempY > 51 || tempY <= tempX)
	{
		std::cerr << "Values of x and y are out of range or invalid." << std::endl;
		return;
	}
	begin_point = tempX;
	end_point = tempY;

	// run Yen's algorithm for top k shortest paths
	Yen yenAlg(my_graph, my_graph.getVertex(begin_point), my_graph.getVertex(end_point));
	int i = 0;
	while (i < TOP_K)
	{
		++i;
		yenAlg.next()->printOut(std::cout);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "The input arguments are wrong. Please try again.\n";
		return 1;
	}
	std::cout << "MAX FLOW: " << PushRelabel_dev(0, TOP_K) << '\n'; // in development state
	std::cout << "TOP " << TOP_K << " SHORTEST PATHS:\n";
	std::string cfg_fileName = argv[1];
	runDSC(cfg_fileName);

	return 0;
}
