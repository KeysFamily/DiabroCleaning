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

#include  "randomLib.h"

namespace  EnemyBoss
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		//this->img = DG::Image::Create("./data/enemy/__.png");
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
		BChara::AttackInfo at = {1,0,0};
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
			break;
		case BossMotion::Teleport:
			break;
		case BossMotion::CoolTime:
			break;
		case BossMotion::Attack:
		{
			vector<BossMotion> sheet;
			sheet.push_back(BossMotion::Attack1);
			if (this->hp.GetRate() <= 0.5f) {//50%�ȉ��ő��i�K
				sheet.push_back(BossMotion::Attack2);
			}
			if (this->hp.GetRate() <= 0.25f) {//25%�ȉ��ő�O�i�K
				sheet.push_back(BossMotion::Attack3);
			}
			sheet.push_back(BossMotion::Attack4);
			nm = sheet[GetRandom<int>(1, sheet.size())];
		}
			break;
		case BossMotion::Attack1:
			break;
		case BossMotion::Attack2:
			break;
		case BossMotion::Attack3:
			break;
		case BossMotion::Attack4:
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

		switch (this->motion)
		{
		case BossMotion::Stand:
			break;
		case BossMotion::Teleport:
			break;
		case BossMotion::CoolTime:
			break;
		case BossMotion::Attack:
			break;
		case BossMotion::Attack1:
			//�v���C���[�Ɍ����ă{�X�̏񂩂�΂̖��@���o��
			break;
		case BossMotion::Attack2:
			//�v���C���[�̑����ɖ��@���o���P�b��ɍU��������
			break;
		case BossMotion::Attack3:
			//���a�R�}�X�ȓ��ɔ͈͍U��
			break;
		case BossMotion::Attack4:
			//���a�T�}�X�ȓ��̃����_���Ȉʒu�ɓG����������
			//�����͈ȉ��̂Ƃ���Ƃ���B
			//
			if (this->hp.GetRate() > 0.5f) {
				//�R�̏���
			}
			else if (this->hp.GetRate() > 0.25f) {
				//�S�̏���
			}
			else {
				//�T�̏���
			}
			break;
		case BossMotion::Lose:
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
			{ML::Box2D(0, 0, 0, 0), ML::Box2D(0, 0, 0, 0), defColor},
		};
		return imageTable[0];
	}
	
	void Object::Received(BEnemy* from_, AttackInfo at_) {
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