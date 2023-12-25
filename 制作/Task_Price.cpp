//?------------------------------------------------------
//�^�X�N��:�l�i�\���^�X�N
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Price.h"

namespace  Price
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgCoin = DG::Image::Create("./data/image/coin.png");
		this->imgCoinSize.Set(32, 32);
		this->imgNum = DG::Image::Create("./data/image/font_number.png");
		this->imgNumSize.Set(20, 26);
		this->srcNumBeginPos = ML::Vec2(0, 35);

		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgCoin.reset();
		this->imgNum.reset();
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
		this->price = 0;
		this->distance = 10;
		this->pos = ML::Vec2(0, 0);
		this->denyColor = ML::Color(1, 1, 0.5f, 0.5f);
		this->active = true;
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
		if (active == false)
		{
			return;
		}

		//�����̌v�Z
		int numPlace = 1;
		int ofsX = 0;
		while (price / numPlace >= 10)
		{
			ofsX -= this->res->imgNumSize.h / 2;
			numPlace *= 10;
		}

		//�R�C���̕`��
		ML::Box2D draw = OL::setBoxCenter(this->res->imgCoinSize);
		ML::Box2D src(0, 0, this->res->imgCoinSize.w, this->res->imgCoinSize.h);
		draw.Offset(this->pos);
		draw.x -= this->res->imgCoinSize.w / 2;
		draw.x += ofsX;

		this->res->imgCoin->Draw(draw, src);

		//���i�̕`��
		ML::Color color(1, 1, 1, 1);
		if (ge->qa_Player != nullptr)
		{
			if (ge->qa_Player->balanceMoney < this->price)
			{
				color = this->denyColor;
			}
		}

		draw = OL::setBoxCenter(this->res->imgNumSize);
		src = ML::Box2D(0, 0, this->res->imgNumSize.w, this->res->imgNumSize.h);
		draw.Offset(this->pos);
		src.Offset(this->res->srcNumBeginPos);
		draw.x += this->res->imgNumSize.w / 2;
		draw.x += ofsX;

		while (numPlace > 0)
		{
			src.x = this->res->imgNumSize.w * (price / numPlace % 10);
			this->res->imgNum->Draw(draw, src, color);
			
			draw.x += this->res->imgNumSize.w;
			numPlace /= 10;
		}
	}
	//-------------------------------------------------------------------
	//���̑��֐���`
	void Object::SetPrice(int price_)
	{
		this->price = price_;
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