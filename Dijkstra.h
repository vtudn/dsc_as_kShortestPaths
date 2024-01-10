#ifndef __DIJKSTRA_H__
#define __DIJKSTRA_H__

/* Dijkstra algorithm to get the shortest path for a pair of vertices in a graph. */
class Dijkstra
{
public:
	Dijkstra(Graph *pGraph) : mpDirectGraph(pGraph) {}
	~Dijkstra(void) { clear(); }

	BasePath*	getShortestPath(BaseVertex* source, BaseVertex* sink);
	void 		setPredecessorVertex(BaseVertex* vt1, BaseVertex* vt2) 	{ mmPredecessorVertex[vt1] = vt2; }
	double 		getStartDistanceAt(BaseVertex* vertex) 					{ return mmStartDistanceIndex.find(vertex)->second; }
	void 		setStartDistanceAt(BaseVertex* vertex, double weight) 	{ mmStartDistanceIndex[vertex] = weight; }
	void 		getShortestPathFlower(BaseVertex* root) 				{ determineShortestPaths(NULL, root, false); }
	void 		clear();
	/* For the top-k shortest paths algorithm */ 
	BasePath*	updateCostForward(BaseVertex* vertex);
	void 		correctCostBackward(BaseVertex* vertex);

protected:
	void 		determineShortestPaths(BaseVertex* source, BaseVertex* sink, bool is_source2sink);
	void 		improve2Vertex(BaseVertex* cur_vertex_pt, bool is_source2sink);

private:
	Graph* 												mpDirectGraph;
	std::map<BaseVertex*, double> 						mmStartDistanceIndex;
	std::map<BaseVertex*, BaseVertex*> 					mmPredecessorVertex;
	std::set<int> 										msDeterminedVertices;
	std::multiset<BaseVertex*, WeightLess<BaseVertex>> 	mqCandidateVertices;
};

#endif // __DIJKSTRA_H__
