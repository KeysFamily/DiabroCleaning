//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map.h"
#include  "Task_Effect00.h"


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
		this->initialHitBase = ML::Box2D(-20, -58, 40, 116);
		this->crouchHitBase = ML::Box2D(-40, -26, 76, 84);
		this->attackBase = ML::Box2D(0, 0, 0, 0);
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
		this->attack2 = false;
		this->attack3 = false;
		this->airattack = true;
		this->canJump = true;
		this->balanceMoney = 100;  //������
		this->hp.SetValues(100, 0, 100);
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
		this->attackBase.OffsetCopy(this->pos);
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
					break;
				}
			}
			auto enemys = ge->GetTasks<BChara>("Enemy");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				if ((*it)->CheckHit(this->attackBase)) {
					BChara::AttackInfo at = { 0, 0, 0 };
					(*it)->Received(this, at);
					break;
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


		ge->debugRect(this->hitBase.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
		ge->debugRectDraw();
		ge->debugRect(this->attackBase.OffsetCopy(this->pos), 5, -ge->camera2D.x, -ge->camera2D.y);
		ge->debugRectDraw();
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		int  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��


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
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			break;
		case  Motion::Fall:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			break;
		case Motion::Fall2:
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			break;
		case  Motion::TakeOff:	//��ї���
			if (this->moveCnt >= 3) { nm = Motion::Jump; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Landing:	//���n
			if (this->moveCnt >= 6) { nm = Motion::Stand; }
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
			if (inp.B4.down) { nm = Motion::Attack; }
			break;
		case Motion::CrouchWalk:	//���Ⴊ�݂Ȃ���ړ�
			if (inp.LStick.BD.off) { nm = Motion::Walk; /*this->pos.y -= 5;*/ }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Crouch; }
			if (inp.LStick.BL.off && inp.LStick.BR.off && inp.LStick.BD.off) { nm = Motion::Stand; /*this->pos.y -= 5;*/ }
			if (inp.B4.down) { nm = Motion::Attack; }
			break;
		case  Motion::Attack:	//�U����
			if (this->moveCnt == 25)
			{
				if (attack2 == true)
				{
					nm = Motion::Attack2;
				}
				else nm = Motion::Stand;
			}
			break;
		case Motion::Attack2:
			if (this->moveCnt == 25)
			{
				if (attack3 == true)
				{
					nm = Motion::Attack3;
				}
				else nm = Motion::Stand;
			}
			break;
		case Motion::Attack3:
			if (this->moveCnt == 30) { nm = Motion::Stand; }
			break;
		case Motion::AirAttack:
			if (this->moveCnt == 20)
			{
				if (canJump == true) nm = Motion::Fall;
				else if (canJump == false)nm = Motion::Fall2;
				if (attack2 == true)
				{
					nm = Motion::AirAttack2;
				}
			}
			break;
		case Motion::AirAttack2:
			if (this->moveCnt == 15)
			{
				if (canJump == true) nm = Motion::Fall;
				else if (canJump == false)nm = Motion::Fall2;
				if (attack3 == true)
				{
					nm = Motion::AirAttack3;
				}
			}
			break;
		case Motion::AirAttack3:
			if (this->CheckFoot() == true) { nm = Motion::AirAttack4; }
			break;
		case Motion::AirAttack4:
			if (this->moveCnt == 15) { nm = Motion::Stand; }
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
		case Motion::AirAttack: break;
		case Motion::AirAttack2: break;
		case Motion::AirAttack3: break;
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
			this->airattack = true;
			this->attackBase = ML::Box2D(0, 0, 0, 0);
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
			this->canJump = false;
			break;
		case Motion::Landing:
			this->canJump = true;
			break;
		case  Motion::Attack:	//�U����
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack2 = true; }
			}
			break;
		case  Motion::Attack2:	//�U����
			this->attack2 = false;
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack3 = true; }
			}
			break;
		case  Motion::Attack3:	//�U����
			this->attack3 = false;
			break;
		case Motion::AirAttack:
			this->airattack = false;
			this->moveVec.y = 0.0f;
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack2 = true; }
			}
			break;
		case  Motion::AirAttack2:	//�U����
			this->moveVec.y = 0.0f;
			this->attack2 = false;
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack3 = true; }
			}
			break;
		case Motion::AirAttack3:
			this->moveVec.y = 20.0f;
			this->attack3 = false;
			break;
		case Motion::AirAttack4:
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
			{ ML::Box2D(-48, -58, 76, 116), ML::Box2D(56,28,76,116), defColor },			//0 ��~1
			{ ML::Box2D(-40, -62, 68, 120), ML::Box2D(264,24,68,120), defColor },			//1 ��~2
			{ ML::Box2D(-44, -62, 76, 120), ML::Box2D(460,24,76,120), defColor },			//2 ��~3
			{ ML::Box2D(-52, -58, 80, 116), ML::Box2D(652,28,80,116), defColor },			//3 ��~4
			{ ML::Box2D(-40, -54, 80, 112), ML::Box2D(268,180,80,112), defColor },			//4 ���s1
			{ ML::Box2D(-40, -50, 80, 108), ML::Box2D(464,184,80,108), defColor },			//5 ���s2
			{ ML::Box2D(-40, -42, 80, 100), ML::Box2D(664,184,80,100), defColor },			//6 ���s3
			{ ML::Box2D(-40, -54, 92, 112), ML::Box2D(868,180,92,112), defColor },			//7 ���s4
			{ ML::Box2D(-40, -50, 80, 108), ML::Box2D(1064,184,80,108), defColor },			//8 ���s5
			{ ML::Box2D(-40, -42, 80, 100), ML::Box2D(1264,192,80,100), defColor },			//9 ���s6
			{ ML::Box2D(-40, -26, 76, 84), ML::Box2D(864,60,76,84), defColor },				//10 ���Ⴊ��
			{ ML::Box2D(-44, -30, 80, 88), ML::Box2D(1060,56,80,88), defColor },			//11 ���Ⴊ�݂Ȃ���ړ�1
			{ ML::Box2D(-44, -30, 76, 88), ML::Box2D(1260,56,76,88), defColor },			//12 ���Ⴊ�݂Ȃ���ړ�2
			{ ML::Box2D(-36, -26, 68, 84), ML::Box2D(68,208,68,84), defColor },				//13 ���Ⴊ�݂Ȃ���ړ�3
			{ ML::Box2D(-32, -40, 76, 108), ML::Box2D(468,324,76,108), defColor },			//14 �W�����v1
			{ ML::Box2D(-52, -40, 84, 92), ML::Box2D(656,316,84,92), defColor },			//15 �W�����v2
			{ ML::Box2D(-40, -64, 68, 124), ML::Box2D(272,448,68,124), defColor },			//16 ����1
			{ ML::Box2D(-40, -64, 68, 120), ML::Box2D(472,452,68,120), defColor },			//17 ����2
			{ ML::Box2D(-44, -28, 80, 86), ML::Box2D(60,344,80,96), defColor },				//18 ��ї����O1
			{ ML::Box2D(-40, -30, 80, 88), ML::Box2D(260,352,80,88), defColor },			//19 ���n
			{ ML::Box2D(-44, -24, 80, 88), ML::Box2D(260,352,80,88), defColor },			//20 �_���[�W(��
			{ ML::Box2D(-80, -30, 108, 88), ML::Box2D(28,944,108,88), defColor },			//21 �U��1_1
			{ ML::Box2D(-48, -86, 136, 144), ML::Box2D(460,888,136,144), defColor },		//22 �U��1_2
			{ ML::Box2D(-48, -86, 108, 144), ML::Box2D(660, 888, 108, 144), defColor },		//23 �U��1_3
			{ ML::Box2D(-48, -70, 76, 128), ML::Box2D(860,904,76,128), defColor },			//24 �U��1_4
			{ ML::Box2D(-48, -46, 72, 104), ML::Box2D(1060,928,72,104), defColor },			//25 �U��1_5
			{ ML::Box2D(-48, -50, 72, 108), ML::Box2D(1260,924,72,108), defColor },			//26 �U��2_1
			{ ML::Box2D(-56, -50, 80, 108), ML::Box2D(52,1072,80,108), defColor },			//27 �U��2_2
			{ ML::Box2D(-68, -58, 148, 116), ML::Box2D(240,1064,148, 116), defColor },		//28 �U��2_3
			{ ML::Box2D(-100, -26, 128,84), ML::Box2D(408,1096,128,84), defColor },			//29 �U��2_4
			{ ML::Box2D(-100, -30, 124,88), ML::Box2D(608,1092,124,88), defColor },			//30 �U��2_5
			{ ML::Box2D(-60, -46, 80,104), ML::Box2D(876,1076,80,104), defColor },			//31 �U��3_1
			{ ML::Box2D(-56, -46, 80,104), ML::Box2D(1080,1076,80,104), defColor },			//32 �U��3_2
			{ ML::Box2D(-104, -34, 192,92), ML::Box2D(1208,1088,192,92), defColor },		//33 �U��3_3
			{ ML::Box2D(-100, -18, 124,76), ML::Box2D(12,1252,124,76), defColor },			//34 �U��3_4
			{ ML::Box2D(-112, -22, 136,80), ML::Box2D(200,1248,136,80), defColor },			//35 �U��3_5
			{ ML::Box2D(-112, -22, 136,80), ML::Box2D(400,1248,136,80), defColor },			//36 �U��3_6
			{ ML::Box2D(-44, -40, 72, 104), ML::Box2D(1060, 1960, 72, 104), defColor },		//37 �󒆍U��1_1
			{ ML::Box2D(-88, -64, 184, 120), ML::Box2D(1208, 1940, 184, 120), defColor },	//38 �󒆍U��1_2
			{ ML::Box2D(-80, -70, 116, 120), ML::Box2D(24, 2084, 116, 120), defColor },		//39 �󒆍U��1_3
			{ ML::Box2D(-84, -74, 108, 124), ML::Box2D(220, 2080, 108, 124), defColor },	//40 �󒆍U��1_4
			{ ML::Box2D(-44, -66, 136, 124), ML::Box2D(460, 2088, 136, 124), defColor },	//41 �󒆍U��2_1
			{ ML::Box2D(-44, -70, 92, 124), ML::Box2D(664, 2084, 92, 124), defColor },		//42 �󒆍U��2_2
			{ ML::Box2D(-44, -54, 64, 104), ML::Box2D(860, 2100, 64, 104), defColor },		//43 �󒆍U��2_3
			{ ML::Box2D(-56, -70, 112, 124), ML::Box2D(1032, 2076, 112, 124), defColor },	//44 �󒆍U��3_1
			{ ML::Box2D(-56, -62, 108, 120), ML::Box2D(1232, 2084, 108, 120), defColor },	//45 �󒆍U��3_2
			{ ML::Box2D(-56, -66, 108, 124), ML::Box2D(32, 2228, 108, 124), defColor },		//46 �󒆍U��3_3
			{ ML::Box2D(-100, -84, 192, 144), ML::Box2D(204, 2220, 192, 144), defColor },	//47 �󒆍U��4_1
			{ ML::Box2D(-92, -62, 184, 120), ML::Box2D(412, 2244, 184, 120), defColor },	//48 �󒆍U��4_2
			{ ML::Box2D(-96, -30, 184, 88), ML::Box2D(608, 2276, 184, 88), defColor },		//49 �󒆍U��4_3
			{ ML::Box2D(-36, -38, 84, 96), ML::Box2D(664, 1232, 84, 96),defColor},			//50 �_���[�W debug���ĂȂ�
		};
		ML::Box2D attackTable[] = {
			ML::Box2D(0,0,0,0),				//imageTable[21]	0
			ML::Box2D(-20,-86,108,128),		//imageTable[22]	1			ML::Box2D(-48, -86, 136, 144)
			ML::Box2D(-24,-86,84,32),		//imageTable[23]	2			ML::Box2D(-48, -86, 108, 144)
			ML::Box2D(-44,-70,20,44),		//imageTable[24]	3			ML::Box2D(-48, -70, 76, 128)
			ML::Box2D(0,0,0,0),				//imageTable[25]	4
			ML::Box2D(0,0,0,0),				//imageTable[26]	5
			ML::Box2D(0,0,0,0),				//imageTable[27]	6
			ML::Box2D(-64,-58,144,116),		//imageTable[28]	7			ML::Box2D(-68, -58, 148, 116)
			ML::Box2D(-100,14,80,32),		//imageTable[29]	8			ML::Box2D(-100, -26, 128, 84)
			ML::Box2D(-100,10,52,24),		//imageTable[30]	9			ML::Box2D(-100, -30, 124, 88)
			ML::Box2D(0,0,0,0),				//imageTable[31]	10
			ML::Box2D(0,0,0,0),				//imageTable[32]	11
			ML::Box2D(-104,-34,192,84),		//imageTable[33]	12			ML::Box2D(-104, -34, 192, 92)
			ML::Box2D(-100, 10,84,40),		//imageTable[34]	13			ML::Box2D(-100, -18, 124,76)
			ML::Box2D(-112,-2,72,32),		//imageTable[35]	14			ML::Box2D(-112, -22, 136,80)
			ML::Box2D(0,0,0,0),				//imageTable[36]	15
			ML::Box2D(0,0,0,0),				//imageTable[37]	16
			ML::Box2D(-88,-64,184,88),		//imageTable[38]	17			ML::Box2D(-88, -64, 184, 120)
			ML::Box2D(-80,-70,40,76),		//imageTable[39]	18			ML::Box2D(-80, -70, 116, 120)
			ML::Box2D(0,0,0,0),				//imageTable[40]	19
			ML::Box2D(-32,-66,124,124),		//imageTable[41]	20			ML::Box2D(-44, -66, 136, 124)
			ML::Box2D(-36,-70,84,68),		//imageTable[42]	21			ML::Box2D(-44, -70, 92, 124)
			ML::Box2D(0,0,0,0),				//imageTable[43]	22
			ML::Box2D(-20,-70,76,124),		//imageTable[44]	23			ML::Box2D(-56, -70, 112, 124)
			ML::Box2D(16,-62,36,120),		//imageTable[45]	24			ML::Box2D(-56, -62, 108, 120)
			ML::Box2D(16,-66,36,124),		//imageTable[46]	25			ML::Box2D(-56, -66, 108, 124)
			ML::Box2D(-100, -84, 192, 144),	//imageTable[47]	26			ML::Box2D(-100, -84, 192, 144)
			ML::Box2D(-92, -62, 184, 120),	//imageTable[48]	27			ML::Box2D(-92, -62, 184, 120)
			ML::Box2D(0,0,0,0),				//imageTable[49]	28
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
			work = this->animCnt / 8;
			work %= 4;
			rtv = imageTable[work];
			break;
			//	���s----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 6;
			work %= 6;
			rtv = imageTable[work + 4];
			break;
			//	����----------------------------------------------------------------------------
		case  Motion::Fall:
			work = this->animCnt / 6;
			work %= 2;
			rtv = imageTable[work + 16];
			break;
		case  Motion::Fall2:
			work = this->animCnt / 8;
			work %= 2;
			rtv = imageTable[work + 16];
			break;
		case  Motion::TakeOff:	rtv = imageTable[18];	break;
		case  Motion::Landing:	rtv = imageTable[19];	break;
		case  Motion::Bound:	rtv = imageTable[50];	break;
		case  Motion::Crouch:
			work = this->animCnt / 20;
			work %= 4;
			rtv = imageTable[work + 10];
			break;
		case Motion::CrouchWalk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 11];
			break;
		case Motion::Attack:
			work = this->animCnt / 5;
			work %= 5;
			rtv = imageTable[work + 21];
			this->attackBase = attackTable[work + 0];
			break;
		case Motion::Attack2:
			work = this->animCnt / 5;
			work %= 5;
			rtv = imageTable[work + 26];
			this->attackBase = attackTable[work + 5];
			break;
		case Motion::Attack3:
			work = this->animCnt / 5;
			work %= 6;
			rtv = imageTable[work + 31];
			this->attackBase = attackTable[work + 10];
			break;
		case Motion::AirAttack:
			work = this->animCnt / 5;
			work %= 4;
			rtv = imageTable[work + 37];
			this->attackBase = attackTable[work + 16];
			break;
		case Motion::AirAttack2:
			work = this->animCnt / 5;
			work %= 3;
			rtv = imageTable[work + 41];
			this->attackBase = attackTable[work + 20];
			break;
		case Motion::AirAttack3:
			if (this->animCnt < 5)work = 0;
			else if (this->animCnt >= 5 && this->animCnt < 10)work = 1;
			else work = 2;
			rtv = imageTable[work + 44];
			this->attackBase = attackTable[work + 23];
			break;
		case Motion::AirAttack4:
			work = this->animCnt / 5;
			work %= 3;
			rtv = imageTable[work + 47];
			this->attackBase = attackTable[work + 26];
			break;
		}
		//this->hitBase = rtv.draw;
		this->hitBase.x = -24;
		this->hitBase.y = rtv.draw.y;
		this->hitBase.w = 40;
		this->hitBase.h = rtv.draw.h;		
		//	�����ɉ����ĉ摜�����E���]����
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			this->hitBase = ML::Box2D(-16, rtv.draw.y, 40, rtv.draw.h);
			this->attackBase.x = -this->attackBase.x - this->attackBase.w;
		}
		if (this->hitBase.h > 116) {
			this->hitBase.h = 116;
			this->hitBase.y = -58;
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
		this->hp.Addval(-at_.power);	//������
		if (this->hp.IsMin()) {
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