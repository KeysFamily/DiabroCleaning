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
		this->ImgItem = DG::Image::Create("./data/Item/image/Item.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->ImgItem.reset();
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
		this->render2D_Priority[1] = 0.4f;

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
		if (initialized)
		{

		}

		this->itemb.Time--;
		this->Think();
		this->Move();
		if (this->itemb.Time < 0) this->Kill();
		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->itemb.Time < 300){
			if (this->itemb.Time < 120) {
				if ((this->itemb.Time / 4) % 2 == 0)return;
			}
			if ((this->itemb.Time/8)%2==0)return;
		}
		auto draw = this->hitBase.OffsetCopy(this->pos);
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		ML::Box2D src((32 * this->itemb.Power_step), (32 * this->itemb.Item_pos), 32, 32);
	    //�X�N���[���Ή�
		this->res->ImgItem->Draw(draw, src);
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
		pl->itemPower += itemb.Attack;
		pl->itemDEF += itemb.Defense;
		pl->itemINT += itemb.Magic;
		pl->itemSpeed += itemb.Speed;
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
		//�E�X�s�[�h
		//�E��������
		//�E�`��ʒu
		//****************************************
		json js = OL::LoadJsonFile("./data/Item/Item.json");

		for (auto& ji : js["item"])
		{
			if (ji["itemName"] == itemName_)
			{
				//�A�C�e�����v���C���[�ɗ^����X�e�[�^�X
				this->itemb.Attack = ji["ATK"];
				this->itemb.Defense = ji["DEF"];
				this->itemb.Magic = ji["INT"]; 
				this->itemb.Speed = ji["SPD"]; 
				this->itemb.Time = ji["TIME"]; //�A�C�e���̐�������
				this->itemb.Item_pos = ji["ITEM_POS"]; //�A�C�e���̉摜��Y�������߂�
				this->itemb.Power_step = ji["POWER_STEP"]; //�A�C�e���̃��A���e�B���A�C�e���̉摜��X�������߂�
			}
		}
	}

	//-----------------------------------------------------------------------------
	//�t�@�C���ǂݍ���
	void Object::ItemInit(ML::Vec2 pos_, int minLevel_, int maxLevel_)
	{
		string str[] = { "Attack","Defense","Magic","Speed" };
		InputJsonFile(str[rand() % 4]);
		int minLevel = min(minLevel_, this->itemb.Power_step);
		int maxLevel = min(maxLevel_, this->itemb.Power_step);
		if (maxLevel - minLevel > 0)
		{
			this->itemb.Power_step = minLevel + (rand() % (maxLevel - minLevel + 1));
		}
		else
		{
			this->itemb.Power_step = minLevel;
		}

		//�X�e�[�^�X����U��
		int statBase = 1;
		for (int i = 0; i < itemb.Power_step; ++i)
		{
			statBase *= 2;
		}
		this->itemb.Attack *= statBase;
		this->itemb.Defense *= ceil(statBase * 0.6f);
		this->itemb.Magic *= statBase;
		this->itemb.Speed *= itemb.Power_step + 1;


		this->angle = ML::ToRadian((float)(rand() % 360));
		this->moveVec = ML::Vec2(cos(angle) * 4, sin(angle) * 4);
		this->hitBase = ML::Box2D(-24, -24, 48, 48);
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��
		this->decSpeed = 0.05f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->maxFallSpeed = 11.0f;	//�ő嗎�����x
		this->motion = Stand;
		this->pos = pos_;

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