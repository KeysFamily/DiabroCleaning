#pragma once
#include <string>	

class GameFileManager
{
	std::string runFolderPath;

public:
	GameFileManager()
		:runFolderPath("")
	{}

	//Run�t�H���_��u���ꏊ��ݒ肷��
	void SetRunPath(const std::string& folderPath_, bool init_);

	//Run�t�H���_�܂ł̃p�X���󂯎��
	std::string GetRunFile() const
	{
		return runFolderPath;
	}
};