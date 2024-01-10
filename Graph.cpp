#include <limits>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "BaseGraph.h"
#include "Graph.h"

const double Graph::DISCONNECT = (std::numeric_limits<double>::max)();

Graph::Graph(const std::string &file_name)
{
	importFromFile(file_name);
}

Graph::Graph(const Graph &graph)
{
	mVertexNum = graph.mVertexNum;
	mEdgeNum = graph.mEdgeNum;
	mvVertices.assign(graph.mvVertices.begin(), graph.mvVertices.end());
	mmFaninVertices.insert(graph.mmFaninVertices.begin(), graph.mmFaninVertices.end());
	mmFanoutVertices.insert(graph.mmFanoutVertices.begin(), graph.mmFanoutVertices.end());
	mmEdgeCodeWeight.insert(graph.mmEdgeCodeWeight.begin(), graph.mmEdgeCodeWeight.end());
	mmVertexIndex.insert(graph.mmVertexIndex.begin(), graph.mmVertexIndex.end());
}

Graph::~Graph(void)
{
	clear();
}

/* Construct the graph by importing the edges from the input file.
The format of the file is as follows:
	1. The first line has an integer as the number of vertices of the graph
	2. Each line afterwards contains a directed edge in the graph: starting point, ending point and the weight of the edge.
	These values are separated by 'white space'. */
void Graph::importFromFile(const std::string &input_file_name)
{
	const char *file_name = input_file_name.c_str();

	// check the validity of the file
	std::ifstream ifs(file_name);
	if (!ifs)
	{
		std::cerr << "The file " << file_name << " can not be opened!" << std::endl;
		exit(1);
	}

	// reset the members of the class
	clear();

	// start to read information from the input file.
	/// note the format of the data in the graph file.
	// the first line has an integer as the number of vertices of the graph
	ifs >> mVertexNum;

	// in the following lines, each line contains a directed edge in the graph:
	/// the id of starting point, the id of ending point, the weight of the edge.
	/// these values are separated by 'white space'.
	int start_vertex, end_vertex;
	double edge_weight;
	int vertex_id = 0;

	while (ifs >> start_vertex)
	{
		if (start_vertex == -1)
		{
			break;
		}
		ifs >> end_vertex;
		ifs >> edge_weight;

		/// construct the vertices
		BaseVertex *start_vertex_pt = getVertex(start_vertex);
		BaseVertex *end_vertex_pt = getVertex(end_vertex);

		/// add the edge weight
		//// note that the duplicate edge would overwrite the one occurring before.
		mmEdgeCodeWeight[getEdgeCode(start_vertex_pt, end_vertex_pt)] = edge_weight;

		/// update the fan-in or fan-out variables
		//// fan-in
		getVertexSetPt(end_vertex_pt, mmFaninVertices)->insert(start_vertex_pt);

		//// fan-out
		getVertexSetPt(start_vertex_pt, mmFanoutVertices)->insert(end_vertex_pt);
	}

	if (mVertexNum != mvVertices.size())
	{
		std::cerr << "The number of nodes in the graph is " << mvVertices.size() << " instead of " << mVertexNum << std::endl;
		exit(1);
	}
	mVertexNum = mvVertices.size();
	mEdgeNum = mmEdgeCodeWeight.size();
	ifs.close();
}

BaseVertex *Graph::getVertex(int node_id)
{
	if (msRemovedVertexIds.find(node_id) != msRemovedVertexIds.end())
	{
		return NULL;
	}
	else
	{
		BaseVertex *vertex_pt = NULL;
		const std::map<int, BaseVertex *>::iterator pos = mmVertexIndex.find(node_id);
		if (pos == mmVertexIndex.end())
		{
			int vertex_id = mvVertices.size();
			vertex_pt = new BaseVertex();
			vertex_pt->setID(node_id);
			mmVertexIndex[node_id] = vertex_pt;
			mvVertices.push_back(vertex_pt);
		}
		else
		{
			vertex_pt = pos->second;
		}
		return vertex_pt;
	}
}

void Graph::clear()
{
	mEdgeNum = 0;
	mVertexNum = 0;

	for (std::map<BaseVertex *, std::set<BaseVertex *> *>::const_iterator pos = mmFaninVertices.begin(); pos != mmFaninVertices.end(); ++pos)
	{
		delete pos->second;
	}
	mmFaninVertices.clear();

	for (std::map<BaseVertex *, std::set<BaseVertex *> *>::const_iterator pos = mmFanoutVertices.begin(); pos != mmFanoutVertices.end(); ++pos)
	{
		delete pos->second;
	}
	mmFanoutVertices.clear();

	mmEdgeCodeWeight.clear();

	// clear the list of vertices objects
	for_each(mvVertices.begin(), mvVertices.end(), DeleteFunc<BaseVertex>());
	mvVertices.clear();
	mmVertexIndex.clear();

	msRemovedVertexIds.clear();
	msRemovedEdge.clear();
}

int Graph::getEdgeCode(const BaseVertex *start_vertex_pt, const BaseVertex *end_vertex_pt) const
{
	// the computation below works only if the result is smaller than the maximum of an integer
	return start_vertex_pt->getID() * mVertexNum + end_vertex_pt->getID();
}

std::set<BaseVertex *> *Graph::getVertexSetPt(BaseVertex *vertex_, std::map<BaseVertex *, std::set<BaseVertex *> *> &vertex_container_index)
{
	BaseVertexPt2SetMapIterator pos = vertex_container_index.find(vertex_);
	if (pos == vertex_container_index.end())
	{
		std::set<BaseVertex *> *vertex_set = new std::set<BaseVertex *>();
		std::pair<BaseVertexPt2SetMapIterator, bool> ins_pos = vertex_container_index.insert(std::make_pair(vertex_, vertex_set));
		pos = ins_pos.first;
	}
	return pos->second;
}

double Graph::getEdgeWeight(const BaseVertex *source, const BaseVertex *sink)
{
	int source_id = source->getID();
	int sink_id = sink->getID();
	if (msRemovedVertexIds.find(source_id) != msRemovedVertexIds.end() || msRemovedVertexIds.find(sink_id) != msRemovedVertexIds.end() || msRemovedEdge.find(std::make_pair(source_id, sink_id)) != msRemovedEdge.end())
	{
		return DISCONNECT;
	}
	else
	{
		return getOriginalEdgeWeight(source, sink);
	}
}

void Graph::getAdjacentVertices(BaseVertex *vertex, std::set<BaseVertex *> &vertex_set)
{
	int starting_vt_id = vertex->getID();
	if (msRemovedVertexIds.find(starting_vt_id) == msRemovedVertexIds.end())
	{
		std::set<BaseVertex *> *vertex_pt_set = getVertexSetPt(vertex, mmFanoutVertices);
		for (std::set<BaseVertex *>::const_iterator pos = (*vertex_pt_set).begin(); pos != (*vertex_pt_set).end(); ++pos)
		{
			int ending_vt_id = (*pos)->getID();
			if (msRemovedVertexIds.find(ending_vt_id) != msRemovedVertexIds.end() || msRemovedEdge.find(std::make_pair(starting_vt_id, ending_vt_id)) != msRemovedEdge.end())
			{
				continue;
			}
			vertex_set.insert(*pos);
		}
	}
}

void Graph::getPrecedentVertices(BaseVertex *vertex, std::set<BaseVertex *> &vertex_set)
{
	if (msRemovedVertexIds.find(vertex->getID()) == msRemovedVertexIds.end())
	{
		int ending_vt_id = vertex->getID();
		std::set<BaseVertex *> *pre_vertex_set = getVertexSetPt(vertex, mmFaninVertices);
		for (std::set<BaseVertex *>::const_iterator pos = (*pre_vertex_set).begin(); pos != (*pre_vertex_set).end(); ++pos)
		{
			int starting_vt_id = (*pos)->getID();
			if (msRemovedVertexIds.find(starting_vt_id) != msRemovedVertexIds.end() || msRemovedEdge.find(std::make_pair(starting_vt_id, ending_vt_id)) != msRemovedEdge.end())
			{
				continue;
			}
			vertex_set.insert(*pos);
		}
	}
}

double Graph::getOriginalEdgeWeight(const BaseVertex *source, const BaseVertex *sink)
{
	std::map<int, double>::const_iterator pos = mmEdgeCodeWeight.find(getEdgeCode(source, sink));
	if (pos != mmEdgeCodeWeight.end())
	{
		return pos->second;
	}
	else
	{
		return DISCONNECT;
	}
}
