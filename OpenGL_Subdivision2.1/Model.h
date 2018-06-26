#pragma once
#include <map>
#include <vector>
#include <algorithm>
//#include "vec3.h"
#include "Data.h"
using namespace glm;
using namespace std;
ofstream lightData("lightData.txt");

//计算法线辅助类
struct PackedVertex {
	glm::vec3 position;
	PackedVertex(vec3 &v) { position=v; }
	bool operator<(const PackedVertex &that) const {
		return memcmp((void*)this, (void*)&that, sizeof(glm::vec3))>0;
	};
};

bool getSimilarVertexIndex_fast(
	PackedVertex & packed,
	std::map<PackedVertex, pair<vec3, unsigned short>> & VertexToOutIndex,
	unsigned short & result
) {
	std::map<PackedVertex, pair<vec3, unsigned short>>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second.second;
		return true;
	}
}

//半边
class HalfEdge
{
public:
	int tail;//半边尾部顶点的索引
	int newVertex;//新增点的索引
	int faceIndex;//半边所在面的顶点索引
	int twin;//成对的另一条半边
	int next;//下一条半边

	HalfEdge()
	{
		next = twin = tail = newVertex = faceIndex = -1;
	}
	~HalfEdge() {};
};

//顶点
class Vertex_Half
{
public:
	vec3 pos;//顶点坐标
	int hEdge;//指向该顶点的半边,有很多，任意一条即可
	int vDegree;//顶点的度
	Vertex_Half()
	{
		hEdge = -1;
		vDegree = 0;
	}
	Vertex_Half(vec3 position)
	{
		pos = position;
		hEdge = -1;
		vDegree = 0;
	}
	~Vertex_Half() {};
};

//面
class Face_Half
{
public:
	vector<int> loop;//该面上的所有半边索引，只保存一条亦可，为了方便，保存所有的
	int newVertex;//新面点的索引
	int faceDegree;//面的边数
	vec3 normal;
	Face_Half()
	{
		newVertex = -1;
		faceDegree = 0;
	}
	~Face_Half() {}

};

//该结构用于存放两个int值，并重载了一些运算方便比较大小，实例中用于作为map的关键字，在查询半边的twin时用到
class TwoPointInd
{
public:
	int first;
	int second;

	TwoPointInd() : first(0), second(0) {}
	TwoPointInd(int f, int s) : first(f), second(s)
	{
		if (first > second)
			swap(first, second);
	}

	TwoPointInd & operator = (const TwoPointInd _next)
	{
		this->first = _next.first;
		this->second = _next.second;
		return *this;
	}
	bool operator < (const TwoPointInd &_next) const
	{
		if (first != _next.first)
			return first < _next.first;
		else
			return second < _next.second;
	}

	bool operator > (const TwoPointInd &_next) const
	{
		if (first != _next.first)
			return first > _next.first;
		else
			return second > _next.second;

	}

	bool operator == (const TwoPointInd &_next) const
	{
		return (first == _next.first && second == _next.second);
	}

	bool operator != (const TwoPointInd &_next) const
	{
		return !(*(this) == _next);
	}
};

class Model
{
private:
	vector<Face_Half> faceLib;
	vector<Vertex_Half> vLib;
	vector<HalfEdge> edgeLib;
	bool isDone;//该层model是否被细分/initModel过，还是只是一个空壳

	//计算新面点，边点，顶点
	void calCCNewVertices();
	//更新邻接关系
	void calCCAdjRelation();
	//生成用于绘制顶点的数组
	void genArrayDrawVertex(vector<GLdouble> &v, int start_face_index, int end_face_index);
	//生成用于绘制框架的数组
	void genArrayDrawFrame(vector<GLdouble> &v, int start_face_index, int end_face_index);
	//生成用于绘制三角形表面的数组
	void genArrayDrawTriFace(vector<GLdouble> &v, int start_face_index, int end_face_index,vector<unsigned short> & out_indices);
	void regenArrayDrawTriFace(vector<GLdouble> &v, int start_face_index, int end_face_index);
	//生成用于绘制多边形的数组
	void genArrayDrawPolygon(vector<GLdouble> &v, int start_face_index, int end_face_index, vector<unsigned int> &degree);
	//计算法向
	void calNormol(int start_face_index, int end_face_index);

public:
	Model();
	Model(Data data);
	~Model();
	int getFaceSize();
	int getVertexSize();
	int getHalfEdgeSize();

	//初始化vlib和faceLib中的数据，并建立邻接关系，初始model建成，为第一次细分做好准备
	void initModel(Data data);
	//细分
	void doCCSubdivision();//细分产生的新model把旧model覆盖
	//前levelSub次细分的结果存在vector<Model> &model中，且细分前进行判断是否细分过，可避免重复计算
	void doCCSubdivision(int levelSub, vector<Model> &model);
	//生成用于不同绘制方式的数组
	void genDrawableArray(SUB_RESULT_USAGE USAGE, vector<GLdouble> &resultArray,vector<unsigned int> &degree, vector<unsigned short> & out_indices);
	//生成obj文件
	void genObjFile(char *path);
	//计算法向量
	vec3 computeNormal(vec3 const & a, vec3 const & b, vec3 const & c);
	

};

Model::Model()
{
	isDone = false;
}

inline Model::Model(Data data)
{
	initModel(data);
}

Model::~Model() {};

inline int Model::getFaceSize()
{
	return faceLib.size();
}

inline int Model::getVertexSize()
{
	return vLib.size();
}

inline int Model::getHalfEdgeSize()
{
	return edgeLib.size();
}

//初始化vlib和faceLib中的数据，并建立邻接关系，初始model建成，为第一次细分做好准备
//当相关属性被初始化后，邻接关系即建立起来，
//VertexHalf中存的有顶点位置的真实信息,第一次从vertices中读入，
//读入后该数组不再被使用，后面直接存在vector<Vertex_Half> vLib中
//HalfEdge中存的有顶点索引值的真实信息，Face_Half中faceDegree个HalfEdge的索引，
//第一次通过遍历vector<Face_Half> faceLib从faces中读入数据构造邻接关系，读入后faces废弃，
//以后通过遍历vector<Face_Half> faceLib建立连接关系和查询

//初始化vlib和faceLib中的数据
inline void Model::initModel(Data data)
{
	isDone = true;
	//向vlib写入位置信息
	int vSize = data.veticesSize();
	int cur_edge_index = 0;
	for (int i = 0; i < vSize; i++)
	{
		vLib.push_back(data.vertices[i].pos);//pos
	}
#pragma region 输出顶点
	//for (int i = 0; i < vSize; i++)
	//{
	//	cout << vLib[i].pos << endl;
	//}
#pragma endregion
	map<TwoPointInd, int> mapHalfEdge;
	pair<map<TwoPointInd, int>::iterator, bool> check_repeat_iter;

	//遍历面，初始化邻接信息
	int faceSize = data.facesSize();
	//为vector<Face_Half> faceLib申请空间
	faceLib.resize(faceSize);
	//初始化每个面的faceDegree，计算总共的HalfEdge数量，别忘了为vector<int> loop申请空间*********
	int numHalfEdge = 0;
	for (int i = 0; i < faceSize; i++)
	{
		faceLib[i].faceDegree = data.faces[i].vIndex.size();//Face_Half::faceDegree
		faceLib[i].loop.resize(faceLib[i].faceDegree);
		numHalfEdge += faceLib[i].faceDegree;
	}
	edgeLib.resize(numHalfEdge);

	for (int i = 0; i < faceSize; i++)
	{
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			faceLib[i].loop[j] = cur_edge_index++;
		}

		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			edgeLib[faceLib[i].loop[j]].tail = data.faces[i].vIndex[j];//HalfEdge::tail
			edgeLib[faceLib[i].loop[j]].faceIndex = i;//HalfEdge::faceIndex
			edgeLib[faceLib[i].loop[j]].next = faceLib[i].loop[(j + 1) % faceLib[i].faceDegree];
			if (vLib[data.faces[i].vIndex[j]].hEdge == -1)//保存任一条即可
				vLib[data.faces[i].vIndex[j]].hEdge = (faceLib[i].loop[(j + faceLib[i].faceDegree - 1) % faceLib[i].faceDegree]);//hEdge
			vLib[data.faces[i].vIndex[j]].vDegree++;
			//find twin
			TwoPointInd tp(data.faces[i].vIndex[j], data.faces[i].vIndex[(j + 1) % faceLib[i].faceDegree]);
			check_repeat_iter = mapHalfEdge.insert(map<TwoPointInd, int>::value_type(tp, faceLib[i].loop[j]));
			//插入失败表示已有，即找到
			if (check_repeat_iter.second == false)
			{
				edgeLib[faceLib[i].loop[j]].twin = mapHalfEdge[tp];//twin
				edgeLib[mapHalfEdge[tp]].twin = faceLib[i].loop[j];//twin
				mapHalfEdge.erase(check_repeat_iter.first);
			}
		}
	}
	mapHalfEdge.clear();

#pragma region 输出面
	//cout << "初始化完成，开始输出面：" << endl;
	//for (int i = 0; i < oldFaceSize; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//		cout << vLib[faceLib[i].loop[j].tail].pos << ' ';
	//	cout << endl;
	//}
	//cout << "输出面完毕" << endl;
#pragma endregion

}

inline void Model::doCCSubdivision()
{
	//计算新顶点
	calCCNewVertices();
	//更新邻接关系
	calCCAdjRelation();
	isDone = true;
}

//this永远存储model[0]，model[i]存储经过i次次细分的结果
//Model temp用来存储model[i-1]，执行temp.doCCSubdivision()避免model[i-1]被改变，
//再将细分后的temp赋值给model[i];
inline void Model::doCCSubdivision(int levelSub, vector<Model>& model)
{
	if (model[levelSub].isDone == true || levelSub == 0)
		return;
	for (int i = 1; i <= levelSub; i++)
	{
		if (model[i].isDone == true)
			continue;
		cerr << "copying model[" << i - 1 << "] to tempModel for level " << i << " subdivision..." << endl;
		Model tempModel(model[i - 1]);
		cerr << "copy is done." << endl;

		cerr << "doing level " << i << " subdivision..." << endl;
		tempModel.doCCSubdivision();
		cerr << "level " << i << " subdivision is done." << endl;

		cerr << "copying tempModel to model[" << i << "]..." << endl;
		model[i] = tempModel;
		cerr << "copy is done." << endl;
	}
}

//计算新面点，边点，顶点
//新增顶点时，vector<Vertex_Half> vLib被扩充，
//新的顶点索引被存在HalfEdge.newVertex和Face_Half.newVertex中，等calCCAdjRelation()时被使用
inline void Model::calCCNewVertices()
{
	int oldFaceSize = faceLib.size();
	int oldVSize = vLib.size();
	int cur_v_index = vLib.size();
	vec3 p0, p1, p2, p3;

	//计算新面点
	//遍历[[[上一层次]]]的每个面!!!******
	//cerr << endl << "计算面点" << endl;
	for (int i = 0; i < oldFaceSize; i++)
	{
		vec3 f_sum, f_average;
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			f_sum += vLib[edgeLib[faceLib[i].loop[j]].tail].pos;
		}
		f_average = f_sum / (faceLib[i].faceDegree * 1.0f);
		//add
		vLib.push_back(f_average);//HalfEdge::pos new
		faceLib[i].newVertex = cur_v_index;//Face_Half::newVertex
		vLib[cur_v_index].vDegree = faceLib[i].faceDegree;//HalfEdge::vDegree new
		cur_v_index++;
	}
	//计算新边点
	for (int i = 0; i < oldFaceSize; i++)
	{
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			if (edgeLib[edgeLib[faceLib[i].loop[j]].twin].newVertex > 0)
			{
				edgeLib[faceLib[i].loop[j]].newVertex = edgeLib[edgeLib[faceLib[i].loop[j]].twin].newVertex;
				continue;//避免重复计算，但也不能不给这条边分配边点！！！****************
			}
			//一条边的两个端点及两个邻接面点的平均
			p0 = vLib[edgeLib[faceLib[i].loop[j]].tail].pos;
			p1 = vLib[edgeLib[edgeLib[faceLib[i].loop[j]].twin].tail].pos;
			p2 = vLib[faceLib[i].newVertex].pos;//相邻面的面点
			p3 = vLib[faceLib[edgeLib[edgeLib[faceLib[i].loop[j]].twin].faceIndex].newVertex].pos;//相邻面的面点
			vec3 ed_average = ed_average = (p0 + p1 + p2 + p3) / vec3(4);
			//add
			vLib.push_back(ed_average);//HalfEdge::pos new
			edgeLib[faceLib[i].loop[j]].newVertex = cur_v_index;//HalfEdge::newVertex
			vLib[vLib.size() - 1].vDegree = 4;//HalfEdge::vDegree new
			cur_v_index++;
		}
	}

	//计算新顶点，注意新的hEdge的初始化
	//计算新顶点时要用到旧顶点，必须先把旧顶点保存起来，
	//避免后计算的顶点时 ###使用更新过的顶点作为旧顶点### ！！！！！！
	vector<vec3> temp_vLib(oldVSize);
	for (int i = 0; i < oldVSize; i++)
	{
		temp_vLib[i] = vLib[i].pos;
	}

	for (int i = 0; i < oldVSize; i++)
	{
		int vd = vLib[i].vDegree;//顶点的度
		//共边的点的平均值和共面的面点的平均
		int curEdge = vLib[i].hEdge;

		#pragma region new
				vec3 v_comEd_average = temp_vLib[edgeLib[curEdge].tail];
				vec3 v_comF_average = vLib[faceLib[edgeLib[curEdge].faceIndex].newVertex].pos;
				for (int j = 0; j < vd - 1; j++)
				{
					curEdge = edgeLib[edgeLib[curEdge].next].twin;
					v_comEd_average += temp_vLib[edgeLib[curEdge].tail];
					v_comF_average += vLib[faceLib[edgeLib[curEdge].faceIndex].newVertex].pos;
				}
				vLib[i].pos = temp_vLib[i] * vec3((vd - 2) / vd) + v_comEd_average / vec3(vd * vd) + v_comF_average / vec3(vd * vd);
		#pragma endregion

		//#pragma region old
		//		vec3 v_comEd_average = vLib[edgeLib[curEdge].tail].pos;
		//		vec3 v_comF_average = vLib[faceLib[edgeLib[curEdge].faceIndex].newVertex].pos;
		//		for (int j = 0; j < vd - 1; j++)
		//		{
		//			curEdge = edgeLib[edgeLib[curEdge].next].twin;
		//			v_comEd_average += vLib[edgeLib[curEdge].tail].pos;
		//			v_comF_average += vLib[faceLib[edgeLib[curEdge].faceIndex].newVertex].pos;
		//		}
		//		vLib[i].pos = vLib[i].pos * (vd * 1.0f - 2.0) / (vd * 1.0f) + v_comEd_average / (vd * vd * 1.0f) + v_comF_average / (vd * vd * 1.0f);
		//#pragma endregion

		//#pragma region WIKI
		//	vec3 v_comEd_mid_average = (temp_vLib[edgeLib[curEdge].tail] + temp_vLib[i]) / (vd * 1.0f);
		//	vec3 v_comF_average = vLib[faceLib[edgeLib[curEdge].faceIndex].newVertex].pos / (vd * 1.0f);
		//	for (int j = 0; j < vd - 1; j++)
		//	{
		//		curEdge = edgeLib[edgeLib[curEdge].next].twin;
		//		v_comEd_mid_average += (temp_vLib[edgeLib[curEdge].tail] + temp_vLib[i]) / (vd * 1.0f);
		//		v_comF_average += vLib[faceLib[edgeLib[curEdge].faceIndex].newVertex].pos / (vd * 1.0f);
		//	}
		//	vLib[i].pos = temp_vLib[i] * (vd * 1.0f - 3 ) / (vd * 1.0f) + v_comEd_mid_average / (vd * 1.0f) + v_comF_average / (vd * 1.0f);
		//#pragma endregion

		//新的hEdge的初始化
		vLib[i].hEdge = -1;
		//vLib[i].vDegree 不变;
	}
}

//更新邻接关系
//新增面时，vector<Face_Half> faceLib和vector<HalfEdge>edgeLib扩充，顶点索引值保存在HalfEdge中
//遍历旧面vector<Face_Half> faceLib，更新各个类的属性，达到建立邻接关系的目的，
//原有面都被抛弃，新面从原有索引值后面开始
//最后进行内存优化，将旧面和旧边覆盖
inline void Model::calCCAdjRelation()
{
	int cur_face_index = faceLib.size();
	int oldFaceSize = faceLib.size();
	int cur_edge_index = edgeLib.size();
	int oldEdgeSize = edgeLib.size();
	//计算初始化前所有面的总度数，即产生新面的数量
	int sum_faceDegree = 0;
	for (int i = 0; i < oldFaceSize; i++)
	{
		sum_faceDegree += faceLib[i].faceDegree;
	}

	//为vector<Face_Half> faceLib和vector<HalfEdge> edgeLib申请空间*******
	faceLib.resize(oldFaceSize + sum_faceDegree);
	edgeLib.resize(edgeLib.size() + sum_faceDegree * 4);

	//vector<Face_Half> faceLib扩充和构建HalfEdge
	for (int i = 0; i < oldFaceSize; i++)
	{
		//每个旧面生成faceDegree个新面，//无论是三角还是四边形都生成四边形
		//生成四个新面的方法相似,用for循环实现
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			faceLib[cur_face_index + j].faceDegree = 4;
			faceLib[cur_face_index + j].loop.resize(4);
			//每个面有4个HalfEdge
			for (int k = 0; k < 4; k++)
			{
				faceLib[cur_face_index + j].loop[k] = cur_edge_index++;
			}

			//loop[0]从新面点出发
			edgeLib[faceLib[cur_face_index + j].loop[0]].tail = faceLib[i].newVertex;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[0]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[0]].next = faceLib[cur_face_index + j].loop[1];//HalfEdge::next

			//loop[1]从旧边的边点出发
			edgeLib[faceLib[cur_face_index + j].loop[1]].tail = edgeLib[faceLib[i].loop[j]].newVertex;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[1]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[1]].next = faceLib[cur_face_index + j].loop[2];//HalfEdge::next

			//loop[2]从新的顶点出发
			edgeLib[faceLib[cur_face_index + j].loop[2]].tail = edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].tail;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[2]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[2]].next = faceLib[cur_face_index + j].loop[3];//HalfEdge::next

			//loop[3]从下一条旧边的边点出发
			edgeLib[faceLib[cur_face_index + j].loop[3]].tail = edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].newVertex;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[3]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[3]].next = faceLib[cur_face_index + j].loop[0];//HalfEdge::next

		}
		cur_face_index += faceLib[i].faceDegree;//每个旧面面产生faceLib[i].faceDegree个新面，第一次细分时不一定都是4个！扩充面的时候也要注意*****
	}

	//计算HalfEdge::twin
	//可能待改进，利用已有的邻接关系去找twin应该更快
	map<TwoPointInd, int> mapHalfEdge;
	pair<map<TwoPointInd, int>::iterator, bool> check_repeat_iter;
	for (int i = oldFaceSize; i < cur_face_index; i++)
	{
		//每个新面有4个半边，//无论是三角还是四边形都生成四边形，有四条新半边
		for (int j = 0; j < 4; j++)
		{
			TwoPointInd tp(edgeLib[faceLib[i].loop[j]].tail, edgeLib[faceLib[i].loop[(j + 1) % 4]].tail);
			check_repeat_iter = mapHalfEdge.insert(map<TwoPointInd, int>::value_type(tp, faceLib[i].loop[j]));
			//插入失败表示已有，即找到
			if (check_repeat_iter.second == false)
			{
				edgeLib[faceLib[i].loop[j]].twin = mapHalfEdge[tp];//twin
				edgeLib[mapHalfEdge[tp]].twin = faceLib[i].loop[j];//twin
				mapHalfEdge.erase(check_repeat_iter.first);
			}
		}
	}

	//构建Vertex_Half::hEdge
	for (int i = oldFaceSize; i < cur_face_index; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (vLib[edgeLib[faceLib[i].loop[j]].tail].hEdge == -1)//保存任一条即可，也只能保存一条
				vLib[edgeLib[faceLib[i].loop[j]].tail].hEdge = (faceLib[i].loop[(j + 3) % 4]);//hEdge
		}
	}

	//删除旧的半边（整体前移覆盖）
	for (int i = 0, j = oldEdgeSize; j < cur_edge_index; i++, j++)
	{
		edgeLib[i].tail = edgeLib[j].tail;//半边尾部顶点的索引
		edgeLib[i].newVertex = -1;//新增点的索引
		edgeLib[i].faceIndex = edgeLib[j].faceIndex - oldFaceSize;//半边所在面的顶点索引
		edgeLib[i].twin = edgeLib[j].twin - oldEdgeSize;//成对的另一条半边
		edgeLib[i].next = edgeLib[j].next - oldEdgeSize;//成对的另一条半边
	}
	edgeLib.resize(cur_edge_index - oldEdgeSize);
	//删除旧面（整体前移覆盖）
	for (int i = 0, j = oldFaceSize; j < cur_face_index; i++, j++)
	{
		faceLib[i].loop.resize(4);
		for (int k = 0; k < 4; k++)
		{
			faceLib[i].loop[k] = faceLib[j].loop[k] - oldEdgeSize;//该面上的半边索引
		}
		faceLib[i].newVertex = -1;//新面点的索引
		faceLib[i].faceDegree = 4;//面的边数
	}
	faceLib.resize(cur_face_index - oldFaceSize);
	//顶点也要更新！！！***
	int vSize = vLib.size();
	for (int i = 0; i < vSize; i++)
	{
		vLib[i].hEdge = vLib[i].hEdge - oldEdgeSize;
	}

}

//生成用于不同绘制方式的数组
inline void Model::genDrawableArray(SUB_RESULT_USAGE USAGE, vector<GLdouble>& resultArray,vector<unsigned int> &degree, vector<unsigned short> & out_indices)
{
	int start_vertex_index = 0;
	int end_vertex_index = getVertexSize() - 1;
	int start_face_index = 0;
	int end_face_index = getFaceSize() - 1;
	switch (USAGE)
	{
	case DRAW_VERTEX:
		genArrayDrawVertex(resultArray, start_vertex_index, end_vertex_index);
		break;
	case DRAW_FRAME:
		genArrayDrawFrame(resultArray, start_face_index, end_face_index);
		break;
	case DRAW_TRIFACE:
		genArrayDrawTriFace(resultArray, start_face_index, end_face_index, out_indices);
		break;
	case DRAW_POLYFACE:case DRAW_POLYLINE:
		genArrayDrawPolygon(resultArray, start_face_index, end_face_index,degree);
		break;


	default:
		cerr << "sorry, but we can't generate this kind of array" << endl;
		break;
	}
}

//生成用于绘制顶点的数组
inline void Model::genArrayDrawVertex(vector<GLdouble>& v, int start_vertex_index, int end_vertex_index)
{
	v.clear();
	for (int i = start_vertex_index; i <= end_vertex_index; i++)
	{
		v.push_back(vLib[i].pos.x);
		v.push_back(vLib[i].pos.y);
		v.push_back(vLib[i].pos.z);
	}
}

//生成用于绘制框架的数组
inline void Model::genArrayDrawFrame(vector<GLdouble>& v, int start_face_index, int end_face_index)
{
	v.clear();
	for (int i = start_face_index; i <= end_face_index; i++)
	{
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j]].tail].pos.x);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j]].tail].pos.y);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j]].tail].pos.z);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].tail].pos.x);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].tail].pos.y);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].tail].pos.z);
		}
	}
}
  
//生成用于绘制三角形表面的数组
inline void Model::genArrayDrawTriFace(vector<GLdouble>& v, int start_face_index, int end_face_index, vector<unsigned short> & out_indices)
{
	v.clear();
	std::map<PackedVertex, pair<vec3,unsigned short>> VertexToOutIndex;
	unsigned short IndexMax = 0;
	int HELP[] = { 0,1,2,0,2,3 };
	for (int i = start_face_index; i <= end_face_index; i++)
	{
		int num_tri = faceLib[i].faceDegree - 2;//每个面被分割成的的三角形数量
		for (int j = 0; j < num_tri; j++)
		{
			//计算该面法向
			vec3 temp;
			temp = normalize(computeNormal(vLib[this->edgeLib[faceLib[i].loop[0]].tail].pos,
				vLib[this->edgeLib[faceLib[i].loop[j+1]].tail].pos,
				vLib[this->edgeLib[faceLib[i].loop[j+2]].tail].pos));
			for (int k = 0; k < 3; k++) {
				PackedVertex packed(vLib[this->edgeLib[faceLib[i].loop[HELP[j*3+k]]].tail].pos);
				unsigned short index;

				bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

				if (found) {
					VertexToOutIndex[packed].first += temp;
					out_indices.push_back(index);
				}
				else {
					out_indices.push_back(IndexMax);
					VertexToOutIndex[packed] = pair<vec3,unsigned short>(temp,IndexMax++);
				}
			}
			//cerr << "sdsffdf";
			/*v.push_back(vLib[this->edgeLib[faceLib[i].loop[0]].tail].pos.x);
			lightData << v.back()<<", ";
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[0]].tail].pos.y);
			lightData << v.back() << ", ";
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[0]].tail].pos.z);
			lightData << v.back() << ", ";
			v.push_back(temp.x); lightData << v.back() << ", ";
			v.push_back(temp.y); lightData << v.back() << ", ";
			v.push_back(temp.z); lightData << v.back() << ", "<<endl;

			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j + 1]].tail].pos.x);
			lightData << v.back() << ", ";
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j + 1]].tail].pos.y);
			lightData << v.back() << ", ";
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j + 1]].tail].pos.z);
			lightData << v.back() << ", ";
			v.push_back(temp.x); lightData << v.back() << ", ";
			v.push_back(temp.y); lightData << v.back() << ", ";
			v.push_back(temp.z); lightData << v.back() << ", " << endl;

			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j + 2]].tail].pos.x);
			lightData << v.back() << ", ";
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j + 2]].tail].pos.y);
			lightData << v.back() << ", ";
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j + 2]].tail].pos.z);
			lightData << v.back() << ", ";

			v.push_back(temp.x);			lightData << v.back() << ", ";

			v.push_back(temp.y);			lightData << v.back() << ", ";

			v.push_back(temp.z);
			lightData << v.back() << ", "<<endl;
*/
		}
	}
	int count = 0;
	vector<unsigned short> TempIndexArray;
	map<unsigned short, unsigned short> f;//Index映射 
	for (auto value : VertexToOutIndex) {
		v.push_back(value.first.position.x);
		v.push_back(value.first.position.y);
		v.push_back(value.first.position.z);
		value.second.first = normalize(value.second.first);
		v.push_back(value.second.first.x);
		v.push_back(value.second.first.y);
		v.push_back(value.second.first.z);
		f[value.second.second] = count++;
	}
	for (int i = 0; i < out_indices.size(); i++)
		TempIndexArray.push_back(f[out_indices[i]]);
	out_indices = TempIndexArray;


}

inline void Model::regenArrayDrawTriFace(vector<GLdouble>& v, int start_face_index, int end_face_index)
{
	v.clear();
	/*
	遍历每个面
	遍历该面的每个点
	该点的法向等于邻接面的法向的和的平均值
	*/
	
	for (int i = start_face_index; i <= end_face_index; i++)//每个面
	{
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			int vd = vLib[edgeLib[faceLib[i].loop[j]].tail].vDegree;
			for (int k = 0; k < vd; k++)
			{

			}
			
		}
		
		
	}
}

inline void Model::genArrayDrawPolygon(vector<GLdouble>& v, int start_face_index, int end_face_index,vector<unsigned int> &degree)
{
	v.clear();
	for (int i = start_face_index; i <= end_face_index; i++)
	{
		degree.push_back(faceLib[i].faceDegree);
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j]].tail].pos.x);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j]].tail].pos.y);
			v.push_back(vLib[this->edgeLib[faceLib[i].loop[j]].tail].pos.z);
		}
	}
}

inline void Model::calNormol(int start_face_index, int end_face_index)
{
	for (int i = start_face_index; i <= end_face_index; i++)
	{
			vec3 temp;
			temp = computeNormal(vLib[this->edgeLib[faceLib[i].loop[0]].tail].pos,
				vLib[this->edgeLib[faceLib[i].loop[1]].tail].pos,
				vLib[this->edgeLib[faceLib[i].loop[2]].tail].pos);
			faceLib[i].normal = temp;
	}
}

//生成obj文件
inline void Model::genObjFile(char *path)
{
	ofstream output(path);
	//统计点，边和面的数量
	output << "num of vertex is : " << getVertexSize() << endl;
	output << "num of edge is : " << getHalfEdgeSize() / 2 << endl;
	output << "num of face is : " << getFaceSize() << endl << endl;
	//顶点
	for (int i = 0; i < vLib.size(); i++)
	{
		output << 'v' << ' ' << vLib[i].pos.x << ' ' << vLib[i].pos.y << ' ' << vLib[i].pos.z << endl;
	}
	//面
	for (int i = 0; i < faceLib.size(); i++)
	{
		output << 'f' << ' ';
		for (int j = 0; j < faceLib[i].faceDegree - 1; j++)
		{
			output << edgeLib[faceLib[i].loop[j]].tail + 1 << ' ';
		}
		output << edgeLib[faceLib[i].loop[faceLib[i].faceDegree - 1]].tail + 1 << ' ';
		output << endl;
	}
}

inline vec3 Model::computeNormal(vec3 const & a, vec3 const & b, vec3 const & c)
{
	return glm::normalize(glm::cross(c - a, b - a));
	return vec3();
}
