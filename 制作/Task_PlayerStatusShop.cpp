//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_PlayerStatusShop.h"

namespace  PlayerStatusShop
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgProgress = DG::Image::Create("./data/image/menu/status/Progress.png");
		this->imgProgressSize.Set(14, 42);
		this->fontDisplaySize.Set(14, 32);
		this->fontDisplay = DG::Font::Create("a", this->fontDisplaySize.w, this->fontDisplaySize.h);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgProgress.reset();
		this->fontDisplay.reset();
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
		this->displayStr = "ATK";
		this->currentStatus.SetValues(0, 0, 10);
		for (int i = 0; i < 10; ++i)
		{
			price.push_back(i + 1);
			addStatus.push_back(1 + i);
		}

		this->pos = ML::Vec2(200, 200);
		this->displayPos = ML::Vec2(0, -50);
		this->progressBeginPos = ML::Vec2(-100, 50);
		this->progressDistance = 10;
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
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//ステータス名描画
		ML::Box2D draw = OL::setBoxCenter(
			(int)(this->res->fontDisplaySize.w * 1.5f) * this->displayStr.size(),
			this->res->fontDisplaySize.h
		);
		draw.Offset(this->pos + this->displayPos);

		this->res->fontDisplay->Draw(draw, this->displayStr);
		
		//追加ステータス描画
		draw = OL::setBoxCenter(this->res->imgProgressSize);
		draw.Offset(this->pos + this->progressBeginPos);
		ML::Box2D src(0, 0, this->res->imgProgressSize.w, this->res->imgProgressSize.h);

		for (int i = 0; i < currentStatus.vmax; ++i)
		{
			this->res->imgProgress->Draw(draw, src);
			draw.x += this->res->imgProgressSize.w + this->progressDistance;
		}
	}
	//-------------------------------------------------------------------
	//その他の関数
	//次の購入に必要な金額を返す
	int Object::GetPrice() const
	{
		if (this->currentStatus.IsMax())
		{
			return 0;
		}
		else
		{
			return price[currentStatus.vnow + 1];
		}
	}

	//購入する
	bool Object::Buy(int& money_)
	{
		if (this->currentStatus.IsMax())
		{
			return 0;
		}
		if (money_ >= price[currentStatus.vnow + 1])
		{
			money_ -= price[currentStatus.vnow + 1];
			currentStatus.Addval(1);
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