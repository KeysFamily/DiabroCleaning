//?------------------------------------------------------
//�^�X�N��:�}�b�v�J�ڃG�t�F�N�g
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_MapTransition.h"

namespace  MapTransition
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imgV = DG::Image::Create("./data/map/image/transition_V.png");
		imgH = DG::Image::Create("./data/map/image/transition_H.png");
		imgVSize.Set(1920,2360);
		imgHSize.Set(3712,1080);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		imgV.reset();
		imgH.reset();
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
		this->pos = ge->GetScreenCenter();
		this->moveVec = ML::Vec2(0, 0);
		this->moveSpeed = 300;
		this->motion = Motion::Non;
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
		switch (this->motion)
		{
		case Motion::Non:
			break;
		case Motion::Stop:
			break;
		case Motion::Appear:
			this->pos += this->moveVec;
			{
				bool passedCenter = false;
				switch (this->tDir)
				{
				case Map::MapDir::Up:
					passedCenter = this->pos.y < ge->GetScreenCenter().y;
					break;
				case Map::MapDir::Down:
					passedCenter = this->pos.y > ge->GetScreenCenter().y;
					break;
				case Map::MapDir::Right:
					passedCenter = this->pos.x > ge->GetScreenCenter().x;
					break;
				case Map::MapDir::Left:
					passedCenter = this->pos.x < ge->GetScreenCenter().x;
					break;
				}
				if (passedCenter)
				{
					this->pos = ge->GetScreenCenter();
					this->motion = Motion::Stop;
				}
			}
			break;
		case Motion::Disappear:
			this->pos += this->moveVec;

			ML::Box2D imgHit(0, 0, 0, 0);
			switch (this->tDir)
			{
			case Map::MapDir::Up:
			case Map::MapDir::Down:
				imgHit = OL::setBoxCenter(this->res->imgVSize);
				break;
			case Map::MapDir::Right:
			case Map::MapDir::Left:
				imgHit = OL::setBoxCenter(this->res->imgHSize);
				break;
			}
			imgHit.Offset(this->pos);

			if (!imgHit.Hit(ge->GetScreenBox()))
			{
				this->Kill();
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->motion == Motion::Non)
		{
			return;
		}

		ML::Box2D draw(0, 0, 0, 0);
		ML::Box2D src(0, 0, 0, 0);
		DG::Image::SP img;

		switch (this->tDir)
		{
		case Map::MapDir::Up:
		case Map::MapDir::Down:
			draw = OL::setBoxCenter(this->res->imgVSize);
			src.w = this->res->imgVSize.w;
			src.h = this->res->imgVSize.h;
			img = this->res->imgV;
			break;
		case Map::MapDir::Right:
		case Map::MapDir::Left:
			draw = OL::setBoxCenter(this->res->imgHSize);
			src.w = this->res->imgHSize.w;
			src.h = this->res->imgHSize.h;
			img = this->res->imgH;
			break;
		}

		draw.Offset(this->pos);
		img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//���̑����\�b�h
	//�g�����W�V�����o��
	void Object::Appear(const Map::MapDir& tDir_)
	{
		switch (tDir_)
		{
		case Map::MapDir::Up:
			this->pos.y += this->res->imgVSize.h;
			this->moveVec.y = -this->moveSpeed;
			break;
		case Map::MapDir::Down:
			this->pos.y -= this->res->imgVSize.h;
			this->moveVec.y = this->moveSpeed;
			break;
		case Map::MapDir::Right:
			this->pos.x -= this->res->imgHSize.w;
			this->moveVec.x = this->moveSpeed;
			break;
		case Map::MapDir::Left:
			this->pos.x += this->res->imgHSize.w;
			this->moveVec.x = -this->moveSpeed;
			break;

		}
		this->tDir = tDir_;
		this->motion = Motion::Appear;
	}
	//�g�����W�V�����ޏ�
	void Object::Disappear()
	{
		this->motion = Motion::Disappear;
	}
	//�o�����I��������
	bool Object::CheckFinishedAppear()
	{
		return this->motion == Motion::Stop;
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