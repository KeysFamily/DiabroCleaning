#include "GameFileManager.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <direct.h>
#include <windows.h>

using namespace std;

void GameFileManager::SetRunPath(const string& folderPath_, bool init_)
{
	
	this->runFolderPath = folderPath_ + "/run/";

	_mkdir((folderPath_ + "/run").c_str());
	_mkdir((runFolderPath + "mapData").c_str());


	if (init_)
	{
		CopyFile("./data/inGame/template/pData_skill.json", (runFolderPath + "pData_skill.json").c_str(), false);
		CopyFile("./data/inGame/template/pData_status.json", (runFolderPath + "pData_status.json").c_str(), false);
	}
	else
	{
		ifstream skill(runFolderPath + "pData_skill.json");
		ifstream status(runFolderPath + "pData_status.json");
		if (!skill || !status)
		{
			CopyFile("./data/inGame/template/pData_skill.json", (runFolderPath + "pData_skill.json").c_str(), false);
			CopyFile("./data/inGame/template/pData_status.json", (runFolderPath + "pData_status.json").c_str(), false);
		}
		skill.close();
		status.close();
	}
}