//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_ItemTrsBox.h"
#include  "Task_Item.h"
#include  "Task_Player.h"
#include  "Task_Item_coin_maneger.h"

namespace  ItemTrsBox
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->ImgTrsBox = DG::Image::Create("./data/Item/image/TreasureBox.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->ImgTrsBox.reset();
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
		this->motion = Stand;
		this->render2D_Priority[1] = 0.4f;
		this->hitBase = ML::Box2D(-32, -32, 64, 64);
		this->pos = ML::Vec2(1200, 700);
		this->minCoin = 1;
		this->maxCoin = 50;
		this->minLevel = 0;
		this->maxLevel = 1;
		this->Hitbool = false;
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
		if (this->Hitbool == true) this->animCnt++;
		this->Think();
		this->Move();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->ImgTrsBox->Draw(di.draw, di.src);
		ge->debugRect(this->CallHitBox(), 4, -ge->camera2D.x, -ge->camera2D.y);
	}

	//-----------------------------------------------------------------
	void Object::Think()
	{
		auto inp = ge->in1->GetState();
		int nm = this->motion;
		switch (nm)
		{
		case Motion::Stand:
			break;
		case Motion::Hit:
			if (this->animCnt > 70)nm = Motion::Lose;
			break;
		case Motion::Lose:
			break;
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);

	}

	//------------------------------------------------------------------------
	void Object::Move()
	{
		switch (this->motion) {
		case Hit:
			//�A�C�e���𐶐�
			if (this->animCnt == 50) {
				auto coinMg = ge->GetTask<BChara>("coin_maneger");
				int genCoin = this->minCoin + (rand() % (this->maxCoin - this->minCoin + 1));
				if (maxCoin - minCoin > 0)
				{
					genCoin = minCoin + (rand() % (minCoin - maxCoin));
				}
				else
				{
					genCoin = minCoin;
				}
				coinMg->Create_coin(this->pos.x, this->pos.y, genCoin);
				
				auto item = Item::Object::Create(true);
				item->pos = this->pos;
				item->ItemInit(this->pos, minLevel, maxLevel);
			}
			break;
		case Lose:
			this->Kill();
			break;
		}
	}

	//-------------------------------------------------------------------
	//�A�j���[�V��������
	BChara::DrawInfo Object::Anim()
	{
		ML::Color defColor(1, 1, 1, 1);

		BChara::DrawInfo imageTable[] = {
			//draw                   src
			{this->hitBase,ML::Box2D(0,0,32,32),defColor},   //�ʏ���
			{this->hitBase,ML::Box2D(32,0,32,32),defColor},
			{this->hitBase,ML::Box2D(64,0,32,32),defColor},
			{this->hitBase,ML::Box2D(96,0,32,32),defColor},
			{this->hitBase,ML::Box2D(128,0,32,32),defColor},
			{this->hitBase,ML::Box2D(160,0,32,32),defColor},
			{this->hitBase,ML::Box2D(192,0,32,32),defColor},
			};
		BChara::DrawInfo  rtv;
		int work;
		switch (this->motion)
		{
		default: rtv = imageTable[0];
		case Stand:
			rtv = imageTable[0];
			break;
		case Hit:
			work = this->animCnt / 10;
			work %= 7;
			rtv = imageTable[work];
			break;
		case Lose:
			rtv = imageTable[6];
			rtv.color = ML::Color(1.0f - (0.05f * this->moveCnt), 1.0f, 1.0f, 1.0f);
			break;
		}

		return rtv;
	}

	//-----------------------------------------------------------------------------
	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		
	}

	void Object::GiftPlayer(BChara* pl_)
	{
		if (this->Hitbool != true) {
			this->motion = Motion::Hit;
			this->Hitbool = true;
			ge->qa_Map->AddOpenedBox(this->mapPos);
		}
	}
	//-----------------------------------------------------------------------------
	//�{�b�N�X������
	void Object::BoxInit(ML::Point mapPos_, int coinMin_, int coinMax_, int levelMin_, int levelMax_)
	{
		this->mapPos = mapPos_;
		this->minCoin = coinMin_;
		this->maxCoin = coinMax_;
		this->minLevel = levelMin_;
		this->maxLevel = levelMax_;
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