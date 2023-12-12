//?------------------------------------------------------
//�^�X�N���@:�GSkyEye
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:�GSkyEye�̓���
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemySkyEye.h"

#include  "Task_Player.h"

#include  "Task_Map.h"

#include  "randomLib.h"
#include  "Task_Item_coin_maneger.h"
//-----------------------------------------------------------------------
// SkyEye
// �󒆎G���G
// 
// �ʏ펞 �󒆂��E����������B
// �U���� �v���C���[�ɓːi����B�n�ʂɂԂ���Ȃ��悤�ɔ�s����
// �e���[�h�̏���
// 
// Stand,	// N��~
// Walk,	// N�ʏ��s
// Tracking,// A���G(�W�I�֑̓�������J�n)
// Attack,	// A�U��(�̓�����)
// Fall,	// ����(���S���̂�)
// Landing,	// ���n
// Turn,	// �t������
// Bound,	// �e����΂���Ă���(�m�b�N�o�b�N����)
// Lose,	// ���Œ�
//-----------------------------------------------------------------------


namespace  EnemySkyEye
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/enemy/image/SkyEye.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = OL::setBoxCenter(50, 50);
		this->angle_LR = Angle_LR::Left;
		this->motion = Motion::Stand;
		this->maxSpeed = 2.0f;
		this->addSpeed = 0.7f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -6.0f;
		this->gravity = ML::Gravity(32.0f) * 5.0f;
		const int HP = 10;
		this->hp.SetValues(HP, 0, HP);
		this->targetPos = ML::Vec2();
		this->altitude = -1000.0f;
		this->attackPow = 10;
		this->dropMoney = 10;
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
		this->UpDate_Std();
		BChara::AttackInfo ai = { 1,0,0 };
		this->Attack_Std(Player::defGroupName, ai, this->CallHitBox());
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ge->debugRect(this->hitBase.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
		this->Render_Std(this->res->img);
		
	}
	
	//-------------------------------------------------------------------
	// �v�l����
	void Object::Think(){
		int nm = this->motion;//�Ƃ肠�������̏�Ԃ��w��
		switch (nm)
		{
		case Motion::Stand:	//�����Ă���
			if (this->moveCnt > 1) {
				nm = Motion::Walk;
			}
			break;
		case Motion::Walk:	//�����Ă���
			if (this->CheckFront_LR()) {
				nm = Motion::Turn;
			}//�����ǂɓ��������������ς���

			if (this->searchCnt > 60 && this->SearchPlayer(800,256)) {
				nm = Motion::Tracking;
			}
			break;

		case Motion::Tracking:
			//�������Փ˂�����
			if (this->CallHitBox().Hit(this->targetPos)) {
				if (this->SearchPlayer(this->hitBase.w, this->hitBase.h)) {
					//�U��������
					nm = Motion::Attack;
				}
				else {
					nm = Motion::Walk;
				}
			}
			break;
		case Motion::Turn:
			if (this->moveCnt >= 5) { 
				if (this->preMotion == Motion::Tracking) {
					nm = Motion::Tracking;
				}
				else {
					nm = Motion::Stand;
				}
			}
			break;
		case Motion::Fall:	//���~��
			if (this->CheckFoot()) { 
				nm = Motion::Lose;
			}//������Q����Œ��n����
			break;
		case Motion::Attack://�U����
			if (this->moveCnt > 16) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Landing://���n
			if (!this->CheckFoot()) { nm = Motion::Fall; }//������Q�Ȃ��Ȃ痎��������
			break;
		case Motion::Bound:
			if (this->moveCnt >= 60) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Lose:
			break;
		default:
			break;
		}
		this->UpdateMotion(nm);
	}
	
	//-------------------------------------------------------------------
	// ���쐧��
	void Object::Move(){
		//�d�͉���
		switch (this->motion) {
		case Motion::Fall:
			//�㏸���������͑����ɒn�ʂ��Ȃ�
			if (this->moveVec.y < 0 || !this->CheckFoot()) {
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//�n�ʂɐڐG���Ă���
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۂȂ��j
		default:				break;
		}

		//�ړ����x����
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			//�n�ʂɐڐG���Ă���
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۂȂ��j
		case Motion::Bound:
		case Motion::Unnon:		break;
		}
		//-------------------------------------------------------------------
		// ���[�V�������ƂɌŗL�̏���
		switch (this->motion)
		{
		case Motion::Stand://�����Ă���
			if (this->altitude < -50.0f) {
				this->altitude = this->pos.y;
			}
			break;
		case Motion::Walk://�����Ă���
		{	//���E�ړ�����
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			this->moveVec.y = 0.0f;

			//�㉺�ړ�����
			if (ge->qa_Map != nullptr) {
				float diffY = this->altitude - this->pos.y;

				//�ړ����V�~�����[�g
				ML::Vec2 smPos = this->pos;
				smPos.y += diffY * 0.05f;
				if (!ge->qa_Map->CheckHit(this->hitBase.OffsetCopy(smPos))) {
					//�ړ���ŏՓ˂��Ȃ���΁A�I�u�W�F�N�g���ړ�������
					ge->Dbg_ToDisplay(smPos.x, smPos.y - 50, "�Փ˂Ȃ��A�ړ����s");
					this->pos = smPos;
				}

			}

		}	break;
		case Motion::Tracking://���G
		{
			ML::Vec2 diff = this->targetPos - this->pos;

			float angRad = atan2(diff.y, diff.x);
			this->moveVec.x = 7.5f * cos(angRad);
			this->moveVec.y = 7.5f * sin(angRad);
		}	break;
		case Motion::Fall://������
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Attack://�U����
			if (this->moveCnt == 15) {
				ML::Box2D hit = this->hitBase;
				hit.Offset(this->pos);

				ge->debugRect(hit, 7, -ge->camera2D.x, -ge->camera2D.y);
				
				BChara::AttackInfo ai = { this->attackPow,0,0 };
				this->Attack_Std(Player::defGroupName, ai, hit);
			}
			break;
		case Motion::Turn:
			if (this->moveCnt == 3) {
				if (this->angle_LR == Angle_LR::Left) {
					this->angle_LR = Angle_LR::Right;
				}
				else {
					this->angle_LR = Angle_LR::Left;
				}
			}
			break;
		case Motion::Lose:
			if (this->moveCnt == 7) { this->DropCoins(this->dropMoney); }
			if (this->moveCnt >= 8) {
				this->Kill();
			}
			break;
		case Motion::Bound:
			this->moveVec *= 0.98f;
			break;
		}
	}
	
	//-------------------------------------------------------------------
	// �A�j���[�V��������
	BEnemy::DrawInfo Object::Anim(){
		ML::Color defColor(1.0f, 1.0f, 1.0f, 1.0f);
		BEnemy::DrawInfo imageTable[] = {
			//Idle
			{this->hitBase,ML::Box2D(  0,  0,50,50),defColor},	//1		0
			{this->hitBase,ML::Box2D( 50,  0,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100,  0,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150,  0,50,50),defColor},	//4
			{this->hitBase,ML::Box2D(200,  0,50,50),defColor},	//5
			{this->hitBase,ML::Box2D(250,  0,50,50),defColor},	//6
			{this->hitBase,ML::Box2D(300,  0,50,50),defColor},	//7
			{this->hitBase,ML::Box2D(350,  0,50,50),defColor},	//8		7

			//Att
			{this->hitBase,ML::Box2D(  0, 50,50,50),defColor},	//1		8
			{this->hitBase,ML::Box2D( 50, 50,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100, 50,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150, 50,50,50),defColor},	//4
			{this->hitBase,ML::Box2D(200, 50,50,50),defColor},	//5
			{this->hitBase,ML::Box2D(250, 50,50,50),defColor},	//6
			{this->hitBase,ML::Box2D(300, 50,50,50),defColor},	//7
			{this->hitBase,ML::Box2D(350, 50,50,50),defColor},	//8		15
			//Damage
			{this->hitBase,ML::Box2D(  0,100,50,50),defColor},	//1		16
			{this->hitBase,ML::Box2D( 50,100,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100,100,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150,100,50,50),defColor},	//4		19
			//Dead
			{this->hitBase,ML::Box2D(  0,150,50,50),defColor},	//1		20
			{this->hitBase,ML::Box2D( 50,150,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100,150,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150,150,50,50),defColor},	//4		23
		};
		int work;
		BEnemy::DrawInfo rtv;
		switch (this->motion)
		{
		default:	rtv = imageTable[0]; break;
		case Motion::Stand:
			work = this->animCnt / 3;
			work %= 11;
			rtv = imageTable[work];
			break;
		case Motion::Tracking:
		case Motion::Walk:
			work = this->animCnt / 2;
			work %= 8;
			rtv = imageTable[work];
			if (this->motion == Motion::Tracking) {
				rtv.color = ML::Color(1.0f, 1.0f, 0.0f, 0.0f);
			}
			break;
		case Motion::Attack:
			work = this->animCnt / 2;
			work %= 8;
			rtv = imageTable[work + 8];
			break;
		case Motion::Bound:
			work = this->animCnt / 2;
			if (work < 4) {
				work %= 4;
				rtv = imageTable[work + 16];
			}
			else {
				rtv = imageTable[19];
			}
			break;
		case Motion::Lose:
			work = this->animCnt / 2;
			work %= 4;
			rtv = imageTable[work + 20];
			break;
		}
		if (this->angle_LR == Angle_LR::Left) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}

	void Object::Received(BChara* from_, AttackInfo at_) {
		if (this->unHitTime > 0) { 
			return; //���G���Ԓ��͏������󂯂Ȃ�
		}
		//this->unHitTime = 20;
		this->hp.Addval(-at_.power);
		if (this->hp.vnow <= 0) {
			this->UpdateMotion(Motion::Fall);
			return;
		}
		//������΂����
		if (from_->angle_LR == Angle_LR::Right) {
			this->moveVec = ML::Vec2(2, -3) * 3;
		}
		else {
			this->moveVec = ML::Vec2(-2, -3) * 3;
		}
		this->UpdateMotion(Motion::Bound);
	}

	//-------------------------------------------------------------------
	// Player�����G���� x�͑O���̍��G�͈�y�͏㉺�̍��G�͈�
	bool Object::SearchPlayer(int distX_, int distY_) {
		bool isFoundPlayer = false;
		this->searchCnt = 0;
		if (ge->qa_Player == nullptr || ge->qa_Map == nullptr) { return false; }

		//�����蔻���`�̍쐬
		ML::Box2D eye = OL::setBoxCenter(30, 30);
		if (this->angle_LR == Angle_LR::Left) {
			eye.Offset(-eye.w, -eye.h);
		}
		else {
			eye.Offset(eye.w, -eye.h);
		}
		eye.Offset(this->pos);

		int eyeW = eye.w;
		int eyeH = eye.h;

		//�v���C���[����`���ɂ��邩�H
		for (int x = this->hitBase.w; x < distX_; x += eyeW) {
			if (ge->qa_Map->CheckHit(eye))break;
			for (int y = 0; y < distY_; y += eyeH) {
				ML::Box2D eb = eye.OffsetCopy(0, y);
				if (ge->qa_Map->CheckHit(eb))break;
				if (ge->qa_Player->CallHitBox().Hit(eb)) {
					this->targetPos = ML::Vec2(
						GetRandom<float>(eb.x, eb.x + eb.w),
						GetRandom<float>(eb.y, eb.y + eb.h)
					);
					isFoundPlayer = true;
					goto Check;
				}
				ge->debugRect(eb, 4, -ge->camera2D.x, -ge->camera2D.y);
				
			}
			for (int y = 0; y > -distY_; y -= eyeH) {
				ML::Box2D eb = eye.OffsetCopy(0, y);
				if (ge->qa_Map->CheckHit(eb))break;
				if (ge->qa_Player->CallHitBox().Hit(eb)) { 
					this->targetPos = ML::Vec2(
						GetRandom<float>(eb.x, eb.x + eb.w),
						GetRandom<float>(eb.y, eb.y + eb.h)
					);
					isFoundPlayer = true;
					goto Check;
				}
				ge->debugRect(eb, 4, -ge->camera2D.x, -ge->camera2D.y);

			}


			if (this->angle_LR == Angle_LR::Left) {
				eye.Offset(-eyeW, 0);
			}
			else {
				eye.Offset(eyeW, 0);
			}
		}
	Check:
		//�����ꍇ�͂����ɔ��
		//���ӁFgoto���͈Öق̃��[���Ŋ�{�I�ɂ͎g��Ȃ��B
		//�@�@�@����͐[���l�X�g����E�o���邽�߂Ɏg�p�����B
		if (isFoundPlayer) {
			ML::Vec2 smDiff = this->targetPos - this->pos;
			float angRad = atan2(smDiff.y, smDiff.x);

			float smMoveX = 25.0f * cos(angRad);
			float smMoveY = 25.0f * sin(angRad);

			while (!this->CallHitBox().OffsetCopy(smDiff).Hit(this->pos)) {
				if (ge->qa_Map->CheckHit(this->CallHitBox().OffsetCopy(smDiff)))return false;

				smDiff.x -= smMoveX;
				smDiff.y -= smMoveY;
			}
			return true;
		}

		return false;
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