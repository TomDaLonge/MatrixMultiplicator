#include "mpi.h"
#include "Logger.h"
#include "MatrixHandler.h"
#include "PRandMatrixMultiplicator.h"
#include <stdlib.h>

#include <map>
#include <string>

using namespace std;

PRandMatrixMultiplicator::PRandMatrixMultiplicator(void)
{
	m_pLogger = Logger::getLogger();
}

PRandMatrixMultiplicator::~PRandMatrixMultiplicator(void)
{
}

bool 
	PRandMatrixMultiplicator::doTheWholeThing(int Seed, 
											  int CommonSize, 
											  int ResultWidth, 
											  int ResultHeight)
{
	bool Success = true;

	//Get rank and world size
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//Prepare Variables
	int ValueCount = ResultWidth * ResultHeight;
	int BufferSize = CommonSize * 2;
	
	int *buffer;
	MPI_Status status;
	
	if (0 == rank)
	{
		//Main / Distribution Rank

		//Set Seed for random numbers
		srand(Seed);
		m_pLogger->LogDebug("Set the seed to " + to_string(Seed));

		m_pLogger->LogTrace("=================================================================");
		m_pLogger->LogTrace("==                Starting to create Matrices..                ==");
		m_pLogger->LogTrace("=================================================================");
		m_pLogger->LogTrace("");

		//Create the Matrices
		MatrixHandler* mCreator = new MatrixHandler();

		//First Matrix
		map<int, map<int, int>> firstMatrix;

		Success &= mCreator->createMatrix(firstMatrix, CommonSize, ResultHeight);

		m_pLogger->LogTrace("## First Matrix: ##");
		m_pLogger->LogTrace("");

		m_pLogger->printMatrix(&firstMatrix, true);

		//Second Matrix
		map<int, map<int, int>> secondMatrix;

		Success &= mCreator->createMatrix(secondMatrix, ResultWidth, CommonSize);

		m_pLogger->LogTrace("## Second Matrix: ##");
		m_pLogger->LogTrace("");

		m_pLogger->printMatrix(&secondMatrix, true);

		delete mCreator;

		m_pLogger->LogTrace("=================================================================");
		m_pLogger->LogTrace("==                 Starting the computation..                  ==");
		m_pLogger->LogTrace("=================================================================");
		m_pLogger->LogTrace("");

		int currentRecv = 2;
		int resultID = 0;

		for (int currentRow = 0; currentRow < ResultHeight; currentRow++)
		{
			for (int currentCol = 0; currentCol < ResultWidth; currentCol++)
			{
				buffer = new int[BufferSize];

				//Add Values of CurrentRow of first Matrix
				for (int fCurrentCol = 0; fCurrentCol < CommonSize; fCurrentCol++)
				{
					buffer[fCurrentCol] = firstMatrix[currentRow][fCurrentCol];
				}

				//Add Values of CurrentCol of second Matrix
				for (int sCurrentRow = 0; sCurrentRow < CommonSize; sCurrentRow++)
				{
					buffer[sCurrentRow + CommonSize] = secondMatrix[sCurrentRow][currentCol];
				}

				//Send to Process
				MPI_Send(buffer, BufferSize, MPI_INT, currentRecv, resultID, MPI_COMM_WORLD);

				m_pLogger->LogDebug("Sent Values for ResultID " + to_string(resultID) + " to process " + to_string(currentRecv) + ".");

				//Update Receiver
				currentRecv++;
				if (world_size <= currentRecv)
				{
					currentRecv = 2;
				}

				//Increment ResultID
				resultID++;

				//Clear Buffer
				delete[] buffer;
				buffer = nullptr;
			}
		}
	}
	else if (1 == rank)
	{
		//Result Receiver Rank

		map<int, map<int, int>> resultMatrix;
		int currentRow = 0;
		int currentCol = 0;

		//Receive the resultvalues from every working process and put them on the right position
		int currentSrc = 2;
		for (int currentResID = 0; currentResID < ValueCount; currentResID++)
		{
			int resultValue;

			MPI_Recv(&resultValue, 1, MPI_INT, currentSrc, currentResID, MPI_COMM_WORLD, &status);
			m_pLogger->LogDebug("Received Result " + to_string(resultValue) + " for ResultID " + to_string(currentResID) + " from Process " + to_string(currentSrc) + ".");

			resultMatrix[currentRow][currentCol] = resultValue;

			//Update Column
			currentCol++;
			if (currentCol == (ResultWidth))
			{
				currentRow++;
				currentCol = 0;
			}

			//Update source
			currentSrc++;
			if (world_size <= currentSrc)
			{
				currentSrc = 2;
			}
		}

		m_pLogger->LogTrace("=================================================================");
		m_pLogger->LogTrace("==                    Got the result Matrix!                   ==");
		m_pLogger->LogTrace("=================================================================");
		m_pLogger->LogTrace("");

		m_pLogger->printMatrix(&resultMatrix, true);
	}
	else
	{
		//Work Ranks

		//Compute how many values you have to compute
		int NumberOfReceives = (ResultWidth * ResultHeight) / (world_size - 2);
		int remainder = (ResultWidth * ResultHeight) % (world_size - 2);
		if ((rank - 2) < remainder)
		{
			NumberOfReceives++;
		}


		int resultID = rank - 2;
		for (int i = 0; i < NumberOfReceives; i++)
		{
			buffer = new int[BufferSize];

			MPI_Recv(buffer, BufferSize, MPI_INT, 0, resultID, MPI_COMM_WORLD, &status);
			m_pLogger->LogDebug("Received Values for ResultID " + to_string(resultID) + ".");

			int resultValue = 0;
			int secondIt = CommonSize;

			//Add up all the values
			for (int firstIt = 0; firstIt < CommonSize; firstIt++)
			{
				int currentValue = buffer[firstIt] * buffer[secondIt];
				resultValue += currentValue;
				secondIt++;
			}

			//Send result back to receiver
			MPI_Send(&resultValue, 1, MPI_INT, 1, resultID, MPI_COMM_WORLD);
			m_pLogger->LogDebug("Computed and sent ResultValue for ResultID " + to_string(resultID) + ".");

			//Clear buffer
			delete[] buffer;
			buffer = nullptr;

			resultID += (world_size - 2);
		}
	}
    return Success;
}