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
		static int mapKeyManager;	//���j�[�N�L�[�����p
		const int mapKey;			//���j�[�N�L�[
		std::string folderPath;		//�t�H���_�ւ̃p�X
		Map::MapDir enter;			//�����
		Map::MapDir exit;			//�o��
		Map::MapDir exitSub;		//2�ڂ̏o��
		MapType mapType;			//�}�b�v�̎��

		//�}�b�v����
		void GenerateFile(int depth_, const std::string& mapName_ = "");
	public:

		//�ʏ�}�b�v��ʘH�Ƃ͈Ⴄ�}�b�v�����Ƃ��̃R���X�g���N�^
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

		//�ʏ�̃R���X�g���N�^
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

		//�t�@�C�����ݒ�
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