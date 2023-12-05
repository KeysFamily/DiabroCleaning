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
		ifstream f("./data/enemy/enemy.json");
		if (!f.is_open()) return false;//ファイルオープンに失敗
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
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->enemyDatas.clear();
		this->enemyNames.clear();
		this->enemyInits.clear();
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
		//★タスクの生成
		SpawnEnemy(ML::Vec2(1000, 600));
		ge->debugRectLoad();

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->residentResource.clear();
		ge->KillAll_G("Enemy");
		ge->debugRectReset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto ms = ge->mouse->GetState();
		ge->qa_Enemys = ge->GetTasks<BEnemy>("Enemy");
 
		//TODO:デバッグ機能、マスターまでに消去すること
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
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->debugRectDraw();
	}

	//-------------------------------------------------------------------
	// 敵スポーン
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