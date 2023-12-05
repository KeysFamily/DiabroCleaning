//?------------------------------------------------------
//タスク名:マップ遷移エフェクト
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_MapTransition.h"

namespace  MapTransition
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imgV = DG::Image::Create("./data/map/image/transition_V.png");
		imgH = DG::Image::Create("./data/map/image/transition_H.png");
		imgVSize.Set(1920,2360);
		imgHSize.Set(3712,1080);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		imgV.reset();
		imgH.reset();
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
		this->pos = ge->GetScreenCenter();
		this->moveVec = ML::Vec2(0, 0);
		this->moveSpeed = 300;
		this->motion = Motion::Non;
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
		switch (this->motion)
		{
		case Motion::Non:
			break;
		case Motion::Stop:
			break;
		case Motion::Appear:
			this->pos += this->moveVec;
			{
				bool passedCenter = false;
				switch (this->tDir)
				{
				case Map::MapDir::Up:
					passedCenter = this->pos.y < ge->GetScreenCenter().y;
					break;
				case Map::MapDir::Down:
					passedCenter = this->pos.y > ge->GetScreenCenter().y;
					break;
				case Map::MapDir::Right:
					passedCenter = this->pos.x > ge->GetScreenCenter().x;
					break;
				case Map::MapDir::Left:
					passedCenter = this->pos.x < ge->GetScreenCenter().x;
					break;
				}
				if (passedCenter)
				{
					this->pos = ge->GetScreenCenter();
					this->motion = Motion::Stop;
				}
			}
			break;
		case Motion::Disappear:
			this->pos += this->moveVec;

			ML::Box2D imgHit(0, 0, 0, 0);
			switch (this->tDir)
			{
			case Map::MapDir::Up:
			case Map::MapDir::Down:
				imgHit = OL::setBoxCenter(this->res->imgVSize);
				break;
			case Map::MapDir::Right:
			case Map::MapDir::Left:
				imgHit = OL::setBoxCenter(this->res->imgHSize);
				break;
			}
			imgHit.Offset(this->pos);

			if (!imgHit.Hit(ge->GetScreenBox()))
			{
				this->Kill();
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->motion == Motion::Non)
		{
			return;
		}

		ML::Box2D draw(0, 0, 0, 0);
		ML::Box2D src(0, 0, 0, 0);
		DG::Image::SP img;

		switch (this->tDir)
		{
		case Map::MapDir::Up:
		case Map::MapDir::Down:
			draw = OL::setBoxCenter(this->res->imgVSize);
			src.w = this->res->imgVSize.w;
			src.h = this->res->imgVSize.h;
			img = this->res->imgV;
			break;
		case Map::MapDir::Right:
		case Map::MapDir::Left:
			draw = OL::setBoxCenter(this->res->imgHSize);
			src.w = this->res->imgHSize.w;
			src.h = this->res->imgHSize.h;
			img = this->res->imgH;
			break;
		}

		draw.Offset(this->pos);
		img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//その他メソッド
	//トランジション出現
	void Object::Appear(const Map::MapDir& tDir_)
	{
		switch (tDir_)
		{
		case Map::MapDir::Up:
			this->pos.y += this->res->imgVSize.h;
			this->moveVec.y = -this->moveSpeed;
			break;
		case Map::MapDir::Down:
			this->pos.y -= this->res->imgVSize.h;
			this->moveVec.y = this->moveSpeed;
			break;
		case Map::MapDir::Right:
			this->pos.x -= this->res->imgHSize.w;
			this->moveVec.x = this->moveSpeed;
			break;
		case Map::MapDir::Left:
			this->pos.x += this->res->imgHSize.w;
			this->moveVec.x = -this->moveSpeed;
			break;

		}
		this->tDir = tDir_;
		this->motion = Motion::Appear;
	}
	//トランジション退場
	void Object::Disappear()
	{
		this->motion = Motion::Disappear;
	}
	//出現が終了したか
	bool Object::CheckFinishedAppear()
	{
		return this->motion == Motion::Stop;
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