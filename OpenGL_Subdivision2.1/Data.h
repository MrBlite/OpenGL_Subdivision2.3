#pragma once
//#include "model.h"
#include "assistance.h"
//#include "vec3.h"
//using namespace std;

#define MAX_FACE_DEGREE 10//һ��������м�����

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
	vector<Face> faces;//�����������
	vector<Vertex> vertices;//��Ŷ�������

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
	while (getline(file, line))//��һ�����ݴ��ļ�������s��
	{
		istringstream sin(line);//��s������sin��
		//��������
		if (line[0] == 'v' && line[1] == ' ')
		{
			sin >> tempStr;
			for (int i = 0; i < 3; i++) sin >> s[i];
			vertices.push_back(vec3(stof(s[0]), stof(s[1]), stof(s[2])));
		}
		//����ÿ����Ķ�������
		if (line[0] == 'f'&& line[1] == ' ')
		{
			sin >> tempStr;
			int curIndex = 0;
			vector<int> tempVec;
			while (sin >> s[curIndex] && s[curIndex] != "")
			{
				tempVec.push_back(stoi(s[curIndex]) - 1);//obj����������1��ʼ
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