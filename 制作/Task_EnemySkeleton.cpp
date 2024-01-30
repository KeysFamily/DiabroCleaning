//?------------------------------------------------------
//�^�X�N���@:�GSkeleton
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:�GSkeleton�̓���
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemySkeleton.h"

#include  "Task_Player.h"

#include  "Task_Map.h"
#include  "sound.h"

#include  "randomLib.h"
#include  "Task_Item_coin_maneger.h"

namespace  EnemySkeleton
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/enemy/image/SkeletonsX4.png");
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
		this->hitBase = OL::setBoxCenter(62, 102);
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
		this->attackPow = 10;
		this->dropMoney = 10;

		//��������������������
		//�ȉ�22CI0329�L�q
		this->standTimeStd = 10;
		this->standTime = 60;
		this->standTimeDif = 60;
		this->trackCoolTime = 20;

		this->attackAnimCnt.SetValues(0, 0, 17);
		this->attackAnimCut = 7;
		this->attackAnimSpeed = 0.2f;
		this->turnCoolTime = 60;
		// ��������������������
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
		BChara::AttackInfo ai = { max(1,this->attackPow * 0.8f),0,0 };
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
			if (moveCnt > this->standTime)
			{
				nm = Motion::Walk;
			}
			if (moveCnt == this->trackCoolTime)
			{
				if (this->SearchPlayer(800, 192))
				{
					nm = Motion::Tracking;
				}
			}

			if (!this->CheckFoot()) { nm = Motion::Fall; }//������Q�Ȃ��Ȃ痎��������

			//if (this->moveCnt > 60 * 3) { nm = Motion::Walk; }
			break;
		case Motion::Walk:	//�����Ă���
		{
			if (this->CheckFront_LR() || !this->CheckFrontFoot_LR()) {
				nm = Motion::Turn;
			}//�����ǂɓ��������������ς���
			if (!this->CheckFoot()) { nm = Motion::Fall; }//������Q�Ȃ��Ȃ痎��������

			if (this->moveCnt > 60 * 5) { nm = Motion::Stand; }
			//�ȍ~�@�v���C�����G

			if (this->searchCnt > 0 && this->SearchPlayer(800, 192)) {
				nm = Motion::Tracking;
			}
		}
		break;

		case Motion::Tracking:
			//�ǐՎ��̏���
			if (this->CheckFront_LR() || !this->CheckFrontFoot_LR()) {
				nm = Motion::Turn;
			}//�����ǂɓ��������������ς���

			if (this->searchCnt > 0) {
				if (!this->SearchPlayer(1000, 256)) {
					//3�񌩂���Ȃ���Βʏ폈���ɖ߂�
					if (this->notFoundPlayerCnt > 3) {
						this->notFoundPlayerCnt = 0;
						nm = Motion::Stand;
					}
					else {
						if (++searchCnt > trackCoolTime) 
						{
							++this->notFoundPlayerCnt;
							nm = Motion::Turn;
							searchCnt = 1;
						}
					}
				}
				else {
					if (this->SearchPlayer(100, this->hitBase.h)) {
						//�U��������
						nm = Motion::Attack;
					}
				}
			}
			if (!this->CheckFoot()) { nm = Motion::Fall; }//������Q�Ȃ��Ȃ痎��������

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
				if (this->preMotion == Motion::Tracking) {
					nm = Motion::Tracking;
				}
				else {
					nm = Motion::Stand;
				}
			}//������Q����Œ��n����
			break;
		case Motion::Attack://�U����
			if (this->moveCnt > (int)(attackAnimCnt.vmax / attackAnimSpeed)) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Landing://���n
			if (!this->CheckFoot()) { nm = Motion::Fall; }//������Q�Ȃ��Ȃ痎��������
			break;
		case Motion::Bound:
			if (this->moveCnt >= 16 && this->CheckFoot()) {
				if (this->preMotion == Motion::Walk) {
					nm = Motion::Tracking;
				}
				else {
					nm = Motion::Stand;
				}
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
		default:
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
		case Motion::Unnon:		break;
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
			if (moveCnt == 0)
			{
				standTime = standTimeStd + (rand() % standTimeDif);
			}
			break;
		case Motion::Walk://�����Ă���
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Tracking://�����Ă���
			//�v���C���[���������Ă��鎞��������
			if (this->SearchPlayer(1000, 256))
			{
				if (this->angle_LR == Angle_LR::Left) {
					this->moveVec.x = max(-this->maxSpeed * 1.5f, this->moveVec.x - this->addSpeed);
				}
				else {
					this->moveVec.x = min(+this->maxSpeed * 1.5f, this->moveVec.x + this->addSpeed);
				}
			}
			break;
		case Motion::Fall://������
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Attack://�U����
			if (this->moveCnt == 0)
			{
				this->attackAnimCnt.Setval(0);
			}
			if ((int)this->attackAnimCnt.vnow == this->attackAnimCut) {
				ML::Box2D hit(
					-this->hitBase.w / 2, 
					this->hitBase.y,
					this->hitBase.w,
					this->hitBase.h
				);
				if (this->angle_LR == Angle_LR::Left) {
					hit.Offset(-100, 0);
				}
				else {
					hit.Offset(100, 0);
				}
				hit.Offset(this->pos);

				ge->debugRect(hit, 7, -ge->camera2D.x, -ge->camera2D.y);
				
				BChara::AttackInfo ai = { static_cast<float>(this->attackPow),0,0 };
				this->Attack_Std(Player::defGroupName, ai, hit);
			}
			attackAnimCnt.Addval(attackAnimSpeed);
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
			if (this->moveCnt == 1)
			{
				ge->CreateEffect(11, this->pos);
				se::Play("enemyDead");
			}
			if (this->moveCnt == 5) { this->DropCoins(this->dropMoney); }
			if (this->moveCnt >= 30) {
				this->Kill();
			}
			break;
		case Motion::Burn:
			if (this->moveCnt == 1)
			{
				ge->CreateEffect(10, this->pos);
			}
			if (this->moveCnt >= 15) {
				this->Kill();
			}
			break;
		}
	}
	
	//-------------------------------------------------------------------
	// �A�j���[�V��������
	BEnemy::DrawInfo Object::Anim(){
		ML::Color defColor(1.0f, 1.0f, 1.0f, 1.0f);
		BEnemy::DrawInfo imageTable[] = {
			//Idle
			{ML::Box2D(-31,-77,96,128),ML::Box2D(  0,0,96,128),defColor},		//1		0
			{ML::Box2D(-31,-77,96,128),ML::Box2D( 96,0,96,128),defColor},		//2
			{ML::Box2D(-31,-77,96,128),ML::Box2D(192,0,96,128),defColor},		//3
			{ML::Box2D(-31,-77,96,128),ML::Box2D(288,0,96,128),defColor},		//4
			{ML::Box2D(-31,-77,96,128),ML::Box2D(384,0,96,128),defColor},		//5
			{ML::Box2D(-31,-77,96,128),ML::Box2D(480,0,96,128),defColor},		//6
			{ML::Box2D(-31,-77,96,128),ML::Box2D(576,0,96,128),defColor},		//7
			{ML::Box2D(-31,-77,96,128),ML::Box2D(672,0,96,128),defColor},		//8
			{ML::Box2D(-31,-77,96,128),ML::Box2D(768,0,96,128),defColor},		//9
			{ML::Box2D(-31,-77,96,128),ML::Box2D(864,0,96,128),defColor},		//10
			{ML::Box2D(-31,-77,96,128),ML::Box2D(960,0,96,128),defColor},		//11	10
			//Walk
			{ML::Box2D(-31,-82,88,132),ML::Box2D(   0,128,88,132),defColor},	//1		11
			{ML::Box2D(-31,-82,88,132),ML::Box2D(  88,128,88,132),defColor},	//2
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 176,128,88,132),defColor},	//3
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 264,128,88,132),defColor},	//4
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 352,128,88,132),defColor},	//5
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 440,128,88,132),defColor},	//6
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 528,128,88,132),defColor},	//7
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 616,128,88,132),defColor},	//8
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 704,128,88,132),defColor},	//9
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 792,128,88,132),defColor},	//10
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 880,128,88,132),defColor},	//11
			{ML::Box2D(-31,-82,88,132),ML::Box2D( 968,128,88,132),defColor},	//12
			{ML::Box2D(-31,-82,88,132),ML::Box2D(1056,128,88,132),defColor},	//13	23
			//Attack
			{ML::Box2D(-41,-97,172,148),ML::Box2D(   0,260,172,148),defColor},	//1		24
			{ML::Box2D(-41,-97,172,148),ML::Box2D( 172,260,172,148),defColor},	//2
			{ML::Box2D(-41,-97,172,148),ML::Box2D( 344,260,172,148),defColor},	//3
			{ML::Box2D(-41,-97,172,148),ML::Box2D( 516,260,172,148),defColor},	//4
			{ML::Box2D(-41,-97,172,148),ML::Box2D( 688,260,172,148),defColor},	//5
			{ML::Box2D(-41,-97,172,148),ML::Box2D( 860,260,172,148),defColor},	//6
			{ML::Box2D(-41,-97,172,148),ML::Box2D(1032,260,172,148),defColor},	//7
			{ML::Box2D(-41,-97,172,148),ML::Box2D(1204,260,172,148),defColor},	//8
			{ML::Box2D(-41,-97,172,148),ML::Box2D(1376,260,172,148),defColor},	//9
			{ML::Box2D(-41,-97,172,148),ML::Box2D(1548,260,172,148),defColor},	//10
			{ML::Box2D(-41,-97,172,148),ML::Box2D(1720,260,172,148),defColor},	//11
			{ML::Box2D(-41,-97,172,148),ML::Box2D(1892,260,172,148),defColor},	//12
			{ML::Box2D(-41,-97,172,148),ML::Box2D(2064,260,172,148),defColor},	//13
			{ML::Box2D(-41,-97,172,148),ML::Box2D(2236,260,172,148),defColor},	//14
			{ML::Box2D(-41,-97,172,148),ML::Box2D(2408,260,172,148),defColor},	//15
			{ML::Box2D(-41,-97,172,148),ML::Box2D(2580,260,172,148),defColor},	//16
			{ML::Box2D(-41,-97,172,148),ML::Box2D(2752,260,172,148),defColor},	//17
			{ML::Box2D(-41,-97,172,148),ML::Box2D(2924,260,172,148),defColor},	//18	41
			//Hit
			{ML::Box2D(-52,-77,120,128),ML::Box2D(  0,408,120,128),defColor},	//1		42
			{ML::Box2D(-52,-77,120,128),ML::Box2D(120,408,120,128),defColor},	//2
			{ML::Box2D(-52,-77,120,128),ML::Box2D(240,408,120,128),defColor},	//3
			{ML::Box2D(-52,-77,120,128),ML::Box2D(360,408,120,128),defColor},	//4
			{ML::Box2D(-52,-77,120,128),ML::Box2D(480,408,120,128),defColor},	//5
			{ML::Box2D(-52,-77,120,128),ML::Box2D(600,408,120,128),defColor},	//6
			{ML::Box2D(-52,-77,120,128),ML::Box2D(720,408,120,128),defColor},	//7
			{ML::Box2D(-52,-77,120,128),ML::Box2D(840,408,120,128),defColor},	//8		49
			//Dead
			{ML::Box2D(-71,-77,132,128),ML::Box2D(   0,536,132,128),defColor},	//1		50
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 132,536,132,128),defColor},	//2
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 264,536,132,128),defColor},	//3
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 396,536,132,128),defColor},	//4
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 528,536,132,128),defColor},	//5
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 660,536,132,128),defColor},	//6
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 792,536,132,128),defColor},	//7
			{ML::Box2D(-71,-77,132,128),ML::Box2D( 924,536,132,128),defColor},	//8
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1056,536,132,128),defColor},	//9
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1188,536,132,128),defColor},	//10
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1320,536,132,128),defColor},	//11
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1452,536,132,128),defColor},	//12
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1584,536,132,128),defColor},	//13
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1716,536,132,128),defColor},	//14
			{ML::Box2D(-71,-77,132,128),ML::Box2D(1848,536,132,128),defColor},	//15	64
		};
		int work;
		BEnemy::DrawInfo rtv;
		switch (this->motion)
		{
		default:
			rtv = imageTable[0];
			if (this->preMotion == Motion::Tracking) {
				rtv.color = ML::Color(1.0f, 1.0f, 0.5f, 0.5f);
			}
			break;
		case Motion::Stand:
			work = this->animCnt / 3;
			work %= 11;
			rtv = imageTable[work];
			break;
		case Motion::Tracking:
			if (!this->SearchPlayer(1000, 256))
			{
				rtv = imageTable[0];
				rtv.color = ML::Color(1.0f, 1.0f, 0.5f, 0.5f);
				break;
			}
		case Motion::Walk:
			work = this->animCnt / 2;
			work %= 13;
			rtv = imageTable[work + 11];
			if (this->motion == Motion::Tracking) {
				rtv.color = ML::Color(1.0f, 1.0f, 0.0f, 0.0f);
			}
			break;
		case Motion::Attack:
			rtv = imageTable[(int)attackAnimCnt.vnow + 24];
			break;
		case Motion::Bound:
			work = this->animCnt / 2;
			if (work < 8) {
				work %= 8;
				rtv = imageTable[work + 42];
			}
			else {
				rtv = imageTable[49];
			}
			break;
		case Motion::Lose:
			work = this->animCnt / 2;
			work %= 15;
			rtv = imageTable[work + 50];
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

		this->unHitTime = 10;
		this->hp.Addval(-at_.power);
		ge->TotalDamage += static_cast<int>(at_.power);
		if (this->hp.vnow <= 0) {
			this->UpdateMotion(Motion::Lose);
			ge->TotalEnemyKill += 1;
			return;
		}
		//������΂����
		if (from_->angle_LR == Angle_LR::Right) {
			this->moveVec = ML::Vec2(1, -3) * 1;
		}
		else {
			this->moveVec = ML::Vec2(-1, -3) * 1;
		}
		this->UpdateMotion(Motion::Bound);
	}

	//-------------------------------------------------------------------
	// Player�����G����
	bool Object::SearchPlayer(int distX_, int distY_) {
#if true
		if (ge->qa_Player == nullptr || ge->qa_Map == nullptr) 
		{ 
			return false;
		}
		ML::Box2D eye(0, 0, distX_, distY_);
		eye.Offset(this->pos);
		if (this->angle_LR == Angle_LR::Left) 
		{
			eye.Offset(-eye.w, -eye.h);
		}
		//�v���C���[���͈͓��ɂ��邩
		if (eye.Hit(ge->qa_Player->CallHitBox()))
		{
			ML::Vec2 toVec = ge->qa_Player->pos - this->pos;
			ML::Vec2 toVecN = toVec.Normalize();
			ML::Vec2 checkPos = this->pos;
			
			while (
				abs(ge->qa_Player->pos.x - checkPos.x) > abs(toVec.x)
				&& abs(ge->qa_Player->pos.x - checkPos.y) > abs(toVec.y)
				)
			{
				if (ge->qa_Map->CheckHit(checkPos))
				{
					return false;
				}
				checkPos += toVecN;
			}

			return true;
		}
		return false;
#else
		this->searchCnt = 0;
		//auto map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);

		if (ge->qa_Player == nullptr || ge->qa_Map == nullptr) { return false; }
		ML::Box2D eye(
			this->hitBase.x,
			this->hitBase.y + this->hitBase.h,
			10,
			10
		);
		if (this->angle_LR == Angle_LR::Left) {
			eye.Offset(-eye.w, -eye.h);
		}
		else {
			eye.Offset(this->hitBase.w, -eye.h);
		}
		eye.Offset(this->pos);

		int eyeW = eye.w;
		int eyeH = eye.h;
		for (int x = 0; x < distX_; x += eyeW) {
			if (ge->qa_Map->CheckHit(eye))break;
			for (int y = 0; y < distY_; y += eyeH) {
				ML::Box2D eb = eye.OffsetCopy(0, -y);
				if (ge->qa_Map->CheckHit(eb))break;
				if (ge->qa_Player != nullptr && ge->qa_Player->CallHitBox().Hit(eb)) { return true; }
				//ge->debugRect(eb, 4, -ge->camera2D.x, -ge->camera2D.y);
				
			}
			if (this->angle_LR == Angle_LR::Left) {
				eye.Offset(-eyeW, 0);
			}
			else {
				eye.Offset(eyeW, 0);
			}
		}

		return false;
#endif
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