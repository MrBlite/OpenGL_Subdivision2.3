#pragma once
//#include "model.h"
#include "assistance.h"
//#include "vec3.h"
//using namespace std;

#define MAX_FACE_DEGREE 10//一个面最大有几个点

struct Vertex {
	vec3 pos;

	Vertex() : pos(vec3()) {}
	Vertex(vec3 p) : pos(p) {}
};

struct Face {

	vector<int> vIndex;

	Face() {}
	Face(vector<int> v)
	{
		vIndex = v;
	}
};

class Data
{
public:
	vector<Face> faces;//存放索引数据
	vector<Vertex> vertices;//存放顶点数据

	Data();
	Data(char* path);
	void loadData(char *path);
	int facesSize();
	int veticesSize();
};

Data::Data() {}

Data::Data(char* path)
{
	loadData(path);
}

void Data::loadData(char *path)
{
	fstream file(path);
	string line, tempStr, s[MAX_FACE_DEGREE + 1];

	//int size;
	while (getline(file, line))//将一行内容从文件流放入s中
	{
		istringstream sin(line);//将s放入流sin中
		//读入坐标
		if (line[0] == 'v' && line[1] == ' ')
		{
			sin >> tempStr;
			for (int i = 0; i < 3; i++) sin >> s[i];
			vertices.push_back(vec3(stof(s[0]), stof(s[1]), stof(s[2])));
		}
		//读入每个面的顶点索引
		if (line[0] == 'f'&& line[1] == ' ')
		{
			sin >> tempStr;
			int curIndex = 0;
			vector<int> tempVec;
			while (sin >> s[curIndex] && s[curIndex] != "")
			{
				tempVec.push_back(stoi(s[curIndex]) - 1);//obj顶点索引从1开始
				curIndex++;
			}
			faces.push_back(tempVec);
			//vector<int> tempVec2(curIndex);
			//for (int i = 0; i < curIndex; i++)
			//{
			//	tempVec2[i] = tempVec[curIndex -1 -i];
			//}
			//faces.push_back(tempVec2);
		}
	}
}

int Data::facesSize()
{
	return faces.size();
}

int Data::veticesSize()
{
	return vertices.size();
}