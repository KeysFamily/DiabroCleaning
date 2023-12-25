//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_PlayerStatusShop.h"
#include  "Task_PlayerStatus.h"
#include  "Task_Price.h"

namespace  PlayerStatusShop
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgProgress = DG::Image::Create("./data/image/menu/status/Progress.png");
		this->imgProgressSize.Set(14, 42);
		this->fontDisplaySize.Set(14, 32);
		this->fontDisplay = DG::Font::Create("a", this->fontDisplaySize.w, this->fontDisplaySize.h);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgProgress.reset();
		this->fontDisplay.reset();
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
		this->render2D_Priority[1] = 0.5f;

		this->displayStr = "";
		this->progressDistance = 10;
		this->currentStatus.SetValues(1, 0, 10);

		for (int i = 0; i < currentStatus.vmax; ++i)
		{
			price.push_back(i + 1000 + i * 1658);
			addStatus.push_back(1 + i);
		}

		this->selectScale = 1.5f;

		this->displayPos = ML::Vec2(10, -40);
		this->progressBeginPos.x = this->res->imgProgressSize.w * (currentStatus.vmax - 2) / -2.0f;
		this->progressBeginPos.x += this->progressDistance * (currentStatus.vmax - 1) / -2.0f;
		this->progressBeginPos.y = 30;
		
		this->priceDpPos = ML::Vec2(15, -10);
		this->priceDp = Price::Object::Create(true);

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
		if (this->currentStatus.vnow < this->currentStatus.vmax - 1)
		{
			this->priceDp->price = this->price[this->currentStatus.vnow];
			this->priceDp->pos = this->priceDpPos + this->pos;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�X�e�[�^�X���`��
		ML::Box2D draw = OL::setBoxCenter(
			(int)(this->res->fontDisplaySize.w * 1.5f) * this->displayStr.size(),
			this->res->fontDisplaySize.h
		);
		draw.Offset(this->pos + this->displayPos);

		this->res->fontDisplay->Draw(draw, this->displayStr);
		
		//�ǉ��X�e�[�^�X�`��
		draw = OL::setBoxCenter(this->res->imgProgressSize);
		draw.Offset(this->pos + this->progressBeginPos);
		ML::Box2D src(0, 0, this->res->imgProgressSize.w, this->res->imgProgressSize.h);

		for (int i = 1; i <= currentStatus.vmax; ++i)
		{
			if (i <= currentStatus.vnow)
			{
				src.x = this->res->imgProgressSize.w * (2 + this->statusType);
			}
			else if (i == currentStatus.vnow + 1)
			{
				src.x = this->res->imgProgressSize.w;
			}
			this->res->imgProgress->Draw(draw, src);
			draw.x += this->res->imgProgressSize.w + this->progressDistance;
			src.x = 0;
		}
	}
	//-------------------------------------------------------------------
	//���̑��̊֐�
	//���̍w���ɕK�v�ȋ��z��Ԃ�
	int Object::GetPrice() const
	{
		if (this->currentStatus.IsMax())
		{
			return 0;
		}
		else
		{
			return price[currentStatus.vnow + 1];
		}
	}

	//���݂̒ǉ��X�e�[�^�X���擾����
	int Object::GetStatusAdd() const
	{
		return addStatus[currentStatus.vnow];
	}

	//�w������
	bool Object::Buy(int& money_)
	{
		if (this->currentStatus.vnow > this->currentStatus.vmax - 1)
		{
			return false;
		}
		else if (this->currentStatus.vnow == this->currentStatus.vmax - 1)
		{
			money_ -= price[currentStatus.vnow];
			currentStatus.Addval(1);
			this->priceDp->active = false;
			return true;
		}
		if (money_ >= price[currentStatus.vnow])
		{
			money_ -= price[currentStatus.vnow];
			currentStatus.Addval(1);
			return true;
		}
		return false;
	}

	//�T�C�Y�ƈʒu��`����
	ML::Box2D Object::GetObjectSize() const
	{
		ML::Box2D result = OL::setBoxCenter(this->res->imgProgressSize);
		result.Offset(this->pos + this->progressBeginPos);
		if (this->currentStatus.IsMax())
		{
			result.x += (this->res->imgProgressSize.w + this->progressDistance) * (this->currentStatus.vnow - 1);
		}
		else
		{
			result.x += (this->res->imgProgressSize.w + this->progressDistance) * this->currentStatus.vnow;
		}
		result.w *= this->selectScale;
		result.h *= this->selectScale;
		result.x -= (result.w - this->res->imgProgressSize.w) / 2;
		result.y -= (result.h - this->res->imgProgressSize.h) / 2;

		return result;
	}

	//�^�[�Q�b�g����
	void Object::IsSelecting()
	{

	}

	//�{�^���������ꂽ��
	void Object::IsDown()
	{
		int money = 9999999;
		this->Buy(money);
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