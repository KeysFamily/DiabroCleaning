//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Item.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace  Item
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
		Object::InputJsonFile("Attack");
		//���f�[�^������
		this->hitBase = ML::Box2D(-16, -16, 32, 32);
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
		int x = rand() % this->itemb.Power_step;
		int y = this->itemb.Item_pos;
		auto draw = this->hitBase.OffsetCopy(this->pos);
		ML::Box2D src(32 * x, 32 * y, 32, 32);
	    //�X�N���[���Ή�
		//di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(draw, src);
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
			if (inp.B2.down)
			{
				//nm = Motion::Suction;
			}
			break;
		case Motion::Fall:
			if (this->CheckFoot() == true)
			{
				{ nm = Motion::Stand; }
			}
			break;
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);

	}
	//------------------------------------------------------------------
	void Object::Move()
	{
		switch (this->motion) {
		default:
			//�d�͉���
			switch (this->motion) {
			default:
				//�㏸���������͑����ɒn�ʂ�����
				if (this->moveVec.y < 0 ||
					this->CheckFoot() == false) {
					this->fallSpeed = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
					this->moveVec.y = this->fallSpeed;
				}
				//�n�ʂɐڐG���Ă���
				else {
					this->fallSpeed = min(this->moveVec.y - this->gravity, this->maxFallSpeed);
					this->moveVec.y -= this->fallSpeed * 1.7;
				}


				break;
				//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
			case Motion::Unnon:	break;
			}
			//�ړ����x����
			switch (this->motion) {
			default:
				if (this->CheckFoot() == true) {
					if (this->moveVec.x < 0) {
						this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
					}
					else {
						this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
					}
				}
				break;

				//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
			case Motion::Bound:
			case Motion::Unnon:	break;
			}

			//switch (this->motion)
			//{
			//case Motion::Suction:
			//	//auto pl = ge->GetTask<Player::Object>("Player");
			//	this->target = ge->qa_Player;
			//	if (auto  tg = this->target.lock()) {
			//		//�^�[�Q�b�g�ւ̑��΍��W�����߂�
			//		ML::Vec2  toVec = tg->pos - this->pos;

			//		float speed = 20;
			//		auto vec = toVec.Normalize();
			//		vec *= speed;
			//		//�^�[�Q�b�g�ɂT���߂Â�
			//		//this->pos += toVec * 0.05f;

			//		this->pos += vec;
			//	}
			//	break;
			//}
		}
	}

	//-----------------------------------------------------------------------------
	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		
		//this->UpdateMotion(Motion::Bound);
		//from_�͍U�����Ă�������A�J�E���^�[�Ȃǂŋt�Ƀ_���[�W��^���������g��
	}

	void Object::GiftPlayer(BChara* pl_)
	{
		Player::Object* pl = dynamic_cast<Player::Object*>(pl_);
		pl->power += itemb.Attack;
		this->Kill();
	}

	//-----------------------------------------------------------------------------
//�t�@�C���ǂݍ���
	void Object::InputJsonFile(string itemName_) {
		//****************************************
		//�t�@�C������󂯕t�������
		//�E�U����
		//�E�h���
		//�E���@��
		// �E�X�s�[�h
		// �E��������
		// �E�`��ʒu
		//****************************************
		ifstream f("./data/enemy/json/Item.json");
		if (!f.is_open()) return;//�t�@�C���I�[�v���Ɏ��s
		json data = json::parse(f);
		auto& i = data[itemName_];

		this->itemb.Attack= i["ATK"];
		this->itemb.Defense = i["DEF"];
		this->itemb.Magic = i["INT"];
		this->itemb.Speed = i["SPD"];
		this->itemb.Time = i["TIME"];
		this->itemb.Item_pos= i["ITEM_POS"];
		this->itemb.Power_step = i["POWER_STEP"];
		f.close();

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