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

#include  "randomLib.h"

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
		this->render2D_Priority[1] = 0.6f;
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
			nm = Motion::Walk;
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

			if (this->searchCnt > 60 && this->SearchPlayer(1000)) {
				nm = Motion::Tracking;
			}
		}
			break;

		case Motion::Tracking:
			//�ǐՎ��̏���
			if (this->CheckFront_LR() || !this->CheckFrontFoot_LR()) {
				nm = Motion::Turn;
			}//�����ǂɓ��������������ς���

			if (this->searchCnt > 30) {
				this->searchCnt = 0;
				if (!this->SearchPlayer(1000)) {
					//3�񌩂���Ȃ���Βʏ폈���ɖ߂�
					if (++this->notFoundPlayerCnt > 3) {
						this->notFoundPlayerCnt = 0;
						nm = Motion::Stand;
					}
					else {
						nm = Motion::Turn;
					}
				}
				else {
					if (this->SearchPlayer(75)) {
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
			if (this->moveCnt > 36) {
				nm = this->preMotion;
			}
			break;
		case Motion::Landing://���n
			if (!this->CheckFoot()) { nm = Motion::Fall; }//������Q�Ȃ��Ȃ痎��������
			break;
		case Motion::Bound:
			if (this->moveCnt >= 16 && this->CheckFoot()) {
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
			break;
		case Motion::Walk://�����Ă���
		case Motion::Tracking://�����Ă���
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
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
			if (this->moveCnt == 16) {
				ML::Box2D hit(
					this->hitBase.x, 
					this->hitBase.y + this->hitBase.h / 2,
					this->hitBase.w,
					this->hitBase.h / 2
				);
				if (this->angle_LR == Angle_LR::Left) {
					hit.Offset(-100, 0);
				}
				else {
					hit.Offset(100, 0);
				}
				hit.Offset(this->pos);

				ge->debugRect(hit, 7, -ge->camera2D.x, -ge->camera2D.y);
				BChara::AttackInfo ai = { 10,0,0 };
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
			if (this->moveCnt >= 30) {
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
		default:	rtv = imageTable[0]; break;
		case Motion::Stand:
			work = this->animCnt / 3;
			work %= 11;
			rtv = imageTable[work];
			break;
		case Motion::Tracking:
		case Motion::Walk:
			work = this->animCnt / 2;
			work %= 13;
			rtv = imageTable[work + 11];
			if (this->motion == Motion::Tracking) {
				rtv.color = ML::Color(1.0f, 1.0f, 0.0f, 0.0f);
			}
			break;
		case Motion::Attack:
			work = this->animCnt / 2;
			work %= 18;
			rtv = imageTable[work + 24];
			break;
		case Motion::Bound:
			work = this->animCnt / 2;
			work %= 8;
			rtv = imageTable[work + 42];
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
		this->unHitTime = 30;
		this->hp.Addval(-at_.power);
		if (this->hp.vnow <= 0) {
			this->UpdateMotion(Motion::Lose);
			return;
		}
		//������΂����
		if (this->pos.x > from_->pos.x) {
			this->moveVec = ML::Vec2(+3, -8);
		}
		else {
			this->moveVec = ML::Vec2(-3, -8);
		}
		this->UpdateMotion(Motion::Bound);
	}

	//-------------------------------------------------------------------
	// Player�����G����
	bool Object::SearchPlayer(int dist) {
		this->searchCnt = 0;
		auto map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);

		if (ge->qa_Player == nullptr || map == nullptr) { return false; }
		ML::Box2D eye(
			this->hitBase.x,
			this->hitBase.y,
			10,
			this->hitBase.h
		);
		if (this->angle_LR == Angle_LR::Left) {
			eye.Offset(-eye.w, 0);
		}
		else {
			eye.Offset(this->hitBase.w, 0);
		}
		eye.Offset(this->pos);


		for (int i = 0; i < dist; ++i) {
			ge->debugRect(eye, 7, -ge->camera2D.x, -ge->camera2D.y);

			if (map->CheckHit(eye))break;
			if (ge->qa_Player != nullptr && ge->qa_Player->CallHitBox().Hit(eye)) { return true; }

			if (this->angle_LR == Angle_LR::Left) {
				eye.Offset(-1, 0);
			}
			else {
				eye.Offset(1, 0);
			}
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