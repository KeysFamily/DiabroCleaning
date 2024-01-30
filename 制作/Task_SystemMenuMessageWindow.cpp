//?------------------------------------------------------
//タスク名:メッセージ欄
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SystemMenuMessageWindow.h"
#include  "Task_ShopStaff.h"

namespace  SystemMenuMessageWindow
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/ShopStaff/BackGround.png");
		this->imgBgSize.Set(1248, 160);
		this->fontMsg = DG::Font::Create("ＭＳ ゴシック", 24, 48);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->fontMsg.reset();
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
		this->pos = ML::Vec2(750, 1000);
		this->staffPos = ML::Vec2(-510, -10);
		this->shopStaff = ShopStaff::Object::Create(true);
		this->shopStaff->pos = this->pos + this->staffPos;

		this->appearMessageCount = 0;
		this->messageStartPos = ML::Vec2(-360, -30);
		this->displayStr = "";
		this->SetMessage("enter");

		this->messageColor = ML::Color(1, 0.2f, 0.2f, 0.2f);
		this->outlineColor = ML::Color(1, 1, 1, 1);

		//this->outlineColor = ML::Color(1, 1, 1, 1);
		//this->messageColor = ML::Color(1, 1.0f, 0.2f, 0.3f);
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
		this->shopStaff->pos = this->pos + this->staffPos;

		this->currentMessagePos.Setval(this->appearMessageCount / appearMessageDistance);
		this->displayStr = this->messageStr.substr(0, this->currentMessagePos.vnow);

		++this->appearMessageCount;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw = OL::setBoxCenter(this->res->imgBgSize);
		ML::Box2D src(0, 0, this->res->imgBgSize.w, this->res->imgBgSize.h);
		draw.Offset(this->pos);
		this->res->imgBg->Draw(draw, src);

		draw = ML::Box2D(0, 0, 1000, 1000);
		draw.Offset(this->pos + this->messageStartPos);
		this->res->fontMsg->DrawF(draw, this->displayStr, DG::Font::x4, this->messageColor, this->outlineColor);
	}
	//-------------------------------------------------------------------
	// その他の関数
	//メッセージの設定
	void Object::SetMessage(const string& fileName_)
	{
		ifstream ifs(string("./data/staff_message/") + fileName_ + ".txt");

		if (!ifs)
		{
			return;
		}

		int distance = 0;
		ifs >> distance;

		if (!ifs)
		{
			return;
		}

		int motion = 0;
		ifs >> motion;

		if (!ifs)
		{
			return;
		}

		ifs >> this->messageStr;

		this->appearMessageDistance = distance;
		this->currentMessagePos.SetValues(0, 0, static_cast<int>(this->messageStr.size()));
		this->shopStaff->SetAnimation((ShopStaff::Motion)motion);
		this->appearMessageCount = 0;

		ifs.close();
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