//?------------------------------------------------------
//タスク名:敵管理タスク
//作　成　者:22CI0333 長谷川勇一朗
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:2023/12/1
//概　　　要:敵を管理する
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyManager.h"

#include  "Task_EnemySkeleton.h"
#include  "Task_EnemySkyEye.h"

namespace  EnemyManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		//****************************************
		//ファイルから受け付けるもの
		//・体力
		//・ジャンプ力
		//・左右 最大速度
		//・左右 加速度
		//・左右 減衰量
		// 
		//・無敵時間
		// 
		//・ドロップするお金の量
		//・攻撃力
		// 
		//****************************************
		ifstream fenemies("./data/enemy/enemy.json");
		if (!fenemies.is_open()) return false;//ファイルオープンに失敗
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



		//TODO: 新たに敵を追加をする際にここに追加。
		// 必ず敵の名前は大文字で始めること
		//記入例）this->enemyInits["Name"] = EnemyName::Object::Create;
		this->enemyInits["Skeleton"] = EnemySkeleton::Object::Create;
		this->enemyInits["SkyEye"] = EnemySkyEye::Object::Create;
		
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->enemyDatas.clear();
		this->enemyNames.clear();
		this->enemyInits.clear();
		this->stateRates.clear();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->residentResource.push_back(EnemySkeleton::Resource::Create());
		this->residentResource.push_back(EnemySkyEye::Resource::Create());
		//★タスクの生成
		SpawnEnemyName("Skeleton",ML::Vec2(1000, 600));
		SpawnEnemyName("SkyEye", ML::Vec2(1300, 500));
		ge->debugRectLoad();
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->residentResource.clear();
		ge->debugRectReset();
		ge->KillAll_G("Enemy");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		ge->qa_Enemys = ge->GetTasks<BEnemy>("Enemy");
 
#ifdef DEBUG_ENEMY
		//TODO:デバッグ機能、マスターまでに消去すること
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
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->debugRectDraw();
	}

	//-------------------------------------------------------------------
	// 敵スポーン
	

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

			//倍率設定
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

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
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
	//リソースクラスの生成
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