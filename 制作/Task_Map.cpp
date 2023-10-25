//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map.h"
#include  "Task_Player.h"

namespace  Map
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/map/map96.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img.reset();
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
		auto result = this->LoadMap("map_start");
		this->testCam = ML::Vec2(0, 0);
		//���^�X�N�̐���
		Player::Object::Create(true);

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
		auto inp = ge->in1->GetState();
		int camSpeed = 10;
		if (inp.LStick.BU.on == true)
		{
			this->testCam.y -= camSpeed;
		}
		if (inp.LStick.BD.on == true)
		{
			this->testCam.y += camSpeed;
		}
		if (inp.LStick.BL.on == true)
		{
			this->testCam.x -= camSpeed;
		}
		if (inp.LStick.BR.on == true)
		{
			this->testCam.x += camSpeed;
		}

	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		int chipsize = 96;
		int drawSize = 64;
		int srcturn = 64;

		for (auto& layer : this->drawMap)
		{
			for (int y = 0; y < layer.height; ++y)
			{
				for (int x = 0; x < layer.width; ++x)
				{
					ML::Box2D draw(drawSize * x, drawSize * y, drawSize, drawSize);
					draw.Offset(-this->testCam);
					ML::Box2D src(
						chipsize * (layer.chipdata[y][x] % srcturn),
						chipsize * (layer.chipdata[y][x] / srcturn),
						chipsize,
						chipsize);

					this->res->img->Draw(draw, src);
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//���̑��̃��\�b�h

	//�}�b�v�ǂݍ���
	bool Object::LoadMap(const string& mapName_)
	{
		//�`��}�b�v�ǂݍ���
		int layerNum = 0;
		while (layerNum < 10)
		{
			ifstream ifs("./data/map/" + mapName_ + "/" + mapName_ + "_R" + to_string(layerNum) + ".csv");
			if (!ifs)
			{
				break;
			}

			this->drawMap.push_back(MapData());

			//�ǂݍ���
			ifs >> this->drawMap[layerNum].width;
			ifs >> this->drawMap[layerNum].height;
			for (int y = 0; y < drawMap[layerNum].height; ++y)
			{
				this->drawMap[layerNum].chipdata.push_back(vector<int>());
				for (int x = 0; x < drawMap[layerNum].width; ++x)
				{
					this->drawMap[layerNum].chipdata[y].push_back(0);
					ifs >> this->drawMap[layerNum].chipdata[y][x];
				}
			}

			++layerNum;
		}

		if (drawMap.empty())
		{
			return false;
		}

		//�I�u�W�F�N�g�}�b�v�ǂݍ���
		ifstream ifs("./data/map/" + mapName_ + "/" + mapName_ + "_H.csv");
		if (!ifs)
		{
			return false;
		}
		//�ǂݍ���
		ifs >> this->ObjectMap.width;
		ifs >> this->ObjectMap.height;
		for (int y = 0; y < ObjectMap.height; ++y)
		{
			this->ObjectMap.chipdata.push_back(vector<int>());
			for (int x = 0; x < ObjectMap.width; ++x)
			{
				this->ObjectMap.chipdata[y].push_back(0);
				ifs >> this->ObjectMap.chipdata[y][x];
			}
		}
		
		return true;
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
}