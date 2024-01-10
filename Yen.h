#ifndef __YEN_H__
#define __YEN_H__

/* Yen's algorithm to get the top k shortest paths connecting a pair of vertices in a graph. */
class Yen
{
public:
	Yen(const Graph &graph)
	{
		Yen(graph, NULL, NULL);
	}
	Yen(const Graph &graph, BaseVertex* pSource, BaseVertex* pTarget) : mpSourceVertex(pSource), mpTargetVertex(pTarget)
	{
		mpGraph = new Graph(graph);
		initialize();
	}
	~Yen(void) { clear(); }

	bool 		hasNext();
	BasePath*	next();
	BasePath*	getShortestPath(BaseVertex* pSource, BaseVertex* pTarget);
	void 		getShortestPaths(BaseVertex* pSource, BaseVertex* pTarget, int top_k, std::vector<BasePath*>&);
	void 		clear();

private:
	Graph*											mpGraph;
	std::vector<BasePath*> 							mvResultList;
	std::map<BasePath*, BaseVertex*> 				mmDerivationVertexIndex;
	std::multiset<BasePath*, WeightLess<BasePath>> 	mqPathCandidates;
	BaseVertex*										mpSourceVertex;
	BaseVertex*										mpTargetVertex;
	int 											mGeneratedPathNum;

	void initialize();
};

#endif // __YEN_H__
