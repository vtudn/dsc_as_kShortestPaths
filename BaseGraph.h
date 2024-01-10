#ifndef __BASEGRAPH_H__
#define __BASEGRAPH_H__

#include <iostream>
#include <string>
#include <vector>

template <class T>
class WeightGreater
{
public:
	/* Determine priority */
	bool 	operator()(const T &a, const T &b) const 	{ return a.Weight() > b.Weight(); }
	bool 	operator()(const T *a, const T *b) const 	{ return a->Weight() > b->Weight(); }
};

template <class T>
class WeightLess
{
public:
	/* Determine priority */
	bool 	operator()(const T &a, const T &b) const 	{ return a.Weight() < b.Weight(); }
	bool 	operator()(const T *a, const T *b) const 	{ return a->Weight() < b->Weight(); }
};

template <class T>
class DeleteFunc
{
public:
	void 	operator()(const T *it) const 				{ delete it; }
};

class BaseVertex
{
public:
	int 	getID() const 								{ return mID; }
	void 	setID(int ID_)								{ mID = ID_; }
	double 	Weight() const 								{ return mWeight; }
	void 	Weight(double val) 							{ mWeight = val; }
	void 	printOut(std::ostream &out_stream) 			{ out_stream << mID; }

private:
	int 		mID;
	double 		mWeight;
};

class BasePath
{
public:
	BasePath(const std::vector<BaseVertex*> &vertex_list, double weight) : mWeight(weight)
	{
		mvVertexList.assign(vertex_list.begin(), vertex_list.end());
		mLength = mvVertexList.size();
	}
	~BasePath(void) {}

	double 				Weight() const 				{ return mWeight; }
	void 				Weight(double val) 			{ mWeight = val; }
	int 				length() 					{ return mLength; }
	BaseVertex*			getVertex(int i) 			{ return mvVertexList.at(i); }

	bool subPath(std::vector<BaseVertex*> &sub_path, BaseVertex* ending_vertex_pt)
	{
		for (std::vector<BaseVertex*>::const_iterator it = mvVertexList.begin(); it != mvVertexList.end(); ++it)
		{
			if (*it != ending_vertex_pt)
			{
				sub_path.push_back(*it);
			}
			else
			{
				// break;
				return true;
			}
		}
		return false;
	}

	void printOut(std::ostream &out_stream) const
	{
		out_stream << "[PATH INFO] Distance: " << mWeight << " km" << " Length: " << mvVertexList.size() << std::endl;
		out_stream << "[VERTICES] ";
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

protected:
	int 							mLength;
	double 							mWeight; // currently weight is distance
	std::vector<BaseVertex*> 		mvVertexList;
};

#endif // __BASEGRAPH_H__
