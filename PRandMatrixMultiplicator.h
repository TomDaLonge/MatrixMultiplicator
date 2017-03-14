#pragma once

class Logger;

class PRandMatrixMultiplicator
{
public:
	PRandMatrixMultiplicator(void);
	~PRandMatrixMultiplicator(void);

	bool doTheWholeThing(int Seed, 
						 int CommonSize, 
						 int ResultWidth, 
						 int ResultHeight);

private:
	Logger* m_pLogger;
};

