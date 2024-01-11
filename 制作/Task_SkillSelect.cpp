//?------------------------------------------------------
//タスク名:スキル選択
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SkillSelect.h"
#include  "Task_SkillShop.h"

namespace  SkillSelect
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/Skill/BackGround.png");
		this->imgBgSize.Set(768, 864);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
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
		this->pos = ML::Vec2(1400, 465);
		this->shopBeginPos = ML::Vec2(0, -220);
		this->shopDistance = 96;
		this->currentShop = nullptr;

		this->LoadShopData("./data/SystemMenu/shopData.json");
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
		int idxShop = 0;
		for (auto& shop : shops)
		{
			//ショップ座標設定
			shop->pos = this->pos + this->shopBeginPos;
			shop->pos.y += this->shopDistance * idxShop;
			++idxShop;

			//現在のショップを取得
			if (shop->selectCount > 0)
			{
				this->currentShop = shop.get();
			}
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
	}
	//-------------------------------------------------------------------
	//その他の関数
	bool Object::LoadShopData(const string& filePath_)
	{
		json js;
		std::ifstream fin(filePath_);
		if (!fin) 
		{
			return false; 
		}
		//JSONファイル読み込み
		fin >> js;
		//ファイル読み込み終了
		fin.close();

		int idxShop = 0;	//要素番号
		//ショップ初期化
		shops.clear();

		for (auto& ji : js["skillShopMenu"])
		{
			//ショップ追加
			shops.push_back(SkillShop::Object::Create(true));

			//データ読み込み
			SkillShop::ShopData shopdata;
			shopdata.skillName = ji["name"];
			shopdata.staffTalkFile = ji["talkFileName"];
			shopdata.skillSrcOfs = ji["srcPosY"];
			shopdata.price = ji["price"];

			//読み込んだデータをショップに反映
			shops[idxShop]->SetShopData(shopdata);

			//ショップ座標設定
			shops[idxShop]->pos = this->pos + this->shopBeginPos;
			shops[idxShop]->pos.y += this->shopDistance + idxShop;

			//移動先設定
			if (idxShop != 0)
			{
				shops[idxShop - 1]->SetNext_Down(shops[idxShop].get());
				shops[idxShop]->SetNext_Up(shops[idxShop - 1].get());
			}

			++idxShop;
		}

		this->currentShop = shops[0].get();

		return true;

	}

	void Object::SetDownObj(MyUI::SelectableObject* nextObj_)
	{
		if (!this->shops.empty())
		{
			(*(shops.end() - 1))->SetNext_Down(nextObj_);
		}
	}
	void Object::SetLeftObj(MyUI::SelectableObject* nextObj_)
	{
		for (auto& shop : shops)
		{
			shop->SetNext_Left(nextObj_);
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