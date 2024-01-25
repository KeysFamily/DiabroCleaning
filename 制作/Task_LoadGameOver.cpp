//?------------------------------------------------------
//タスク名:ゲームオーバーロードタスク
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_LoadGameOver.h"
#include  "Task_GameOverMenu.h"

namespace  LoadGameOver
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/GameOverBG.png");
		this->imgText = DG::Image::Create("./data/image/TextGameOver.png");
		this->imgTextSize.Set(96, 64);
		this->imgTextSize = imgTextSize * 2;
		this->textResSize = 9;
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->imgText.reset();
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
		this->render2D_Priority[1] = 0.1f;
		this->textPos = ML::Vec2(ge->screenWidth / 2, 200);
		this->textDistance = 150;
		this->state = State::Invalid;
		this->textAppearDistance = 20;
		this->textAppearCnt = 0;

		easing::Create("gameOverBgAp", easing::EASINGTYPE::BOUNCEOUT, 0, ge->screenWidth / 2, 120);
		for (int i = 0; i < this->res->textResSize; ++i)
		{
			easing::Create("gameOverTxtAp" + to_string(i), easing::EASINGTYPE::SINEOUT, -this->res->imgTextSize.h, this->textPos.y, 30);
		}
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
		switch (this->state)
		{
		case State::Invalid:
			break;
		case State::FinishAppear:
			break;
		case State::Appear:
			this->AppearUpDate();
			break;
		case State::Disappear:
			this->DisappearUpDate();
			break;
		}

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		float bgPos = easing::GetPos("gameOverBgAp");
		
		//背景の描画
		{
			ML::Box2D drawBase = OL::setBoxCenter(ge->screenWidth / 2, ge->screenHeight);
			ML::Box2D src(0, 0, 8, 8);

			ML::Box2D drawL = drawBase.OffsetCopy((float)ge->screenWidth / -4, (float)ge->screenHeight / 2);
			drawL.x += bgPos;
			this->res->imgBg->Draw(drawL, src);


			ML::Box2D drawR = drawBase.OffsetCopy((float)ge->screenWidth + (float)ge->screenWidth / 4, (float)ge->screenHeight / 2);
			drawR.x -= bgPos;
			this->res->imgBg->Draw(drawR, src);
		}

		//文字の描画
		ML::Box2D draw = OL::setBoxCenter(this->res->imgTextSize);
		ML::Box2D src(0, 0, this->res->imgTextSize.w / 2, this->res->imgTextSize.h / 2);
		draw.x += this->textPos.x;
		draw.x -= (this->textDistance * (this->res->textResSize - 1) / 2);
		
		for (int i = 0; i < this->res->textResSize; ++i)
		{
			draw.y = easing::GetPos("gameOverTxtAp" + to_string(i));
			this->res->imgText->Draw(draw, src);

			draw.x += this->textDistance;
			src.x += this->res->imgTextSize.w / 2;
		}

		
	}
	//-------------------------------------------------------------------
	//出現
	void Object::Appear()
	{
		if (this->state == State::Invalid)
		{
			easing::Start("gameOverBgAp");
			easing::Start("gameOverTxtAp0");
			this->state = State::Appear;
		}
	}
	//出現処理の更新
	void Object::AppearUpDate()
	{
		if (textAppearCnt >= textAppearDistance * this->res->textResSize)
		{
			if (easing::GetState("gameOverTxtAp" + to_string(this->res->textResSize - 1)) == easing::EQ_STATE::EQ_END)
			{
				GameOverMenu::Object::Create(true);
				this->state = State::FinishAppear;
			}
			return;
		}
		int currentChar = textAppearCnt / textAppearDistance;

		if (easing::GetState("gameOverTxtAp" + to_string(currentChar)) == easing::EQ_STATE::EQ_SET)
		{
			easing::Start("gameOverTxtAp" + to_string(currentChar));
		}


		++textAppearCnt;
	}

	//出現処理が終了したか
	bool Object::CheckFinishdAppear()
	{
		return this->state == State::FinishAppear;
	}

	//消滅処理開始
	void Object::Disappear()
	{
		float startPos = easing::GetPos("gameOverBgAp");
		easing::Create("gameOverBgAp", easing::EASINGTYPE::QUADOUT, startPos, 0, 120);
		easing::Start("gameOverBgAp");
		for (int i = 0; i < this->res->textResSize; ++i)
		{
			startPos = easing::GetPos("gameOverTxtAp" + to_string(i));

			easing::Create("gameOverTxtAp" + to_string(i), easing::EASINGTYPE::QUADOUT, startPos, -this->res->imgTextSize.h, 30);
			easing::Start("gameOverTxtAp" + to_string(i));
		}
		this->state = State::Disappear;
	}

	//消滅処理
	void Object::DisappearUpDate()
	{
		if (easing::GetState("gameOverTxtAp" + to_string(this->res->textResSize - 1)) == easing::EQ_STATE::EQ_END)
		{
			this->Kill();
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