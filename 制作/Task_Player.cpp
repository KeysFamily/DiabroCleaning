//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map.h"
#include  "Task_Effect00.h"
#include "Task_Item_coin.h"


namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/adventure4x.png");
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
		this->hitBase = ML::Box2D(-48, -58, 76, 116);
		this->initialHitBase = ML::Box2D(-48, -58, 76, 116);
		this->crouchHitBase = ML::Box2D(-38, -42, 76, 84);
		this->angle_LR = Angle_LR::Right;
		this->controller = ge->in1;
		this->motion = Motion::Stand;		//キャラ初期状態
		this->maxSpeed = 8.0f;		//最大移動速度（横）
		this->addSpeed = 1.0f;		//歩行加速度（地面の影響である程度打ち消される
		this->crouchSpeed = 2.5f;	//しゃがみながら移動最大速度
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 11.0f;	//最大落下速度
		this->jumpPow = -11.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量
		this->drawScale = 1;
		this->balanceMoney = 100;  //所持金
		this->hp = this->balanceMoney; //hp=所持金
		ge->debugRectLoad();
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->debugRectReset();

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
		this->hitBase = this->DrawScale(this->initialHitBase, this->drawScale);
		if (this->unHitTime > 0) { this->unHitTime--; }
		//思考・状況判断
		this->Think();
		//現モーションに対応した制御
		this->Move();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);
		//hitbase更新
		BChara::DrawInfo  di = this->Anim();
		//this->hitBase = di.draw;
		//あたり判定
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("アイテム");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				//相手に接触の有無を確認させる
				if ((*it)->CheckHit(me)) {
					//相手にダメージの処理を行わせる
					BChara::AttackInfo at = { 0, 0, 0 };
					(*it)->Received(this, at);
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0) {
			if ((this->unHitTime / 4) % 2 == 0) {
				return;//8フレーム中4フレーム画像を表示しない
			}
		}
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);


		ge->debugRect(this->hitBase.OffsetCopy(this->pos),7, -ge->camera2D.x, -ge->camera2D.y);
		ge->debugRectDraw();
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		int nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Motion::Stand:	//立っている
			if (inp.LStick.BL.on) { nm = Motion::Walk; }
			if (inp.LStick.BR.on) { nm = Motion::Walk; }
			if (inp.LStick.BD.on) { nm = Motion::Crouch; }
			if (inp.LStick.BD.on && inp.LStick.BL.on) { nm = Motion::CrouchWalk; }
			if (inp.LStick.BD.on && inp.LStick.BR.on) { nm = Motion::CrouchWalk; }
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//足元 障害　無し
			break;
		case  Motion::Walk:		//歩いている
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			if (inp.LStick.BD.on) { nm = Motion::CrouchWalk; this->moveVec.x = 0; }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Stand; }
			break;
		case  Motion::Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			break;
		case  Motion::Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			break;
		case Motion::Fall2:
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			break;
		case  Motion::Attack:	//攻撃中
			if (this->moveCnt == 40) { nm = Motion::Stand; }
			break;
		case  Motion::TakeOff:	//飛び立ち
			if (this->moveCnt >= 3) { nm = Motion::Jump; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Landing:	//着地
			if (this->moveCnt >= 3) { nm = Motion::Stand; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case Motion::Bound:
			if (this->moveCnt >= 12 &&
				this->CheckFoot() == true) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Crouch:    //しゃがむ
			if (inp.LStick.BD.off) { nm = Motion::Stand; /*this->pos.y -= 5;*/ }
			if (inp.LStick.BL.on) { nm = Motion::CrouchWalk; }
			if (inp.LStick.BR.on) { nm = Motion::CrouchWalk; }
			break;
		case Motion::CrouchWalk:	//しゃがみながら移動
			if (inp.LStick.BD.off) { nm = Motion::Walk; /*this->pos.y -= 5;*/ }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Crouch; }
			if (inp.LStick.BL.off && inp.LStick.BR.off && inp.LStick.BD.off) { nm = Motion::Stand; /*this->pos.y -= 5;*/ }
			break;
		}
		//モーション更新
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//モーションに対応した処理
	//(モーションは変更しない）
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();
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
				this->moveVec.y = 0.0f;
			}
			break;
			//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Motion::Unnon:	break;
		}

		//移動速度減衰
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Motion::Bound:
		case Motion::Unnon:	break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Motion::Stand:	//立っている
			break;
		case  Motion::Walk:		//歩いている
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case  Motion::Fall:		//落下中
			if (inp.LStick.BL.on) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Fall2:
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Jump:		//上昇中
			if (this->CheckHead() == true) { this->moveVec.y = 0; }
			if (this->moveCnt == 0) {
				this->moveVec.y = this->jumpPow;
			}
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Jump2:
			if (this->CheckHead() == true) { this->moveVec.y = 0; }
			if (this->moveCnt == 0) {
				this->moveVec.y = this->jumpPow * 0.9f;
			}
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Attack:	//攻撃中
			break;
		case Motion::Crouch:	//しゃがむ
			break;
		case Motion::CrouchWalk:	//しゃがみながら移動
			if (inp.LStick.BL.on) {
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = max(-this->crouchSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->crouchSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-48, -58, 76, 116), ML::Box2D(56,28,76,116), defColor },	//0 停止1
			{ ML::Box2D(-40, -62, 68, 120), ML::Box2D(264,24,68,120), defColor },	//1 停止2
			{ ML::Box2D(-44, -62, 76, 120), ML::Box2D(460,24,76,120), defColor },	//2 停止3
			{ ML::Box2D(-52, -58, 80, 116), ML::Box2D(652,28,80,116), defColor },	//3 停止4 ここまで編集済み
			{ ML::Box2D(-40, -56, 80, 112), ML::Box2D(268,180,80,112), defColor },	//4 歩行1
			{ ML::Box2D(-40, -54, 80, 108), ML::Box2D(464,184,80,108), defColor },	//5 歩行2
			{ ML::Box2D(-40, -50, 80, 100), ML::Box2D(664,184,80,100), defColor },	//6 歩行3
			{ ML::Box2D(-46, -56, 92, 112), ML::Box2D(868,180,92,112), defColor },	//7 歩行4
			{ ML::Box2D(-40, -54, 80, 108), ML::Box2D(1064,184,80,108), defColor },	//8 歩行5
			{ ML::Box2D(-40, -50, 80, 100), ML::Box2D(1264,192,80,100), defColor },	//9 歩行6
			{ ML::Box2D(-10, -7, 76, 84), ML::Box2D(864,60,76,84), defColor },	//10 しゃがみ
			{ ML::Box2D(-11, -8, 80, 88), ML::Box2D(1060,56,80,88), defColor },	//11 しゃがみながら移動1
			{ ML::Box2D(-11, -8, 76, 88), ML::Box2D(1260,56,76,88), defColor },	//12 しゃがみながら移動2
			{ ML::Box2D(-9, -7, 68, 84), ML::Box2D(68,208,68,84), defColor },	//13 しゃがみながら移動3
			{ ML::Box2D(-10, -14, 76, 108), ML::Box2D(468,324,76,108), defColor },	//14 ジャンプ1
			{ ML::Box2D(-11, -12, 84, 92), ML::Box2D(656,316,84,92), defColor },	//15 ジャンプ2
			{ ML::Box2D(-9, -16, 68, 124), ML::Box2D(272,448,68,124), defColor },	//16 落下1
			{ ML::Box2D(-9, -15, 68, 120), ML::Box2D(472,452,68,120), defColor },	//17 落下2
			{ ML::Box2D(-10, -12, 80, 86), ML::Box2D(60,344,80,96), defColor },	//18 飛び立つ直前1
			{ ML::Box2D(-10, -11, 80, 88), ML::Box2D(260,352,80,88), defColor },	//19 飛び立つ直前2
			{ ML::Box2D(-10, -11, 80, 88), ML::Box2D(260,352,80,88), defColor },	//20 着地
			{ ML::Box2D(-14, -8, 108, 88), ML::Box2D(28,944,108,88), defColor },	//21 攻撃1_1
			{ ML::Box2D(-17, -18, 136, 144), ML::Box2D(460,888,136,144), defColor },	//22 攻撃1_2
			{ ML::Box2D(-13, -18, 108, 144), ML::Box2D(660, 888, 108, 144), defColor },	//23 攻撃1_3
			{ ML::Box2D(-10, -16, 76, 128), ML::Box2D(860,904,76,128), defColor },	//24 攻撃1_4
			{ ML::Box2D(-9, -13, 72, 104), ML::Box2D(1060,928,72,104), defColor },	//25 攻撃1_5
			{ ML::Box2D(-24, -24, 48, 80), ML::Box2D(176, 0, 48, 80),defColor},		//ダメージ
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Motion::Jump:
			rtv = imageTable[14];
			if (this->animCnt > 10)rtv = imageTable[15];
			break;
		case  Motion::Jump2:
			rtv = imageTable[14];
			if (this->animCnt > 10)rtv = imageTable[15];
			break;
			//	停止----------------------------------------------------------------------------
		case  Motion::Stand:
			work = this->animCnt / 30;
			work %= 4;
			rtv = imageTable[work];
			break;
			//	歩行----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 6;
			rtv = imageTable[work + 4];
			break;
			//	落下----------------------------------------------------------------------------
		case  Motion::Fall:
			rtv = imageTable[16];
			if (this->animCnt > 10)rtv = imageTable[17];
			break;
		case  Motion::Fall2:
			rtv = imageTable[16];
			if (this->animCnt > 10)rtv = imageTable[17];
			break;
		case  Motion::TakeOff:	rtv = imageTable[18];	break;
		case  Motion::Landing:	rtv = imageTable[19];	break;
		case  Motion::Bound:	rtv = imageTable[20];	break;
		case Motion::Crouch:
			work = this->animCnt / 16;
			work %= 4;
			rtv = imageTable[work + 10];
			break;
		case Motion::CrouchWalk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 11];
			break;
		case Motion::Attack:
			work = this->animCnt / 8;
			work %= 5;
			rtv = imageTable[work + 21];
			break;
		}
		this->hitBase = rtv.draw;
		//	向きに応じて画像を左右反転する
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			this->hitBase = ML::Box2D(rtv.draw.x + rtv.draw.w, rtv.draw.y, -rtv.draw.w, rtv.draw.h);
		}

		rtv.draw = this->DrawScale(rtv.draw, this->drawScale);
		rtv.src = this->DrawScale(rtv.src, this->drawScale);

		return rtv;
	}
	//-----------------------------------------------------------------------------
	//接触時の応答処理（これ自体はダミーのようなモノ）
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		if (this->unHitTime > 0) {
			return;//無敵時間中はダメージを受けない
		}
		this->unHitTime = 90;
		this->hp -= at_.power;	//仮処理
		if (this->hp <= 0) {
			this->Kill();
		}
		//吹き飛ばされる
		if (this->pos.x > from_->pos.x) {
			this->moveVec = ML::Vec2(+4, -9);
		}
		else {
			this->moveVec = ML::Vec2(-4, -9);
		}
		this->UpdateMotion(Motion::Bound);
		//from_は攻撃してきた相手、カウンターなどで逆にダメージを与えたい時使う
	}
	//-------------------------------------------------------------------
	//矩形倍率
	ML::Box2D Object::DrawScale(ML::Box2D& me, const int drawScale)
	{
		ML::Box2D sample;
		sample.x = me.x * drawScale;
		sample.y = me.y * drawScale;
		sample.h = me.h * drawScale;
		sample.w = me.w * drawScale;
		return sample;
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
				//（メソッド名が変なのは旧バージョンのコピーによるバグを回避するため
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