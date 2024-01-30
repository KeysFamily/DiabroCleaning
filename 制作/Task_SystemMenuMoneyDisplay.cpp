//?------------------------------------------------------
//タスク名:所持金表示ディスプレイ
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SystemMenuMoneyDisplay.h"

namespace  SystemMenuMoneyDisplay
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/PriceDisplay/BackGround.png");
		this->imgBgSize.Set(640, 128);
		this->imgNum = DG::Image::Create("./data/image/Menu/PriceDisplay/font_number4x.png");
		this->imgNumSize.Set(40, 52);
		this->srcNumBeginPos = ML::Vec2(0, 70);
		this->imgCoin = DG::Image::Create("./data/image/Menu/PriceDisplay/coin4x.png");
		this->imgCoinSize.Set(128, 128);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->imgNum.reset();
		this->imgCoin.reset();
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
		this->render2D_Priority[1] = 0.6f;
		this->pos = ML::Vec2(420, 130);
		this->coinPos = ML::Vec2(-250, 0);
		this->moneyBeginPos = ML::Vec2(-160, 0);
		this->maxNumPlace = 100000000;
		this->moneyDisplay = 20;
		this->moneyMoveCnt.SetValues(0, 0, 30);
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
		if (ge->qa_Player != nullptr)
		{
			if (this->moneyDisplay != ge->qa_Player->balanceMoney)
			{
				if (this->moneyMoveCnt.IsMin())
				{
					this->moneyDifference = ge->qa_Player->balanceMoney - this->moneyDisplay;
				}

				this->moneyMoveCnt.Addval(1);
				float moveRate = (moneyMoveCnt.vmax - moneyMoveCnt.vnow) / (float)moneyMoveCnt.vmax;
				this->moneyDisplay = static_cast<int>(ge->qa_Player->balanceMoney - moneyDifference * moveRate);

				if (this->moneyMoveCnt.IsMax())
				{
					this->moneyMoveCnt.Setval(this->moneyMoveCnt.vmin);
					this->moneyDisplay = ge->qa_Player->balanceMoney;
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//背景の描画
		ge->DrawStd(this->res->imgBg, this->res->imgBgSize, this->pos);

		//コインの描画
		ge->DrawStd(this->res->imgCoin, this->res->imgCoinSize, this->pos + this->coinPos);

		//数字の描画
		int numPlace = maxNumPlace;
		ML::Box2D draw = OL::setBoxCenter(this->res->imgNumSize);
		ML::Box2D src(0, 0, this->res->imgNumSize.w, this->res->imgNumSize.h);
		draw.Offset(this->pos + this->moneyBeginPos);
		src.Offset(this->res->srcNumBeginPos);

		while (numPlace > 0)
		{
			src.x = this->res->imgNumSize.w * (moneyDisplay / numPlace % 10);
			this->res->imgNum->Draw(draw, src);

			draw.x += this->res->imgNumSize.w;
			numPlace /= 10;
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