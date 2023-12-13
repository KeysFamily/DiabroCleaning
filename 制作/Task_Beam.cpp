//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Beam.h"
#include  "BEnemy.h"
#include  "Task_Player.h"

namespace  Beam
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/effect/beam.png");
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
		this->hitBase = ML::Box2D(0, -16, 0, 32);
		this->pos = ML::Vec2(0, 0);
		this->power = 1.0f;
		this->cost = 0;
		this->motion = Motion::Start;
		this->length = 100;
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
		this->moveCnt++;
		this->animCnt++;
		auto pl = ge->GetTask<BChara>("Player");
		if (this->angle_LR == Angle_LR::Right)
		{
			this->pos.x = pl->pos.x + 60;
		}
		else 
		{
			this->pos.x = pl->pos.x - 60;
			this->hitBase.x -= this->length;
		}
		this->hitBase.w += this->length;
		this->pos.y = pl->pos.y;
		if (true == this->CheckFront_LR()) { this->length = 0; }
		this->Think();
		this->Move();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);

		ge->debugRect(this->hitBase.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
		ge->debugRectDraw();
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		int  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��
		auto pl = ge->GetTask<BChara>("Player");
		switch (nm) {
		case Motion::Start:
			if (this->moveCnt > 12) {
				nm = Motion::Infinite;
			}
			break;
		case Motion::Infinite:
			if (this->moveCnt > 3 && pl->motion != Player::Object::Motion::MagicAttack) { nm = Motion::End; }
			if (pl->balanceMoney <= this->cost) { nm = Motion::End; }
			break;
		case Motion::End:
			if (this->moveCnt > 9) { this->Kill(); }
			break;
		}
		if(pl->motion != Player::Object::Motion::MagicAttack) { nm = Motion::End; }
		//���[�V�����X�V
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//���[�V�����ɑΉ���������
	void  Object::Move()
	{
		auto enemys = ge->GetTasks<BChara>("Enemy");
		auto pl = ge->GetTask<BChara>("Player");
		switch (this->motion) {
		case Motion::Start:
			break;
		case Motion::Infinite:
			for (auto it = enemys->begin();
				it != enemys->end();
				++it) {
				if ((*it)->CheckHit(this->hitBase.OffsetCopy(this->pos))) {
					BChara::AttackInfo at = { this->power, 0, 0 };
					(*it)->Received(this, at);
				}
			}
			if (this->moveCnt % 60 == 1) { pl->balanceMoney -= this->cost; }
			break;
		case Motion::End:
			break;
		}
	}
	//--------------------------------------------------------------------
	// �A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(0, -3, 224, 6), ML::Box2D(30, 30, 224, 6), defColor },					//0
			{ ML::Box2D(0, -6, 224, 12), ML::Box2D(31, 90, 224, 12), defColor },				//1
			{ ML::Box2D(0, -11, 224, 22), ML::Box2D(32, 148, 224, 22), defColor },				//2
			{ ML::Box2D(0, -5, 224, 10), ML::Box2D(36, 220, 224, 10), defColor },				//3
			{ ML::Box2D(0, -4, 224, 8), ML::Box2D(44, 285, 224, 8), defColor },					//4
			{ ML::Box2D(0, -1, 224, 2), ML::Box2D(98, 352, 224, 2), defColor },					//5
			{ ML::Box2D(0, -1, 224, 2), ML::Box2D(0, 64*6, 224, 2), defColor },				//6
		};
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
		case Motion::Start:
			if (this->animCnt <= 3)work = 0;
			if (this->animCnt > 3 && this->animCnt <= 6)work = 1;
			if (this->animCnt > 6 && this->animCnt <= 9)work = 2;
			if (this->animCnt > 9)work = 3;
			rtv = imageTable[work];
			break;
		case Motion::Infinite:
			rtv = imageTable[3];
			break;
		case Motion::End:
			if (this->animCnt <= 3)work = 0;
			if (this->animCnt > 3 && this->animCnt <= 6)work = 1;
			if (this->animCnt > 6)work = 2;
			rtv = imageTable[work + 4];
			break;
		}
		rtv.draw.w = this->hitBase.w;
		//	�����ɉ����ĉ摜�����E���]����
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
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