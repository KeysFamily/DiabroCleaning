//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyHPBar.h"

namespace  EnemyHPBar
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/enemyHPBar.png");
		this->imgSize.Set(96, 15);
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
		this->render2D_Priority[1] = 0.2f;
		this->pos = ML::Vec2(0, 0);
		this->offset = ML::Vec2(0, 60);
		this->decleaseTime.SetValues(0, 0, 15);
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
		if (target.expired())
		{
			this->Kill();
			return;
		}

		const BChara::SP& en = target.lock();
		this->pos = en->pos + this->offset;

		if (hpDisplay != en->hp.vnow)
		{
			decleaseTime.vnow = decleaseTime.vmin;
			hpDecleased += hpDisplay - en->hp.vnow;
			hpDisplay = en->hp.vnow;
		}

		if (decleaseTime.IsMax() == false)
		{
			decleaseTime.Addval(1);
			if (decleaseTime.IsMax())
			{
				hpDecleased = 0;
			}
		}

	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (!target.expired())
		{
			auto en = target.lock();
			if (en->hp.IsMax() || en->hp.IsMin())
			{
				return;
			}

			//�g�Ɣw�i�̕`��
			ML::Box2D draw = OL::setBoxCenter(this->res->imgSize);
			draw.Offset(this->pos);
			draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
			ML::Box2D src(0, 0, this->res->imgSize.w, this->res->imgSize.h);
			this->res->img->Draw(draw, src);

			//��_���[�W���̔��o�[�`��
			if (decleaseTime.IsMax() == false)
			{
				float rate = 1 - (float)decleaseTime.vnow / decleaseTime.vmax;
				ML::Box2D draw2 = OL::setBoxCenter(this->res->imgSize);
				draw2.Offset(this->pos);
				draw2.Offset(-ge->camera2D.x, -ge->camera2D.y);
				ML::Box2D src2(0, this->res->imgSize.h * 2, this->res->imgSize.w, this->res->imgSize.h);

				draw2.w = this->res->imgSize.w * (hpDisplay + hpDecleased * rate) / en->hp.vmax;
				src2.w = this->res->imgSize.w * (hpDisplay + hpDecleased * rate) / en->hp.vmax;

				this->res->img->Draw(draw2, src2);
			}

			//���݂̗̑͂�`��
			draw = OL::setBoxCenter(this->res->imgSize);
			draw.Offset(this->pos);
			draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
			src = ML::Box2D(0, this->res->imgSize.h, this->res->imgSize.w, this->res->imgSize.h);
			draw.w = this->res->imgSize.w * hpDisplay / (float)en->hp.vmax;
			src.w = this->res->imgSize.w * hpDisplay / (float)en->hp.vmax;
			this->res->img->Draw(draw, src);
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
}