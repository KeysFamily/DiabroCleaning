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
#include  "Task_PlayerStatus.h"
#include  "Task_Price.h"
#include  "Task_SystemMenuMessageWindow.h"
#include  "sound.h"

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
		this->render2D_Priority[1] = 0.5f;

		this->displayStr = "";
		this->progressDistance = 10;
		this->statusLvMax = 5;
		this->currentStatus.SetValues(0, 0, statusLvMax - 1);

		for (int i = 0; i <= currentStatus.vmax; ++i)
		{
			price.push_back(i + 1000 + i * 1658);
			addStatus.push_back(1 + i);
		}

		this->selectScale = 1.5f;

		this->displayPos = ML::Vec2(10, -40);
		this->progressBeginPos.x = this->res->imgProgressSize.w * (currentStatus.vmax - 1) / -2.0f;
		this->progressBeginPos.x += this->progressDistance * currentStatus.vmax / -2.0f;
		this->progressBeginPos.y = 30;
		
		this->priceDpPos = ML::Vec2(15, -10);
		this->priceDp = Price::Object::Create(true);

		this->selectCount = 0;

		this->staffTalkFileName = "";
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
		if (this->currentStatus.vnow + 1 <= this->currentStatus.vmax)
		{
			this->priceDp->price = this->price[this->currentStatus.vnow + 1];
			this->priceDp->pos = this->priceDpPos + this->pos;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//ステータス名描画
		ML::Box2D draw = OL::setBoxCenter(
			(int)(this->res->fontDisplaySize.w * 1.5f) * static_cast<int>(this->displayStr.size()),
			this->res->fontDisplaySize.h
		);
		draw.Offset(this->pos + this->displayPos);

		this->res->fontDisplay->Draw(draw, this->displayStr);
		
		//追加ステータス描画
		draw = OL::setBoxCenter(this->res->imgProgressSize);
		draw.Offset(this->pos + this->progressBeginPos);
		ML::Box2D src(0, 0, this->res->imgProgressSize.w, this->res->imgProgressSize.h);

		for (int i = 0; i <= currentStatus.vmax; ++i)
		{
			if (i <= currentStatus.vnow)
			{
				src.x = this->res->imgProgressSize.w * (2 + this->statusType);
			}
			else if (i == currentStatus.vnow + 1)
			{
				src.x = this->res->imgProgressSize.w;
			}
			this->res->imgProgress->Draw(draw, src);
			draw.x += static_cast<int>(this->res->imgProgressSize.w + this->progressDistance);
			src.x = 0;
		}
	}
	//-------------------------------------------------------------------
	//その他の関数
	//次の購入に必要な金額を返す
	bool Object::LoadShopData(const string& fileName_)
	{
		ifstream ifs(("./data/SystemMenu/Status/") + fileName_ + ".txt");
		if (!ifs)
		{
			return false;
		}

		price.clear();
		addStatus.clear();
		statusLvMax = 0;

		while (ifs)
		{
			int readPrice;
			int readAddStatus;
			ifs >> readPrice;

			if (!ifs)
			{
				break;
			}

			ifs >> readAddStatus;

			price.push_back(readPrice);
			addStatus.push_back(readAddStatus);
			++statusLvMax;
		}

		this->currentStatus.SetValues(0, 0, statusLvMax - 1);
		this->progressBeginPos.x = this->res->imgProgressSize.w * (currentStatus.vmax - 1) / -2.0f;
		this->progressBeginPos.x += this->progressDistance * currentStatus.vmax / -2.0f;

		return true;
	}

	//店員の会話ファイルを設定する
	void Object::SetStaffTalkFile(const string& fileName_)
	{
		this->staffTalkFileName = fileName_;
	}

	//次の購入に必要な金額を返す
	int Object::GetPrice() const
	{
		if (this->currentStatus.IsMax())
		{
			return 0;
		}
		else
		{
			return price[currentStatus.vnow];
		}
	}

	//現在の追加ステータスを取得する
	int Object::GetStatusAdd() const
	{
		return addStatus[currentStatus.vnow];
	}

	//購入する
	bool Object::Buy(int& money_)
	{
		if (this->currentStatus.IsMax())
		{
			return false;
		}
		else if (money_ >= price[currentStatus.vnow + 1])
		{
			money_ -= price[currentStatus.vnow + 1];
			ge->TotalUsedCoinCnt += price[currentStatus.vnow + 1];
			currentStatus.Addval(1);
			if (currentStatus.IsMax())
			{
				this->priceDp->active = false;
			}
			se::Stop("bought");
			se::Play("bought");
			return true;
		}

		auto msg = ge->GetTask<SystemMenuMessageWindow::Object>("SystemMenu", "MessageWindow");
		msg->SetMessage("cantBuy");
		se::Stop("errorSelect");
		se::Play("errorSelect");
		return false;
	}

	//サイズと位置を伝える
	ML::Box2D Object::GetObjectSize() const
	{
		ML::Box2D result = OL::setBoxCenter(this->res->imgProgressSize);
		result.Offset(this->pos + this->progressBeginPos);
		if (this->currentStatus.IsMax())
		{
			result.x += static_cast<int>(this->res->imgProgressSize.w + this->progressDistance) * (this->currentStatus.vnow);
		}
		else
		{
			result.x += static_cast<int>(this->res->imgProgressSize.w + this->progressDistance) * (this->currentStatus.vnow + 1);
		}
		result.w *= this->selectScale;
		result.h *= this->selectScale;
		result.x -= (result.w - this->res->imgProgressSize.w) / 2;
		result.y -= (result.h - this->res->imgProgressSize.h) / 2;

		return result;
	}

	//ターゲット中か
	void Object::IsSelecting()
	{
		if (this->selectCount == 0)
		{
			auto msg = ge->GetTask<SystemMenuMessageWindow::Object>("SystemMenu", "MessageWindow");
			msg->SetMessage(this->staffTalkFileName);
		}
		++this->selectCount;
	}

	//ターゲット終了時の処理
	void Object::FinishSelect()
	{
		this->selectCount = 0;
	}

	//ボタンが押されたか
	void Object::IsDown()
	{
		if (ge->qa_Player != nullptr)
		{
			if (this->Buy(ge->qa_Player->balanceMoney) == false)
			{
				se::Stop("errorSelect");
				se::Play("errorSelect");
			}
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