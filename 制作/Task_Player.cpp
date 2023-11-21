//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map.h"
#include  "Task_Effect00.h"
#include "Task_Item_coin.h"


namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/adventure4x.png");
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-48, -58, 76, 116);
		this->initialHitBase = ML::Box2D(-48, -58, 76, 116);
		this->crouchHitBase = ML::Box2D(-38, -42, 76, 84);
		this->angle_LR = Angle_LR::Right;
		this->controller = ge->in1;
		this->motion = Motion::Stand;		//�L�����������
		this->maxSpeed = 8.0f;		//�ő�ړ����x�i���j
		this->addSpeed = 1.0f;		//���s�����x�i�n�ʂ̉e���ł�����x�ł��������
		this->crouchSpeed = 2.5f;	//���Ⴊ�݂Ȃ���ړ��ő呬�x
		this->decSpeed = 0.5f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->maxFallSpeed = 11.0f;	//�ő嗎�����x
		this->jumpPow = -11.0f;		//�W�����v�́i�����j
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��
		this->drawScale = 1;
		this->balanceMoney = 100;  //������
		this->hp = this->balanceMoney; //hp=������
		ge->debugRectLoad();
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->debugRectReset();

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
		this->hitBase = this->DrawScale(this->initialHitBase, this->drawScale);
		if (this->unHitTime > 0) { this->unHitTime--; }
		//�v�l�E�󋵔��f
		this->Think();
		//�����[�V�����ɑΉ���������
		this->Move();
		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);
		//hitbase�X�V
		BChara::DrawInfo  di = this->Anim();
		//this->hitBase = di.draw;
		//�����蔻��
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("�A�C�e��");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				//����ɐڐG�̗L�����m�F������
				if ((*it)->CheckHit(me)) {
					//����Ƀ_���[�W�̏������s�킹��
					BChara::AttackInfo at = { 0, 0, 0 };
					(*it)->Received(this, at);
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0) {
			if ((this->unHitTime / 4) % 2 == 0) {
				return;//8�t���[����4�t���[���摜��\�����Ȃ�
			}
		}
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);


		ge->debugRect(this->hitBase.OffsetCopy(this->pos),7, -ge->camera2D.x, -ge->camera2D.y);
		ge->debugRectDraw();
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		int nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

		//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
		//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�
		switch (nm) {
		case  Motion::Stand:	//�����Ă���
			if (inp.LStick.BL.on) { nm = Motion::Walk; }
			if (inp.LStick.BR.on) { nm = Motion::Walk; }
			if (inp.LStick.BD.on) { nm = Motion::Crouch; }
			if (inp.LStick.BD.on && inp.LStick.BL.on) { nm = Motion::CrouchWalk; }
			if (inp.LStick.BD.on && inp.LStick.BR.on) { nm = Motion::CrouchWalk; }
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//���� ��Q�@����
			break;
		case  Motion::Walk:		//�����Ă���
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			if (inp.LStick.BD.on) { nm = Motion::CrouchWalk; this->moveVec.x = 0; }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Stand; }
			break;
		case  Motion::Jump:		//�㏸��
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			break;
		case  Motion::Fall:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			break;
		case Motion::Fall2:
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			break;
		case  Motion::Attack:	//�U����
			if (this->moveCnt == 40) { nm = Motion::Stand; }
			break;
		case  Motion::TakeOff:	//��ї���
			if (this->moveCnt >= 3) { nm = Motion::Jump; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Landing:	//���n
			if (this->moveCnt >= 3) { nm = Motion::Stand; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case Motion::Bound:
			if (this->moveCnt >= 12 &&
				this->CheckFoot() == true) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Crouch:    //���Ⴊ��
			if (inp.LStick.BD.off) { nm = Motion::Stand; /*this->pos.y -= 5;*/ }
			if (inp.LStick.BL.on) { nm = Motion::CrouchWalk; }
			if (inp.LStick.BR.on) { nm = Motion::CrouchWalk; }
			break;
		case Motion::CrouchWalk:	//���Ⴊ�݂Ȃ���ړ�
			if (inp.LStick.BD.off) { nm = Motion::Walk; /*this->pos.y -= 5;*/ }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Crouch; }
			if (inp.LStick.BL.off && inp.LStick.BR.off && inp.LStick.BD.off) { nm = Motion::Stand; /*this->pos.y -= 5;*/ }
			break;
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//���[�V�����ɑΉ���������
	//(���[�V�����͕ύX���Ȃ��j
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();
		//�d�͉���
		switch (this->motion) {
		default:
			//�㏸���������͑����ɒn�ʂ�����
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//�n�ʂɐڐG���Ă���
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		case Motion::Unnon:	break;
		}

		//�ړ����x����
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		case Motion::Bound:
		case Motion::Unnon:	break;
		}
		//-----------------------------------------------------------------
		//���[�V�������ɌŗL�̏���
		switch (this->motion) {
		case  Motion::Stand:	//�����Ă���
			break;
		case  Motion::Walk:		//�����Ă���
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case  Motion::Fall:		//������
			if (inp.LStick.BL.on) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Fall2:
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Jump:		//�㏸��
			if (this->CheckHead() == true) { this->moveVec.y = 0; }
			if (this->moveCnt == 0) {
				this->moveVec.y = this->jumpPow;
			}
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Jump2:
			if (this->CheckHead() == true) { this->moveVec.y = 0; }
			if (this->moveCnt == 0) {
				this->moveVec.y = this->jumpPow * 0.9f;
			}
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Attack:	//�U����
			break;
		case Motion::Crouch:	//���Ⴊ��
			break;
		case Motion::CrouchWalk:	//���Ⴊ�݂Ȃ���ړ�
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = max(-this->crouchSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->crouchSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-48, -58, 76, 116), ML::Box2D(56,28,76,116), defColor },	//0 ��~1
			{ ML::Box2D(-40, -62, 68, 120), ML::Box2D(264,24,68,120), defColor },	//1 ��~2
			{ ML::Box2D(-44, -62, 76, 120), ML::Box2D(460,24,76,120), defColor },	//2 ��~3
			{ ML::Box2D(-52, -58, 80, 116), ML::Box2D(652,28,80,116), defColor },	//3 ��~4 �����܂ŕҏW�ς�
			{ ML::Box2D(-40, -56, 80, 112), ML::Box2D(268,180,80,112), defColor },	//4 ���s1
			{ ML::Box2D(-40, -54, 80, 108), ML::Box2D(464,184,80,108), defColor },	//5 ���s2
			{ ML::Box2D(-40, -50, 80, 100), ML::Box2D(664,184,80,100), defColor },	//6 ���s3
			{ ML::Box2D(-46, -56, 92, 112), ML::Box2D(868,180,92,112), defColor },	//7 ���s4
			{ ML::Box2D(-40, -54, 80, 108), ML::Box2D(1064,184,80,108), defColor },	//8 ���s5
			{ ML::Box2D(-40, -50, 80, 100), ML::Box2D(1264,192,80,100), defColor },	//9 ���s6
			{ ML::Box2D(-10, -7, 76, 84), ML::Box2D(864,60,76,84), defColor },	//10 ���Ⴊ��
			{ ML::Box2D(-11, -8, 80, 88), ML::Box2D(1060,56,80,88), defColor },	//11 ���Ⴊ�݂Ȃ���ړ�1
			{ ML::Box2D(-11, -8, 76, 88), ML::Box2D(1260,56,76,88), defColor },	//12 ���Ⴊ�݂Ȃ���ړ�2
			{ ML::Box2D(-9, -7, 68, 84), ML::Box2D(68,208,68,84), defColor },	//13 ���Ⴊ�݂Ȃ���ړ�3
			{ ML::Box2D(-10, -14, 76, 108), ML::Box2D(468,324,76,108), defColor },	//14 �W�����v1
			{ ML::Box2D(-11, -12, 84, 92), ML::Box2D(656,316,84,92), defColor },	//15 �W�����v2
			{ ML::Box2D(-9, -16, 68, 124), ML::Box2D(272,448,68,124), defColor },	//16 ����1
			{ ML::Box2D(-9, -15, 68, 120), ML::Box2D(472,452,68,120), defColor },	//17 ����2
			{ ML::Box2D(-10, -12, 80, 86), ML::Box2D(60,344,80,96), defColor },	//18 ��ї����O1
			{ ML::Box2D(-10, -11, 80, 88), ML::Box2D(260,352,80,88), defColor },	//19 ��ї����O2
			{ ML::Box2D(-10, -11, 80, 88), ML::Box2D(260,352,80,88), defColor },	//20 ���n
			{ ML::Box2D(-14, -8, 108, 88), ML::Box2D(28,944,108,88), defColor },	//21 �U��1_1
			{ ML::Box2D(-17, -18, 136, 144), ML::Box2D(460,888,136,144), defColor },	//22 �U��1_2
			{ ML::Box2D(-13, -18, 108, 144), ML::Box2D(660, 888, 108, 144), defColor },	//23 �U��1_3
			{ ML::Box2D(-10, -16, 76, 128), ML::Box2D(860,904,76,128), defColor },	//24 �U��1_4
			{ ML::Box2D(-9, -13, 72, 104), ML::Box2D(1060,928,72,104), defColor },	//25 �U��1_5
			{ ML::Box2D(-24, -24, 48, 80), ML::Box2D(176, 0, 48, 80),defColor},		//�_���[�W
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	�W�����v------------------------------------------------------------------------
		case  Motion::Jump:
			rtv = imageTable[14];
			if (this->animCnt > 10)rtv = imageTable[15];
			break;
		case  Motion::Jump2:
			rtv = imageTable[14];
			if (this->animCnt > 10)rtv = imageTable[15];
			break;
			//	��~----------------------------------------------------------------------------
		case  Motion::Stand:
			work = this->animCnt / 30;
			work %= 4;
			rtv = imageTable[work];
			break;
			//	���s----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 6;
			rtv = imageTable[work + 4];
			break;
			//	����----------------------------------------------------------------------------
		case  Motion::Fall:
			rtv = imageTable[16];
			if (this->animCnt > 10)rtv = imageTable[17];
			break;
		case  Motion::Fall2:
			rtv = imageTable[16];
			if (this->animCnt > 10)rtv = imageTable[17];
			break;
		case  Motion::TakeOff:	rtv = imageTable[18];	break;
		case  Motion::Landing:	rtv = imageTable[19];	break;
		case  Motion::Bound:	rtv = imageTable[20];	break;
		case Motion::Crouch:
			work = this->animCnt / 16;
			work %= 4;
			rtv = imageTable[work + 10];
			break;
		case Motion::CrouchWalk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 11];
			break;
		case Motion::Attack:
			work = this->animCnt / 8;
			work %= 5;
			rtv = imageTable[work + 21];
			break;
		}
		this->hitBase = rtv.draw;
		//	�����ɉ����ĉ摜�����E���]����
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			this->hitBase = ML::Box2D(rtv.draw.x + rtv.draw.w, rtv.draw.y, -rtv.draw.w, rtv.draw.h);
		}

		rtv.draw = this->DrawScale(rtv.draw, this->drawScale);
		rtv.src = this->DrawScale(rtv.src, this->drawScale);

		return rtv;
	}
	//-----------------------------------------------------------------------------
	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		if (this->unHitTime > 0) {
			return;//���G���Ԓ��̓_���[�W���󂯂Ȃ�
		}
		this->unHitTime = 90;
		this->hp -= at_.power;	//������
		if (this->hp <= 0) {
			this->Kill();
		}
		//������΂����
		if (this->pos.x > from_->pos.x) {
			this->moveVec = ML::Vec2(+4, -9);
		}
		else {
			this->moveVec = ML::Vec2(-4, -9);
		}
		this->UpdateMotion(Motion::Bound);
		//from_�͍U�����Ă�������A�J�E���^�[�Ȃǂŋt�Ƀ_���[�W��^���������g��
	}
	//-------------------------------------------------------------------
	//��`�{��
	ML::Box2D Object::DrawScale(ML::Box2D& me, const int drawScale)
	{
		ML::Box2D sample;
		sample.x = me.x * drawScale;
		sample.y = me.y * drawScale;
		sample.h = me.h * drawScale;
		sample.w = me.w * drawScale;
		return sample;
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
				//�i���\�b�h�����ςȂ̂͋��o�[�W�����̃R�s�[�ɂ��o�O��������邽��
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