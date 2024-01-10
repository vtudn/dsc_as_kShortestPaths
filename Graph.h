#ifndef __GRAPH_H__
#define __GRAPH_H__

class Path : public BasePath
{
public:
	Path(const std::vector<BaseVertex*> &vertex_list, double weight) : BasePath(vertex_list, weight) {}

	void printOut(std::ostream &out_stream) const
	{
		out_stream << "[PATH INFO] Total distance: " << mWeight << " km" << std::endl;
		out_stream << "[NODES] ";
		for (std::vector<BaseVertex *>::const_iterator it = mvVertexList.begin(); it != mvVertexList.end(); ++it)
		{
			(*it)->printOut(out_stream);
			if ((it + 1) == mvVertexList.end()) {
				out_stream << ".";
			}
			else {
				out_stream << "->";
			}
		}
		out_stream << std::endl << "*********************************************" << std::endl;
	}
};

class Graph
{
public:
	typedef std::set<BaseVertex*>::iterator 								VertexPtSetIterator;
	typedef std::map<BaseVertex*, std::set<BaseVertex*>*>::iterator 		BaseVertexPt2SetMapIterator;
	const static double 													DISCONNECT;

	Graph(const std::string &file_name);
	Graph(const Graph &rGraph);
	~Graph(void);

	BaseVertex*					getVertex(int node_id);
	int 						getEdgeCode(const BaseVertex* start_vertex_pt, const BaseVertex* end_vertex_pt) const;
	std::set<BaseVertex*>*		getVertexSetPt(BaseVertex* vertex_, std::map<BaseVertex*, std::set<BaseVertex*>*> &vertex_container_index);
	double 						getOriginalEdgeWeight(const BaseVertex* source, const BaseVertex* sink);
	double 						getEdgeWeight(const BaseVertex* source, const BaseVertex* sink);
	void 						getAdjacentVertices(BaseVertex* vertex, std::set<BaseVertex*> &vertex_set);
	void 						getPrecedentVertices(BaseVertex* vertex, std::set<BaseVertex*> &vertex_set);
	void 						clear();
	/* Graph modification */
	void 						removeEdge(const std::pair<int, int> edge) 				{ msRemovedEdge.insert(edge); }
	void 						removeVertex(const int vertex_id) 						{ msRemovedVertexIds.insert(vertex_id); }
	void 						recoverRemovedEdges() 									{ msRemovedEdge.clear(); }
	void 						recoverRemovedVertices() 								{ msRemovedVertexIds.clear(); }
	void 						recoverRemovedEdge(const std::pair<int, int> edge) 		{ msRemovedEdge.erase(msRemovedEdge.find(edge)); }
	void 						recoverRemovedVertex(int vertex_id) 					{ msRemovedVertexIds.erase(msRemovedVertexIds.find(vertex_id)); }

protected:
	/* Basic information */
	std::map<BaseVertex*, std::set<BaseVertex*>*> 		mmFanoutVertices;
	std::map<BaseVertex*, std::set<BaseVertex*>*> 		mmFaninVertices;
	std::map<int, double> 								mmEdgeCodeWeight;
	std::vector<BaseVertex*> 							mvVertices;
	int 												mEdgeNum;
	int 												mVertexNum;
	std::map<int, BaseVertex*> 							mmVertexIndex;
	/* Graph modification */
	std::set<int> 										msRemovedVertexIds;
	std::set<std::pair<int, int>> 						msRemovedEdge;

private:
	void importFromFile(const std::string &file_name);
};

#endif // __GRAPH_H__
