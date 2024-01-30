//?------------------------------------------------------
//タスク名:コントローラーガイド
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GuideControll.h"

namespace  GuideControll
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/UI/gamepadButton.png");
		this->imgSize.Set(32, 32);
		this->imgBg = DG::Image::Create("./data/image/UI/buttonBack.png");
		this->imgBgSize.Set(8, 8);
		this->imgCountLine = 4;
		this->fontSize.Set(16, 32);
		this->font = DG::Font::Create("ＭＳ ゴシック", fontSize.w, fontSize.h);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
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
		this->render2D_Priority[1] = 0.01f;

		this->posRB.x = static_cast<float>(ge->screenWidth);
		this->posRB.y = static_cast<float>(ge->screenHeight);
		this->distance = 20;
		this->currentGuide = GuideType::Game;
		this->Load("./data/guideControll");
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
		ML::Vec2 imgPos;
		imgPos.x = this->posRB.x - this->res->imgSize.w / 2;
		imgPos.y = this->posRB.y - this->res->imgSize.h / 2;
		for (auto& gd : this->guides[this->currentGuide])
		{
			//画像の設定
			imgPos.x -= this->res->imgSize.w / 2;
			imgPos.x -= gd.guideStr.size() * this->res->fontSize.w;
			ML::Box2D draw = OL::setBoxCenter(this->res->imgSize);
			draw.Offset(imgPos);
			ML::Box2D src = GetButtonSrc(gd.buttonNum);

			//文字の設定
			ML::Box2D drawf = ML::Box2D(0, 0, this->res->fontSize.w * static_cast<int>(gd.guideStr.size()), this->res->fontSize.h);
			drawf.x = static_cast<int>(imgPos.x + this->res->imgSize.w / 2);
			drawf.y = static_cast<int>(this->posRB.y - this->res->fontSize.h);
			
			//背景の設定
			ML::Box2D drawbg(0, 0, 0, 0);
			ML::Box2D srcbg(0, 0, this->res->imgBgSize.w, this->res->imgBgSize.h);
			drawbg.x = draw.x;
			drawbg.y = min(draw.y, drawf.y);
			drawbg.w = draw.w + drawf.w;
			drawbg.h = max(draw.h, drawf.h);

			//描画
			this->res->imgBg->Draw(drawbg, srcbg);
			this->res->img->Draw(draw, src);
			this->res->font->DrawF(drawf, gd.guideStr, DG::Font::x4, ML::Color(1, 0, 0, 0), ML::Color(1, 1, 1, 1));

			imgPos.x -= this->distance;
		}


	}
	//-------------------------------------------------------------------
	//ガイド追加
	void Object::AddGuides(GuideType type_, Button bu_, const string& guideStr_)
	{
		GuideButton freshButton;
		freshButton.buttonNum = bu_;
		freshButton.guideStr = guideStr_;
		this->guides[type_].push_back(freshButton);
	}
	//-------------------------------------------------------------------
	//読み込み処理
	void Object::Load(const string& folderPath_)
	{
		for (auto gd : this->guides)
		{
			gd.clear();
		}
		{
			ifstream ifs(folderPath_ + "/game.txt");
			while (ifs)
			{
				int btNum;
				ifs >> btNum;
				if (!ifs)
				{
					break;
				}
				string btStr;
				ifs >> btStr;
				AddGuides(GuideType::Game, (Button)btNum, btStr);
			}
			ifs.clear();
		}
		{
			ifstream ifs(folderPath_ + "/menu.txt");
			while (ifs)
			{
				int btNum;
				ifs >> btNum;
				if (!ifs)
				{
					break;
				}
				string btStr;
				ifs >> btStr;
				AddGuides(GuideType::Menu, (Button)btNum, btStr);
			}
			ifs.clear();
		}
	}
	//-------------------------------------------------------------------
	//対応するボタン画像の矩形を返す
	ML::Box2D Object::GetButtonSrc(Button bt_)
	{
		return ML::Box2D(
			(bt_ % this->res->imgCountLine) * this->res->imgSize.w
			, (bt_ / this->res->imgCountLine) * this->res->imgSize.h
			, this->res->imgSize.w
			, this->res->imgSize.h
		);
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