#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//プレイヤー
//-------------------------------------------------------------------
#include "BChara.h"

namespace  Player
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Player");	//グループ名
	const  string  defName("Player");		//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()		override;
		bool  Finalize()		override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
			//共有する変数はここに追加する
		DG::Image::SP img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BChara
	{
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:

		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()		override;	//「実行」１フレーム毎に行う処理
		void  Render2D_AF()	override;	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	public:
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
			//追加したい変数・メソッドはここに追加する
			//BCharaに含まれないモノのみここに追加する

		OL::Size2D cutSize;
		int imgTurn;
		OL::Limit<int> select;
		int drawScale;
		ML::Box2D initialHitBase;
		ML::Box2D crouchHitBase;

		void LoadImg();

		XI::GamePad::SP  controller;

		//キャラクタの行動状態フラグ
		enum Motion
		{
			Unnon = -1,	//	無効(使えません）
			Stand,		//	停止
			Walk,		//	歩行
			Attack,		//	攻撃1
			Attack2,	//	攻撃2
			Attack3,	//	攻撃3
			AirAttack,	//	空中攻撃1
			AirAttack2,	//	空中攻撃2
			AirAttack3,	//	空中攻撃3
			AirAttack4,	//	空中攻撃4
			Jump,		//	ジャンプ
			Jump2,		//	二段ジャンプ
			Fall,		//	落下
			Fall2,		//	落下(二段ジャンプ後)
			TakeOff,	//	飛び立つ瞬間
			Landing,	//	着地
			Crouch,		//  しゃがみ
			CrouchWalk,	//	しゃがみながら移動
			Turn,		//	方向転換
			Bound,		//	弾き飛ばされてる
			Lose,		//  消滅中
			Dash,		//  ダッシュ
			MagicAttack,//	魔法
		};

		enum Magic
		{
			NoMagic = -1,	//  無効
			FireBall,
			WaterBlast,
			Thunder,
			Beam,
		};
		Magic magicSelect;

		//思考＆状況判断(ステータス決定）
		void  Think();
		//モーションに対応した処理
		void  Move();
		//アニメーション制御
		BChara::DrawInfo  Anim();
		//接触時の応答処理(必ず受け身の処理として実装する)
		void Received(BChara* from_, AttackInfo at_) override;

		//倍率
		ML::Box2D DrawScale(ML::Box2D& me ,const int drawScale);
		//攻撃判定
		ML::Box2D attackBase;
		//連続攻撃フラグ
		bool attack2, attack3;
		//空中攻撃フラグ
		bool airattack;
		//二段ジャンプフラグ
		bool canJump;
		bool canDash;

		//攻撃力
		int power;
		//攻撃力倍率
		float powerScale;

		//攻撃発動
		void MakeAttack();

		//--------------------------------------
		//0329
		//マップ移動処理
		OL::Limit<int> moveMapCoolTime;		//マップ移動のクールタイム
		void CheckMoveMap();				//マップ移動判定
		bool CheckFallGround(float preY_, float estY_);				//すり抜ける床判定
		bool CheckFoot() override;
		//めり込まない移動処理
		virtual  void  CheckMove(ML::Vec2& e_) override;

		//--------------------------------------
	};
}