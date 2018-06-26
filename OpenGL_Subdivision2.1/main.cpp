#include "Draw.h"
#include "Data.h"
#include "Model.h"
#define MAX_LEVEL 10


int main()
{
	vector<GLdouble> resultArray;
	vector<unsigned int> degree;
	int level = 0;
	int levelSub = level;
	int levelDraw = level;
	//SUB_RESULT_USAGE USAGE = DRAW_VERTEX;
	//SUB_RESULT_USAGE USAGE = DRAW_POLYFACE;
	//SUB_RESULT_USAGE USAGE = DRAW_TRIFACE;
	SUB_RESULT_USAGE USAGE = DRAW_TRIFACE;
	vector<Model> model(MAX_LEVEL);
	//char *path_out("swc110_0.obj");
	//char *path_out("swc108_2_S_3.obj");
	//char *path_out("cylinder_1.obj");
	//char *path_out("cylinder_4.obj");
	//char *path_out("quad_44.obj");

	cerr << "loading data from obj file..." << endl;
	//Data data("swc108_2.obj");
	Data data("swc_4_12.obj");//只可以展示光照，不可以细分
	//Data data("cylinder_1.obj");
	//Data data("quad.obj");
	//Data data("basicModel\\ball.obj");
	//Data data("cylinder.obj");
	//Data data("multi.obj");
	//Data data("prism.obj");//棱柱
	//Data data("pyramid.obj");//金字塔
	cerr << "loading data is done." << endl << endl;

	cerr << "initializing model[0]..." << endl;
	model[0].initModel(data);
	cerr << "initialization is done." << endl << endl;

	if (levelSub > 0)
	{
		cerr << "start doCCSubdivision..." << endl;
		model[0].doCCSubdivision(levelSub, model);
		cerr << "doCCSubdivision is done." << endl << endl;
	}
	
	vector<unsigned short> out_indices;

	cerr << "start genDrawableArray..." << endl;
	model[levelDraw].genDrawableArray(USAGE, resultArray,degree, out_indices);
	cerr << "genDrawableArray is done." << endl;
	cerr << "size of resultArray is: " << resultArray.size() << endl << endl;

	cerr << "start drawing..." << endl;
	draw(USAGE, resultArray,degree, out_indices);
	cerr << "drawing is done." << endl << endl;;

	//cerr << "start genObjFile: \""<<path_out<<"\" ..." << endl;
	//model[levelDraw].genObjFile(path_out);
	//cerr << "genObjFile is done." << endl;

	//system(path_out);

	system("pause");
	return 0;
}
