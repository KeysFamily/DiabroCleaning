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
		this->ImgItem = DG::Image::Create("./data/Item/image/Item.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->ImgItem.reset();
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
		if (initialized)
		{

		}

		this->itemb.Time--;
		this->Think();
		this->Move();
		if (this->itemb.Time < 0) this->Kill();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->itemb.Time < 300){
			if (this->itemb.Time < 120) {
				if ((this->itemb.Time / 4) % 2 == 0)return;
			}
			if ((this->itemb.Time/8)%2==0)return;
		}
		auto draw = this->hitBase.OffsetCopy(this->pos);
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		ML::Box2D src((32 * this->itemb.Power_step), (32 * this->itemb.Item_pos), 32, 32);
	    //スクロール対応
		this->res->ImgItem->Draw(draw, src);
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
		pl->itemPower += itemb.Attack;
		pl->itemDEF += itemb.Defense;
		pl->itemINT += itemb.Magic;
		pl->itemSpeed += itemb.Speed;
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
		//・スピード
		//・持続時間
		//・描画位置
		//****************************************
		json js = OL::LoadJsonFile("./data/Item/Item.json");

		for (auto& ji : js["item"])
		{
			if (ji["itemName"] == itemName_)
			{
				//アイテムがプレイヤーに与えるステータス
				this->itemb.Attack = ji["ATK"];
				this->itemb.Defense = ji["DEF"];
				this->itemb.Magic = ji["INT"]; 
				this->itemb.Speed = ji["SPD"]; 
				this->itemb.Time = ji["TIME"]; //アイテムの生存時間
				this->itemb.Item_pos = ji["ITEM_POS"]; //アイテムの画像のY軸を決める
				this->itemb.Power_step = ji["POWER_STEP"]; //アイテムのレアリティ＆アイテムの画像のX軸を決める
			}
		}
	}

	//-----------------------------------------------------------------------------
	//ファイル読み込み
	void Object::ItemInit(ML::Vec2 pos_, int minLevel_, int maxLevel_)
	{
		string str[] = { "Attack","Defense","Magic","Speed" };
		InputJsonFile(str[rand() % 4]);
		int minLevel = min(minLevel_, this->itemb.Power_step);
		int maxLevel = min(maxLevel_, this->itemb.Power_step);
		if (maxLevel - minLevel > 0)
		{
			this->itemb.Power_step = minLevel + (rand() % (maxLevel - minLevel + 1));
		}
		else
		{
			this->itemb.Power_step = minLevel;
		}

		//ステータス割り振り
		int statBase = 1;
		for (int i = 0; i < itemb.Power_step; ++i)
		{
			statBase *= 2;
		}
		this->itemb.Attack *= statBase;
		this->itemb.Defense *= ceil(statBase * 0.6f);
		this->itemb.Magic *= statBase;
		this->itemb.Speed *= itemb.Power_step + 1;


		this->angle = ML::ToRadian((float)(rand() % 360));
		this->moveVec = ML::Vec2(cos(angle) * 4, sin(angle) * 4);
		this->hitBase = ML::Box2D(-24, -24, 48, 48);
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量
		this->decSpeed = 0.05f;		//接地状態の時の速度減衰量（摩擦
		this->maxFallSpeed = 11.0f;	//最大落下速度
		this->motion = Stand;
		this->pos = pos_;

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