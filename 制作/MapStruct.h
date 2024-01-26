#pragma once
#include <string>
namespace Map
{
	enum OtherMapData
	{
		MAPSIZE_MAX = 30,
	};
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

	//ダメージ床データ
	struct SpikeData
	{
		int damage;				//ダメージ
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
		static int mapIdManager;	//ユニークキー生成用
		const int mapId;			//ユニークキー
		std::string folderPath;		//フォルダへのパス
		std::string mapName;		//マップ名
		Map::MapDir enter;			//入り口
		Map::MapDir exit;			//出口
		Map::MapDir exitSub;		//2つ目の出口
		MapType mapType;			//マップの種類
		int		depth;				//深度
		bool	isSub;				//外れのマップか

	public:

		//通常マップや通路とは違うマップを作るときのコンストラクタ
		MapObject(int depth_, const std::string& mapName_ = "")
			: mapId(mapIdManager)
			, folderPath("")
			, mapName(mapName_)
			, mapType(MapType::Other)
			, enter(Map::MapDir::Non)
			, exit(Map::MapDir::Non)
			, exitSub(Map::MapDir::Non)
			, depth(depth_)
			, isSub(false)
		{
			++mapIdManager;
		}

		//通常のコンストラクタ
		MapObject(int depth_, MapType mapType_, Map::MapDir enter_, Map::MapDir exit_, Map::MapDir exitSub_ = Map::MapDir::Non)
			: mapId(mapIdManager)
			, folderPath("")
			,mapName("")
			, mapType(mapType_)
			, enter(enter_)
			, exit(exit_)
			, exitSub(exitSub_)
			, depth(depth_)
			, isSub(false)

		{
			++mapIdManager;
		}

		//ゲッタ
		int GetId() const{ return mapId; }
		Map::MapDir GetEnter() const { return enter; }
		Map::MapDir GetExit() const { return exit; }
		Map::MapDir GetExitSub() const { return exitSub; }
		int GetDepth() const { return depth; }
		bool GetSub() { return isSub; }
		std::string GetFolderPath() const { return folderPath; }
		Map::MapType GetMapType() const { return mapType; }
		//セッタ
		void SetEnter(Map::MapDir enter_)  { this->enter = enter_; }
		void SetExit(Map::MapDir exit_)  { exit = exit_; }
		void SetExitSub(Map::MapDir exitSub_)  { exitSub = exitSub_; }
		void SetSub(bool flag_) { isSub = flag_; }

		//マップ生成
		void GenerateFile(const std::string& savePath_);
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