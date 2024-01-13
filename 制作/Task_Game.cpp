//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"

#include  "Task_Player.h"
#include  "Task_Map.h"
#include  "Task_Sprite.h"
#include  "Task_Item_coin.h"
#include  "Task_Item_coin_maneger.h"
#include  "Task_EnemyManager.h"
#include  "Task_Ending.h"
#include  "Task_GameUI.h"
#include  "Task_MapManager.h"
#include  "Task_GameUI_MiniMap.h"
#include  "Task_SystemMenu.h"

#include  "sound.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->haikei = DG::Image::Create("./data/image/haikei.jpg");
		se::LoadFile("enemyDead", "./data/sound/se/se_enemyDead.wav");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->haikei.reset();
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
		this->render2D_Priority[1] = 1;
		// ◇◇◇◇◇◇◇◇◇◇
		//22ci0308
		bgm::LoadFile("bgm3", "./data/sound/bgm/industrial_zone.mp3");
		bgm::Play("bgm3");
		//this->volume.SetValues(100, 0, 100);
		// ◆◆◆◆◆◆◆◆◆◆

		//★タスクの生成
		auto player = Player::Object::Create(true);
		player->pos.x = 1200;
		player->pos.y = 500;
		player->render2D_Priority[1] = 0.5f;

		MapManager::Object::Create(true);
		MiniMap::Object::Create(true);
		
		auto map = Map::Object::Create(true);
		map->render2D_Priority[1] = 0.9f;

		auto spr = Sprite::Object::Create(true);
		spr->pos = player->pos;
		spr->target = player;
		spr->render2D_Priority[1] = 0.6f;

		ge->camera2D.x = 0;
		ge->camera2D.y = 0;
		ge->camera2D.w = ge->screenWidth;
		ge->camera2D.h = ge->screenHeight;

		auto coin_man = coin_maneger::Object::Create(true);

		auto UI = GameUI::Object::Create(true);
		UI->numPos = ML::Vec2(50, 50);

		EnemyManager::Object::Create(true);

		this->cnt = 0;

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{

		//★データ＆タスク解放
		ge->KillAll_G("本編");
		ge->KillAll_G("item");
		ge->KillAll_G("coin_maneger");
		ge->KillAll_G("UI");
		ge->KillAll_G("GameUI");
		ge->KillAll_G("Enemy");
		ge->KillAll_G("EnemyManager");
		ge->KillAll_G("Player");
		ge->KillAll_G("Map");
		ge->KillAll_G("MapManager");
		ge->KillAll_G("MapTransition");
		ge->KillAll_G("Sprite");
		ge->KillAll_G("MagicManager");
		ge->KillAll_G("Magic");
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
		//メニュー画面を開いているときの処理
		if (this->openingMenu == true)
		{
			if (this->CheckFinishedMenu())
			{
				this->ResumeGameObj();
				bgm::Play("bgm3");
				this->openingMenu = false;
				auto gameObjs = this->GetGameObj();
				for (auto& gameObj : *gameObjs)
				{
					gameObj->render2D_Priority[1] -= 1;
				}
				this->render2D_Priority[1] -= 1;
			}
			else
			{
				return;
			}
		}


		//(22CI0333)他のタスクで以下の処理は行わなくてよい
		ge->qa_Player = ge->GetTask<Player::Object>(Player::defGroupName, Player::defName);
		ge->qa_Map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);

		auto inp = ge->in1->GetState( );

		this->cnt++;

		if (inp.ST.down) {
			//◇◇◇◇◇◇◇◇◇◇
			//以下22CI0329記述
			SystemMenu::Object::Create(true);
			bgm::Pause("bgm3");
			auto gameObjs = this->GetGameObj();
			for (auto& gameObj : *gameObjs)
			{
				gameObj->render2D_Priority[1] += 1;
			}
			this->render2D_Priority[1] += 1;
			this->StopGameObj();
			this->openingMenu = true;
			return;
			// ◆◆◆◆◆◆◆◆◆◆
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, 1920, 1080);
		ML::Box2D src(0, 0, 1920, 1080);
		
		this->res->haikei->Draw(draw, src);	//仮で背景を用意する


		ge->Dbg_ToDisplay(100, 100, "Game");
	}
	//-------------------------------------------------------------------
	//その他のメソッド
	void Object::StopGameObj()
	{
		ge->StopAll_G("item");
		ge->StopAll_G("coin_maneger");
		ge->StopAll_G("UI");
		ge->StopAll_G("GameUI");
		ge->StopAll_G("Enemy");
		ge->StopAll_G("EnemyManager");
		ge->StopAll_G("Player");
		ge->StopAll_G("Map");
		ge->StopAll_G("MapManager");
		ge->StopAll_G("MapTransition");
		ge->StopAll_G("Sprite");
		ge->StopAll_G("MagicManager");
		ge->StopAll_G("Magic");
	}

	void Object::ResumeGameObj()
	{
		ge->StopAll_G("item",false);
		ge->StopAll_G("coin_maneger",false);
		ge->StopAll_G("UI",false);
		ge->StopAll_G("GameUI",false);
		ge->StopAll_G("Enemy",false);
		ge->StopAll_G("EnemyManager",false);
		ge->StopAll_G("Player",false);
		ge->StopAll_G("Map",false);
		ge->StopAll_G("MapManager",false);
		ge->StopAll_G("MapTransition",false);
		ge->StopAll_G("Sprite",false);
		ge->StopAll_G("MagicManager",false);
		ge->StopAll_G("Magic", false);

	}

	bool Object::CheckFinishedMenu()
	{
		auto menu = ge->GetTask<SystemMenu::Object>("SystemMenu", "System");
		if (menu == nullptr)
		{
			return true;
		}

		return false;
	}

	void MergeObj(shared_ptr<vector<BTask::SP>> main_, shared_ptr<vector<BTask::SP>> sub_)
	{
		main_->insert(main_->end(), sub_->begin(), sub_->end());
		return;
	}

	shared_ptr<vector<BTask::SP>> Object::GetGameObj()
	{
		shared_ptr<vector<BTask::SP>> gameObjects = shared_ptr<vector<BTask::SP>>(new vector<BTask::SP>());

		MergeObj(gameObjects, ge->GetTasks<BTask>("item"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("coin_maneger"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("UI"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("GameUI"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("Enemy"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("EnemyManager"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("Player"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("Map"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("MapManager"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("MapTransition"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("Sprite"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("MagicManager"));
		MergeObj(gameObjects, ge->GetTasks<BTask>("Magic"));

		return gameObjects;
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