//?------------------------------------------------------
//�^�X�N��:�_���[�W�\�L
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_DamageNum.h"

namespace  DamageNum
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/font_number.png");
		this->imgSize.Set(20, 32);
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
		this->damageNum = 0;
		this->drawScale = 1;
		this->lifeTime.SetValues(0, 0, 30);
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
		this->pos.y -= 1;

		this->lifeTime.Addval(1);
		if (this->lifeTime.IsMax())
		{
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�����̌v�Z
		int numPlace = 1;
		int ofsX = 0;
		while (this->damageNum / numPlace >= 10)
		{
			ofsX -= this->res->imgSize.w / 2;
			numPlace *= 10;
		}

		ML::Color color(1, 1, 0.2f, 0.2f);

		//�}�C�i�X�̕`��
		{
			ML::Box2D draw = OL::setBoxCenter(this->res->imgSize);
			ML::Box2D src(12, 19, 13,13);
			draw.Offset(this->pos);
			draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
			draw.x -= this->res->imgSize.w / 2;
			draw.x += ofsX;
			this->res->img->Draw(draw, src, color);
		}

		//���i�̕`��

		ML::Box2D draw = OL::setBoxCenter(this->res->imgSize);
		ML::Box2D src(0, 32, this->res->imgSize.w, this->res->imgSize.h);
		draw.Offset(this->pos);
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		draw.x += this->res->imgSize.w / 2;
		draw.x += ofsX;

		while (numPlace > 0)
		{
			src.x = this->res->imgSize.w * (this->damageNum / numPlace % 10);
			this->res->img->Draw(draw, src, color);

			draw.x += this->res->imgSize.w;
			numPlace /= 10;
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