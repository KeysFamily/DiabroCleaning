//?------------------------------------------------------
//�^�X�N��:�G�Ǘ��^�X�N
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2023/12/1
//�T�@�@�@�v:�G���Ǘ�����
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyManager.h"

#include  "Task_EnemySkeleton.h"
#include  "Task_EnemySkyEye.h"

namespace  EnemyManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		//****************************************
		//�t�@�C������󂯕t�������
		//�E�̗�
		//�E�W�����v��
		//�E���E �ő呬�x
		//�E���E �����x
		//�E���E ������
		// 
		//�E���G����
		// 
		//�E�h���b�v���邨���̗�
		//�E�U����
		// 
		//****************************************
		ifstream fenemies("./data/enemy/enemy.json");
		if (!fenemies.is_open()) return false;//�t�@�C���I�[�v���Ɏ��s
		json edata = json::parse(fenemies);
		for (auto& e : edata["enemies"]) {
			string en = e["name"];
			EnemyData ed;
			ed.hp        = e["hp"];
			ed.jumpPow   = e["jumpPow"];
			ed.maxSpeed  = e["maxSpeed"];
			ed.addSpeed  = e["addSpeed"];
			ed.decSpeed  = e["decSpeed"];
			ed.unHitTime = e["unHitTime"];
			ed.dropMoney = e["dropMoney"];
			ed.attackPow = e["attack"];

			this->enemyDatas[en] = ed;
			this->enemyNames.push_back(en);
		}
		fenemies.close();

		ifstream f("./data/enemy/stateRate.json");
		if (!f.is_open())return false;
		json esdata = json::parse(f);
		for (int i = 1; i <= 3; ++i) {
			string rate = "Rate" + to_string(i);

			for (auto& e : esdata[rate]) {
				string en = e["name"];
				EnemyStatusRate esr;
				esr.hpRate     = e["hpRate"];
				esr.speedRate  = e["speedRate"];
				esr.moneyRate  = e["moneyRate"];
				esr.attackRate = e["attackRate"];
				
				this->stateRates[i][en] = esr;
			}
		}



		//TODO: �V���ɓG��ǉ�������ۂɂ����ɒǉ��B
		// �K���G�̖��O�͑啶���Ŏn�߂邱��
		//�L����jthis->enemyInits["Name"] = EnemyName::Object::Create;
		this->enemyInits["Skeleton"] = EnemySkeleton::Object::Create;
		this->enemyInits["SkyEye"] = EnemySkyEye::Object::Create;
		
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->enemyDatas.clear();
		this->enemyNames.clear();
		this->enemyInits.clear();
		this->stateRates.clear();
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
		this->residentResource.push_back(EnemySkeleton::Resource::Create());
		this->residentResource.push_back(EnemySkyEye::Resource::Create());
		//���^�X�N�̐���
		SpawnEnemyName("Skeleton",ML::Vec2(1000, 600));
		SpawnEnemyName("SkyEye", ML::Vec2(1300, 500));
		ge->debugRectLoad();
		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		this->residentResource.clear();
		ge->debugRectReset();
		ge->KillAll_G("Enemy");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		ge->qa_Enemys = ge->GetTasks<BEnemy>("Enemy");
 
#ifdef DEBUG_ENEMY
		//TODO:�f�o�b�O�@�\�A�}�X�^�[�܂łɏ������邱��
		auto ms = ge->mouse->GetState();
		ML::Vec2 spos;
		spos.x = ms.pos.x + ge->camera2D.x;
		spos.y = ms.pos.y + ge->camera2D.y;
		if (ms.LB.down) {
			for (auto it = ge->qa_Enemys->begin(); it != ge->qa_Enemys->end(); ++it) {
				if ((*it)->CallHitBox().Hit(spos)) {
					BChara::AttackInfo at = { INT_MAX,0,0 };
					(*it)->Received(nullptr, at);
				}
			}
		}
		if (ms.CB.down) {
			//for (auto it = ge->qa_Enemys->begin(); it != ge->qa_Enemys->end(); ++it) {
			//	BChara::AttackInfo at = { INT_MAX,0,0 };
			//	(*it)->Received(nullptr, at);
			//}
			this->KillAllEnemys();
		}
		if (ms.RB.down) {
			this->SpawnEnemyNum(rand() % 2, spos);
		}
#endif // DEBUG_ENEMY
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ge->debugRectDraw();
	}

	//-------------------------------------------------------------------
	// �G�X�|�[��
	

	void Object::SpawnEnemyNum(int enemyNum_, ML::Vec2 pos_, int depth_) {
		int size = this->res->enemyNames.size();
		if (enemyNum_ < 0 || enemyNum_ >= size)return;

		string name = this->res->enemyNames[enemyNum_];

		this->SpawnEnemyName(name, pos_, depth_);
	}

	void Object::SpawnEnemyName(string name_, ML::Vec2 pos_, int depth_) {
		if (this->res->enemyDatas.count(name_) > 0 &&
			this->res->enemyInits.count(name_) > 0) {
			
			auto e       = this->res->enemyInits[name_](true);
			e->pos       = pos_;
			float HP       = this->res->enemyDatas[name_].hp;
			
			e->jumpPow   = this->res->enemyDatas[name_].jumpPow;
			e->maxSpeed  = this->res->enemyDatas[name_].maxSpeed;
			e->addSpeed  = this->res->enemyDatas[name_].addSpeed;
			e->decSpeed  = this->res->enemyDatas[name_].decSpeed;
			e->unHitTime = this->res->enemyDatas[name_].unHitTime;
			e->dropMoney = this->res->enemyDatas[name_].dropMoney;
			e->attackPow = this->res->enemyDatas[name_].attackPow;

			//�{���ݒ�
			int Rate = 1;

			HP *= this->res->stateRates[depth_][name_].hpRate;

			e->hp.SetValues(HP, 0, HP);
			e->maxSpeed *= this->res->stateRates[depth_][name_].speedRate;
			e->addSpeed *= this->res->stateRates[depth_][name_].speedRate;
			e->decSpeed *= this->res->stateRates[depth_][name_].speedRate;

			e->dropMoney *= this->res->stateRates[1][name_].moneyRate;
			e->attackPow *= this->res->stateRates[1][name_].attackRate;

		}
	}

	void Object::KillAllEnemys() {
		if (ge->qa_Enemys == nullptr)return;
		for (auto it = ge->qa_Enemys->begin(); it != ge->qa_Enemys->end(); ++it) {
			(*it)->Kill();
		}
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