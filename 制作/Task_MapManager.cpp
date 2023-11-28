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
		srand(1701140554);
		ge->printToDebugFile(to_string(mapSeed),1);
		this->Generate();



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
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
	}
	//-------------------------------------------------------------------
	//���̑��̃��\�b�h
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
		map[0][1] = new Object::Connect(MapEnter::Left, MapExit::Right);
		this->GenerateMap(2,0, 2, 6, MapEnter::Left);

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

	void Object::GenerateMap(int x_, int y_, int depth_, int depthRest_, MapEnter enter_)
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
			map[y_][x_] = new Map(enter_, MapExit::Non, depth_);
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
			map[y_][x_] = new Map(enter_, MapExit::Non, depth_);
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
		MapEnter enterDir;
		MapExit exitDir;

		MapEnter connectEnter = MapEnter::Up;
		MapExit connectExit = MapExit::Right;
		MapExit connectExitSub = MapExit::Non;

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
				enterDir = MapEnter::Left;
				connectEnter = MapEnter::Left;
				exitDir = MapExit::Right;
				connectExit = MapExit::Right;
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
				enterDir = MapEnter::Up;
				connectEnter = MapEnter::Up;
				exitDir = MapExit::Down;
				connectExit = MapExit::Down;
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
				enterDir = MapEnter::Up;
				connectEnter = MapEnter::Left;
				exitDir = MapExit::Right;
				connectExit = MapExit::Down;
				break;
			}


			//���̈ʒu�𐶐�
			GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);

			//���򂪗L���Ȃ�΂߂�����
			if (generateSub)
			{
				genX = 1;
				genY = 1;
				if (enterDir == MapEnter::Up)
				{
					enterDir = MapEnter::Left;
					connectExitSub = MapExit::Right;
				}
				else
				{
					enterDir = MapEnter::Up;
					connectExitSub = MapExit::Down;
				}
				GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
			}
			map[y_][x_] = new Map(enter_, exitDir, depth_);
			map[y_ + conY][x_ + conX] = new Connect(connectEnter, connectExit, connectExitSub);

			finishedGenerate = false;
		}
	}




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
