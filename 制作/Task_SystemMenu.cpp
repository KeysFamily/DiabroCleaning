//?------------------------------------------------------
//タスク名:メニュー画面管理
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "sound.h"
#include  "Task_SystemMenu.h"
#include  "Task_PlayerStatus.h"
#include  "Task_SystemMenuMessageWindow.h"
#include  "Task_SystemMenuSelectObject.h"
#include  "Task_PlayerStatusShop.h"
#include  "Task_ShopStaff.h"
#include  "Task_SkillSelect.h"
#include  "Task_SkillShop.h"
#include  "Task_SystemMenuMoneyDisplay.h"
#include  "Task_SystemMenuBackButton.h"


namespace  SystemMenu
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/backGround.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		this->render2D_Priority[1] = 1.0f;
		//位置設定
		this->pos = ML::Vec2(0, -1080);
		this->skillPos = ML::Vec2(1400, 465);
		this->statusPos = ML::Vec2(500, 560);
		this->backPos = ML::Vec2(1650, 1000);
		this->msgPos = ML::Vec2(750, 1000);
		this->moneyPos = ML::Vec2(420, 130);
		//終了判定
		this->finishedAppear = false;
		this->finishFlag = true;
		//ゲーム用ファイルを生成しておく
		this->CreateNewFile();

		//★タスクの生成
		this->status = PlayerStatus::Object::Create(true);
		this->message = SystemMenuMessageWindow::Object::Create(true);
		this->skill = SkillSelect::Object::Create(true);
		this->moneyDp = SystemMenuMoneyDisplay::Object::Create(true);
		this->back = SystemMenuBackButton::Object::Create(true);
		
		this->SetPos();
		this->status->ShopUpdate();
		this->skill->ShopUpDate();

		//選択位置設定
		skill->SetLeftObj(status->currentShop);
		skill->SetDownObj(back.get());
		status->SetRightObj(skill->currentShop);
		status->SetDownObj(back.get());
		back->SetNext_Up((skill->shops.end() - 1)->get());

		//最初は無効にしておく
		this->SuspendMenu(true);


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
		this->SetPos();
		if (finishFlag == true)
		{
			this->DisappearUpdate();
			return;
		}
		if (finishedAppear == false)
		{
			this->AppearUpDate();
			return;
		}

		//終了処理
		auto inp = ge->in1->GetState();
		if (inp.ST.down) 
		{
			this->FinishMenu();
		}


		this->skill->SetLeftObj(this->status->currentShop);
		this->status->SetRightObj(this->skill->currentShop);
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, 1920, 1080);
		ML::Box2D src(0,0,1024,1024);
		draw.Offset(this->pos);
		this->res->imgBg->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//その他の関数
	//位置設定
	void Object::SetPos()
	{
		this->status->pos = this->pos + this->statusPos;
		this->message->pos = this->pos + this->msgPos;
		this->skill->pos = this->pos + this->skillPos;
		this->moneyDp->pos = this->pos + this->moneyPos;
		this->back->pos = this->pos + this->backPos;
	}

	//メニューオブジェクト無効化
	void Object::SuspendMenu(bool f_)
	{
		if (true)
		{
			this->menuObj = ge->GetTasks<BTask>("SystemMenu");
		}
		for (auto& obj : *(this->menuObj))
		{
			obj->Suspend(f_);
		}
	}

	//出現処理
	void Object::AppearUpDate()
	{
		this->pos.y = easing::GetPos("menuAp");
		if (easing::GetState("menuAp") == easing::EQ_STATE::EQ_END)
		{
			auto sobj = SystemMenuSelectObject::Object::Create(true);
			sobj->sto = status->shops[0].get();

			this->finishedAppear = true;
		}

	}
	//消滅処理
	void Object::DisappearUpdate()
	{
		this->pos.y = easing::GetPos("menuDisap");
		if (easing::GetState("menuDisap") == easing::EQ_STATE::EQ_END)
		{
			this->Suspend(true);
		}
	}

	//メニュー画面開始
	void Object::StartMenu()
	{
		//位置設定
		this->pos = ML::Vec2(0, -1080);
		this->skillPos = ML::Vec2(1400, 465);
		this->statusPos = ML::Vec2(500, 560);
		this->backPos = ML::Vec2(1650, 1000);
		this->msgPos = ML::Vec2(750, 1000);
		this->moneyPos = ML::Vec2(420, 130);
		//終了判定
		this->finishedAppear = false;
		this->finishFlag = false;
		//イージング設定
		easing::Create("menuAp", easing::EASINGTYPE::BOUNCEOUT, this->pos.y, 0, 30);
		easing::Start("menuAp");
		//サウンド設定
		bgm::LoadFile("bgmMenu", "./data/sound/bgm/shop.mp3");
		bgm::Play("bgmMenu");
		se::LoadFile("shopin", "./data/sound/se/shopin.wav");
		se::Play("shopin");
		//関連タスク有効化
		this->SuspendMenu(false);
		this->SetPos();
		this->message->SetMessage("enter");
	}

	//メニュー画面終了
	void Object::FinishMenu()
	{
		ge->KillAll_GN("SystemMenu", "SelectObject");
		this->SaveToFile();
		easing::Create("menuDisap", easing::EASINGTYPE::CUBICOUT, this->pos.y, -1080, 30);
		easing::Start("menuDisap");
		bgm::Stop("bgmMenu");
		se::Play("shopin");
		this->finishFlag = true;
	}

	//新規ファイル作成
	void Object::CreateNewFile()
	{
		CopyFile("./data/inGame/template/pData_skill.json", "./data/inGame/run/pData_skill.json", false);
		CopyFile("./data/inGame/template/pData_status.json", "./data/inGame/run/pData_status.json", false);
	}

	//ファイルにセーブ
	void Object::SaveToFile()
	{
		json js;
		std::ifstream fin("./data/inGame/run/pData_skill.json");
		if (!fin)
		{
			return;
		}
		//JSONファイル読み込み
		fin >> js;
		//ファイル読み込み終了
		fin.close();


		for (auto& ji : js["pData_skill"])
		{
			for (auto& sk : this->skill->shops)
			{
				if (ji["name"] == sk->shopData.skillName)
				{
					ji["isBought"] = sk->currentState == SkillShop::Object::State::BOUGHT;
				}
			}
		}

		ofstream fout("./data/inGame/run/pData_skill.json");
		fout << js.dump(4);

		fout.close();

		return;
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