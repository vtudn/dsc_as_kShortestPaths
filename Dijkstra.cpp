#include <set>
#include <map>
#include <vector>
#include "BaseGraph.h"
#include "Graph.h"
#include "Dijkstra.h"

BasePath *Dijkstra::getShortestPath(BaseVertex *source, BaseVertex *sink)
{
	determineShortestPaths(source, sink, true);
	std::vector<BaseVertex *> vertex_list;
	std::map<BaseVertex *, double>::const_iterator pos = mmStartDistanceIndex.find(sink);
	double weight = pos != mmStartDistanceIndex.end() ? pos->second : Graph::DISCONNECT;
	if (weight < Graph::DISCONNECT)
	{
		BaseVertex *cur_vertex_pt = sink;
		do
		{
			vertex_list.insert(vertex_list.begin(), cur_vertex_pt);
			std::map<BaseVertex *, BaseVertex *>::const_iterator pre_pos = mmPredecessorVertex.find(cur_vertex_pt);
			if (pre_pos == mmPredecessorVertex.end())
			{
				break;
			}
			cur_vertex_pt = pre_pos->second;
		} while (cur_vertex_pt != source);
		vertex_list.insert(vertex_list.begin(), source);
	}
	return new BasePath(vertex_list, weight);
}

void Dijkstra::determineShortestPaths(BaseVertex *source, BaseVertex *sink, bool is_source2sink)
{
	// clear the intermediate variables
	clear();

	// initiate the local variables
	BaseVertex *end_vertex = is_source2sink ? sink : source;
	BaseVertex *start_vertex = is_source2sink ? source : sink;
	mmStartDistanceIndex[start_vertex] = 0;
	start_vertex->Weight(0);
	mqCandidateVertices.insert(start_vertex);

	// start searching for the shortest path
	while (!mqCandidateVertices.empty())
	{
		std::multiset<BaseVertex *, WeightLess<BaseVertex>>::const_iterator pos = mqCandidateVertices.begin();
		BaseVertex *cur_vertex_pt = *pos; // mqCandidateVertices.top();
		mqCandidateVertices.erase(pos);
		if (cur_vertex_pt == end_vertex)
		{
			break;
		}
		msDeterminedVertices.insert(cur_vertex_pt->getID());
		improve2Vertex(cur_vertex_pt, is_source2sink);
	}
}

void Dijkstra::improve2Vertex(BaseVertex *cur_vertex_pt, bool is_source2sink)
{
	// get the neighboring vertices
	std::set<BaseVertex *> *neighbor_vertex_list_pt = new std::set<BaseVertex *>();
	if (is_source2sink)
	{
		mpDirectGraph->getAdjacentVertices(cur_vertex_pt, *neighbor_vertex_list_pt);
	}
	else
	{
		mpDirectGraph->getPrecedentVertices(cur_vertex_pt, *neighbor_vertex_list_pt);
	}

	// update the distance passing on the current vertex
	for (std::set<BaseVertex *>::iterator cur_neighbor_pos = neighbor_vertex_list_pt->begin(); cur_neighbor_pos != neighbor_vertex_list_pt->end(); ++cur_neighbor_pos)
	{
		// skip if it has been visited before
		if (msDeterminedVertices.find((*cur_neighbor_pos)->getID()) != msDeterminedVertices.end())
		{
			continue;
		}

		// calculate the distance
		std::map<BaseVertex *, double>::const_iterator cur_pos = mmStartDistanceIndex.find(cur_vertex_pt);
		double distance = cur_pos != mmStartDistanceIndex.end() ? cur_pos->second : Graph::DISCONNECT;
		distance += is_source2sink ? mpDirectGraph->getEdgeWeight(cur_vertex_pt, *cur_neighbor_pos) : mpDirectGraph->getEdgeWeight(*cur_neighbor_pos, cur_vertex_pt);

		// update the distance if necessary
		cur_pos = mmStartDistanceIndex.find(*cur_neighbor_pos);
		if (cur_pos == mmStartDistanceIndex.end() || cur_pos->second > distance)
		{
			mmStartDistanceIndex[*cur_neighbor_pos] = distance;
			mmPredecessorVertex[*cur_neighbor_pos] = cur_vertex_pt;
			(*cur_neighbor_pos)->Weight(distance);
			std::multiset<BaseVertex *, WeightLess<BaseVertex>>::const_iterator pos = mqCandidateVertices.begin();
			for (; pos != mqCandidateVertices.end(); ++pos)
			{
				if ((*pos)->getID() == (*cur_neighbor_pos)->getID())
				{
					break;
				}
			}
			if (pos != mqCandidateVertices.end())
			{
				mqCandidateVertices.erase(pos);
			}
			mqCandidateVertices.insert(*cur_neighbor_pos);
		}
	}
}

void Dijkstra::clear()
{
	msDeterminedVertices.clear();
	mmPredecessorVertex.clear();
	mmStartDistanceIndex.clear();
	mqCandidateVertices.clear();
}

BasePath *Dijkstra::updateCostForward(BaseVertex *vertex)
{
	double cost = Graph::DISCONNECT;

	// get the set of successors of the input vertex
	std::set<BaseVertex *> *adj_vertex_set = new std::set<BaseVertex *>();
	mpDirectGraph->getAdjacentVertices(vertex, *adj_vertex_set);

	// make sure the input vertex exists in the index
	std::map<BaseVertex *, double>::iterator pos4vertexInStartDistIndex = mmStartDistanceIndex.find(vertex);
	if (pos4vertexInStartDistIndex == mmStartDistanceIndex.end())
	{
		pos4vertexInStartDistIndex = (mmStartDistanceIndex.insert(std::make_pair(vertex, Graph::DISCONNECT))).first;
	}

	// update the distance from the root to the input vertex if necessary
	for (std::set<BaseVertex *>::const_iterator pos = adj_vertex_set->begin(); pos != adj_vertex_set->end(); ++pos)
	{
		// get the distance from the root to one successor of the input vertex
		std::map<BaseVertex *, double>::const_iterator cur_vertex_pos = mmStartDistanceIndex.find(*pos);
		double distance = cur_vertex_pos == mmStartDistanceIndex.end() ? Graph::DISCONNECT : cur_vertex_pos->second;

		// calculate the distance from the root to the input vertex
		distance += mpDirectGraph->getEdgeWeight(vertex, *pos);

		// update the distance if necessary
		double cost_of_vertex = pos4vertexInStartDistIndex->second;
		if (cost_of_vertex > distance)
		{
			mmStartDistanceIndex[vertex] = distance;
			mmPredecessorVertex[vertex] = cur_vertex_pos->first;
			cost = distance;
		}
	}

	// create the sub_path if exists
	BasePath *sub_path = NULL;
	if (cost < Graph::DISCONNECT)
	{
		std::vector<BaseVertex *> vertex_list;
		vertex_list.push_back(vertex);
		std::map<BaseVertex *, BaseVertex *>::const_iterator pos4PredVertexMap = mmPredecessorVertex.find(vertex);
		while (pos4PredVertexMap != mmPredecessorVertex.end())
		{
			BaseVertex *pred_vertex_pt = pos4PredVertexMap->second;
			vertex_list.push_back(pred_vertex_pt);
			pos4PredVertexMap = mmPredecessorVertex.find(pred_vertex_pt);
		}
		sub_path = new BasePath(vertex_list, cost);
	}
	return sub_path;
}

void Dijkstra::correctCostBackward(BaseVertex *vertex)
{
	// initialize the list of vertex to be updated
	std::vector<BaseVertex *> vertex_pt_list;
	vertex_pt_list.push_back(vertex);

	// update the cost of relevant precedents of the input vertex
	while (!vertex_pt_list.empty())
	{
		BaseVertex *cur_vertex_pt = *(vertex_pt_list.begin());
		vertex_pt_list.erase(vertex_pt_list.begin());
		double cost_of_cur_vertex = mmStartDistanceIndex[cur_vertex_pt];
		std::set<BaseVertex *> pre_vertex_set;
		mpDirectGraph->getPrecedentVertices(cur_vertex_pt, pre_vertex_set);
		for (std::set<BaseVertex *>::const_iterator pos = pre_vertex_set.begin(); pos != pre_vertex_set.end(); ++pos)
		{
			std::map<BaseVertex *, double>::const_iterator pos4StartDistIndexMap = mmStartDistanceIndex.find(*pos);
			double cost_of_pre_vertex = mmStartDistanceIndex.end() == pos4StartDistIndexMap ? Graph::DISCONNECT : pos4StartDistIndexMap->second;
			double fresh_cost = cost_of_cur_vertex + mpDirectGraph->getEdgeWeight(*pos, cur_vertex_pt);
			if (cost_of_pre_vertex > fresh_cost)
			{
				mmStartDistanceIndex[*pos] = fresh_cost;
				mmPredecessorVertex[*pos] = cur_vertex_pt;
				vertex_pt_list.push_back(*pos);
			}
		}
	}
}
