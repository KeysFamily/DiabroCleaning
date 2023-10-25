//-------------------------------------------------------------------
//プレイヤー
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/adventure.png");
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
		this->hitBase = ML::Box2D(-9, -14, 19, 29);
		this->angle_LR = Angle_LR::Right;
		this->controller = ge->in1;
		this->hp = 10;
		this->motion = Motion::Stand;		//キャラ初期状態
		this->maxSpeed = 5.0f;		//最大移動速度（横）
		this->addSpeed = 1.0f;		//歩行加速度（地面の影響である程度打ち消される
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 10.0f;	//最大落下速度
		this->jumpPow = -10.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 0; //重力加速度＆時間速度による加算量
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
		if (this->unHitTime > 0) { this->unHitTime--; }
		//思考・状況判断
		this->Think();
		//現モーションに対応した制御
		this->Move();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);
		//あたり判定
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("Enemy");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				//相手に接触の有無を確認させる
				if ((*it)->CheckHit(me)) {
					//相手にダメージの処理を行わせる
					BChara::AttackInfo at = { 0, 0, 0 };
					(*it)->Received(this, at);
					break;
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
	}
	//-------------------------------------------------------------------
	//その他のメソッド
	void Object::LoadImg()
	{
		ifstream ifs("./data/testImgData.txt");
		if (!ifs)
		{
			return;
		}

		string imgPath;
		ifs >> imgPath;
		this->res->img.reset();
		this->res->img = DG::Image::Create(imgPath);
		ifs >> cutSize.w;
		ifs >> cutSize.h;
		ifs >> imgTurn;
		ifs >> select.vmax;
		ifs >> drawScale;
		select.Setval(0);
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Motion::Stand:	//立っている
			if (inp.LStick.BL.on) { nm = Motion::Walk; }
			if (inp.LStick.BR.on) { nm = Motion::Walk; }
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//足元 障害　無し
			break;
		case  Motion::Walk:		//歩いている
			if (inp.B1.down) { nm = Motion::TakeOff; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Stand; }
			break;
		case  Motion::Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			break;
		case  Motion::Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			break;
		case  Motion::Attack:	//攻撃中
			if (this->moveCnt == 8) { nm = Motion::Stand; }
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
		case  Motion::Attack:	//攻撃中
			if (this->moveCnt == 4) {

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
			{ ML::Box2D(-9, -15, 19, 30), ML::Box2D(14, 7, 19, 29), defColor },	//停止1
			{ ML::Box2D(-9, -15, 19, 30), ML::Box2D(14, 7, 19, 29), defColor },	//停止2
			{ ML::Box2D(-9, -14, 19, 29), ML::Box2D(14, 7, 19, 29), defColor },	//停止3
			{ ML::Box2D(-9, -14, 19, 29), ML::Box2D(14, 7, 19, 29), defColor },	//停止4
			{ ML::Box2D(-4, -40, 32, 80), ML::Box2D(32, 0, 32, 80), defColor },	//歩行1
			{ ML::Box2D(-20, -40, 48, 80), ML::Box2D(64, 0, 48, 80), defColor },	//歩行2
			{ ML::Box2D(-20, -40, 48, 80), ML::Box2D(112, 0, 48, 80), defColor },	//歩行3
			{ ML::Box2D(-24, -40, 48, 80), ML::Box2D(48, 80, 48, 80), defColor },	//ジャンプ
			{ ML::Box2D(-24, -40, 48, 80), ML::Box2D(96, 80, 48, 80), defColor },	//落下
			{ ML::Box2D(-24, -24, 48, 64), ML::Box2D(0, 80, 48, 64), defColor },	//飛び立つ直前
			{ ML::Box2D(-24, -24, 48, 64), ML::Box2D(144, 80, 48, 64), defColor },	//着地
			{ ML::Box2D(-24, -24, 48, 80), ML::Box2D(176, 0, 48, 80),defColor},		//ダメージ
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Motion::Jump:		rtv = imageTable[4];	break;
			//	停止----------------------------------------------------------------------------
		case  Motion::Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 4];
			break;
			//	落下----------------------------------------------------------------------------
		case  Motion::Fall:		rtv = imageTable[5];	break;
		case  Motion::TakeOff:	rtv = imageTable[6];	break;
		case  Motion::Landing:	rtv = imageTable[7];	break;
		case  Motion::Bound:	rtv = imageTable[8];	break;
		}
		//	向きに応じて画像を左右反転する
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}

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