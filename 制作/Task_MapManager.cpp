//?------------------------------------------------------
//�^�X�N��:�}�b�v�Ǘ�
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_MapManager.h"
#include  "Task_MapTransition.h"
#include  "Task_GameUI_MiniMap.h"
#include  "Task_Sprite.h"

using namespace Map;

namespace  MapManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		//�V�[�h�l�ݒ�
		this->mapSeed = (unsigned int)time(NULL);
		srand(mapSeed);
		ge->printToDebugFile(to_string(mapSeed),1);
		//����m��
		this->generateSubRate = 0.5f;
		this->subDepthMax = 2;
		this->depthMax = 5;
		this->depthInLevel = 1;
		//�Z�[�u��
		this->saveFolderPath = ge->filemg.GetRunFile() + "mapData/";

		this->moveMapDir = Map::MapDir::Non;

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		this->Destroy();


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		MoveMapUpDate();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
	}
	//-------------------------------------------------------------------
	//���̑��̃��\�b�h
	//�����S��
	bool Object::Generate()
	{
		if (generated == true)
		{
			return false;
		}

		for (int y = 0; y < Map::MAPSIZE_MAX; ++y)
		{
			for (int x = 0; x < Map::MAPSIZE_MAX; ++x)
			{
				map[y][x] = nullptr;
			}
		}

		map[0][0] = new MapObject(0,"map_start");

#if false
		map[0][1] = new MapObject(1, "map_goal");
#else
		map[0][1] = new MapObject(0, MapType::Connect, MapDir::Left, MapDir::Right);
#endif
		this->GenerateMap(2, 0, 1, this->depthMax, MapDir::Left);

		this->GenerateSub();



		//�_���W��������
		ofstream ofs(this->saveFolderPath + "dungeonData.txt");
		ofs << Map::MAPSIZE_MAX << endl;
		for (int y = 0; y < Map::MAPSIZE_MAX; ++y)
		{
			for (int x = 0; x < Map::MAPSIZE_MAX; ++x)
			{
				if (map[y][x] != nullptr)
				{
					map[y][x]->GenerateFile(this->saveFolderPath);
					mapid[y][x] = map[y][x]->GetId();
					ofs << map[y][x]->GetId() << ' ';

					
				}
				else
				{
					ofs << -1 << ' ';
				}
			}
			ofs << endl;
		}

		ofs.close();

		this->Destroy();

		if (minimap == nullptr) 
		{
			this->minimap = MiniMap::Object::Create(true);
		}
		this->minimap->LoadData(this->saveFolderPath);

		ge->qa_Map = Map::Object::Create(true);
		ge->qa_Map->render2D_Priority[1] = 0.9f;
		ge->qa_Map->depthInLevel = this->depthInLevel;
		ge->qa_Map->LoadMap(this->saveFolderPath + "mapId_" + to_string(this->mapid[currentPos.y][currentPos.x]));
		this->generated = true;

		return true;
	}
	//-------------------------------------------------------------------
	//1�}�b�v��������
	void Object::GenerateMap(int x_, int y_, int depth_, int depthRest_, MapDir enter_, bool setSub_)
	{
		enum GenerateDir
		{
			Right = 0,
			Down,
			RightDown,
		};

		//�ŉ��w�Ȃ玟�̐��������͍s��Ȃ�
		if (depthRest_ <= 1)
		{
			//�O��̓��łȂ���΃S�[��
			if (setSub_ == false)
			{
				map[y_][x_] = new MapObject(depth_, "map_goal");
				return;
			}
			else
			{
				map[y_][x_] = new MapObject(depth_, MapType::Map, enter_, MapDir::Non);
				map[y_][x_]->SetSub(true);
				return;
			}
		}

		//�����s�ȏꏊ��T���i������������Ƀ}�b�v�E�������͒ʘH�����邩����j
		int cantGeneratesTotal = 0;
		cantGeneratesTotal += map[y_][x_ + 2] != nullptr || map[y_][x_ + 1] != nullptr;
		cantGeneratesTotal += map[y_ + 1][x_ + 1] != nullptr || map[y_ + 1][x_] != nullptr && map[y_][x_ + 1] != nullptr;
		cantGeneratesTotal += map[y_ + 2][x_] != nullptr || map[y_ + 1][x_] != nullptr;

		//�����\�ȏꏊ���Ȃ���Ύ��̐��������͍s��Ȃ�
		if (cantGeneratesTotal >= 3)
		{
			map[y_][x_] = new MapObject(depth_, MapType::Map, enter_, MapDir::Non);
			return;
		}

		//�����̎�ށi�E�E���E�΂߁j
		int generateTypes = 3;

		int genX = 0;
		int genY = 0;
		int conX = 0;
		int conY = 0;
		Map::MapDir enterDir;
		Map::MapDir exitDir;

		Map::MapDir connectEnter = Map::MapDir::Up;
		Map::MapDir connectExit = Map::MapDir::Right;
		Map::MapDir connectExitSub = Map::MapDir::Non;

		//��������������܂Ń��[�v����
		bool finishedGenerate = true;
		while (finishedGenerate)
		{
			//�����̕����������_���Ō���
			int generatePos = rand() % generateTypes;

			//�����̕����ɉ����Ēl�𒲐�
			switch (generatePos)
			{
			case GenerateDir::Right:
				if (map[y_][x_ + 2] != nullptr || map[y_][x_ + 1] != nullptr)
				{
					continue;
				}
				genX = 2;
				genY = 0;
				conX = 1;
				conY = 0;
				enterDir = Map::MapDir::Left;
				connectEnter = Map::MapDir::Left;
				exitDir = Map::MapDir::Right;
				connectExit = Map::MapDir::Right;
				break;
			case GenerateDir::Down:
				if (map[y_ + 2][x_] != nullptr || map[y_ + 1][x_] != nullptr)
				{
					continue;
				}
				genX = 0;
				genY = 2;
				conX = 0;
				conY = 1;
				enterDir = Map::MapDir::Up;
				connectEnter = Map::MapDir::Up;
				exitDir = Map::MapDir::Down;
				connectExit = Map::MapDir::Down;
				break;
			case GenerateDir::RightDown:
				if (map[y_ + 1][x_ + 1] != nullptr || map[y_ + 1][x_] != nullptr && map[y_][x_ + 1] != nullptr)
				{
					continue;
				}
				genX = 1;
				genY = 1;
				conX = 1;
				conY = 0;
				enterDir = Map::MapDir::Up;
				connectEnter = Map::MapDir::Left;
				exitDir = Map::MapDir::Right;
				connectExit = Map::MapDir::Down;
				break;
			}


			//���̈ʒu�𐶐�
			GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir, setSub_);

			map[y_][x_] = new MapObject(depth_, MapType::Map, enter_, exitDir);
			map[y_][x_]->SetSub(setSub_);
			map[y_ + conY][x_ + conX] = new MapObject(depth_, MapType::Connect, connectEnter, connectExit, connectExitSub);
			map[y_ + conY][x_ + conX]->SetSub(setSub_);

			finishedGenerate = false;
		}
	}
	//-------------------------------------------------------------------
	//����𐶐�
	void Object::GenerateSub()
	{
		for (int y = 0; y < Map::MAPSIZE_MAX; ++y)
		{
			for (int x = 0; x < Map::MAPSIZE_MAX; ++x)
			{
				if (this->GetSubFlag(x, y) == true)
				{
					if (map[y][x]->GetExit() == MapDir::Right)
					{
						if (y + 1 < Map::MAPSIZE_MAX)
						{
							if (map[y + 1][x] == nullptr)
							{
								map[y][x]->SetExitSub(MapDir::Down);
								int subDepth = (rand() % this->subDepthMax) + 1;
								GenerateMap(x, y + 1, map[y][x]->GetDepth() + 1, subDepth, MapDir::Up, true);
							}
							else
							{
								continue;
							}
						}
					}
					else if (map[y][x]->GetExit() == MapDir::Down)
					{
						if (x + 1 < Map::MAPSIZE_MAX)
						{
							if (map[y][x + 1] == nullptr)
							{
								map[y][x]->SetExitSub(MapDir::Right);
								int subDepth = (rand() % this->subDepthMax) + 1;
								GenerateMap(x + 1, y, map[y][x]->GetDepth() + 1, subDepth, MapDir::Left, true);
							}
							else
							{
								continue;
							}
						}
					}
				}
			}
		}
	}
	//����𐶐����邩�̃t���O���擾����
	bool Object::GetSubFlag(int connX_, int connY_)
	{
		if (map[connY_][connX_] != nullptr)
		{
			if (map[connY_][connX_]->GetMapType() == MapType::Connect)
			{
				if (map[connY_][connX_]->GetSub() == false)
				{
					if (OL::RandomBool(this->generateSubRate) == true)
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//���[�h
	void Object::MoveMap(const Map::MapDir& mapDirection_)
	{
		if (this->moveMapDir != Map::MapDir::Non)
		{
			return;
		}

		this->moveMapDir = mapDirection_;
		this->mapTransition = MapTransition::Object::Create(true);
		this->mapTransition->Appear(Map::MapFunc::ReverseMapDir(mapDirection_));
	}

	void Object::MoveMapUpDate()
	{
		if (this->moveMapDir == Map::MapDir::Non)
		{
			return;
		}

		if (this->mapTransition == nullptr)
		{
			return;
		}



		if (this->mapTransition->CheckFinishedAppear() == false)
		{
			return;
		}

		//�G�̏��ŏ����i���j
		ge->KillAll_G("enemy");

		switch (this->moveMapDir)
		{
		case Map::MapDir::Up:
			--currentPos.y;
			break;
		case Map::MapDir::Down:
			++currentPos.y;
			break;
		case Map::MapDir::Left:
			--currentPos.x;
			break;
		case Map::MapDir::Right:
			++currentPos.x;
			break;
		}

		ge->qa_Map->LoadMap(this->saveFolderPath + "mapId_" + to_string(this->mapid[currentPos.y][currentPos.x]));
		ge->qa_Player->pos = ge->qa_Map->GetPlayerEnterPos(Map::MapFunc::ReverseMapDir(moveMapDir));
		if (moveMapDir == MapDir::Up)
		{
			ge->qa_Player->motion = Player::Object::Motion::Jump;
			ge->qa_Player->moveCnt = -1;
		}
		auto camera = ge->GetTask<Sprite::Object>("Sprite");
		camera->MoveImmediately();
		this->mapTransition->Disappear();
		this->minimap->SetVisit(this->currentPos.x, this->currentPos.y);

		this->moveMapDir = Map::MapDir::Non;
		this->mapTransition = nullptr;
	}

	//-------------------------------------------------------------------
	//���Ŏ��̏���
	void Object::Destroy()
	{
		for (int y = 0; y < Map::MAPSIZE_MAX; ++y)
		{
			for (int x = 0; x < Map::MAPSIZE_MAX; ++x)
			{
				if (map[y][x] != nullptr)
				{
					delete map[y][x];
					map[y][x] = nullptr;
				}
			}
		}
	}

	//-------------------------------------------------------------------
	//�Z�b�^
	void Object::SetMaxDepth(int depth_)
	{
		this->depthMax = depth_;
	}
	void Object::SetDepthInLevel(int dil_)
	{
		this->depthInLevel = dil_;
	}
	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }

	//-------------------------------------------------------------------

}
