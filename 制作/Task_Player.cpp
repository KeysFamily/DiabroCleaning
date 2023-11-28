//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map.h"
#include  "Task_Effect00.h"
#include "Task_Item_coin.h"
#include  "BEnemy.h"
#include  "Task_EnemySkeleton.h"



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
		this->initialHitBase = ML::Box2D(-20, -58, 40, 116);
		this->crouchHitBase = ML::Box2D(-40, -26, 76, 84);
		this->attackBase = ML::Box2D(0, 0, 0, 0);
		this->angle_LR = Angle_LR::Right;
		this->controller = ge->in1;
		this->motion = Motion::Stand;		//キャラ初期状態
		this->maxSpeed = 8.0f;		//最大移動速度（横）
		this->addSpeed = 1.0f;		//歩行加速度（地面の影響である程度打ち消される
		this->crouchSpeed = 2.5f;	//しゃがみながら移動最大速度
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 11.0f;	//最大落下速度
		this->jumpPow = -12.5f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量
		this->drawScale = 1;
		this->attack2 = false;
		this->attack3 = false;
		this->airattack = true;
		this->canJump = true;
		this->canDash = true;
		this->balanceMoney = 100;  //所持金
		this->hp.SetValues(100, 0, 100);
		this->power = 1;
		this->powerScale = 1.0f;
		this->balanceMoney = 100;

		ge->debugRectLoad();
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
			auto targets = ge->GetTasks<BChara>("item");
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


		//ge->debugRect(this->hitBase.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
		//ge->debugRectDraw();
		//ge->debugRect(this->attackBase.OffsetCopy(this->pos), 5, -ge->camera2D.x, -ge->camera2D.y);
		//ge->debugRectDraw();
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		int  nm = this->motion;	//とりあえず今の状態を指定


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
			if (this->CheckFoot() == false) {
				tempCnt++;
				if (tempCnt > 10) {
					nm = Motion::Fall;
				}
			}//足元 障害　無し
			if (this->CheckFoot() == true)tempCnt = 0;
			break;
		case  Motion::Walk:		//歩いている
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) {
				tempCnt++;
				if (tempCnt > 18) {
					nm = Motion::Fall;
				}
			}//足元 障害　無し
			if (this->CheckFoot() == true)tempCnt = 0;
			if (inp.LStick.BD.on) { nm = Motion::CrouchWalk; this->moveVec.x = 0; }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Stand; }
			break;
		case  Motion::Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			if (canDash == true) { if (inp.B2.down) { nm = Motion::Dash; } }
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			if (canDash == true) { if (inp.B2.down) { nm = Motion::Dash; } }
			break;
		case  Motion::Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B1.down) { nm = Motion::Jump2; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			if (canDash == true) { if (inp.B2.down) { nm = Motion::Dash; } }
			break;
		case Motion::Fall2:
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (airattack == true) {
				if (inp.B4.down) { nm = Motion::AirAttack; }
			}
			if (canDash == true) { if (inp.B2.down) { nm = Motion::Dash; } }
			break;
		case Motion::Dash:
			if (this->moveCnt > 10) {
				if (preMotion == Motion::Jump2 || preMotion == Motion::Fall2) { nm = Motion::Fall2; }
				else { nm = Motion::Fall; }
			}
			break;
		case  Motion::TakeOff:	//飛び立ち
			if (this->moveCnt >= 3) { nm = Motion::Jump; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Landing:	//着地
			if (this->moveCnt >= 6) { nm = Motion::Stand; }
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
			if (this->CheckFoot() == false) {
				tempCnt++;
				if (tempCnt > 10) {
					nm = Motion::Fall;
				}
			}//足元 障害　無し
			if (this->CheckFoot() == true)tempCnt = 0;
			if (inp.B4.down) { nm = Motion::Attack; }
			break;
		case Motion::CrouchWalk:	//しゃがみながら移動
			if (inp.LStick.BD.off) { nm = Motion::Walk; /*this->pos.y -= 5;*/ }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Crouch; }
			if (inp.LStick.BL.off && inp.LStick.BR.off && inp.LStick.BD.off) { nm = Motion::Stand; /*this->pos.y -= 5;*/ }
			if (this->CheckFoot() == false) {
				tempCnt++;
				if (tempCnt > 10) {
					nm = Motion::Fall;
				}
			}//足元 障害　無し
			if (this->CheckFoot() == true)tempCnt = 0;
			if (inp.B4.down) { nm = Motion::Attack; }
			break;
		case  Motion::Attack:	//攻撃中
			if (this->moveCnt == 25)
			{
				if (attack2 == true)
				{
					nm = Motion::Attack2;
				}
				else nm = Motion::Stand;
			}
			break;
		case Motion::Attack2:
			if (this->moveCnt == 25)
			{
				if (attack3 == true)
				{
					nm = Motion::Attack3;
				}
				else nm = Motion::Stand;
			}
			break;
		case Motion::Attack3:
			if (this->moveCnt == 30) { nm = Motion::Stand; }
			break;
		case Motion::AirAttack:
			if (this->moveCnt == 20)
			{
				if (canJump == true) nm = Motion::Fall;
				else if (canJump == false)nm = Motion::Fall2;
				if (attack2 == true)
				{
					nm = Motion::AirAttack2;
				}
			}
			break;
		case Motion::AirAttack2:
			if (this->moveCnt == 15)
			{
				if (canJump == true) nm = Motion::Fall;
				else if (canJump == false)nm = Motion::Fall2;
				if (attack3 == true)
				{
					nm = Motion::AirAttack3;
				}
			}
			break;
		case Motion::AirAttack3:
			if (this->CheckFoot() == true) { nm = Motion::AirAttack4; }
			break;
		case Motion::AirAttack4:
			if (this->moveCnt == 15) { nm = Motion::Stand; }
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
		case Motion::AirAttack: break;
		case Motion::AirAttack2: break;
		case Motion::AirAttack3: break;
		case Motion::Dash: break;
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
			this->canJump = true;
			this->canDash = true;
			this->airattack = true;
			this->attackBase = ML::Box2D(0, 0, 0, 0);
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
				this->angle_LR = Angle_LR::Left;
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
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (inp.LStick.BR.on) {
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
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
			this->canJump = false;
			break;
		case Motion::Dash:
			this->moveVec.y = 0;
			if (this->angle_LR == Angle_LR::Right) { this->moveVec.x = 30; }
			if (this->angle_LR == Angle_LR::Left) { this->moveVec.x = -30; }
			if (this->moveCnt == 10) { this->moveVec.x = 0; }
			this->canDash = false;
			break;
		case Motion::Landing:
			
			break;
		case  Motion::Attack:	//�U����
			this->powerScale = 1.0f;
			if (this->moveCnt == 6)this->MakeAttack();
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack2 = true; }
			}
			break;
		case  Motion::Attack2:	//�U����
			this->powerScale = 1.5f;
			this->attack2 = false;
			if (this->moveCnt == 11)this->MakeAttack();
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack3 = true; }
			}
			break;
		case  Motion::Attack3:	//�U����
			this->powerScale = 2.0f;
			this->attack3 = false;
			if (this->moveCnt == 11)this->MakeAttack();
			break;
		case Motion::AirAttack:
			this->airattack = false;
			this->moveVec.y = 0.0f;
			this->powerScale = 1.0f;
			if (this->moveCnt == 6)this->MakeAttack();
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack2 = true; }
			}
			break;
		case  Motion::AirAttack2:	//攻撃中
			this->moveVec.y = 0.0f;
			this->attack2 = false;
			this->powerScale = 1.5f;
			if (this->moveCnt == 1)this->MakeAttack();
			if (moveCnt > 0) {
				if (inp.B4.down) { this->attack3 = true; }
			}
			break;
		case Motion::AirAttack3:
			this->moveVec.y = 20.0f;
			this->attack3 = false;
			this->powerScale = 2.0f;
			if (this->moveCnt == 1)this->MakeAttack();
			break;
		case Motion::AirAttack4:
			this->powerScale = 2.5f;
			if (this->moveCnt == 1)this->MakeAttack();
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
		case Motion::Bound:
			this->attackBase = ML::Box2D(0, 0, 0, 0);
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
			{ ML::Box2D(-48, -58, 76, 116), ML::Box2D(56,28,76,116), defColor },			//0 停止1
			{ ML::Box2D(-40, -62, 68, 120), ML::Box2D(264,24,68,120), defColor },			//1 停止2
			{ ML::Box2D(-44, -62, 76, 120), ML::Box2D(460,24,76,120), defColor },			//2 停止3
			{ ML::Box2D(-52, -58, 80, 116), ML::Box2D(652,28,80,116), defColor },			//3 停止4
			{ ML::Box2D(-40, -54, 80, 112), ML::Box2D(268,180,80,112), defColor },			//4 歩行1
			{ ML::Box2D(-40, -50, 80, 108), ML::Box2D(464,184,80,108), defColor },			//5 歩行2
			{ ML::Box2D(-40, -42, 80, 100), ML::Box2D(664,184,80,100), defColor },			//6 歩行3
			{ ML::Box2D(-40, -54, 92, 112), ML::Box2D(868,180,92,112), defColor },			//7 歩行4
			{ ML::Box2D(-40, -50, 80, 108), ML::Box2D(1064,184,80,108), defColor },			//8 歩行5
			{ ML::Box2D(-40, -42, 80, 100), ML::Box2D(1264,192,80,100), defColor },			//9 歩行6
			{ ML::Box2D(-40, -26, 76, 84), ML::Box2D(864,60,76,84), defColor },				//10 しゃがみ
			{ ML::Box2D(-44, -30, 80, 88), ML::Box2D(1060,56,80,88), defColor },			//11 しゃがみながら移動1
			{ ML::Box2D(-44, -30, 76, 88), ML::Box2D(1260,56,76,88), defColor },			//12 しゃがみながら移動2
			{ ML::Box2D(-36, -26, 68, 84), ML::Box2D(68,208,68,84), defColor },				//13 しゃがみながら移動3
			{ ML::Box2D(-32, -40, 76, 108), ML::Box2D(468,324,76,108), defColor },			//14 ジャンプ1
			{ ML::Box2D(-52, -40, 84, 92), ML::Box2D(656,316,84,92), defColor },			//15 ジャンプ2
			{ ML::Box2D(-40, -64, 68, 124), ML::Box2D(272,448,68,124), defColor },			//16 落下1
			{ ML::Box2D(-40, -64, 68, 120), ML::Box2D(472,452,68,120), defColor },			//17 落下2
			{ ML::Box2D(-44, -28, 80, 86), ML::Box2D(60,344,80,96), defColor },				//18 飛び立つ直前1
			{ ML::Box2D(-40, -30, 80, 88), ML::Box2D(260,352,80,88), defColor },			//19 着地
			{ ML::Box2D(-44, -24, 80, 88), ML::Box2D(260,352,80,88), defColor },			//20 ダメージ(仮
			{ ML::Box2D(-80, -30, 108, 88), ML::Box2D(28,944,108,88), defColor },			//21 攻撃1_1
			{ ML::Box2D(-48, -86, 136, 144), ML::Box2D(460,888,136,144), defColor },		//22 攻撃1_2
			{ ML::Box2D(-48, -86, 108, 144), ML::Box2D(660, 888, 108, 144), defColor },		//23 攻撃1_3
			{ ML::Box2D(-48, -70, 76, 128), ML::Box2D(860,904,76,128), defColor },			//24 攻撃1_4
			{ ML::Box2D(-48, -46, 72, 104), ML::Box2D(1060,928,72,104), defColor },			//25 攻撃1_5
			{ ML::Box2D(-48, -50, 72, 108), ML::Box2D(1260,924,72,108), defColor },			//26 攻撃2_1
			{ ML::Box2D(-56, -50, 80, 108), ML::Box2D(52,1072,80,108), defColor },			//27 攻撃2_2
			{ ML::Box2D(-68, -58, 148, 116), ML::Box2D(240,1064,148, 116), defColor },		//28 攻撃2_3
			{ ML::Box2D(-100, -26, 128,84), ML::Box2D(408,1096,128,84), defColor },			//29 攻撃2_4
			{ ML::Box2D(-100, -30, 124,88), ML::Box2D(608,1092,124,88), defColor },			//30 攻撃2_5
			{ ML::Box2D(-60, -46, 80,104), ML::Box2D(876,1076,80,104), defColor },			//31 攻撃3_1
			{ ML::Box2D(-56, -46, 80,104), ML::Box2D(1080,1076,80,104), defColor },			//32 攻撃3_2
			{ ML::Box2D(-104, -34, 192,92), ML::Box2D(1208,1088,192,92), defColor },		//33 攻撃3_3
			{ ML::Box2D(-100, -18, 124,76), ML::Box2D(12,1252,124,76), defColor },			//34 攻撃3_4
			{ ML::Box2D(-112, -22, 136,80), ML::Box2D(200,1248,136,80), defColor },			//35 攻撃3_5
			{ ML::Box2D(-112, -22, 136,80), ML::Box2D(400,1248,136,80), defColor },			//36 攻撃3_6
			{ ML::Box2D(-44, -40, 72, 104), ML::Box2D(1060, 1960, 72, 104), defColor },		//37 空中攻撃1_1
			{ ML::Box2D(-88, -64, 184, 120), ML::Box2D(1208, 1940, 184, 120), defColor },	//38 空中攻撃1_2
			{ ML::Box2D(-80, -70, 116, 120), ML::Box2D(24, 2084, 116, 120), defColor },		//39 空中攻撃1_3
			{ ML::Box2D(-84, -74, 108, 124), ML::Box2D(220, 2080, 108, 124), defColor },	//40 空中攻撃1_4
			{ ML::Box2D(-44, -66, 136, 124), ML::Box2D(460, 2088, 136, 124), defColor },	//41 空中攻撃2_1
			{ ML::Box2D(-44, -70, 92, 124), ML::Box2D(664, 2084, 92, 124), defColor },		//42 空中攻撃2_2
			{ ML::Box2D(-44, -54, 64, 104), ML::Box2D(860, 2100, 64, 104), defColor },		//43 空中攻撃2_3
			{ ML::Box2D(-56, -70, 112, 124), ML::Box2D(1032, 2076, 112, 124), defColor },	//44 空中攻撃3_1
			{ ML::Box2D(-56, -62, 108, 120), ML::Box2D(1232, 2084, 108, 120), defColor },	//45 空中攻撃3_2
			{ ML::Box2D(-56, -66, 108, 124), ML::Box2D(32, 2228, 108, 124), defColor },		//46 空中攻撃3_3
			{ ML::Box2D(-100, -84, 192, 144), ML::Box2D(204, 2220, 192, 144), defColor },	//47 空中攻撃4_1
			{ ML::Box2D(-92, -62, 184, 120), ML::Box2D(412, 2244, 184, 120), defColor },	//48 空中攻撃4_2
			{ ML::Box2D(-96, -30, 184, 88), ML::Box2D(608, 2276, 184, 88), defColor },		//49 空中攻撃4_3
			{ ML::Box2D(-36, -38, 84, 96), ML::Box2D(664, 1232, 84, 96),defColor},			//50 ダメージ debugしてない
			{ ML::Box2D(-98, -38, 142,112), ML::Box2D(814, 2256, 142, 112),defColor},			//51 ダッシュ

		};
		ML::Box2D attackTable[] = {
			ML::Box2D(0,0,0,0),				//imageTable[21]	0
			ML::Box2D(-20,-86,108,128),		//imageTable[22]	1			ML::Box2D(-48, -86, 136, 144)
			ML::Box2D(-24,-86,84,32),		//imageTable[23]	2			ML::Box2D(-48, -86, 108, 144)
			ML::Box2D(-44,-70,20,44),		//imageTable[24]	3			ML::Box2D(-48, -70, 76, 128)
			ML::Box2D(0,0,0,0),				//imageTable[25]	4
			ML::Box2D(0,0,0,0),				//imageTable[26]	5
			ML::Box2D(0,0,0,0),				//imageTable[27]	6
			ML::Box2D(-64,-58,144,116),		//imageTable[28]	7			ML::Box2D(-68, -58, 148, 116)
			ML::Box2D(-100,14,80,32),		//imageTable[29]	8			ML::Box2D(-100, -26, 128, 84)
			ML::Box2D(-100,10,52,24),		//imageTable[30]	9			ML::Box2D(-100, -30, 124, 88)
			ML::Box2D(0,0,0,0),				//imageTable[31]	10
			ML::Box2D(0,0,0,0),				//imageTable[32]	11
			ML::Box2D(-104,-34,192,84),		//imageTable[33]	12			ML::Box2D(-104, -34, 192, 92)
			ML::Box2D(-100, 10,84,40),		//imageTable[34]	13			ML::Box2D(-100, -18, 124,76)
			ML::Box2D(-112,-2,72,32),		//imageTable[35]	14			ML::Box2D(-112, -22, 136,80)
			ML::Box2D(0,0,0,0),				//imageTable[36]	15
			ML::Box2D(0,0,0,0),				//imageTable[37]	16
			ML::Box2D(-88,-64,184,88),		//imageTable[38]	17			ML::Box2D(-88, -64, 184, 120)
			ML::Box2D(-80,-70,40,76),		//imageTable[39]	18			ML::Box2D(-80, -70, 116, 120)
			ML::Box2D(0,0,0,0),				//imageTable[40]	19
			ML::Box2D(-32,-66,124,124),		//imageTable[41]	20			ML::Box2D(-44, -66, 136, 124)
			ML::Box2D(-36,-70,84,68),		//imageTable[42]	21			ML::Box2D(-44, -70, 92, 124)
			ML::Box2D(0,0,0,0),				//imageTable[43]	22
			ML::Box2D(-20,-70,76,124),		//imageTable[44]	23			ML::Box2D(-56, -70, 112, 124)
			ML::Box2D(16,-62,36,120),		//imageTable[45]	24			ML::Box2D(-56, -62, 108, 120)
			ML::Box2D(16,-66,36,124),		//imageTable[46]	25			ML::Box2D(-56, -66, 108, 124)
			ML::Box2D(-100, -84, 192, 144),	//imageTable[47]	26			ML::Box2D(-100, -84, 192, 144)
			ML::Box2D(-92, -62, 184, 120),	//imageTable[48]	27			ML::Box2D(-92, -62, 184, 120)
			ML::Box2D(0,0,0,0),				//imageTable[49]	28
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
			work = this->animCnt / 8;
			work %= 4;
			rtv = imageTable[work];
			break;
			//	歩行----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 6;
			work %= 6;
			rtv = imageTable[work + 4];
			break;
			//	落下----------------------------------------------------------------------------
		case  Motion::Fall:
			work = this->animCnt / 6;
			work %= 2;
			rtv = imageTable[work + 16];
			break;
		case  Motion::Fall2:
			work = this->animCnt / 8;
			work %= 2;
			rtv = imageTable[work + 16];
			break;
		case Motion::Dash:		rtv = imageTable[51];	break;
		case  Motion::TakeOff:	rtv = imageTable[18];	break;
		case  Motion::Landing:	rtv = imageTable[19];	break;
		case  Motion::Bound:	rtv = imageTable[50];	break;
		case  Motion::Crouch:
			work = this->animCnt / 20;
			work %= 4;
			rtv = imageTable[work + 10];
			break;
		case Motion::CrouchWalk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 11];
			break;
		case Motion::Attack:
			work = this->animCnt / 5;
			work %= 5;
			rtv = imageTable[work + 21];
			this->attackBase = attackTable[work + 0];
			break;
		case Motion::Attack2:
			work = this->animCnt / 5;
			work %= 5;
			rtv = imageTable[work + 26];
			this->attackBase = attackTable[work + 5];
			break;
		case Motion::Attack3:
			work = this->animCnt / 5;
			work %= 6;
			rtv = imageTable[work + 31];
			this->attackBase = attackTable[work + 10];
			break;
		case Motion::AirAttack:
			work = this->animCnt / 5;
			work %= 4;
			rtv = imageTable[work + 37];
			this->attackBase = attackTable[work + 16];
			break;
		case Motion::AirAttack2:
			work = this->animCnt / 5;
			work %= 3;
			rtv = imageTable[work + 41];
			this->attackBase = attackTable[work + 20];
			break;
		case Motion::AirAttack3:
			if (this->animCnt < 5)work = 0;
			else if (this->animCnt >= 5 && this->animCnt < 10)work = 1;
			else work = 2;
			rtv = imageTable[work + 44];
			this->attackBase = attackTable[work + 23];
			break;
		case Motion::AirAttack4:
			work = this->animCnt / 5;
			work %= 3;
			rtv = imageTable[work + 47];
			this->attackBase = attackTable[work + 26];
			break;
		}

		//this->hitBase = rtv.draw;
		this->hitBase.x = -24;
		this->hitBase.y = rtv.draw.y;
		this->hitBase.w = 40;
		this->hitBase.h = rtv.draw.h;
		//	向きに応じて画像を左右反転する
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			this->hitBase = ML::Box2D(-16, rtv.draw.y, 40, rtv.draw.h);
			this->attackBase.x = -this->attackBase.x - this->attackBase.w;
		}
		if (this->hitBase.h > 116) {
			this->hitBase.h = 116;
			this->hitBase.y = -58;
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
		if (this->motion == Motion::Dash) {
			return;
		}
		this->unHitTime = 90;
		this->hp.Addval(-at_.power);	//仮処理
		this->balanceMoney -= at_.power;
		if (this->balanceMoney <= 0)this->balanceMoney = 0; //仮処理
		if (this->hp.IsMin()) {
			//this->Kill();
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
	//----------------------------------------------------------------------------
	//�U������
	void Object::MakeAttack()
	{
		auto enemys = ge->GetTasks<BChara>("Enemy");
		for (auto it = enemys->begin();
			it != enemys->end();
			++it) {
			if ((*it)->CheckHit(this->attackBase.OffsetCopy(this->pos))) {
				BChara::AttackInfo at = { this->power * this->powerScale, 0, 0 };
				(*it)->Received(this, at);
				break;
			}
		}
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