//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_BossHPBar.h"
#include  "sound.h"

namespace  BossHPBar
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/bossHPBar.png");
		this->imgSize.Set(192, 64);
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
		se::LoadFile("charge", "./data/sound/se/se_energy.wav");
		se::Play("charge");
		this->render2D_Priority[1] = 0.2f;
		this->pos = ML::Vec2(ge->screenWidth - ge->screenWidth / 3 / 2, this->res->imgSize.h / 2);
		this->offset = ML::Vec2(0, 60);
		this->decleaseTime.SetValues(0, 0, 30);
		this->drawBase = OL::setBoxCenter(ge->screenWidth / 3, this->res->imgSize.h);
		this->isAppearing = true;
		this->hpDisplay = 0;
		this->appearTime.SetValues(0, 0, 180);
		this->hpDecleased = 0;
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
		if (target.expired())
		{
			this->Kill();
			return;
		}

		if (this->isAppearing == true)
		{
			this->AppearUpDate();
			return;
		}

		const BChara::SP& en = target.lock();

		if (hpDisplay != (int)en->hp.vnow)
		{
			decleaseTime.vnow = decleaseTime.vmin;
			hpDecleased += hpDisplay - static_cast<int>(en->hp.vnow);
			hpDisplay = static_cast<int>(en->hp.vnow);
		}

		if (decleaseTime.IsMax() == false)
		{
			decleaseTime.Addval(1);
			if (decleaseTime.IsMax())
			{
				hpDecleased = 0;
			}
		}

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (!target.expired())
		{
			auto en = target.lock();
			if (en->hp.IsMin())
			{
				return;
			}

			//枠と背景の描画
			ML::Box2D draw = drawBase;
			draw.Offset(this->pos);
			ML::Box2D src(0, 0, this->res->imgSize.w, this->res->imgSize.h);
			this->res->img->Draw(draw, src);

			//被ダメージ時の白バー描画
			if (decleaseTime.IsMax() == false)
			{
				float rate = 1 - (float)decleaseTime.vnow / decleaseTime.vmax;
				ML::Box2D draw2 = drawBase;
				draw2.Offset(this->pos);
				ML::Box2D src2(0, this->res->imgSize.h * 2, this->res->imgSize.w, this->res->imgSize.h);

				draw2.w = static_cast<int>(drawBase.w * (hpDisplay + hpDecleased * rate) / en->hp.vmax);
				src2.w = static_cast<int>(this->res->imgSize.w * (hpDisplay + hpDecleased * rate) / en->hp.vmax);

				this->res->img->Draw(draw2, src2);
			}

			//現在の体力を描画
			draw = drawBase;
			draw.Offset(this->pos);
			src = ML::Box2D(0, this->res->imgSize.h, this->res->imgSize.w, this->res->imgSize.h);
			draw.w = static_cast<int>(drawBase.w * hpDisplay / (float)en->hp.vmax);
			src.w = static_cast<int>(this->res->imgSize.w * hpDisplay / (float)en->hp.vmax);
			this->res->img->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//出現処理
	void Object::AppearUpDate()
	{
		auto en = target.lock();
		this->hpDisplay = en->hp.vnow * (this->appearTime.GetRate());
		this->appearTime.Addval(1);
		if (this->appearTime.IsMax())
		{
			this->hpDisplay = en->hp.vnow;
			this->isAppearing = false;
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