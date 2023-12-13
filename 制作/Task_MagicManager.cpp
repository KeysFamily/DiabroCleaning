//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_MagicManager.h"
#include  "Task_FireBall.h"
#include  "Task_WaterBlast.h"
#include  "Task_Thunder.h"
#include  "Task_Player.h"

namespace  MagicManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
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
		this->moveCnt = 0;
		this->LR = true;
		this->pos = ML::Vec2(0, 0);
		this->magicSelect = Magic::Unnon;
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
		auto pl = ge->GetTask<BChara>("Player");
		switch (this->magicSelect) {
		case Magic::Unnon:
			break;
		case Magic::FireBall:
			if (this->moveCnt % 30 == 1) {
				auto fb = FireBall::Object::Create(true); //(��)
				if (pl->balanceMoney > fb->cost) {
					if (this->LR) {
						fb->angle_LR = BChara::Angle_LR::Right;
						fb->moveVec = ML::Vec2(fb->speed, 0);
						fb->pos.x = this->pos.x + 60;
						fb->pos.y = this->pos.y;
					}
					if (!this->LR) {
						fb->angle_LR = BChara::Angle_LR::Left;
						fb->moveVec = ML::Vec2(-fb->speed, 0);
						fb->pos.x = this->pos.x - 60;
						fb->pos.y = this->pos.y;
					}
					pl->balanceMoney -= fb->cost;
				}
				else {
					fb->Kill();
				}
			}
			break;
		case Magic::WaterBlast:
			if (this->moveCnt == 1) {
				auto wb = WaterBlast::Object::Create(true);
				if (pl->balanceMoney > wb->cost) {
					if (this->LR) {
						wb->pos.x = this->pos.x + 100;
						wb->pos.y = this->pos.y;
					}
					if (!this->LR) {
						wb->pos.x = this->pos.x - 100;
						wb->pos.y = this->pos.y;
					}
					pl->balanceMoney -= wb->cost;
				}
			}
			break;
		case Magic::Thunder:
			if (this->moveCnt % 30 == 1) {
				auto th = Thunder::Object::Create(true); //(��)
				if (pl->balanceMoney > th->cost) {
					if (this->LR) {
						th->angle_LR = BChara::Angle_LR::Right;
						th->moveVec = ML::Vec2(th->speed, 0);
						th->pos.x = pl->pos.x + 60;
						th->pos.y = pl->pos.y;
					}
					if (!this->LR) {
						th->angle_LR = BChara::Angle_LR::Left;
						th->moveVec = ML::Vec2(-th->speed, 0);
						th->pos.x = pl->pos.x - 60;
						th->pos.y = pl->pos.y;
					}
					pl->balanceMoney -= th->cost;
				}
				else {
					th->Kill();
				}
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
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