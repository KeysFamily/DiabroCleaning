//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Item.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace  Item
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
		Object::InputJsonFile("Attack");
		//★データ初期化
		this->hitBase = ML::Box2D(-16, -16, 32, 32);
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
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		int x = rand() % this->itemb.Power_step;
		int y = this->itemb.Item_pos;
		auto draw = this->hitBase.OffsetCopy(this->pos);
		ML::Box2D src(32 * x, 32 * y, 32, 32);
	    //スクロール対応
		//di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(draw, src);
		ge->debugRect(this->CallHitBox(), 4, -ge->camera2D.x, -ge->camera2D.y);
	}

	//-----------------------------------------------------------------
	void Object::Think()
	{
		auto inp = ge->in1->GetState();
		int nm = this->motion;
		switch (nm)
		{
		case Motion::Stand:
			if (inp.B2.down)
			{
				//nm = Motion::Suction;
			}
			break;
		case Motion::Fall:
			if (this->CheckFoot() == true)
			{
				{ nm = Motion::Stand; }
			}
			break;
		}
		//モーション更新
		this->UpdateMotion(nm);

	}
	//------------------------------------------------------------------
	void Object::Move()
	{
		switch (this->motion) {
		default:
			//重力加速
			switch (this->motion) {
			default:
				//上昇中もしくは足元に地面が無い
				if (this->moveVec.y < 0 ||
					this->CheckFoot() == false) {
					this->fallSpeed = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
					this->moveVec.y = this->fallSpeed;
				}
				//地面に接触している
				else {
					this->fallSpeed = min(this->moveVec.y - this->gravity, this->maxFallSpeed);
					this->moveVec.y -= this->fallSpeed * 1.7;
				}


				break;
				//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
			case Motion::Unnon:	break;
			}
			//移動速度減衰
			switch (this->motion) {
			default:
				if (this->CheckFoot() == true) {
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

			//switch (this->motion)
			//{
			//case Motion::Suction:
			//	//auto pl = ge->GetTask<Player::Object>("Player");
			//	this->target = ge->qa_Player;
			//	if (auto  tg = this->target.lock()) {
			//		//ターゲットへの相対座標を求める
			//		ML::Vec2  toVec = tg->pos - this->pos;

			//		float speed = 20;
			//		auto vec = toVec.Normalize();
			//		vec *= speed;
			//		//ターゲットに５％近づく
			//		//this->pos += toVec * 0.05f;

			//		this->pos += vec;
			//	}
			//	break;
			//}
		}
	}

	//-----------------------------------------------------------------------------
	//接触時の応答処理（これ自体はダミーのようなモノ）
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		
		//this->UpdateMotion(Motion::Bound);
		//from_は攻撃してきた相手、カウンターなどで逆にダメージを与えたい時使う
	}

	void Object::GiftPlayer(BChara* pl_)
	{
		Player::Object* pl = dynamic_cast<Player::Object*>(pl_);
		pl->power += itemb.Attack;
		this->Kill();
	}

	//-----------------------------------------------------------------------------
//ファイル読み込み
	void Object::InputJsonFile(string itemName_) {
		//****************************************
		//ファイルから受け付けるもの
		//・攻撃力
		//・防御力
		//・魔法力
		// ・スピード
		// ・持続時間
		// ・描画位置
		//****************************************
		ifstream f("./data/enemy/json/Item.json");
		if (!f.is_open()) return;//ファイルオープンに失敗
		json data = json::parse(f);
		auto& i = data[itemName_];

		this->itemb.Attack= i["ATK"];
		this->itemb.Defense = i["DEF"];
		this->itemb.Magic = i["INT"];
		this->itemb.Speed = i["SPD"];
		this->itemb.Time = i["TIME"];
		this->itemb.Item_pos= i["ITEM_POS"];
		this->itemb.Power_step = i["POWER_STEP"];
		f.close();

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