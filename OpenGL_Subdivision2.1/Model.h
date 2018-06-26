#pragma once
#include <map>
#include <vector>
#include <algorithm>
//#include "vec3.h"
#include "Data.h"
using namespace glm;
using namespace std;
ofstream lightData("lightData.txt");

//���㷨�߸�����
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

//���
class HalfEdge
{
public:
	int tail;//���β�����������
	int newVertex;//�����������
	int faceIndex;//���������Ķ�������
	int twin;//�ɶԵ���һ�����
	int next;//��һ�����

	HalfEdge()
	{
		next = twin = tail = newVertex = faceIndex = -1;
	}
	~HalfEdge() {};
};

//����
class Vertex_Half
{
public:
	vec3 pos;//��������
	int hEdge;//ָ��ö���İ��,�кܶ࣬����һ������
	int vDegree;//����Ķ�
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

//��
class Face_Half
{
public:
	vector<int> loop;//�����ϵ����а��������ֻ����һ����ɣ�Ϊ�˷��㣬�������е�
	int newVertex;//����������
	int faceDegree;//��ı���
	vec3 normal;
	Face_Half()
	{
		newVertex = -1;
		faceDegree = 0;
	}
	~Face_Half() {}

};

//�ýṹ���ڴ������intֵ����������һЩ���㷽��Ƚϴ�С��ʵ����������Ϊmap�Ĺؼ��֣��ڲ�ѯ��ߵ�twinʱ�õ�
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
	bool isDone;//�ò�model�Ƿ�ϸ��/initModel��������ֻ��һ���տ�

	//��������㣬�ߵ㣬����
	void calCCNewVertices();
	//�����ڽӹ�ϵ
	void calCCAdjRelation();
	//�������ڻ��ƶ��������
	void genArrayDrawVertex(vector<GLdouble> &v, int start_face_index, int end_face_index);
	//�������ڻ��ƿ�ܵ�����
	void genArrayDrawFrame(vector<GLdouble> &v, int start_face_index, int end_face_index);
	//�������ڻ��������α��������
	void genArrayDrawTriFace(vector<GLdouble> &v, int start_face_index, int end_face_index,vector<unsigned short> & out_indices);
	void regenArrayDrawTriFace(vector<GLdouble> &v, int start_face_index, int end_face_index);
	//�������ڻ��ƶ���ε�����
	void genArrayDrawPolygon(vector<GLdouble> &v, int start_face_index, int end_face_index, vector<unsigned int> &degree);
	//���㷨��
	void calNormol(int start_face_index, int end_face_index);

public:
	Model();
	Model(Data data);
	~Model();
	int getFaceSize();
	int getVertexSize();
	int getHalfEdgeSize();

	//��ʼ��vlib��faceLib�е����ݣ��������ڽӹ�ϵ����ʼmodel���ɣ�Ϊ��һ��ϸ������׼��
	void initModel(Data data);
	//ϸ��
	void doCCSubdivision();//ϸ�ֲ�������model�Ѿ�model����
	//ǰlevelSub��ϸ�ֵĽ������vector<Model> &model�У���ϸ��ǰ�����ж��Ƿ�ϸ�ֹ����ɱ����ظ�����
	void doCCSubdivision(int levelSub, vector<Model> &model);
	//�������ڲ�ͬ���Ʒ�ʽ������
	void genDrawableArray(SUB_RESULT_USAGE USAGE, vector<GLdouble> &resultArray,vector<unsigned int> &degree, vector<unsigned short> & out_indices);
	//����obj�ļ�
	void genObjFile(char *path);
	//���㷨����
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

//��ʼ��vlib��faceLib�е����ݣ��������ڽӹ�ϵ����ʼmodel���ɣ�Ϊ��һ��ϸ������׼��
//��������Ա���ʼ�����ڽӹ�ϵ������������
//VertexHalf�д���ж���λ�õ���ʵ��Ϣ,��һ�δ�vertices�ж��룬
//���������鲻�ٱ�ʹ�ã�����ֱ�Ӵ���vector<Vertex_Half> vLib��
//HalfEdge�д���ж�������ֵ����ʵ��Ϣ��Face_Half��faceDegree��HalfEdge��������
//��һ��ͨ������vector<Face_Half> faceLib��faces�ж������ݹ����ڽӹ�ϵ�������faces������
//�Ժ�ͨ������vector<Face_Half> faceLib�������ӹ�ϵ�Ͳ�ѯ

//��ʼ��vlib��faceLib�е�����
inline void Model::initModel(Data data)
{
	isDone = true;
	//��vlibд��λ����Ϣ
	int vSize = data.veticesSize();
	int cur_edge_index = 0;
	for (int i = 0; i < vSize; i++)
	{
		vLib.push_back(data.vertices[i].pos);//pos
	}
#pragma region �������
	//for (int i = 0; i < vSize; i++)
	//{
	//	cout << vLib[i].pos << endl;
	//}
#pragma endregion
	map<TwoPointInd, int> mapHalfEdge;
	pair<map<TwoPointInd, int>::iterator, bool> check_repeat_iter;

	//�����棬��ʼ���ڽ���Ϣ
	int faceSize = data.facesSize();
	//Ϊvector<Face_Half> faceLib����ռ�
	faceLib.resize(faceSize);
	//��ʼ��ÿ�����faceDegree�������ܹ���HalfEdge������������Ϊvector<int> loop����ռ�*********
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
			if (vLib[data.faces[i].vIndex[j]].hEdge == -1)//������һ������
				vLib[data.faces[i].vIndex[j]].hEdge = (faceLib[i].loop[(j + faceLib[i].faceDegree - 1) % faceLib[i].faceDegree]);//hEdge
			vLib[data.faces[i].vIndex[j]].vDegree++;
			//find twin
			TwoPointInd tp(data.faces[i].vIndex[j], data.faces[i].vIndex[(j + 1) % faceLib[i].faceDegree]);
			check_repeat_iter = mapHalfEdge.insert(map<TwoPointInd, int>::value_type(tp, faceLib[i].loop[j]));
			//����ʧ�ܱ�ʾ���У����ҵ�
			if (check_repeat_iter.second == false)
			{
				edgeLib[faceLib[i].loop[j]].twin = mapHalfEdge[tp];//twin
				edgeLib[mapHalfEdge[tp]].twin = faceLib[i].loop[j];//twin
				mapHalfEdge.erase(check_repeat_iter.first);
			}
		}
	}
	mapHalfEdge.clear();

#pragma region �����
	//cout << "��ʼ����ɣ���ʼ����棺" << endl;
	//for (int i = 0; i < oldFaceSize; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//		cout << vLib[faceLib[i].loop[j].tail].pos << ' ';
	//	cout << endl;
	//}
	//cout << "��������" << endl;
#pragma endregion

}

inline void Model::doCCSubdivision()
{
	//�����¶���
	calCCNewVertices();
	//�����ڽӹ�ϵ
	calCCAdjRelation();
	isDone = true;
}

//this��Զ�洢model[0]��model[i]�洢����i�δ�ϸ�ֵĽ��
//Model temp�����洢model[i-1]��ִ��temp.doCCSubdivision()����model[i-1]���ı䣬
//�ٽ�ϸ�ֺ��temp��ֵ��model[i];
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

//��������㣬�ߵ㣬����
//��������ʱ��vector<Vertex_Half> vLib�����䣬
//�µĶ�������������HalfEdge.newVertex��Face_Half.newVertex�У���calCCAdjRelation()ʱ��ʹ��
inline void Model::calCCNewVertices()
{
	int oldFaceSize = faceLib.size();
	int oldVSize = vLib.size();
	int cur_v_index = vLib.size();
	vec3 p0, p1, p2, p3;

	//���������
	//����[[[��һ���]]]��ÿ����!!!******
	//cerr << endl << "�������" << endl;
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
	//�����±ߵ�
	for (int i = 0; i < oldFaceSize; i++)
	{
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			if (edgeLib[edgeLib[faceLib[i].loop[j]].twin].newVertex > 0)
			{
				edgeLib[faceLib[i].loop[j]].newVertex = edgeLib[edgeLib[faceLib[i].loop[j]].twin].newVertex;
				continue;//�����ظ����㣬��Ҳ���ܲ��������߷���ߵ㣡����****************
			}
			//һ���ߵ������˵㼰�����ڽ�����ƽ��
			p0 = vLib[edgeLib[faceLib[i].loop[j]].tail].pos;
			p1 = vLib[edgeLib[edgeLib[faceLib[i].loop[j]].twin].tail].pos;
			p2 = vLib[faceLib[i].newVertex].pos;//����������
			p3 = vLib[faceLib[edgeLib[edgeLib[faceLib[i].loop[j]].twin].faceIndex].newVertex].pos;//����������
			vec3 ed_average = ed_average = (p0 + p1 + p2 + p3) / vec3(4);
			//add
			vLib.push_back(ed_average);//HalfEdge::pos new
			edgeLib[faceLib[i].loop[j]].newVertex = cur_v_index;//HalfEdge::newVertex
			vLib[vLib.size() - 1].vDegree = 4;//HalfEdge::vDegree new
			cur_v_index++;
		}
	}

	//�����¶��㣬ע���µ�hEdge�ĳ�ʼ��
	//�����¶���ʱҪ�õ��ɶ��㣬�����ȰѾɶ��㱣��������
	//��������Ķ���ʱ ###ʹ�ø��¹��Ķ�����Ϊ�ɶ���### ������������
	vector<vec3> temp_vLib(oldVSize);
	for (int i = 0; i < oldVSize; i++)
	{
		temp_vLib[i] = vLib[i].pos;
	}

	for (int i = 0; i < oldVSize; i++)
	{
		int vd = vLib[i].vDegree;//����Ķ�
		//���ߵĵ��ƽ��ֵ�͹��������ƽ��
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

		//�µ�hEdge�ĳ�ʼ��
		vLib[i].hEdge = -1;
		//vLib[i].vDegree ����;
	}
}

//�����ڽӹ�ϵ
//������ʱ��vector<Face_Half> faceLib��vector<HalfEdge>edgeLib���䣬��������ֵ������HalfEdge��
//��������vector<Face_Half> faceLib�����¸���������ԣ��ﵽ�����ڽӹ�ϵ��Ŀ�ģ�
//ԭ���涼�������������ԭ������ֵ���濪ʼ
//�������ڴ��Ż���������;ɱ߸���
inline void Model::calCCAdjRelation()
{
	int cur_face_index = faceLib.size();
	int oldFaceSize = faceLib.size();
	int cur_edge_index = edgeLib.size();
	int oldEdgeSize = edgeLib.size();
	//�����ʼ��ǰ��������ܶ��������������������
	int sum_faceDegree = 0;
	for (int i = 0; i < oldFaceSize; i++)
	{
		sum_faceDegree += faceLib[i].faceDegree;
	}

	//Ϊvector<Face_Half> faceLib��vector<HalfEdge> edgeLib����ռ�*******
	faceLib.resize(oldFaceSize + sum_faceDegree);
	edgeLib.resize(edgeLib.size() + sum_faceDegree * 4);

	//vector<Face_Half> faceLib����͹���HalfEdge
	for (int i = 0; i < oldFaceSize; i++)
	{
		//ÿ����������faceDegree�����棬//���������ǻ����ı��ζ������ı���
		//�����ĸ�����ķ�������,��forѭ��ʵ��
		for (int j = 0; j < faceLib[i].faceDegree; j++)
		{
			faceLib[cur_face_index + j].faceDegree = 4;
			faceLib[cur_face_index + j].loop.resize(4);
			//ÿ������4��HalfEdge
			for (int k = 0; k < 4; k++)
			{
				faceLib[cur_face_index + j].loop[k] = cur_edge_index++;
			}

			//loop[0]����������
			edgeLib[faceLib[cur_face_index + j].loop[0]].tail = faceLib[i].newVertex;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[0]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[0]].next = faceLib[cur_face_index + j].loop[1];//HalfEdge::next

			//loop[1]�Ӿɱߵıߵ����
			edgeLib[faceLib[cur_face_index + j].loop[1]].tail = edgeLib[faceLib[i].loop[j]].newVertex;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[1]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[1]].next = faceLib[cur_face_index + j].loop[2];//HalfEdge::next

			//loop[2]���µĶ������
			edgeLib[faceLib[cur_face_index + j].loop[2]].tail = edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].tail;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[2]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[2]].next = faceLib[cur_face_index + j].loop[3];//HalfEdge::next

			//loop[3]����һ���ɱߵıߵ����
			edgeLib[faceLib[cur_face_index + j].loop[3]].tail = edgeLib[faceLib[i].loop[(j + 1) % faceLib[i].faceDegree]].newVertex;//HalfEdge::tail
			edgeLib[faceLib[cur_face_index + j].loop[3]].faceIndex = cur_face_index + j;//HalfEdge::faceIndex
			edgeLib[faceLib[cur_face_index + j].loop[3]].next = faceLib[cur_face_index + j].loop[0];//HalfEdge::next

		}
		cur_face_index += faceLib[i].faceDegree;//ÿ�����������faceLib[i].faceDegree�����棬��һ��ϸ��ʱ��һ������4�����������ʱ��ҲҪע��*****
	}

	//����HalfEdge::twin
	//���ܴ��Ľ����������е��ڽӹ�ϵȥ��twinӦ�ø���
	map<TwoPointInd, int> mapHalfEdge;
	pair<map<TwoPointInd, int>::iterator, bool> check_repeat_iter;
	for (int i = oldFaceSize; i < cur_face_index; i++)
	{
		//ÿ��������4����ߣ�//���������ǻ����ı��ζ������ı��Σ��������°��
		for (int j = 0; j < 4; j++)
		{
			TwoPointInd tp(edgeLib[faceLib[i].loop[j]].tail, edgeLib[faceLib[i].loop[(j + 1) % 4]].tail);
			check_repeat_iter = mapHalfEdge.insert(map<TwoPointInd, int>::value_type(tp, faceLib[i].loop[j]));
			//����ʧ�ܱ�ʾ���У����ҵ�
			if (check_repeat_iter.second == false)
			{
				edgeLib[faceLib[i].loop[j]].twin = mapHalfEdge[tp];//twin
				edgeLib[mapHalfEdge[tp]].twin = faceLib[i].loop[j];//twin
				mapHalfEdge.erase(check_repeat_iter.first);
			}
		}
	}

	//����Vertex_Half::hEdge
	for (int i = oldFaceSize; i < cur_face_index; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (vLib[edgeLib[faceLib[i].loop[j]].tail].hEdge == -1)//������һ�����ɣ�Ҳֻ�ܱ���һ��
				vLib[edgeLib[faceLib[i].loop[j]].tail].hEdge = (faceLib[i].loop[(j + 3) % 4]);//hEdge
		}
	}

	//ɾ���ɵİ�ߣ�����ǰ�Ƹ��ǣ�
	for (int i = 0, j = oldEdgeSize; j < cur_edge_index; i++, j++)
	{
		edgeLib[i].tail = edgeLib[j].tail;//���β�����������
		edgeLib[i].newVertex = -1;//�����������
		edgeLib[i].faceIndex = edgeLib[j].faceIndex - oldFaceSize;//���������Ķ�������
		edgeLib[i].twin = edgeLib[j].twin - oldEdgeSize;//�ɶԵ���һ�����
		edgeLib[i].next = edgeLib[j].next - oldEdgeSize;//�ɶԵ���һ�����
	}
	edgeLib.resize(cur_edge_index - oldEdgeSize);
	//ɾ�����棨����ǰ�Ƹ��ǣ�
	for (int i = 0, j = oldFaceSize; j < cur_face_index; i++, j++)
	{
		faceLib[i].loop.resize(4);
		for (int k = 0; k < 4; k++)
		{
			faceLib[i].loop[k] = faceLib[j].loop[k] - oldEdgeSize;//�����ϵİ������
		}
		faceLib[i].newVertex = -1;//����������
		faceLib[i].faceDegree = 4;//��ı���
	}
	faceLib.resize(cur_face_index - oldFaceSize);
	//����ҲҪ���£�����***
	int vSize = vLib.size();
	for (int i = 0; i < vSize; i++)
	{
		vLib[i].hEdge = vLib[i].hEdge - oldEdgeSize;
	}

}

//�������ڲ�ͬ���Ʒ�ʽ������
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

//�������ڻ��ƶ��������
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

//�������ڻ��ƿ�ܵ�����
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
  
//�������ڻ��������α��������
inline void Model::genArrayDrawTriFace(vector<GLdouble>& v, int start_face_index, int end_face_index, vector<unsigned short> & out_indices)
{
	v.clear();
	std::map<PackedVertex, pair<vec3,unsigned short>> VertexToOutIndex;
	unsigned short IndexMax = 0;
	int HELP[] = { 0,1,2,0,2,3 };
	for (int i = start_face_index; i <= end_face_index; i++)
	{
		int num_tri = faceLib[i].faceDegree - 2;//ÿ���汻�ָ�ɵĵ�����������
		for (int j = 0; j < num_tri; j++)
		{
			//������淨��
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
	map<unsigned short, unsigned short> f;//Indexӳ�� 
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
	����ÿ����
	���������ÿ����
	�õ�ķ�������ڽ���ķ���ĺ͵�ƽ��ֵ
	*/
	
	for (int i = start_face_index; i <= end_face_index; i++)//ÿ����
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

//����obj�ļ�
inline void Model::genObjFile(char *path)
{
	ofstream output(path);
	//ͳ�Ƶ㣬�ߺ��������
	output << "num of vertex is : " << getVertexSize() << endl;
	output << "num of edge is : " << getHalfEdgeSize() / 2 << endl;
	output << "num of face is : " << getFaceSize() << endl << endl;
	//����
	for (int i = 0; i < vLib.size(); i++)
	{
		output << 'v' << ' ' << vLib[i].pos.x << ' ' << vLib[i].pos.y << ' ' << vLib[i].pos.z << endl;
	}
	//��
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
