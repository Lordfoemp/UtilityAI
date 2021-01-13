#pragma once
#include "Brain.h"

class DataFile
{
	public:
		DataFile();
		~DataFile();

		DataFile(const DataFile&) = delete;
		DataFile(DataFile&&) noexcept = delete;
		DataFile& operator=(const DataFile&) = delete;
		DataFile& operator=(DataFile&&) noexcept = delete;
		
		void Initiaze();

		bool SaveBrain();
		bool LoadBrain();
	
	private:
		std::string m_PathProject;
		std::string m_PathBuild;
		std::string m_PathActive;

		void SaveData(ofstream& myfile, const Brain::BrainType bType);
};

