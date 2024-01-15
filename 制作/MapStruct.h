#pragma once
#include <string>


namespace Map
{
	enum class MapDir
	{
		Non = -1,
		Up = 0,
		Down,
		Right,
		Left
	};

	enum class MapType
	{
		Empty = 0,
		Map,
		Connect,
		Other,
	};

	class MapFunc 
	{
	public:
		static MapDir ReverseMapDir(const MapDir& mapDirection_)
		{
			switch (mapDirection_)
			{
			case MapDir::Non:
				return MapDir::Non;
				break;
			case MapDir::Up:
				return MapDir::Down;
				break;
			case MapDir::Down:
				return MapDir::Up;
				break;
			case MapDir::Right:
				return MapDir::Left;
				break;
			case MapDir::Left:
				return MapDir::Right;
				break;
			}
			return MapDir::Non;
		}

	};


	class MapObject
	{
		static int mapKeyManager;	//ユニークキー生成用
		const int mapKey;			//ユニークキー
		std::string folderPath;		//フォルダへのパス
		Map::MapDir enter;			//入り口
		Map::MapDir exit;			//出口
		Map::MapDir exitSub;		//2つ目の出口
		MapType mapType;			//マップの種類

		//マップ生成
		void GenerateFile(int depth_, const std::string& mapName_ = "");
	public:

		//通常マップや通路とは違うマップを作るときのコンストラクタ
		MapObject(int depth_, const std::string& mapName_ = "")
			: mapKey(mapKeyManager)
			, folderPath("")
			, mapType(MapType::Other)
			, enter(Map::MapDir::Non)
			, exit(Map::MapDir::Non)
			, exitSub(Map::MapDir::Non)
		{
			this->GenerateFile(depth_, mapName_);
			++mapKeyManager;
		}

		//通常のコンストラクタ
		MapObject(int depth_, MapType mapType_, Map::MapDir enter_, Map::MapDir exit_, Map::MapDir exitSub_ = Map::MapDir::Non)
			: mapKey(mapKeyManager)
			, folderPath("")
			, mapType(mapType_)
			, enter(enter_)
			, exit(exit_)
			, exitSub(exitSub_)

		{
			this->GenerateFile(depth_);
			++mapKeyManager;
		}


		Map::MapDir GetEnter() { return enter; }
		Map::MapDir GetExit() { return exit; }
		Map::MapDir GetExitSub() { return exitSub; }
		std::string GetFolderPath() { return folderPath; }
		Map::MapType GetMapType() { return mapType; }

	private:

		//ファイル名設定
		std::string SetMapName_Map()
		{
			std::string genMapName = "map_";
			std::string mapDirStr[4] = { "Up", "Down", "Right", "Left" };

			genMapName += mapDirStr[(int)enter];
			if (exit == Map::MapDir::Non)
			{
				genMapName += mapDirStr[(int)enter];
			}
			else
			{
				genMapName += mapDirStr[(int)exit];
			}

			genMapName += "_" + std::to_string(rand() % 3 + 1);

			return genMapName;
		}
		std::string SetMapName_Connect()
		{
			std::string genMapName = "pass_";
			std::string mapDirStr[4] = { "Up", "Down", "Right", "Left" };

			genMapName += mapDirStr[(int)enter];

			if (exitSub != Map::MapDir::Non)
			{
				if (exit == Map::MapDir::Down)
				{
					genMapName += mapDirStr[(int)exitSub] + mapDirStr[(int)exit];
				}
				else
				{
					genMapName += mapDirStr[(int)exit] + mapDirStr[(int)exitSub];
				}
			}
			else
			{
				genMapName += mapDirStr[(int)exit];
			}

			return genMapName;
		}
	};
}