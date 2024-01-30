//?------------------------------------------------------
//�^�X�N��:�GBoss
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2024/1/9
//�T�@�@�@�v:�G�{�X
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyBoss.h"

#include  "Task_EnemyManager.h"
#include  "Task_EnemyFireBall.h"
#include  "Task_EnemyBlast.h"

#include  "randomLib.h"
#include  "RateLot.h"

#define DEPTH_CONST	//�����G�ꗥ

namespace  EnemyBoss
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/enemy/image/BossO.png");
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
		this->hitBase = OL::setBoxCenter(128, 256);
		this->angle_LR = Angle_LR::Left;
		this->motion = BossMotion::Stand;
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
		this->depth = 1;
		
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		ge->GameClearFlag = true;
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
		//�����蔻�艼����
		BChara::AttackInfo at = { attackPow * 0.1f,0,0 };
		if(this->Attack_Std(Player::defGroupName,at,this->CallHitBox())){
		// 	//�ڐG�����ہA���g�ɉ���������ۂ̏���
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		this->Render_Std(this->res->img);
	}
	
	//-------------------------------------------------------------------
	// �v�l����
	void Object::Think(){
		int nm = this->motion;
		switch (nm)
		{
		case BossMotion::Stand:
			if (this->moveCnt > 60 * 3) {
				nm = BossMotion::Teleport;
			}
			break;
		case BossMotion::Teleport:
			//��΂��ꏊ
			if (this->moveCnt == 20) {
				nm = BossMotion::Attack;
			}
			break;
		case BossMotion::CoolTime:
			if (this->moveCnt > 60 * 3) {
				nm = BossMotion::Teleport;
			}
			break;
		case BossMotion::Attack:
		{
			vector<BossMotion> sheet;
			vector<float> rate;
			sheet.push_back(BossMotion::Attack1);
			rate.push_back(0.5f);
			if (this->hp.GetRate() <= 0.5f) {//50%�ȉ��ő��i�K���
				sheet.push_back(BossMotion::Attack2);
				rate.push_back(0.5f);
			}
			if (this->hp.GetRate() <= 0.25f) {//25%�ȉ��ő�O�i�K���
				sheet.push_back(BossMotion::Attack3);
				rate.push_back(0.5f);
			}
			if (ge->qa_Enemys->size() < 100) {//�G�̐���100�̈ȓ��Ȃ�G����������
				sheet.push_back(BossMotion::Attack4);
				rate.push_back(0.1f);
			}
			RateLot rl(rate);

			nm = sheet[rl.result()];
		}
			break;
		case BossMotion::Attack1:
			if (this->moveCnt > 39) {
				nm = BossMotion::CoolTime;
			}
			break;
		case BossMotion::Attack2:
			if (this->moveCnt > 39) {
				nm = BossMotion::CoolTime;
			}
			break;
		case BossMotion::Attack3:
			if (this->moveCnt > 39) {
				nm = BossMotion::CoolTime;
			}
			break;
		case BossMotion::Attack4:
			if (this->moveCnt > 39) {
				nm = BossMotion::CoolTime;
			}
			break;
		case BossMotion::Lose:
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
		//switch (this->motion) {
		//default:
		//	//�㏸���������͑����ɒn�ʂ��Ȃ�
		//	if (this->moveVec.y < 0 || !this->CheckFoot()) {
		//		this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
		//	}
		//	//�n�ʂɐڐG���Ă���
		//	else {
		//		this->moveVec.y = 0.0f;
		//	}
		//	break;
		//	//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۂȂ��j
		//case Motion::Unnon:		break;
		//}

		switch (this->motion)
		{
		case BossMotion::Stand:
			break;
		case BossMotion::Teleport:
			if (this->moveCnt == 10) {
				ML::Vec2 tpPos = this->pos;

				for (int i = 0; i < 100; ++i) {
					int mcsize = ge->qa_Map->res->drawSize;	//�}�b�v�`�b�v��Ђ̃T�C�Y
					float dist = GetRandom<float>(mcsize * 10.0f, mcsize * 20.0f);
					float rad = GetRandom<float>(0, 2 * ML::PI);
					ML::Vec2 offsetPos;
					offsetPos.x = cos(rad);
					offsetPos.y = sin(rad);
					offsetPos *= dist;

					ML::Box2D test = this->CallHitBox().OffsetCopy(offsetPos);
					if (!ge->qa_Map->CheckHit(test)
						&& !ge->qa_Map->CheckFallGround(test)
						&& test.Hit(ge->GetScreenBox())) {
						tpPos += offsetPos;
						break;
					}
				}

				this->pos = tpPos;

				if (ge->qa_Player->pos.x - this->pos.x >= 0) {
					this->angle_LR = Angle_LR::Right;
				}
				else {
					this->angle_LR = Angle_LR::Left;
				}
			}
			break;
		case BossMotion::CoolTime:
			break;
		case BossMotion::Attack:
			break;
		case BossMotion::Attack1:
			//�v���C���[�Ɍ����ă{�X�̏񂩂�΂̖��@���o��
			if (this->moveCnt == 21) {
				ML::Vec2 spos = this->pos;
				if (this->angle_LR == Angle_LR::Left) {
					spos += ML::Vec2(-70, -166);
				}
				else {
					spos += ML::Vec2(+70, -166);
				}
				float mcsize = static_cast<float>(ge->qa_Map->res->drawSize);
				ML::Vec2 tpos = ge->qa_Player->pos + mcsize * ML::Vec2(GetRandom<float>(-1.0f, 1.0f), GetRandom<float>(-1.0f, 1.0f));

				ML::Vec2 diff = tpos - spos;
				float rad = atan2(diff.y, diff.x);

				auto fb = EnemyFireBall::Object::Create(true);
				fb->Setting(spos, 15.0f, rad, this->attackPow * 0.6f);
			}
			break;
		case BossMotion::Attack2:
			//�v���C���[�̑����ɖ��@���o���P�b��ɍU��������
			if (this->moveCnt == 21) {
				auto bl = EnemyBlast::Object::Create(true);
				bl->power = this->attackPow * 0.8f;
				float mcsize = static_cast<float>(ge->qa_Map->res->drawSize);
				bl->pos.y = ge->qa_Player->pos.y;
				bl->pos.x = ge->qa_Player->pos.x + mcsize * GetRandom<float>(-1.0f, 1.0f);

			}
			break;
		case BossMotion::Attack3:
			//���a�R�}�X�ȓ��ɔ͈͍U��
			if (this->moveCnt == 24) {
				float r = ge->qa_Map->res->drawSize * 10.0f;
				auto ew = ge->CreateEffect(9, this->pos);
				auto es = ew.lock();
				es->drawSizeX = es->drawSizeY = r * 4;

				if (this->PosInMyCircle(this->pos, ge->qa_Player->pos, r)) {
					BChara::AttackInfo at = { attackPow,0,0 };
					ge->qa_Player->Received(this, at);
				}

				for (auto it = ge->qa_Enemys->begin(); it != ge->qa_Enemys->end(); ++it) {
					if (this->PosInMyCircle(this->pos, (*it)->pos, r)) {
						(*it)->Kill();
					}
				}
			}
			break;
		case BossMotion::Attack4:
			if (this->moveCnt == 21) {
				//���a�T�}�X�ȓ��̃����_���Ȉʒu�ɓG����������
				//�����͈ȉ��̂Ƃ���Ƃ���B
				//
				int spawnMax;//������
				if (this->hp.GetRate() > 0.5f) {
					spawnMax = 3;
				}
				else if (this->hp.GetRate() > 0.25f) {
					spawnMax = 4;
				}
				else {
					spawnMax = 5;
				}

				for (int i = 0; i < spawnMax; ++i) {
#ifdef DEPTH_CONST
					this->SpawnEnemyIsBoss(depth);
#else
					this->SpawnEnemyIsBoss(GetRandom<int>(1, this->depth));
#endif // DEPTH_CONST
				}
			}
			break;
		case BossMotion::Lose:
			if (this->moveCnt >= 21) {
				this->Kill();
			}
			break;
		default:
			break;
		}
	}
	
	//-------------------------------------------------------------------
	// �A�j���[�V��������
	BEnemy::DrawInfo Object::Anim(){
		ML::Color defColor(1.0f, 1.0f, 1.0f, 1.0f);
		BEnemy::DrawInfo imageTable[] = {
			//Stand
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 192, 224, 256, 256), defColor},	// 1	0
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 832, 224, 256, 256), defColor},	// 2
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(1472, 224, 256, 256), defColor},	// 3
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(2112, 224, 256, 256), defColor},	// 4
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(2752, 224, 256, 256), defColor},	// 5
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(3392, 224, 256, 256), defColor},	// 6
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(4032, 224, 256, 256), defColor},	// 7
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(4672, 224, 256, 256), defColor},	// 8	7
			//Attack��
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 192, 736, 256, 256), defColor},	// 1	8
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 832, 736, 256, 256), defColor},	// 2
			{ML::Box2D(-128,-160, 256, 288), ML::Box2D(1472, 704, 256, 288), defColor},	// 3
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(2112, 640, 256, 352), defColor},	// 4
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(2752, 640, 256, 352), defColor},	// 5
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(3392, 640, 256, 352), defColor},	// 6
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(4032, 640, 256, 352), defColor},	// 7
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(4672, 640, 256, 352), defColor},	// 8
			{ML::Box2D(-128,-228, 320, 416), ML::Box2D(5312, 576, 320, 416), defColor},	// 9
			{ML::Box2D(-128,-228, 320, 416), ML::Box2D(5952, 576, 320, 416), defColor},	//10
			{ML::Box2D(-128,-320, 384, 448), ML::Box2D(6592, 544, 384, 448), defColor},	//11
			{ML::Box2D(-128,-320, 384, 448), ML::Box2D(7232, 544, 384, 448), defColor},	//12
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(7872, 736, 256, 256), defColor},	//13	20
			//Attack��
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 192,1248, 256, 256), defColor},	// 1	21
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 832,1248, 256, 256), defColor},	// 2
			{ML::Box2D(-128,-160, 256, 288), ML::Box2D(1472,1216, 256, 288), defColor},	// 3
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(2112,1152, 256, 352), defColor},	// 4
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(2752,1152, 256, 352), defColor},	// 5
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(3392,1152, 256, 352), defColor},	// 6
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(4032,1152, 256, 352), defColor},	// 7
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(4672,1152, 256, 352), defColor},	// 8
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(5312,1152, 256, 352), defColor},	// 9
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(5952,1152, 256, 352), defColor},	//10
			{ML::Box2D(-128,-224, 256, 352), ML::Box2D(6592,1152, 256, 352), defColor},	//11
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(7232,1248, 256, 256), defColor},	//12
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(7872,1248, 256, 256), defColor},	//13	33
			//Hit
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 192,1760, 256, 256), defColor},	// 1	34
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 832,1760, 256, 256), defColor},	// 2 
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(1472,1760, 256, 256), defColor},	// 3
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(2112,1760, 256, 256), defColor},	// 4
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(2752,1760, 256, 256), defColor},	// 5	38
			//Dead
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 192,2272, 256, 256), defColor},	// 1	39
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D( 832,2272, 256, 256), defColor},	// 2
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(1472,2272, 256, 256), defColor},	// 3
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(2112,2272, 256, 256), defColor},	// 4
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(2752,2272, 256, 256), defColor},	// 5
			{ML::Box2D(-128,-128, 256, 256), ML::Box2D(3392,2272, 256, 256), defColor},	// 6	44
			{ML::Box2D(-128,-160, 256, 288), ML::Box2D(4032,2240, 256, 288), defColor},	// 7	45
		};
		BEnemy::DrawInfo rtv;
		int work = this->animCnt / 3;
		switch (this->motion)
		{
		default:	rtv = imageTable[0];	break;
		case BossMotion::Stand:
		case BossMotion::Teleport:
		case BossMotion::CoolTime:
			work %= 8;
			rtv = imageTable[work];
			break;
		case BossMotion::Attack1:
		case BossMotion::Attack2:
		case BossMotion::Attack4:
			work %= 13;
			rtv = imageTable[work + 21];
			if (this->motion == BossMotion::Attack4) {
				rtv.color = ML::Color(1.0f, 1.0f, 0.0f, 0.0f);
			}
			break;
		case BossMotion::Attack3:
			work %= 13;
			rtv = imageTable[work + 8];
			break;
		case BossMotion::Lose:
			work %= 7;
			rtv = imageTable[work + 39];
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
	}

	void Object::SpawnEnemyIsBoss(int depth_) {
		auto em = ge->GetTask<EnemyManager::Object>(EnemyManager::defGroupName, EnemyManager::defName);

		ML::Vec2 spos = this->pos;
		if (this->angle_LR == Angle_LR::Left) {
			spos += ML::Vec2(-70, -166);
		}
		else {
			spos += ML::Vec2(+70, -166);
		}

		for (int i = 0; i < 50; ++i) {
			float dist = GetRandom<float>(0.0f, ge->qa_Map->res->drawSize * 5.0f);
			float rad = GetRandom<float>(0.0f, 2.0f * ML::PI);

			ML::Vec2 offsetPos;
			offsetPos.x = cos(rad);
			offsetPos.y = sin(rad);
			offsetPos *= dist;

			ML::Box2D test = OL::setBoxCenter(128, 148).OffsetCopy(spos + offsetPos);
			if (!ge->qa_Map->CheckHit(test)
				&& !ge->qa_Map->CheckFallGround(test)
				&& test.Hit(ge->GetScreenBox())) {
				spos += offsetPos;
				break;
			}
		}

		em->SpawnEnemyNum(GetRandom<int>(0, 1), spos, depth_);

	}

	bool Object::PosInMyCircle(const ML::Vec2 me_, const ML::Vec2 you_, const float dist_) {
		float x = you_.x - me_.x;
		float y = you_.y - me_.y;

		return x * x + y * y < dist_ * dist_;
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