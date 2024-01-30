//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyHPBar.h"

namespace  EnemyHPBar
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/enemyHPBar.png");
		this->imgSize.Set(96, 15);
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
		this->render2D_Priority[1] = 0.2f;
		this->pos = ML::Vec2(0, 0);
		this->offset = ML::Vec2(0, 60);
		this->decleaseTime.SetValues(0, 0, 15);
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

		const BChara::SP& en = target.lock();
		this->pos = en->pos + this->offset;

		if (hpDisplay != en->hp.vnow)
		{
			decleaseTime.vnow = decleaseTime.vmin;
			hpDecleased += hpDisplay - en->hp.vnow;
			hpDisplay = en->hp.vnow;
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
			if (en->hp.IsMax() || en->hp.IsMin())
			{
				return;
			}

			//枠と背景の描画
			ML::Box2D draw = OL::setBoxCenter(this->res->imgSize);
			draw.Offset(this->pos);
			draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
			ML::Box2D src(0, 0, this->res->imgSize.w, this->res->imgSize.h);
			this->res->img->Draw(draw, src);

			//被ダメージ時の白バー描画
			if (decleaseTime.IsMax() == false)
			{
				float rate = 1 - (float)decleaseTime.vnow / decleaseTime.vmax;
				ML::Box2D draw2 = OL::setBoxCenter(this->res->imgSize);
				draw2.Offset(this->pos);
				draw2.Offset(-ge->camera2D.x, -ge->camera2D.y);
				ML::Box2D src2(0, this->res->imgSize.h * 2, this->res->imgSize.w, this->res->imgSize.h);

				draw2.w = this->res->imgSize.w * (hpDisplay + hpDecleased * rate) / en->hp.vmax;
				src2.w = this->res->imgSize.w * (hpDisplay + hpDecleased * rate) / en->hp.vmax;

				this->res->img->Draw(draw2, src2);
			}

			//現在の体力を描画
			draw = OL::setBoxCenter(this->res->imgSize);
			draw.Offset(this->pos);
			draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
			src = ML::Box2D(0, this->res->imgSize.h, this->res->imgSize.w, this->res->imgSize.h);
			draw.w = this->res->imgSize.w * hpDisplay / (float)en->hp.vmax;
			src.w = this->res->imgSize.w * hpDisplay / (float)en->hp.vmax;
			this->res->img->Draw(draw, src);
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