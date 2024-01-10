#include "main.h"
#include "BaseGraph.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Yen.h"

#define TOP_K 5

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
	std::cout << "TOP " << TOP_K << " SHORTEST PATHS:\n";
	std::string cfg_fileName = argv[1];
	runDSC(cfg_fileName);

	return 0;
}
