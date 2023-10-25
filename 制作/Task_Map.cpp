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
		auto  pl = Player::Object::Create(true);
		pl->pos.x = ge->screen2DWidth / 2;
		pl->pos.y = ge->screen2DHeight * 2 / 3;

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
	//-------------------------------------------------------------------
	//�����蔻��
	bool  Object::CheckHit(const  ML::Box2D& hit_)
	{
		ML::Rect  r = { hit_.x, hit_.y, hit_.x + hit_.w, hit_.y + hit_.h };
		//��`���}�b�v�O�ɏo�Ă�����T�C�Y��ύX����
		ML::Rect  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h
		};
		if (r.left < m.left) { r.left = m.left; }
		if (r.top < m.top) { r.top = m.top; }
		if (r.right > m.right) { r.right = m.right; }
		if (r.bottom > m.bottom) { r.bottom = m.bottom; }

		//���[�v�͈͒���
		int sx, sy, ex, ey;
		sx = r.left / 32;
		sy = r.top / 32;
		ex = (r.right - 1) / 32;
		ey = (r.bottom - 1) / 32;

		//�͈͓��̏�Q����T��
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				if (8 <= this->arr[y][x]) {
					return true;
				}
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu��������
	void  Object::AdjustCameraPos()
	{
		//�J�����ƃ}�b�v�͈̔͂�p��
		ML::Rect  c = {
			ge->camera2D.x,
			ge->camera2D.y,
			ge->camera2D.x + ge->camera2D.w,
			ge->camera2D.y + ge->camera2D.h };
		ML::Rect  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h };

		//�J�����̈ʒu�𒲐�
		if (c.right > m.right) { ge->camera2D.x = m.right - ge->camera2D.w; }
		if (c.bottom > m.bottom) { ge->camera2D.y = m.bottom - ge->camera2D.h; }
		if (c.left < m.left) { ge->camera2D.x = m.left; }
		if (c.top < m.top) { ge->camera2D.y = m.top; }
		//�}�b�v���J������菬�����ꍇ
		if (this->hitBase.w < ge->camera2D.w) { ge->camera2D.x = m.left; }
		if (this->hitBase.h < ge->camera2D.h) { ge->camera2D.y = m.top; }
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