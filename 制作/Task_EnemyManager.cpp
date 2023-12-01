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
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		SpawnEnemy();


		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->residentResource.clear();
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
		auto ms = ge->mouse->GetState();
		ge->qa_Enemys = ge->GetTasks<BEnemy>("Enemy");
 
		//TODO:デバッグ機能、マスターまでに消去すること
		if (ms.LB.down) {
			ML::Vec2 spos;
			spos.x = ms.pos.x + ge->camera2D.x;
			spos.y = ms.pos.y + ge->camera2D.y;

			auto sk = EnemySkeleton::Object::Create(true);
			sk->pos = spos;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}

	//-------------------------------------------------------------------
	// 敵スポーン
	void Object::SpawnEnemy() {
		auto sk = EnemySkeleton::Object::Create(true);
		sk->pos.x = 1100;
		sk->pos.y = 500;
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