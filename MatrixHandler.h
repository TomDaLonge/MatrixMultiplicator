#pragma once

#include <map>

class Logger;

using namespace std;

class MatrixHandler
{
public:
	MatrixHandler(void);
	~MatrixHandler(void);

	bool createMatrix(map<int, map<int, int>> &currentMatrix,
					  int width,
					  int height);

private:
	Logger* m_pLogger;
};

