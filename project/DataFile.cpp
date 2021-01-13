#include "stdafx.h"
#include "DataFile.h"

#include <fstream>
//#include "Exam_HelperStructs.h"
#include <filesystem>

DataFile::DataFile()
		: m_PathProject{}
		, m_PathBuild{}
		, m_PathActive{}
{
}

DataFile::~DataFile()
{
}

void DataFile::Initiaze()
{
	m_PathProject = "./Resources/brainData.txt";
	m_PathBuild = "./Resources/brainData.txt";
	m_PathActive = m_PathProject;
}

bool DataFile::SaveBrain()
{
	//namespace fs = std::experimental::filesystem;
	//bool exists = std::filesystem::exists(m_PathProject);

	ofstream myfile;
	myfile.open(m_PathActive, std::ios::in);
	if (!myfile.good())
	{
		std::cout << "Cannot open file, file does not exist. Creating new file..";

		myfile.open(m_PathProject, fstream::in | fstream::out | fstream::trunc);
		myfile << "File created! \n\n\n\n";
	}

	if (myfile.good())
	{
		//SaveData(myfile, BrainType::INPUTS);
		//myfile << "\n\n\n\n";
		//SaveData(myfile, BrainType::OUTPUTS);
		//myfile << "\n\n\n\n";
		SaveData(myfile, Brain::BrainType::BRAIN_ENEMY);

		myfile << "Brain saved!";
		std::cout << "Brian saved \n";
	}

	myfile.close();

	return false;
}

bool DataFile::LoadBrain()
{
	std::ifstream in(m_PathProject, std::ios_base::app);
	if (!in)
	{
		std::cout << ("Model \"" + m_PathProject + "\" does not exist.");
		in.close();
		return false;
	}

	bool StateMatrixArrDone{ false };
	bool ActionMatrixArrDone{ false };
	std::string line;
	int rows{};
	int columns{};
	int columnCounter{};
	int rowCounter{};

	while (getline(in, line))
	{
		if (line.substr(0, 6) == "Rows: ")
		{
			std::istringstream s{ line.substr(6) };
			std::string ss{};
			s >> ss;
			rows = std::stoi(ss);
			rowCounter = 0;
		}
		if (line.substr(0, 6) == "Cols: ")
		{
			std::istringstream s{ line.substr(6) };
			std::string ss{};
			s >> ss;
			columns = std::stoi(ss);
			columnCounter = 0;
		}
		if (line.substr(0, 9) == "B-Enemy: ")
		{
			std::istringstream s{ line.substr(9) };
			char space{};
			std::string ss{};
			while (s >> ss)
			{
				//m_BotBrainEnemy->Set(rowCounter, columnCounter, std::stof(ss));
				if (rowCounter + 1 < rows && columnCounter + 1 == columns)
				{
					rowCounter++;
					columnCounter = 0;
				}
				else
				{
					if (columnCounter + 1 < columns)
						columnCounter++;
				}
			}
		}
	}
	in.close();

	//m_BotBrainEnemy->Print();

	return false;
}

void DataFile::SaveData(ofstream& myfile, const Brain::BrainType bType)
{
	//FMatrix* matrixArrToSave{ nullptr };
	//std::string identifier{};
	////if (bType == BrainType::INPUTS)
	//{
	//	myfile << "-------------------------- \n";
	//	myfile << "State Matrix Memory Array  \n";
	//	myfile << "-------------------------- \n";

	//	matrixArrToSave = m_pInputMatrixMemoryArr;

	//	identifier = "States: ";
	//}
	////else if (bType == BrainType::OUTPUTS)
	//{
	//	myfile << "-------------------------- \n";
	//	myfile << "Action Matrix Memory Array \n";
	//	myfile << "-------------------------- \n";

	//	//matrixArrToSave = m_pActionMatrixMemoryArr;

	//	identifier = "Actions: ";
	//}
	////else if (bType == BrainType::BRAIN_ENEMY)
	//{
	//	myfile << "-------------------------- \n";
	//	myfile << "Bot brain                  \n";
	//	myfile << "-------------------------- \n";

	//	matrixArrToSave = m_BotBrainEnemy;

	//	identifier = "B-Enemy: ";
	//}

	///* Loop over all the arrays */
	//for (int array{}; array < m_MemSize; ++array)
	//{
	//	bool isMatrixArr{ (bType != BrainType::BRAIN_ENEMY) };

	//	int nmrOfRows{ (isMatrixArr) ? matrixArrToSave[array].GetNrOfRows() : matrixArrToSave->GetNrOfRows() };
	//	int nmrOfColumns{ (isMatrixArr) ? matrixArrToSave[array].GetNrOfColumns() : matrixArrToSave->GetNrOfColumns() };

	//	myfile << std::fixed << setprecision(2) << "Array #: " << array << '\n';
	//	myfile << std::fixed << setprecision(2) << "Rows: " << nmrOfRows << '\n';
	//	myfile << std::fixed << setprecision(2) << "Cols: " << nmrOfColumns << '\n';

	//	/* Loop over all the rows in the array */
	//	for (int row{}; row < nmrOfRows; ++row)
	//	{
	//		myfile << identifier;
	//		/* Loop over all the columns in the array */
	//		for (int column{}; column < nmrOfColumns; ++column)
	//		{
	//			const float arrValue{ matrixArrToSave[array].Get(row, column) };
	//			myfile << std::fixed << setprecision(10) << arrValue << " ";
	//		}
	//		myfile << '\n';
	//	}

	//	if (!isMatrixArr)
	//		break;
	//}
}
