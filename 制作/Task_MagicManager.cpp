//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_MagicManager.h"
#include  "Task_FireBall.h"
#include  "Task_WaterBlast.h"
#include  "Task_Thunder.h"
#include  "Task_Player.h"

namespace  MagicManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
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
		this->moveCnt = 0;
		this->LR = true;
		this->pos = ML::Vec2(0, 0);
		this->magicSelect = Magic::Unnon;
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
		this->moveCnt++;
		auto pl = ge->GetTask<BChara>("Player");
		switch (this->magicSelect) {
		case Magic::Unnon:
			break;
		case Magic::FireBall:
			if (this->moveCnt % 30 == 1) {
				auto fb = FireBall::Object::Create(true); //(仮)
				if (pl->balanceMoney > fb->cost) {
					if (this->LR) {
						fb->angle_LR = BChara::Angle_LR::Right;
						fb->moveVec = ML::Vec2(fb->speed, 0);
						fb->pos.x = this->pos.x + 60;
						fb->pos.y = this->pos.y;
					}
					if (!this->LR) {
						fb->angle_LR = BChara::Angle_LR::Left;
						fb->moveVec = ML::Vec2(-fb->speed, 0);
						fb->pos.x = this->pos.x - 60;
						fb->pos.y = this->pos.y;
					}
					pl->balanceMoney -= fb->cost;
				}
				else {
					fb->Kill();
				}
			}
			break;
		case Magic::WaterBlast:
			if (this->moveCnt == 1) {
				auto wb = WaterBlast::Object::Create(true);
				if (pl->balanceMoney > wb->cost) {
					if (this->LR) {
						wb->pos.x = this->pos.x + 100;
						wb->pos.y = this->pos.y;
					}
					if (!this->LR) {
						wb->pos.x = this->pos.x - 100;
						wb->pos.y = this->pos.y;
					}
					pl->balanceMoney -= wb->cost;
				}
			}
			break;
		case Magic::Thunder:
			if (this->moveCnt % 30 == 1) {
				auto th = Thunder::Object::Create(true); //(仮)
				if (pl->balanceMoney > th->cost) {
					if (this->LR) {
						th->angle_LR = BChara::Angle_LR::Right;
						th->moveVec = ML::Vec2(th->speed, 0);
						th->pos.x = pl->pos.x + 60;
						th->pos.y = pl->pos.y;
					}
					if (!this->LR) {
						th->angle_LR = BChara::Angle_LR::Left;
						th->moveVec = ML::Vec2(-th->speed, 0);
						th->pos.x = pl->pos.x - 60;
						th->pos.y = pl->pos.y;
					}
					pl->balanceMoney -= th->cost;
				}
				else {
					th->Kill();
				}
			}
			break;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
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