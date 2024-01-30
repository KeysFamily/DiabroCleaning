//?------------------------------------------------------
//タスク名:Task_Item_coin
//作　成　者:荻野　剛志
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:11/15
//概　　　要:coinの元データ
//------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map.h"
#include  "Task_Player.h"
#include  "Task_Item_coin.h"
#include  "sound.h"
#include  "Task_Map.h"

namespace  Item_coin
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/coin.png");

		//22ci0308
		//se
		se::LoadFile("se_get_coin", "./data/sound/se/se_select2.wav");
		//
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
		this->render2D_Priority[1] = 0.4f;
		this->hitBase = OL::setBoxCenter(32, 32);
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量
		this->decSpeed = 0.05f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 11.0f;	//最大落下速度
		this->angle = ML::ToRadian((float)(rand() % 360));
		this->moveVec = ML::Vec2(cos(angle) * 4, sin(angle) * 4);
		this->motion = Stand;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->CreateEffect(58, this->pos);

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
		if (this->unHitTime > 0) { this->unHitTime--; }

		this->Think();
		this->Move();

		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//画面内にコインがあるか
		this->out_coin(ge->qa_Map->ObjectMap.width, ge->qa_Map->ObjectMap.height);
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(di.draw, di.src, di.color);
	}
	
	//-----------------------------------------------------------------
	void Object::Think()
	{
		int nm = this->motion;
		switch (nm)
		{
		case Motion::Stand:
			
			break;
		case Motion::Fall:
			if (this->CheckFoot())
			{
				nm = Motion::Stand;
			}
			break;
		}
		//モーション更新
		this->UpdateMotion(nm);

	}

	//------------------------------------------------------------------
	void Object::Move()
	{
		//重力加速
		switch (this->motion) {
		default:
			//上昇中もしくは足元に地面が無い
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//地面に接触している
			else {
				this->fallSpeed = min(this->moveVec.y - this->gravity, this->maxFallSpeed);
				this->moveVec.y -= this->fallSpeed * 1.7f;
			}


			break;
			//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Motion::Unnon:	break;
		}
		//移動速度減衰
		switch (this->motion) {
		default:
			if (this->CheckFoot()) {
				if (this->moveVec.x < 0) {
					this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
				}
				else {
					this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
				}
			}
			break;

			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Motion::Bound:
		case Motion::Unnon:	break;
		}

		switch (this->motion)
		{
		case Motion::Suction:
			this->target = ge->qa_Player;
			if (auto  tg = this->target.lock()) {
				//ターゲットへの相対座標を求める
				ML::Vec2  toVec = tg->pos - this->pos;

				float speed = 20;
				auto vec = toVec.Normalize();
				vec *= speed;

				this->pos += vec;
			}
			break;
		}
	}

	//-------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo Object::Anim()
	{
		ML::Color defColor(1, 1, 1, 1);

		BChara::DrawInfo imageTable[] = {
			//draw                   src
			{this->hitBase,ML::Box2D( 6,6,20,20),defColor},
			{this->hitBase,ML::Box2D(40,6,16,20),defColor},
			{this->hitBase,ML::Box2D(76,6, 8,20),defColor},
			{this->hitBase,ML::Box2D(40,6,16,20),defColor},
		};
		BChara::DrawInfo  rtv;
		int work;

		work = this->animCnt / 5;
		work %= 4;
		rtv = imageTable[work];

		return rtv;
	}
	//-----------------------------------------------------------------------------
	// 
	//接触時の応答処理（これ自体はダミーのようなモノ）
	void Object::Received(BChara* from_, AttackInfo at_)
	{

		//this->UpdateMotion(Motion::Bound);
		//from_は攻撃してきた相手、カウンターなどで逆にダメージを与えたい時使う
	}

	void Object::GiftPlayer(BChara* pl_)
	{
		pl_->balanceMoney += 1;
		ge->TotalGetCoinCnt += 1;
		this->Kill();
		se::Stop("se_get_coin");
		se::Play("se_get_coin");
		//this->UpdateMotion(Motion::Bound);
		//from_は攻撃してきた相手、カウンターなどで逆にダメージを与えたい時使う
	}

	//------------------------------------------------------------------------------
	//画面外にcoinが出たら消す処理
	void Object::out_coin(int x_,int y_)
	{
		ML::Box2D drawsize(0, 0, x_ * 64, y_ * 64);
		if (!drawsize.Hit(this->CallHitBox())) {
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