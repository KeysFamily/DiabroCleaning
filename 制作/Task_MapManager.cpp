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
		srand(mapSeed);
		this->Destroy();


		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

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
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				map[y][x] = nullptr;
			}
		}

		map[0][0] = new MapObject("map_start");
		map[0][1] = new Object::Connect(MapEnter::Left, MapExit::Right);
		

	}

	void Object::GenerateMap(int x_, int y_, int depth_, int depthRest_, MapEnter enter_)
	{
		enum GenerateDir
		{
			Right = 0,
			Down,
			RightDown,
		};

		int generatePos = rand() % 3;
		bool generateSub = rand() % 2;

		if (generateSub && generatePos == 3)
		{
			--generatePos;
		}


		int genX;
		int genY;
		MapEnter enterDir;
		MapExit exitDir;
		
		switch (generatePos)
		{
		case GenerateDir::Right:
			genX = 2;
			genY = 0;
			enterDir = MapEnter::Left;
			exitDir = MapExit::Right;
			break;
		case GenerateDir::Down:
			genX = 0;
			genY = 2;
			enterDir = MapEnter::Up;
			exitDir = MapExit::Down;
			break;
		case GenerateDir::RightDown:
			genX = 1;
			genY = 1;
			enterDir = MapEnter::Up;
			exitDir = MapExit::Right;
			break;
		}
		
		GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
		if (generateSub)
		{
			genX = 1;
			genY = 1;
			if (enterDir == MapEnter::Up)
			{
				enterDir = MapEnter::Left;
			}
			else
			{
				enterDir = MapEnter::Up;
			}
			GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
		}


		map[y_][x_] = new Map(enter_, exitDir, depth_);
	}




	//���Ŏ��̏���
	void Object::Destroy()
	{
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
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
