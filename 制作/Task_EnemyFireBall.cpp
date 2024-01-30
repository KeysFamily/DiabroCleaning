//?------------------------------------------------------
//タスク名:火球
//作　成　者:22CI0306 王 功 健
//TODO:もしいれば下記へ記述
//編　集　者:22CI0333 長谷川 勇一朗
//作成年月日:
//概　　　要:火球(ボス攻撃)
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyFireBall.h"

//#include  "Task_Effect00.h"

namespace  EnemyFireBall
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/effect/fireball.png");
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = OL::setBoxCenter(52, 52);
		this->pos = ML::Vec2(0, 0);
		this->power = 5.0f;
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
		this->animCnt++;
		if (this->CheckFront_LR()) { ge->CreateEffect(8, this->pos);  this->Kill(); }
		this->pos += this->moveVec;
#if false
		auto enemys = ge->GetTasks<BChara>("Enemy");
		for (auto it = enemys->begin();
			it != enemys->end();
			++it) {
			if ((*it)->CheckHit(this->hitBase.OffsetCopy(this->pos))) {
				ge->CreateEffect(89, (*it)->pos);
				BChara::AttackInfo at = { this->power, 0, 0 };
				(*it)->Received(this, at);
				ge->CreateEffect(8, this->pos);
				this->Kill();
				break;
			}
		}
#endif
		if (ge->qa_Player == nullptr)return;
		if (ge->qa_Player->CheckHit(this->CallHitBox())) {
			ge->CreateEffect(89, this->pos);
			BChara::AttackInfo at = { this->power, 0, 0 };
			ge->qa_Player->Received(this, at);
			ge->CreateEffect(8, this->pos);
			this->Kill();
		}

		
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src, di.color);
	}

	//--------------------------------------------------------------------
	// 火球設定
	void Object::Setting(ML::Vec2 setPos_, float speed_, float rad_, float AttackPow_) {
		this->pos = setPos_;
		this->moveVec.x = speed_ * cos(rad_);
		this->moveVec.y = speed_ * sin(rad_);
		this->power = AttackPow_;
	}

	//--------------------------------------------------------------------
	// アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1.0f, 0.1f, 1.0f, 2.0f);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-26,-26,52,52), ML::Box2D(0, 0, 26, 26), defColor },			//0
			{ ML::Box2D(-26,-26,52,52), ML::Box2D(26, 0, 26, 26), defColor },			//1
			{ ML::Box2D(-26,-26,52,52), ML::Box2D(26*2, 0, 26, 26), defColor },			//2
		};
		BChara::DrawInfo  rtv;
		int  work;
		work = this->animCnt / 6;
		work %= 3;
		rtv = imageTable[work];

		return rtv;
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