//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "sound.h"

#include  "Task_PlayerScore.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->Ending_clear_img = DG::Image::Create("./data/ending/Diobro_Cleaning_Game_clear.png");
		this->Ending_over_img = DG::Image::Create("./data/ending/Diobro_Cleaning_Game_over.png");
		this->Key_img = DG::Image::Create("./data/ending/PleaseKey.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->Ending_clear_img.reset();
		this->Ending_over_img.reset();
		this->Key_img.reset();
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
		this->KeyTime = 0;

		//ゲームクリア時とゲームオーバー時の効果音
		bgm::LoadFile("bgm_GameClear", "./data/sound/bgm/GameClear.mp3");
		bgm::LoadFile("bgm_GameOver", "./data/sound/bgm/GameOver.mp3");

		if (ge->GameClearFlag == true) {
			bgm::Play("bgm_GameClear");
		}else{
			bgm::Play("bgm_GameOver");
		}

		//★タスクの生成
		
		PlayerScore::Object::Create(true);
		
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		bgm::Stop("bgm_GameClear");
		bgm::Stop("bgm_GameOver");

		ge->KillAll_G("エンディング");
		bgm::AllStop();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		this->KeyTime++;
		if (KeyTime % 60 < 30) this->KeyOnOff = true; 
		else this->KeyOnOff = false;

		auto inp = ge->in1->GetState();
		if (inp.ST.down && ge->getCounterFlag("End") != ge->ACTIVE) {
			ge->StartCounter("End", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));

		}
		if (ge->getCounterFlag("End") == ge->LIMIT) {
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, 1920, 1080);
		ML::Box2D src(0, 0, 1920, 1080);

		ML::Box2D KeyDraw((ge->screen2DWidth/2)-250,ge->screen2DHeight*3/4,500,150);
		ML::Box2D KeySrc(0,0,372,97);
		if (ge->GameClearFlag == true) 	this->res->Ending_clear_img->Draw(draw, src);	//背景を用意する
		else this->res->Ending_over_img->Draw(draw, src);   //背景を用意する
		
		if(this->KeyOnOff==true)this->res->Key_img->Draw(KeyDraw, KeySrc);
		
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