#include "MatrixHandler.h"
#include "mpi.h"
#include "Logger.h"

#include <string>

using namespace std;

MatrixHandler::MatrixHandler(void)
{
	m_pLogger = Logger::getLogger();
}

MatrixHandler::~MatrixHandler(void)
{
}

bool 
	MatrixHandler::createMatrix(map<int, map<int, int>> &currentMatrix, int width, int height)
{
	bool Success = true;

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (int row = 0; row < height; row++)
	{
		map<int, int> currentRow;
		for (int col = 0; col < width; col++)
		{
			currentRow[col] = rand() % 10;
		}

		currentMatrix[row] = currentRow;
	}

	return Success;
}