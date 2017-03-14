#include "mpi.h"
#include "PRandMatrixMultiplicator.h"
#include "Logger.h"

#include <ctime>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	bool Success = true;

	//Get your own rank
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Set LogLevel
	int LogLevel = 2;
	if (2 < argc)
	{
		LogLevel = atoi(argv[2]);
	}

	Logger* iLogger = Logger::getLogger();
	iLogger->setLogLevel(LogLevel);
	
	//Check input parameters for break criteria (world size > 2 and seed existing)
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (3 > world_size)
	{
		Success &= false;
		if (0 == rank)
		{
			iLogger->LogError("There must be at least three processes. First one for task distribution, second one for receiving the data in the end and at least one for computation.");
		}
	}

	int mSeed;
	if (1 < argc)
	{
		mSeed = atoi(argv[1]);
	}
	else
	{
		Success &= false;
		if (0 == rank)
		{
			iLogger->LogError("Please give at least one input argument that can be used as seed.");
		}
	}

	if (Success)
	{
		time_t StartingTime;
		if (0 == rank)
		{
			iLogger->LogTrace("#################################################################");
			iLogger->LogTrace("##           MatrixMultiplicator | Tom Daniek | 2016           ##");
			iLogger->LogTrace("##    Parallel Computing | Intelligent Systems Design | HSHL   ##");
			iLogger->LogTrace("#################################################################");
			iLogger->LogTrace("");

			time(&StartingTime);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		iLogger->LogDebug("## Process launched. ##");
		MPI_Barrier(MPI_COMM_WORLD);

		//Default Values
		int CommonSize = 10;
		int ResultWidth = 10;
		int ResultHeight = 10;

		//Check for input parameters
		//Additional checks to be sure they are not zero or in a format that can not be converted to int
		if (3 < argc)
		{
			int tmpCommonSize = atoi(argv[3]);
			if (0 != tmpCommonSize)
			{
				CommonSize = tmpCommonSize;
			}

			if (4 < argc)
			{
				int tmpResultWidth = atoi(argv[4]);
				if (0 != tmpResultWidth)
				{
					ResultWidth = tmpResultWidth;
				}

				if (5 < argc)
				{
					int tmpResultHeight = atoi(argv[5]);
					if (0 != tmpResultHeight)
					{
						ResultHeight = tmpResultHeight;
					}
				}
			}
		}
		
		PRandMatrixMultiplicator mainThing;

		mainThing.doTheWholeThing(mSeed, CommonSize, ResultWidth, ResultHeight);

		MPI_Barrier(MPI_COMM_WORLD);
		iLogger->LogDebug("## Process terminated. ##");
		MPI_Barrier(MPI_COMM_WORLD);

		if (0 == rank)
		{
			time_t EndingTime;
			time(&EndingTime);

			int seconds = static_cast<int>(difftime(EndingTime, StartingTime));

			iLogger->LogTrace("");
			iLogger->LogTrace("Needed " + to_string(seconds) + " seconds for whole computation.");
			iLogger->LogTrace("");
		}
	}
	
	if (0 == rank)
	{
		iLogger->printAnalysis();
		iLogger->LogTrace("");
	}

	iLogger->deleteLogger();

	MPI_Finalize();
}