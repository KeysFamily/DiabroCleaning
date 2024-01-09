//?------------------------------------------------------
//タスク名:敵Boss
//作　成　者:22CI0333 長谷川勇一朗
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:2024/1/9
//概　　　要:敵ボス
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyBoss.h"

#include  "Task_EnemyManager.h"

#include  "randomLib.h"

namespace  EnemyBoss
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		//this->img = DG::Image::Create("./data/enemy/__.png");
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
		this->hitBase = OL::setBoxCenter(62, 102);
		this->angle_LR = Angle_LR::Left;
		this->motion = BossMotion::Stand;
		this->maxSpeed = 2.0f;
		this->addSpeed = 0.7f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -6.0f;
		this->gravity = ML::Gravity(32.0f) * 5.0f;
		const int HP = 10;
		this->hp.SetValues(HP, 0, HP);
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

		ge->GameClearFlag = true;
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
		//当たり判定仮処理
		BChara::AttackInfo at = {1,0,0};
		if(this->Attack_Std(Player::defGroupName,at,this->CallHitBox())){
		// 	//接触した際、自身に何かをする際の処理
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		this->Render_Std(this->res->img);
	}
	
	//-------------------------------------------------------------------
	// 思考制御
	void Object::Think(){
		int nm = this->motion;
		switch (nm)
		{
		case BossMotion::Stand:
			break;
		case BossMotion::Teleport:
			break;
		case BossMotion::CoolTime:
			break;
		case BossMotion::Attack:
		{
			vector<BossMotion> sheet;
			sheet.push_back(BossMotion::Attack1);
			if (this->hp.GetRate() <= 0.5f) {//50%以下で第二段階
				sheet.push_back(BossMotion::Attack2);
			}
			if (this->hp.GetRate() <= 0.25f) {//25%以下で第三段階
				sheet.push_back(BossMotion::Attack3);
			}
			sheet.push_back(BossMotion::Attack4);
			nm = sheet[GetRandom<int>(1, sheet.size())];
		}
			break;
		case BossMotion::Attack1:
			break;
		case BossMotion::Attack2:
			break;
		case BossMotion::Attack3:
			break;
		case BossMotion::Attack4:
			break;
		case BossMotion::Lose:
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
		default:
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
		case Motion::Unnon:		break;
		}

		switch (this->motion)
		{
		case BossMotion::Stand:
			break;
		case BossMotion::Teleport:
			break;
		case BossMotion::CoolTime:
			break;
		case BossMotion::Attack:
			break;
		case BossMotion::Attack1:
			//プレイヤーに向けてボスの杖から火の魔法を出す
			break;
		case BossMotion::Attack2:
			//プレイヤーの足元に魔法を出し１秒後に攻撃をする
			break;
		case BossMotion::Attack3:
			//半径３マス以内に範囲攻撃
			break;
		case BossMotion::Attack4:
			//半径５マス以内のランダムな位置に敵を召喚する
			//条件は以下のとおりとする。
			//
			if (this->hp.GetRate() > 0.5f) {
				//３体召喚
			}
			else if (this->hp.GetRate() > 0.25f) {
				//４体召喚
			}
			else {
				//５体召喚
			}
			break;
		case BossMotion::Lose:
			break;
		default:
			break;
		}
	}
	
	//-------------------------------------------------------------------
	// アニメーション制御
	BEnemy::DrawInfo Object::Anim(){
		ML::Color defColor(1.0f, 1.0f, 1.0f, 1.0f);
		BEnemy::DrawInfo imageTable[] = {
			{ML::Box2D(0, 0, 0, 0), ML::Box2D(0, 0, 0, 0), defColor},
		};
		return imageTable[0];
	}
	
	void Object::Received(BEnemy* from_, AttackInfo at_) {
		if (this->unHitTime > 0) { 
			return; //無敵時間中は処理を受けない
		}
		this->unHitTime = 30;
		this->hp.Addval(-at_.power);
		if (this->hp.vnow <= 0) {
			this->UpdateMotion(Motion::Lose);
			return;
		}
		//吹き飛ばされる
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