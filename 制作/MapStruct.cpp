#include "MapStruct.h"	
#include "nlohmann/json.hpp"
#include "OriginalLibrary.h"
#include <direct.h>

using namespace std;
using json = nlohmann::json;

namespace Map
{
	int MapObject::mapKeyManager = 0;

	void MapObject::GenerateFile(int depth_, const string& mapName_)
	{
		//フォルダ作成
		this->folderPath = ("./data/inGame/run/mapData/mapId_" + to_string(mapKey));
		if (mkdir(this->folderPath.c_str()) != 0)
		{
			return;
		}

		string mapName = mapName_;
		//マップの名前を決める
		if (mapName == "")
		{
			switch (this->mapType)
			{
			case MapType::Map:
				mapName = this->SetMapName_Map();
				break;
			case MapType::Connect:
				mapName = this->SetMapName_Connect();
				break;
			}
		}


		json js = OL::LoadJsonFile("./data/inGame/template/mapData/mapData.json");
		js["visited"] = false;
		js["mapType"] = (int)this->mapType;
		js["depth"] = depth_;
		js["mapName"] = mapName_;
		OL::SaveJsonFile(js, folderPath + "/mapData.json");
	}

}