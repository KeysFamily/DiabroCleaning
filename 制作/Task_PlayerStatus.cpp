//?------------------------------------------------------
//タスク名:プレイヤーステータス画面
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "MyPG.h"
#include "Task_PlayerStatus.h"
#include "Task_PlayerStatusShop.h"
#include "Task_SystemMenuSelectObject.h"

namespace  PlayerStatus
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/menu/status/BackGround.png");
		this->imgBgSize.Set(960, 674);

		this->systemFontSize.Set(32, 64);
		this->systemFont = DG::Font::Create("ＭＳ ゴシック", systemFontSize.w, systemFontSize.h);

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->systemFont.reset();
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
		this->pos = ML::Vec2(500, 560);
		this->shopDistance = 120;
		this->shopOffset = ML::Vec2(190, 30);

		for (int i = 0; i < 4; ++i)
		{
			this->shops[i] = PlayerStatusShop::Object::Create(true);
			this->shops[i]->pos = this->pos + ML::Vec2(0, -(shopDistance + shopDistance / 2));
			this->shops[i]->pos += shopOffset;
			this->shops[i]->pos.y += shopDistance * i;
			this->shops[i]->statusType = i;
		}

		//繋がりの設定
		this->shops[0]->SetNext_Down(this->shops[1].get());
		this->shops[1]->SetNext_Up(this->shops[0].get());
		this->shops[1]->SetNext_Down(this->shops[2].get());
		this->shops[2]->SetNext_Up(this->shops[1].get());
		this->shops[2]->SetNext_Down(this->shops[3].get());
		this->shops[3]->SetNext_Up(this->shops[2].get());

		//値の設定
		this->LoadShopFile("./data/SystemMenu/Status/shopData.json");

		this->currentStatus = 0;

		this->statusBeginPos = ML::Vec2(-300, 6);
		this->statusDistance = 74;

		this->currentShop = shops[0].get();

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
		this->ShopUpdate();

		if (ge->qa_Player != nullptr)
		{
			ge->qa_Player->power = this->shops[ATK]->GetStatusAdd();
			ge->qa_Player->DEF = this->shops[DEF]->GetStatusAdd();
			ge->qa_Player->INT = this->shops[INT]->GetStatusAdd();
			ge->qa_Player->speed = this->shops[SPD]->GetStatusAdd();
		}

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw = OL::setBoxCenter(this->res->imgBgSize);
		ML::Box2D src(0, 0, this->res->imgBgSize.w, this->res->imgBgSize.h);
		draw.Offset(this->pos);

		this->res->imgBg->Draw(draw, src);

		if (ge->qa_Player == nullptr)
		{
			return;
		}

		draw = ML::Box2D(0, 0, 500, 500);
		draw.Offset(this->pos + this->statusBeginPos);

		this->res->systemFont->Draw(draw, to_string((int)ge->qa_Player->power));
		draw.y += this->statusDistance;
		this->res->systemFont->Draw(draw, to_string((int)ge->qa_Player->DEF));
		draw.y += this->statusDistance;
		this->res->systemFont->Draw(draw, to_string((int)ge->qa_Player->INT));
		draw.y += this->statusDistance;
		this->res->systemFont->Draw(draw, to_string((int)ge->qa_Player->speed));
	}
	//-------------------------------------------------------------------
	//その他メソッド
	//ショップデータを読み込む
	bool Object::LoadShopFile(const string& filePath_)
	{
		ifstream ifs(filePath_);
		if (!ifs)
		{
			return false;
		}
		
		json js;
		ifs >> js;
		ifs.close();

		for (auto& ji : js["statusShop"])
		{
			shops[ji["id"]]->displayStr = ji["name"];
			shops[ji["id"]]->LoadShopData(ji["paramFile"]);
			shops[ji["id"]]->SetStaffTalkFile(ji["talkFile"]);
		}
	}
	//ショップの座標等の更新
	void Object::ShopUpdate()
	{
		int idxShop = 0;
		for (auto& shop : shops)
		{
			//位置設定
			shop->pos = this->pos + ML::Vec2(0, -(shopDistance + shopDistance / 2));
			shop->pos += shopOffset;
			shop->pos.y += shopDistance * idxShop;

			//現在のショップを取得
			if (shop->selectCount > 0)
			{
				this->currentShop = shop.get();
			}
			++idxShop;
		}
	}


	void Object::SetDownObj(MyUI::SelectableObject* nextObj_)
	{
		{
			this->shops[3]->SetNext_Down(nextObj_);
		}
	}
	void Object::SetRightObj(MyUI::SelectableObject* nextObj_)
	{
		for (auto& shop : shops)
		{
			shop->SetNext_Right(nextObj_);
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