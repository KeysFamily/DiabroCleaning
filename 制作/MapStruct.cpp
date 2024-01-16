#include "MapStruct.h"	
#include "nlohmann/json.hpp"
#include "OriginalLibrary.h"
#include <direct.h>

using namespace std;
using json = nlohmann::json;

namespace Map
{
	int MapObject::mapIdManager = 0;

	void MapObject::GenerateFile(const string& savePath_)
	{
		//フォルダ作成
		this->folderPath = savePath_ + "mapId_" + to_string(mapId);
		mkdir(this->folderPath.c_str());

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
		js["depth"] = this->depth;
		js["enterDir"] = (int)this->enter;
		js["exitDir"] = (int)this->exit;
		js["exitSubDir"] = (int)this->exitSub;
		js["mapName"] = this->mapName;
		js["isSub"] = this->isSub;
		OL::SaveJsonFile(js, folderPath + "/mapData.json");
	}

}