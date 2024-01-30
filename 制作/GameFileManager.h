#pragma once
#include <string>	

class GameFileManager
{
	std::string runFolderPath;

public:
	GameFileManager()
		:runFolderPath("")
	{}

	//Runフォルダを置く場所を設定する
	void SetRunPath(const std::string& folderPath_, bool init_);

	//Runフォルダまでのパスを受け取る
	std::string GetRunFile() const
	{
		return runFolderPath;
	}
};