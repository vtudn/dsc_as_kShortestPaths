#include <set>
#include <map>
#include <queue>
#include <vector>
#include "BaseGraph.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Yen.h"

void Yen::clear()
{
	mGeneratedPathNum = 0;
	mmDerivationVertexIndex.clear();
	mvResultList.clear();
	mqPathCandidates.clear();
}

void Yen::initialize()
{
	clear();
	if (mpSourceVertex != NULL && mpTargetVertex != NULL)
	{
		BasePath *pShortestPath = getShortestPath(mpSourceVertex, mpTargetVertex);
		if (pShortestPath != NULL && pShortestPath->length() > 1)
		{
			mqPathCandidates.insert(pShortestPath);
			mmDerivationVertexIndex[pShortestPath] = mpSourceVertex;
		}
	}
}

BasePath *Yen::getShortestPath(BaseVertex *pSource, BaseVertex *pTarget)
{
	Dijkstra dijkstra_alg(mpGraph);
	return dijkstra_alg.getShortestPath(pSource, pTarget);
}

bool Yen::hasNext()
{
	return !mqPathCandidates.empty();
}

BasePath *Yen::next()
{
	// prepare for removing vertices and arcs
	BasePath *cur_path = *(mqPathCandidates.begin()); // mqPathCandidates.top();

	// mqPathCandidates.pop();
	mqPathCandidates.erase(mqPathCandidates.begin());
	mvResultList.push_back(cur_path);
	int count = mvResultList.size();
	BaseVertex *cur_derivation_pt = mmDerivationVertexIndex.find(cur_path)->second;
	std::vector<BaseVertex *> sub_path_of_derivation_pt;
	cur_path->subPath(sub_path_of_derivation_pt, cur_derivation_pt);
	int sub_path_length = sub_path_of_derivation_pt.size();

	// remove the vertices and arcs in the graph
	for (int i = 0; i < count - 1; ++i)
	{
		BasePath *cur_result_path = mvResultList.at(i);
		std::vector<BaseVertex *> cur_result_sub_path_of_derivation_pt;
		if (!cur_result_path->subPath(cur_result_sub_path_of_derivation_pt, cur_derivation_pt))
		{
			continue;
		}
		if (sub_path_length != cur_result_sub_path_of_derivation_pt.size())
		{
			continue;
		}
		bool is_equal = true;
		for (int i = 0; i < sub_path_length; ++i)
		{
			if (sub_path_of_derivation_pt.at(i) != cur_result_sub_path_of_derivation_pt.at(i))
			{
				is_equal = false;
				break;
			}
		}
		if (!is_equal)
		{
			continue;
		}
		BaseVertex *cur_succ_vertex = cur_result_path->getVertex(sub_path_length + 1);
		mpGraph->removeEdge(std::make_pair(cur_derivation_pt->getID(), cur_succ_vertex->getID()));
	}

	// remove vertices and edges along the current result
	int path_length = cur_path->length();
	for (int i = 0; i < path_length - 1; ++i)
	{
		mpGraph->removeVertex(cur_path->getVertex(i)->getID());
		mpGraph->removeEdge(std::make_pair(cur_path->getVertex(i)->getID(), cur_path->getVertex(i + 1)->getID()));
	}

	// calculate the shortest tree rooted at target vertex in the graph
	Dijkstra reverse_tree(mpGraph);
	reverse_tree.getShortestPathFlower(mpTargetVertex);

	// recover the deleted vertices and update the cost and identify the new candidates results
	bool is_done = false;
	for (int i = path_length - 2; i >= 0 && !is_done; --i)
	{
		// get the vertex to be recovered
		BaseVertex *cur_recover_vertex = cur_path->getVertex(i);
		mpGraph->recoverRemovedVertex(cur_recover_vertex->getID());

		// check if we should stop continuing in the next iteration
		if (cur_recover_vertex->getID() == cur_derivation_pt->getID())
		{
			is_done = true;
		}

		// calculate cost using forward star form
		BasePath *sub_path = reverse_tree.updateCostForward(cur_recover_vertex);

		// get one candidate result if possible
		if (sub_path != NULL)
		{
			++mGeneratedPathNum;

			// get the prefix from the concerned path
			double cost = 0;
			reverse_tree.correctCostBackward(cur_recover_vertex);
			std::vector<BaseVertex *> pre_path_list;
			for (int j = 0; j < path_length; ++j)
			{
				BaseVertex *cur_vertex = cur_path->getVertex(j);
				if (cur_vertex->getID() == cur_recover_vertex->getID())
				{
					// j = path_length;
					break;
				}
				else
				{
					cost += mpGraph->getOriginalEdgeWeight(cur_path->getVertex(j), cur_path->getVertex(1 + j));
					pre_path_list.push_back(cur_vertex);
				}
			}
			for (int j = 0; j < sub_path->length(); ++j)
			{
				pre_path_list.push_back(sub_path->getVertex(j));
			}

			// compose a candidate
			sub_path = new Path(pre_path_list, cost + sub_path->Weight());

			// put it in the candidate pool if new
			if (mmDerivationVertexIndex.find(sub_path) == mmDerivationVertexIndex.end())
			{
				mqPathCandidates.insert(sub_path);
				mmDerivationVertexIndex[sub_path] = cur_recover_vertex;
			}
		}

		// restore the edge
		BaseVertex *succ_vertex = cur_path->getVertex(i + 1);
		mpGraph->recoverRemovedEdge(std::make_pair(cur_recover_vertex->getID(), succ_vertex->getID()));

		// update cost if necessary
		double cost_1 = mpGraph->getEdgeWeight(cur_recover_vertex, succ_vertex) + reverse_tree.getStartDistanceAt(succ_vertex);
		if (reverse_tree.getStartDistanceAt(cur_recover_vertex) > cost_1)
		{
			reverse_tree.setStartDistanceAt(cur_recover_vertex, cost_1);
			reverse_tree.setPredecessorVertex(cur_recover_vertex, succ_vertex);
			reverse_tree.correctCostBackward(cur_recover_vertex);
		}
	}

	// restore everything
	mpGraph->recoverRemovedEdges();
	mpGraph->recoverRemovedVertices();
	return cur_path;
}

void Yen::getShortestPaths(BaseVertex *pSource, BaseVertex *pTarget, int top_k, std::vector<BasePath *> &result_list)
{
	mpSourceVertex = pSource;
	mpTargetVertex = pTarget;
	initialize();
	int count = 0;
	while (hasNext() && count < top_k)
	{
		next();
		++count;
	}
	result_list.assign(mvResultList.begin(), mvResultList.end());
}
