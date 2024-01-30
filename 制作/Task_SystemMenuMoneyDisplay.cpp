//?------------------------------------------------------
//�^�X�N��:�������\���f�B�X�v���C
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SystemMenuMoneyDisplay.h"

namespace  SystemMenuMoneyDisplay
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/PriceDisplay/BackGround.png");
		this->imgBgSize.Set(640, 128);
		this->imgNum = DG::Image::Create("./data/image/Menu/PriceDisplay/font_number4x.png");
		this->imgNumSize.Set(40, 52);
		this->srcNumBeginPos = ML::Vec2(0, 70);
		this->imgCoin = DG::Image::Create("./data/image/Menu/PriceDisplay/coin4x.png");
		this->imgCoinSize.Set(128, 128);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->imgNum.reset();
		this->imgCoin.reset();
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
		this->render2D_Priority[1] = 0.6f;
		this->pos = ML::Vec2(420, 130);
		this->coinPos = ML::Vec2(-250, 0);
		this->moneyBeginPos = ML::Vec2(-160, 0);
		this->maxNumPlace = 100000000;
		this->moneyDisplay = 20;
		this->moneyMoveCnt.SetValues(0, 0, 30);
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
		if (ge->qa_Player != nullptr)
		{
			if (this->moneyDisplay != ge->qa_Player->balanceMoney)
			{
				if (this->moneyMoveCnt.IsMin())
				{
					this->moneyDifference = ge->qa_Player->balanceMoney - this->moneyDisplay;
				}

				this->moneyMoveCnt.Addval(1);
				float moveRate = (moneyMoveCnt.vmax - moneyMoveCnt.vnow) / (float)moneyMoveCnt.vmax;
				this->moneyDisplay = static_cast<int>(ge->qa_Player->balanceMoney - moneyDifference * moveRate);

				if (this->moneyMoveCnt.IsMax())
				{
					this->moneyMoveCnt.Setval(this->moneyMoveCnt.vmin);
					this->moneyDisplay = ge->qa_Player->balanceMoney;
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�w�i�̕`��
		ge->DrawStd(this->res->imgBg, this->res->imgBgSize, this->pos);

		//�R�C���̕`��
		ge->DrawStd(this->res->imgCoin, this->res->imgCoinSize, this->pos + this->coinPos);

		//�����̕`��
		int numPlace = maxNumPlace;
		ML::Box2D draw = OL::setBoxCenter(this->res->imgNumSize);
		ML::Box2D src(0, 0, this->res->imgNumSize.w, this->res->imgNumSize.h);
		draw.Offset(this->pos + this->moneyBeginPos);
		src.Offset(this->res->srcNumBeginPos);

		while (numPlace > 0)
		{
			src.x = this->res->imgNumSize.w * (moneyDisplay / numPlace % 10);
			this->res->imgNum->Draw(draw, src);

			draw.x += this->res->imgNumSize.w;
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