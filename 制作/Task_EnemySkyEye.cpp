//?------------------------------------------------------
//タスク名　:敵SkyEye
//作　成　者:22CI0333 長谷川勇一朗
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:敵SkyEyeの動作
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemySkyEye.h"

#include  "Task_Player.h"

#include  "Task_Map.h"

#include  "randomLib.h"
#include  "Task_Item_coin_maneger.h"
//-----------------------------------------------------------------------
// SkyEye
// 空中雑魚敵
// 
// 通常時 空中を右往左往する。
// 攻撃時 プレイヤーに突進する。地面にぶつからないように飛行する
// 各モードの処理
// 
// Stand,	// N停止
// Walk,	// N通常飛行
// Tracking,// A索敵(標的へ体当たりを開始)
// Attack,	// A攻撃(体当たり)
// Fall,	// 落下(死亡時のみ)
// Landing,	// 着地
// Turn,	// 逆を向く
// Bound,	// 弾き飛ばされている(ノックバック処理)
// Lose,	// 消滅中
//-----------------------------------------------------------------------


namespace  EnemySkyEye
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/enemy/image/SkyEye.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = OL::setBoxCenter(50, 50);
		this->angle_LR = Angle_LR::Left;
		this->motion = Motion::Stand;
		this->maxSpeed = 2.0f;
		this->addSpeed = 0.7f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -6.0f;
		this->gravity = ML::Gravity(32.0f) * 5.0f;
		const int HP = 10;
		this->hp.SetValues(HP, 0, HP);
		this->targetPos = ML::Vec2();
		this->altitude = -1000.0f;
		this->attackPow = 10;
		this->dropMoney = 10;
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
		this->UpDate_Std();
		BChara::AttackInfo ai = { 1,0,0 };
		this->Attack_Std(Player::defGroupName, ai, this->CallHitBox());
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->debugRect(this->hitBase.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
		this->Render_Std(this->res->img);
		
	}
	
	//-------------------------------------------------------------------
	// 思考制御
	void Object::Think(){
		int nm = this->motion;//とりあえず今の状態を指定
		switch (nm)
		{
		case Motion::Stand:	//立っている
			if (this->moveCnt > 1) {
				nm = Motion::Walk;
			}
			break;
		case Motion::Walk:	//歩いている
			if (this->CheckFront_LR()) {
				nm = Motion::Turn;
			}//もし壁に当たったら向きを変える

			if (this->searchCnt > 60 && this->SearchPlayer(800,256)) {
				nm = Motion::Tracking;
			}
			break;

		case Motion::Tracking:
			//もしも衝突せずに
			if (this->CallHitBox().Hit(this->targetPos)) {
				if (this->SearchPlayer(this->hitBase.w, this->hitBase.h)) {
					//攻撃させる
					nm = Motion::Attack;
				}
				else {
					nm = Motion::Walk;
				}
			}
			break;
		case Motion::Turn:
			if (this->moveCnt >= 5) { 
				if (this->preMotion == Motion::Tracking) {
					nm = Motion::Tracking;
				}
				else {
					nm = Motion::Stand;
				}
			}
			break;
		case Motion::Fall:	//下降中
			if (this->CheckFoot()) { 
				nm = Motion::Lose;
			}//足元障害ありで着地する
			break;
		case Motion::Attack://攻撃中
			if (this->moveCnt > 16) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Landing://着地
			if (!this->CheckFoot()) { nm = Motion::Fall; }//足元障害なしなら落下させる
			break;
		case Motion::Bound:
			if (this->moveCnt >= 60) {
				nm = Motion::Stand;
			}
			break;
		case Motion::Lose:
			break;
		default:
			break;
		}
		this->UpdateMotion(nm);
	}
	
	//-------------------------------------------------------------------
	// 動作制御
	void Object::Move(){
		//重力加速
		switch (this->motion) {
		case Motion::Fall:
			//上昇中もしくは足元に地面がない
			if (this->moveVec.y < 0 || !this->CheckFoot()) {
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//地面に接触している
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象なし）
		default:				break;
		}

		//移動速度減衰
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			//地面に接触している
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く（現在対象なし）
		case Motion::Bound:
		case Motion::Unnon:		break;
		}
		//-------------------------------------------------------------------
		// モーションごとに固有の処理
		switch (this->motion)
		{
		case Motion::Stand://立っている
			if (this->altitude < -50.0f) {
				this->altitude = this->pos.y;
			}
			break;
		case Motion::Walk://歩いている
		{	//左右移動処理
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			this->moveVec.y = 0.0f;

			//上下移動処理
			if (ge->qa_Map != nullptr) {
				float diffY = this->altitude - this->pos.y;

				//移動をシミュレート
				ML::Vec2 smPos = this->pos;
				smPos.y += diffY * 0.05f;
				if (!ge->qa_Map->CheckHit(this->hitBase.OffsetCopy(smPos))) {
					//移動先で衝突しなければ、オブジェクトを移動させる
					ge->Dbg_ToDisplay(smPos.x, smPos.y - 50, "衝突なし、移動続行");
					this->pos = smPos;
				}

			}

		}	break;
		case Motion::Tracking://索敵
		{
			ML::Vec2 diff = this->targetPos - this->pos;

			float angRad = atan2(diff.y, diff.x);
			this->moveVec.x = 7.5f * cos(angRad);
			this->moveVec.y = 7.5f * sin(angRad);
		}	break;
		case Motion::Fall://落下中
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x = max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			else {
				this->moveVec.x = min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case Motion::Attack://攻撃中
			if (this->moveCnt == 15) {
				ML::Box2D hit = this->hitBase;
				hit.Offset(this->pos);

				ge->debugRect(hit, 7, -ge->camera2D.x, -ge->camera2D.y);
				
				BChara::AttackInfo ai = { this->attackPow,0,0 };
				this->Attack_Std(Player::defGroupName, ai, hit);
			}
			break;
		case Motion::Turn:
			if (this->moveCnt == 3) {
				if (this->angle_LR == Angle_LR::Left) {
					this->angle_LR = Angle_LR::Right;
				}
				else {
					this->angle_LR = Angle_LR::Left;
				}
			}
			break;
		case Motion::Lose:
			if (this->moveCnt == 7) { this->DropCoins(this->dropMoney); }
			if (this->moveCnt >= 8) {
				this->Kill();
			}
			break;
		case Motion::Bound:
			this->moveVec *= 0.98f;
			break;
		}
	}
	
	//-------------------------------------------------------------------
	// アニメーション制御
	BEnemy::DrawInfo Object::Anim(){
		ML::Color defColor(1.0f, 1.0f, 1.0f, 1.0f);
		BEnemy::DrawInfo imageTable[] = {
			//Idle
			{this->hitBase,ML::Box2D(  0,  0,50,50),defColor},	//1		0
			{this->hitBase,ML::Box2D( 50,  0,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100,  0,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150,  0,50,50),defColor},	//4
			{this->hitBase,ML::Box2D(200,  0,50,50),defColor},	//5
			{this->hitBase,ML::Box2D(250,  0,50,50),defColor},	//6
			{this->hitBase,ML::Box2D(300,  0,50,50),defColor},	//7
			{this->hitBase,ML::Box2D(350,  0,50,50),defColor},	//8		7

			//Att
			{this->hitBase,ML::Box2D(  0, 50,50,50),defColor},	//1		8
			{this->hitBase,ML::Box2D( 50, 50,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100, 50,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150, 50,50,50),defColor},	//4
			{this->hitBase,ML::Box2D(200, 50,50,50),defColor},	//5
			{this->hitBase,ML::Box2D(250, 50,50,50),defColor},	//6
			{this->hitBase,ML::Box2D(300, 50,50,50),defColor},	//7
			{this->hitBase,ML::Box2D(350, 50,50,50),defColor},	//8		15
			//Damage
			{this->hitBase,ML::Box2D(  0,100,50,50),defColor},	//1		16
			{this->hitBase,ML::Box2D( 50,100,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100,100,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150,100,50,50),defColor},	//4		19
			//Dead
			{this->hitBase,ML::Box2D(  0,150,50,50),defColor},	//1		20
			{this->hitBase,ML::Box2D( 50,150,50,50),defColor},	//2
			{this->hitBase,ML::Box2D(100,150,50,50),defColor},	//3
			{this->hitBase,ML::Box2D(150,150,50,50),defColor},	//4		23
		};
		int work;
		BEnemy::DrawInfo rtv;
		switch (this->motion)
		{
		default:	rtv = imageTable[0]; break;
		case Motion::Stand:
			work = this->animCnt / 3;
			work %= 11;
			rtv = imageTable[work];
			break;
		case Motion::Tracking:
		case Motion::Walk:
			work = this->animCnt / 2;
			work %= 8;
			rtv = imageTable[work];
			if (this->motion == Motion::Tracking) {
				rtv.color = ML::Color(1.0f, 1.0f, 0.0f, 0.0f);
			}
			break;
		case Motion::Attack:
			work = this->animCnt / 2;
			work %= 8;
			rtv = imageTable[work + 8];
			break;
		case Motion::Bound:
			work = this->animCnt / 2;
			if (work < 4) {
				work %= 4;
				rtv = imageTable[work + 16];
			}
			else {
				rtv = imageTable[19];
			}
			break;
		case Motion::Lose:
			work = this->animCnt / 2;
			work %= 4;
			rtv = imageTable[work + 20];
			break;
		}
		if (this->angle_LR == Angle_LR::Left) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}

	void Object::Received(BChara* from_, AttackInfo at_) {
		if (this->unHitTime > 0) { 
			return; //無敵時間中は処理を受けない
		}
		//this->unHitTime = 20;
		this->hp.Addval(-at_.power);
		if (this->hp.vnow <= 0) {
			this->UpdateMotion(Motion::Fall);
			return;
		}
		//吹き飛ばされる
		if (from_->angle_LR == Angle_LR::Right) {
			this->moveVec = ML::Vec2(2, -3) * 3;
		}
		else {
			this->moveVec = ML::Vec2(-2, -3) * 3;
		}
		this->UpdateMotion(Motion::Bound);
	}

	//-------------------------------------------------------------------
	// Playerを索敵する xは前方の索敵範囲yは上下の索敵範囲
	bool Object::SearchPlayer(int distX_, int distY_) {
		bool isFoundPlayer = false;
		this->searchCnt = 0;
		if (ge->qa_Player == nullptr || ge->qa_Map == nullptr) { return false; }

		//当たり判定矩形の作成
		ML::Box2D eye = OL::setBoxCenter(30, 30);
		if (this->angle_LR == Angle_LR::Left) {
			eye.Offset(-eye.w, -eye.h);
		}
		else {
			eye.Offset(eye.w, -eye.h);
		}
		eye.Offset(this->pos);

		int eyeW = eye.w;
		int eyeH = eye.h;

		//プレイヤーが矩形内にいるか？
		for (int x = this->hitBase.w; x < distX_; x += eyeW) {
			if (ge->qa_Map->CheckHit(eye))break;
			for (int y = 0; y < distY_; y += eyeH) {
				ML::Box2D eb = eye.OffsetCopy(0, y);
				if (ge->qa_Map->CheckHit(eb))break;
				if (ge->qa_Player->CallHitBox().Hit(eb)) {
					this->targetPos = ML::Vec2(
						GetRandom<float>(eb.x, eb.x + eb.w),
						GetRandom<float>(eb.y, eb.y + eb.h)
					);
					isFoundPlayer = true;
					goto Check;
				}
				ge->debugRect(eb, 4, -ge->camera2D.x, -ge->camera2D.y);
				
			}
			for (int y = 0; y > -distY_; y -= eyeH) {
				ML::Box2D eb = eye.OffsetCopy(0, y);
				if (ge->qa_Map->CheckHit(eb))break;
				if (ge->qa_Player->CallHitBox().Hit(eb)) { 
					this->targetPos = ML::Vec2(
						GetRandom<float>(eb.x, eb.x + eb.w),
						GetRandom<float>(eb.y, eb.y + eb.h)
					);
					isFoundPlayer = true;
					goto Check;
				}
				ge->debugRect(eb, 4, -ge->camera2D.x, -ge->camera2D.y);

			}


			if (this->angle_LR == Angle_LR::Left) {
				eye.Offset(-eyeW, 0);
			}
			else {
				eye.Offset(eyeW, 0);
			}
		}
	Check:
		//いた場合はここに飛ぶ
		//注意：goto文は暗黙のルールで基本的には使わない。
		//　　　今回は深いネストから脱出するために使用した。
		if (isFoundPlayer) {
			ML::Vec2 smDiff = this->targetPos - this->pos;
			float angRad = atan2(smDiff.y, smDiff.x);

			float smMoveX = 25.0f * cos(angRad);
			float smMoveY = 25.0f * sin(angRad);

			while (!this->CallHitBox().OffsetCopy(smDiff).Hit(this->pos)) {
				if (ge->qa_Map->CheckHit(this->CallHitBox().OffsetCopy(smDiff)))return false;

				smDiff.x -= smMoveX;
				smDiff.y -= smMoveY;
			}
			return true;
		}

		return false;
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