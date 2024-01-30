//?------------------------------------------------------
//タスク名:タイトルメニュー
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_TitleMenu.h"
#include  "Task_Title.h"

namespace  TitleMenu
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imgBg = DG::Image::Create("./data/image/title/selectBG.png");
		imgBgSize.Set(704, 416);
		imgSelect = DG::Image::Create("./data/image/title/selectObject.png");
		imgSelectSize.Set(32, 32);
		fontTitleSize.Set(24, 48);
		fontTitle = DG::Font::Create("ＭＳ ゴシック", this->fontTitleSize.w, this->fontTitleSize.h);
		fontMainSize.Set(16, 32);
		fontMain = DG::Font::Create("ＭＳ ゴシック", fontMainSize.w, fontMainSize.h);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->imgSelect.reset();
		this->fontTitle.reset();
		this->fontMain.reset();
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
		this->pos = ML::Vec2(ge->screenWidth / 2, ge->screenHeight / 4 * 3);
		this->titlePos = ML::Vec2(0, -110);
		this->mainBeginPos = ML::Vec2(0, -40);
		this->mainTextDistance = 20;
		this->selectObjDistance = 21;

		this->LoadMenuAction("./data/title/MenuData.txt");

		this->RunMenuAction(0);
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
		auto inp = ge->in1->GetState();

		if (inp.LStick.BU.down)
		{
			--this->selectingMenu;
			if (this->selectingMenu < 0)
			{
				this->selectingMenu = menuDisplay.size() - 1;
			}
		}
		if (inp.LStick.BD.down)
		{
			++this->selectingMenu;
			if (this->selectingMenu >= menuDisplay.size())
			{
				this->selectingMenu = 0;
			}
		}

		if (inp.B1.down)
		{
			this->RunMenuAction(menuDisplay[selectingMenu]);
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//背景表示
		ge->DrawStd(this->res->imgBg, this->res->imgBgSize, this->pos);

		//タイトル表示
		float titleSizeW = this->res->fontTitleSize.w * this->titleStr.size();
		ML::Box2D draw = OL::setBoxCenter(titleSizeW, this->res->fontTitleSize.h);
		draw.Offset(this->pos + this->titlePos);
		this->res->fontTitle->Draw(draw, this->titleStr);
		
		//メニュー表示
		int selectObjCnt = 0;
		ML::Vec2 mainDrawPos = this->mainBeginPos;
		for (auto& dp : this->menuDisplay)
		{
			float menuSizeW = this->res->fontMainSize.w * this->menuObj[dp].text.size();
			draw = OL::setBoxCenter(menuSizeW, this->res->fontMainSize.h);
			draw.Offset(this->pos + mainDrawPos);
			
			for (auto& menu : menuObj)
			{
				if (menu.actionId == dp)
				{
					this->res->fontMain->Draw(draw, menu.text);
				}

			}

			if(selectObjCnt == this->selectingMenu)
			{
				//選択オブジェクト表示
				ML::Vec2 selectObjPos = this->pos;
				selectObjPos.x = draw.x - this->res->imgSelectSize.w - this->selectObjDistance;
				selectObjPos.y = draw.y + (draw.h / 2);
				ge->DrawStd(this->res->imgSelect, this->res->imgSelectSize, selectObjPos);
			}

			++selectObjCnt;
			mainDrawPos.y += this->res->fontMainSize.h + mainTextDistance;
		}


	}
	//-------------------------------------------------------------------
	//メニューデータ読み込み
	void Object::LoadMenuAction(const string& filePath_)
	{
		ifstream ifs(filePath_);
		while (ifs)
		{
			MenuObject obj;
			ifs >> obj.actionId;
			if (!ifs)
			{
				ifs.close();
				return;
			}
			ifs >> obj.text;
			this->menuObj.push_back(obj);
		}
		ifs.close();
	}
	//-------------------------------------------------------------------
	//メニュー一覧
	void Object::RunMenuAction(int actId_)
	{
		switch (actId_)
		{
		case 0:
			this->ResetMenu();
			this->AddMenu(1);
			this->AddMenu(2);
			this->AddMenu(3);
			this->AddMenu(4);
			this->titleStr = "難易度選択";
			break;
		case 1:
		{
			ge->filemg.SetRunPath("./data/inGame", true);
			auto title = ge->GetTask<Title::Object>("title", "NoName");
			title->CreateGame(3);
			this->Kill();
		}
		break;
		case 2:
		{
			ge->filemg.SetRunPath("./data/inGame", true);
			auto title = ge->GetTask<Title::Object>("title", "NoName");
			title->CreateGame(5);
			this->Kill();
		}
		break;
		case 3:
		{
			ge->filemg.SetRunPath("./data/inGame", true);
			auto title = ge->GetTask<Title::Object>("title", "NoName");
			title->CreateGame(10);
			this->Kill();
		}
		break;
		case 4:
			this->Kill();
			break;
		default:
			break;
		}

	}
	//-------------------------------------------------------------------
	//メニュー一覧リセット
	void Object::ResetMenu()
	{
		this->selectingMenu = 0;
		this->menuDisplay.clear();
	}
	//-------------------------------------------------------------------
	//メニュー一覧に追加
	void Object::AddMenu(int id_)
	{
		this->menuDisplay.push_back(id_);
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