//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_ItemTrsBox.h"
#include  "Task_Item.h"
#include  "Task_Player.h"
#include  "Task_Item_coin_maneger.h"

namespace  ItemTrsBox
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->ImgTrsBox = DG::Image::Create("./data/Item/image/TreasureBox.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->ImgTrsBox.reset();
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
		this->motion = Stand;
		this->render2D_Priority[1] = 0.4f;
		this->hitBase = ML::Box2D(-32, -32, 64, 64);
		this->pos = ML::Vec2(1200, 700);
		this->minCoin = 1;
		this->maxCoin = 50;
		this->minLevel = 0;
		this->maxLevel = 1;
		this->Hitbool = false;
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
		if (this->Hitbool == true) this->animCnt++;
		this->Think();
		this->Move();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->ImgTrsBox->Draw(di.draw, di.src);
		ge->debugRect(this->CallHitBox(), 4, -ge->camera2D.x, -ge->camera2D.y);
	}

	//-----------------------------------------------------------------
	void Object::Think()
	{
		auto inp = ge->in1->GetState();
		int nm = this->motion;
		switch (nm)
		{
		case Motion::Stand:
			break;
		case Motion::Hit:
			if (this->animCnt > 70)nm = Motion::Lose;
			break;
		case Motion::Lose:
			break;
		}
		//モーション更新
		this->UpdateMotion(nm);

	}

	//------------------------------------------------------------------------
	void Object::Move()
	{
		switch (this->motion) {
		case Hit:
			//アイテムを生成
			if (this->animCnt == 50) {
				auto coinMg = ge->GetTask<BChara>("coin_maneger");
				int genCoin = this->minCoin + (rand() % (this->maxCoin - this->minCoin + 1));
				if (maxCoin - minCoin > 0)
				{
					genCoin = minCoin + (rand() % (minCoin - maxCoin));
				}
				else
				{
					genCoin = minCoin;
				}
				coinMg->Create_coin(this->pos.x, this->pos.y, genCoin);
				
				auto item = Item::Object::Create(true);
				item->pos = this->pos;
				item->ItemInit(this->pos, minLevel, maxLevel);
			}
			break;
		case Lose:
			this->Kill();
			break;
		}
	}

	//-------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo Object::Anim()
	{
		ML::Color defColor(1, 1, 1, 1);

		BChara::DrawInfo imageTable[] = {
			//draw                   src
			{this->hitBase,ML::Box2D(0,0,32,32),defColor},   //通常状態
			{this->hitBase,ML::Box2D(32,0,32,32),defColor},
			{this->hitBase,ML::Box2D(64,0,32,32),defColor},
			{this->hitBase,ML::Box2D(96,0,32,32),defColor},
			{this->hitBase,ML::Box2D(128,0,32,32),defColor},
			{this->hitBase,ML::Box2D(160,0,32,32),defColor},
			{this->hitBase,ML::Box2D(192,0,32,32),defColor},
			};
		BChara::DrawInfo  rtv;
		int work;
		switch (this->motion)
		{
		default: rtv = imageTable[0];
		case Stand:
			rtv = imageTable[0];
			break;
		case Hit:
			work = this->animCnt / 10;
			work %= 7;
			rtv = imageTable[work];
			break;
		case Lose:
			rtv = imageTable[6];
			rtv.color = ML::Color(1.0f - (0.05f * this->moveCnt), 1.0f, 1.0f, 1.0f);
			break;
		}

		return rtv;
	}

	//-----------------------------------------------------------------------------
	//接触時の応答処理（これ自体はダミーのようなモノ）
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		
	}

	void Object::GiftPlayer(BChara* pl_)
	{
		if (this->Hitbool != true) {
			this->motion = Motion::Hit;
			this->Hitbool = true;
			ge->qa_Map->AddOpenedBox(this->mapPos);
		}
	}
	//-----------------------------------------------------------------------------
	//ボックス初期化
	void Object::BoxInit(ML::Point mapPos_, int coinMin_, int coinMax_, int levelMin_, int levelMax_)
	{
		this->mapPos = mapPos_;
		this->minCoin = coinMin_;
		this->maxCoin = coinMax_;
		this->minLevel = levelMin_;
		this->maxLevel = levelMax_;
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