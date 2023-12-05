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
		ifstream f("./data/enemy/enemy.json");
		if (!f.is_open()) return false;//�t�@�C���I�[�v���Ɏ��s
		json data = json::parse(f);
		for (auto& e : data["enemies"]) {
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
		f.close();

		this->enemyInits["Skeleton"] = EnemySkeleton::Object::Create;
		
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->enemyDatas.clear();
		this->enemyNames.clear();
		this->enemyInits.clear();
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
		//���^�X�N�̐���
		SpawnEnemy(ML::Vec2(1000, 600));
		ge->debugRectLoad();

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		this->residentResource.clear();
		ge->KillAll_G("Enemy");
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
		auto ms = ge->mouse->GetState();
		ge->qa_Enemys = ge->GetTasks<BEnemy>("Enemy");
 
		//TODO:�f�o�b�O�@�\�A�}�X�^�[�܂łɏ������邱��
		if (ms.LB.down) {
			ML::Vec2 spos;
			spos.x = ms.pos.x + ge->camera2D.x;
			spos.y = ms.pos.y + ge->camera2D.y;

#if false
			auto sk = EnemySkeleton::Object::Create(true);
			sk->pos = spos;
#else
			this->SpawnEnemy("Skeleton", spos);
#endif
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ge->debugRectDraw();
	}

	//-------------------------------------------------------------------
	// �G�X�|�[��
	void Object::SpawnEnemy(ML::Vec2 pos_) {

		auto e = EnemySkeleton::Object::Create(true);
		e->pos = pos_;
	}

	void Object::SpawnEnemy(string name_, ML::Vec2 pos_) {

		if (this->res->enemyDatas.count(name_) > 0 &&
			this->res->enemyInits.count(name_) > 0) {
			//auto e = EnemySkeleton::Object::Create(true);
			auto e       = this->res->enemyInits[name_](true);
			e->pos       = pos_;
			int HP       = this->res->enemyDatas[name_].hp;
			e->hp.SetValues(HP, 0, HP);
			e->jumpPow   = this->res->enemyDatas[name_].jumpPow;
			e->maxSpeed  = this->res->enemyDatas[name_].maxSpeed;
			e->addSpeed  = this->res->enemyDatas[name_].addSpeed;
			e->decSpeed  = this->res->enemyDatas[name_].decSpeed;
			e->unHitTime = this->res->enemyDatas[name_].unHitTime;
			//e->dropMoney = this->res->enemyDatas[name].dropMoney;
			//e->attackPow = this->res->enemyDatas[name].attackPow;
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