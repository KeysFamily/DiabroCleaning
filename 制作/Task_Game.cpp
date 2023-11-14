//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"

#include  "Task_Player.h"
#include  "Task_Map.h"
#include  "Task_Sprite.h"

#include "Task_EnemySkeleton.h"

#include  "Task_Ending.h"

namespace  Game
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

		//★タスクの生成
		auto player = Player::Object::Create(true);

		player->pos.x = 1200;
		player->pos.y = 500;
		player->render2D_Priority[1] = 0.5f;
		auto map = Map::Object::Create(true);
		map->render2D_Priority[1] = 0.9f;
		auto spr = Sprite::Object::Create(true);
		spr->pos = player->pos;
		spr->target = player;
		spr->render2D_Priority[1] = 0.6f;

		auto sk = EnemySkeleton::Object::Create(true);
		sk->pos.x = 1100;
		sk->pos.y = 500;

		ge->camera2D.x = 0;
		ge->camera2D.y = 0;
		ge->camera2D.w = ge->screenWidth;
		ge->camera2D.h = ge->screenHeight;


		


		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("本編");
		ge->KillAll_G(Player::defGroupName);
		ge->KillAll_G(Map::defGroupName);
		ge->KillAll_G(Sprite::defGroupName);
		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto next = Ending::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState( );
		if (inp.ST.down && ge->getCounterFlag("Game") != ge->ACTIVE) {
			ge->StartCounter("Game", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));

		}
		if (ge->getCounterFlag("Game") == ge->LIMIT) {
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Game");
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