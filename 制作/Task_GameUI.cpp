//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GameUI.h"
#include  "Task_Player.h"

namespace  GameUI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->coinImg = DG::Image::Create("./data/image/coin.png");
		this->num = DG::Image::Create("./data/image/font_number.png");
		this->magic = DG::Image::Create("./data/effect/magicSelect.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->coinImg.reset();
		this->num.reset();
		this->magic.reset();
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
		this->render2D_Priority[1] = 0.2f;
		this->coinPos = ML::Vec2(0, 0);
		this->numPos = ML::Vec2(0, 0);
		this->magicPos = ML::Vec2(0, 0);
		this->num = 0;
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto player = ge->GetTask<BChara>("Player");
		this->num = player->balanceMoney;
		this->coinPos = ML::Vec2(32, 35);
		this->numPos = ML::Vec2(50,20);
		this->magicPos = ML::Vec2(224, 40);

		auto pl = ge->GetTask<Player::Object>("Player");
		this->magicSelect = pl->magicSelect + 1;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		char nums[7];
		sprintf(nums, "%06d", this->num);
		//8桁の数字表示
		for (int i = 0; i < 6; ++i) {
			ML::Box2D draw(i * 20, 0, 20, 32);
			ML::Box2D src((nums[i] - '0') * 20, 32, 20, 32);
			this->res->num->Draw(draw.OffsetCopy(numPos), src);
		}
		ML::Box2D coin_draw(-20, -20, 40, 40);
		ML::Box2D coin_src(0, 0, 32, 32);
		this->res->coinImg->Draw(coin_draw.OffsetCopy(coinPos), coin_src);

		
		ML::Box2D magic_draw(-32, -32, 64, 64);
		ML::Box2D magic_src(0, this->magicSelect * 64, 64, 64);
		this->res->magic->Draw(magic_draw.OffsetCopy(magicPos), magic_src);
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