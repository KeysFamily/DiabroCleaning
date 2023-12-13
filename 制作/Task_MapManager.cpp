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
#include  "Task_Sprite.h"

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
		this->mapSeed = (unsigned int)time(NULL);
		srand(mapSeed);
		ge->printToDebugFile(to_string(mapSeed),1);
		this->Generate();

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
	void Object::Generate()
	{
		for (int y = 0; y < 30; ++y)
		{
			for (int x = 0; x < 30; ++x)
			{
				map[y][x] = nullptr;
			}
		}

		map[0][0] = new MapObject("map_start");

		map[0][1] = new Object::Connect(Map::MapDir::Left, Map::MapDir::Right);

		this->GenerateMap(2, 0, 2, 6, Map::MapDir::Left);
		//����
		for (int y = 0; y < 30; ++y)
		{
			for (int x = 0; x < 30; ++x)
			{
				if (map[y][x] != nullptr)
				{
					map[y][x]->Generate();
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//1�}�b�v��������
	void Object::GenerateMap(int x_, int y_, int depth_, int depthRest_, Map::MapDir enter_)
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
			map[y_][x_] = new Area(enter_, Map::MapDir::Non, depth_);
			return;
		}

		//�����s�ȏꏊ��T���i������������Ƀ}�b�v�E�������͒ʘH�����邩����j
		int cantGeneratesTotal = 0;
		cantGeneratesTotal += map[y_][x_ + 2] != nullptr || map[y_][x_ + 1] != nullptr;
		cantGeneratesTotal += map[y_ + 1][x_ + 1] != nullptr || map[y_ + 1][x_] != nullptr && map[y_][x_ + 1] != nullptr;
		cantGeneratesTotal += map[y_ + 2][x_] != nullptr || map[y_ + 1][x_] != nullptr;

		//�����\�ȏꏊ���Ȃ���Ύ��̐��������͍s��Ȃ�
		if (cantGeneratesTotal >= 3)
		{
			map[y_][x_] = new Area(enter_, Map::MapDir::Non, depth_);
			return;
		}

		//�����̎�ށi�E�E���E�΂߁j
		int generateTypes = 3;

		//���򂳂��邩
		bool generateSub = rand() % 2 && cantGeneratesTotal < 1 && map[y_ + 1][x_ + 1] == nullptr;
		//���򂪗L���Ȃ�A�E�������ŏ��ɐ����i�΂߂͊m��Ȃ̂Łj
		if (generateSub)
		{
			--generateTypes;
		}

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
			GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);

			//���򂪗L���Ȃ�΂߂�����
			if (generateSub)
			{
				genX = 1;
				genY = 1;
				if (enterDir == Map::MapDir::Up)
				{
					enterDir = Map::MapDir::Left;
					connectExitSub = Map::MapDir::Right;
				}
				else
				{
					enterDir = Map::MapDir::Up;
					connectExitSub = Map::MapDir::Down;
				}
				GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
			}
			map[y_][x_] = new Area(enter_, exitDir, depth_);
			map[y_ + conY][x_ + conX] = new Connect(connectEnter, connectExit, connectExitSub);

			finishedGenerate = false;
		}
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

		ge->qa_Map->LoadMap(map[currentPos.y][currentPos.x]->mapName);
		ge->qa_Player->pos = ge->qa_Map->GetPlayerEnterPos(Map::MapFunc::ReverseMapDir(moveMapDir));
		auto camera = ge->GetTask<Sprite::Object>("Sprite");
		camera->MoveImmediately();

		this->mapTransition->Disappear();

		this->moveMapDir = Map::MapDir::Non;
		this->mapTransition = nullptr;
	}

	//-------------------------------------------------------------------
	//���Ŏ��̏���
	void Object::Destroy()
	{
		for (int y = 0; y < 30; ++y)
		{
			for (int x = 0; x < 30; ++x)
			{
				delete map[y][x];
			}
		}
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
