//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "sound.h"

#include  "Task_Effect00.h"
#include "Task_MapManager.h"
#include  "Task_TitleMenu.h"


#include "Task_Game.h"
#include "Task_Ending.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		bgm::LoadFile("bgmTitle", "./data/sound/bgm/titleUra_bgm.mp3");
		bgm::Play("bgmTitle");
		this->img = DG::Image::Create("./data/title/Diobro_Cleaning_title.png");
		this->Logo = DG::Image::Create("./data/title/title_text.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
		this->Logo.reset();
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
		ge->GameCnt=0; 
		ge->TotalEnemyKill=0; 
		ge->TotalDamage = 0;
		ge->TotalGetCoinCnt = 0;
		ge->TotalUsedCoinCnt=0;
		ge->GameClearFlag = true;
		
		this->createdMenu = false;

		MapManager::Object::Create(true);
		
		return true;
	}	
	//--TotalGetCoinCnt; -----------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		bgm::Stop("bgmTitle");
		ge->KillAll_G("title");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		this->cnt++;

		auto tm = ge->GetTask<TitleMenu::Object>("title", "Menu");
		if (tm == nullptr)
		{
			createdMenu = false;
		}

		if (inp.ST.down ) 
		{
			if (createdMenu == false)
			{
				createdMenu = true;
				TitleMenu::Object::Create(true);
			}
			else
			{
				createdMenu = false;
				ge->KillAll_GN("title", "Menu");
			}
		}
		if (ge->getCounterFlag("title") == ge->LIMIT) {
			this->Kill();
		}


		return;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, 1920, 1080);
		ML::Box2D src(0, 0, 1920, 1080);
		this->res->img->Draw(draw, src);	//背景を用意する

		if (createdMenu == false)
		{
			if (this->cnt % 60 <= 30) {
				ML::Box2D Lg_draw((ge->screen2DWidth / 2) - 300, (ge->screen2DHeight / 4) * 3, 600, 100);
				ML::Box2D Lg_src(0, 0, 400, 74);
				this->res->Logo->Draw(Lg_draw, Lg_src); //テキストロゴを用意する
			}
		}

		ge->Dbg_ToDisplay(100, 100, "Title");
	}
	//-------------------------------------------------------------------
	//ゲーム生成処理
	void Object::CreateGame(int mapMaxDepth_)
	{
		if (ge->getCounterFlag("title") == ge->ACTIVE)
		{
			return;
		}

		auto manager = ge->GetTask<MapManager::Object>("MapManager");
		manager->SetMaxDepth(mapMaxDepth_);

		ge->StartCounter("title", 45); //フェードは90フレームなので半分の45で切り替え
		ge->CreateEffect(98, ML::Vec2(0, 0));
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