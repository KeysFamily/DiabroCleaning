//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_WaterBlast.h"
#include  "Task_Player.h"
#include  "BEnemy.h"

namespace  WaterBlast
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img1 = DG::Image::Create("./data/effect/WaterBlast.png");
		this->img2 = DG::Image::Create("./data/effect/WaterBlastEnd.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img1.reset();
		this->img2.reset();
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
		this->hitBase = ML::Box2D(-60, -140, 120, 200);
		this->pos = ML::Vec2(0, 0);
		/*this->speed = 10.0f;*/
		this->power = 1.0f;
		this->cost = 0;//仮
		this->motion = Motion::Start;
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
		this->Think();
		this->Move();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		if (this->motion == Motion::Start || this->motion == Motion::Infinite) {
			this->res->img1->Draw(di.draw, di.src);
		}
		if(this->motion == Motion::End){
			this->res->img2->Draw(di.draw, di.src);
		}

		ge->debugRect(this->hitBase.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		int  nm = this->motion;	//とりあえず今の状態を指定
		auto pl = ge->GetTask<BChara>("Player");
		switch (nm) {
		case Motion::Start:
			if (this->moveCnt > 12) {
				nm = Motion::Infinite;
			}
			break;
		case Motion::Infinite:
			if (this->moveCnt > 24 && pl->motion != Player::Object::Motion::MagicAttack) { nm = Motion::End; }
			if (pl->balanceMoney <= this->cost) { nm = Motion::End; }
			break;
		case Motion::End:
			if (this->moveCnt > 24) { this->Kill(); }
			break;
		}
		if (pl->motion != Player::Object::Motion::MagicAttack) { nm = Motion::End; }
		//モーション更新
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//モーションに対応した処理
	void  Object::Move()
	{
		auto enemys = ge->GetTasks<BChara>("Enemy");
		auto pl = ge->GetTask<BChara>("Player");
		switch (this->motion) {
		case Motion::Start:
			break;
		case Motion::Infinite:
			for (auto it = enemys->begin();
				it != enemys->end();
				++it) {
				if ((*it)->CheckHit(this->hitBase.OffsetCopy(this->pos))) {
					BChara::AttackInfo at = { this->power, 0, 0 };
					(*it)->Received(this, at);
				}
			}
			if (this->moveCnt % 60 == 1) { pl->balanceMoney -= this->cost; }
			break;
		case Motion::End:
			break;
		}
	}
	//--------------------------------------------------------------------
	// アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(0, 0, 128, 128), defColor },					//0
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128, 0, 128, 128), defColor },					//1
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128 * 2, 0, 128, 128), defColor },				//2
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128 * 3, 0, 128, 128), defColor },				//3
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(0, 128, 128, 128), defColor },					//4
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128, 128, 128, 128), defColor },				//5
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128 * 2, 128, 128, 128), defColor },			//6
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128 * 3, 128, 128, 128), defColor },			//7
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(0, 128 * 2, 128, 128), defColor },				//8
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128, 128 * 2, 128, 128), defColor },			//9
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128 * 2, 128 * 2, 128, 128), defColor },		//10
			{ ML::Box2D(-150,-210,300,300), ML::Box2D(128 * 3, 128 * 2, 128, 128), defColor },		//11
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
		case Motion::Start:
			if (this->animCnt <= 3)work = 0;
			if (this->animCnt > 3 && this->animCnt <= 6)work = 1;
			if (this->animCnt > 6 && this->animCnt <= 9)work = 2;
			if (this->animCnt > 9)work = 3;
			rtv = imageTable[work];
			break;
		case Motion::Infinite:
			work = this->animCnt / 3;
			work %= 8;
			rtv = imageTable[work + 4];
			break;
		case Motion::End:
			if (this->animCnt <= 3)work = 0;
			if (this->animCnt > 3 && this->animCnt <= 6)work = 1;
			if (this->animCnt > 6 && this->animCnt <= 9)work = 2;
			if (this->animCnt > 9 && this->animCnt <= 12)work = 4;
			if (this->animCnt > 12 && this->animCnt <= 15)work = 5;
			if (this->animCnt > 15 && this->animCnt <= 18)work = 6;
			if (this->animCnt > 18 && this->animCnt <= 21)work = 8;
			if (this->animCnt > 21)work = 9;
			rtv = imageTable[work];
			break;
		}
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