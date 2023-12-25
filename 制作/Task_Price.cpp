//?------------------------------------------------------
//タスク名:値段表示タスク
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Price.h"

namespace  Price
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgCoin = DG::Image::Create("./data/image/coin.png");
		this->imgCoinSize.Set(32, 32);
		this->imgNum = DG::Image::Create("./data/image/font_number.png");
		this->imgNumSize.Set(20, 26);
		this->srcNumBeginPos = ML::Vec2(0, 35);

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgCoin.reset();
		this->imgNum.reset();
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
		this->price = 0;
		this->distance = 10;
		this->pos = ML::Vec2(0, 0);
		this->denyColor = ML::Color(1, 1, 0.5f, 0.5f);
		this->active = true;
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
		if (active == false)
		{
			return;
		}

		//桁数の計算
		int numPlace = 1;
		int ofsX = 0;
		while (price / numPlace >= 10)
		{
			ofsX -= this->res->imgNumSize.h / 2;
			numPlace *= 10;
		}

		//コインの描画
		ML::Box2D draw = OL::setBoxCenter(this->res->imgCoinSize);
		ML::Box2D src(0, 0, this->res->imgCoinSize.w, this->res->imgCoinSize.h);
		draw.Offset(this->pos);
		draw.x -= this->res->imgCoinSize.w / 2;
		draw.x += ofsX;

		this->res->imgCoin->Draw(draw, src);

		//価格の描画
		ML::Color color(1, 1, 1, 1);
		if (ge->qa_Player != nullptr)
		{
			if (ge->qa_Player->balanceMoney < this->price)
			{
				color = this->denyColor;
			}
		}

		draw = OL::setBoxCenter(this->res->imgNumSize);
		src = ML::Box2D(0, 0, this->res->imgNumSize.w, this->res->imgNumSize.h);
		draw.Offset(this->pos);
		src.Offset(this->res->srcNumBeginPos);
		draw.x += this->res->imgNumSize.w / 2;
		draw.x += ofsX;

		while (numPlace > 0)
		{
			src.x = this->res->imgNumSize.w * (price / numPlace % 10);
			this->res->imgNum->Draw(draw, src, color);
			
			draw.x += this->res->imgNumSize.w;
			numPlace /= 10;
		}
	}
	//-------------------------------------------------------------------
	//その他関数定義
	void Object::SetPrice(int price_)
	{
		this->price = price_;
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