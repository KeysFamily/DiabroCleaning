#pragma once
#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
// キャラクタ汎用スーパークラス
// 作　成　者:全体
// TODO:↓いれば下も書いてください
// 編　集　者:
// 作成年月日:2023/07/05
// 概　　　要:キャラクター関係のクラスに継承させるためのクラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数
	ML::Vec2    pos;		//キャラクタ位置
	ML::Box2D   hitBase;	//あたり判定範囲
	ML::Box2D   map_hitBase;//マップ当たり判定
	ML::Vec2	moveVec;	//移動ベクトル
	int			moveCnt;	//行動カウンタ
	float       hp;         //体力
	float		maxHp;		//最大体力
	float		speed;      //移動スピード
	//向き（2D視点）
	float angle;
	//左右の向き（2D横視点ゲーム専用）
	enum class Angle_LR { Left, Right };
	Angle_LR	angle_LR;
	WP			target;

	//キャラクタの行動状態フラグ
	enum class Motion
	{
		Unnon = -1,	//	無効(使えません）
		Stand,		//	停止
		Walk,		//	歩行
		Attack,		//	攻撃
		Jump,		//	ジャンプ
		Fall,		//	落下
		TakeOff,	//	飛び立つ瞬間
		Landing,	//	着地
		Crouch,		//  しゃがみ
		CrouchWalk,	//	しゃがみながら移動
		Turn,		//	方向転換
		Bound,		//	弾き飛ばされてる
		Lose,		//  消滅中
	};
	Motion			motion;			//	現在の行動を示すフラグ
	int				animCnt;		//　アニメーションカウンタ
	float			jumpPow;		//	ジャンプ初速
	float			maxFallSpeed;	//	落下最大速度
	float			gravity;		//	フレーム単位の加算量
	float			maxSpeed;		//	左右方向への移動の加算量
	float			addSpeed;		//	左右方向への移動の加算量
	float			crouchSpeed;	//	しゃがみながら移動の加算量
	float			decSpeed;		//	左右方向への移動の減衰量
	int				unHitTime;		//　無敵時間

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加する事★★
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, map_hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		, hp(1.f)
		, maxHp(1.f)
		, speed(0.f)
		, angle(0.f)
		, angle_LR(Angle_LR::Right)
		, motion(Motion::Stand)
		, animCnt(0)
		, jumpPow(0.f)
		, maxFallSpeed(0.f)
		, gravity(0.f)
		, maxSpeed(0.f)
		, addSpeed(0.f)
		, crouchSpeed(0)
		, decSpeed(0.f)
		, unHitTime(0)
	{
	}
	virtual  ~BChara() {}

	ML::Box2D CallHitBox() const;
	virtual void Damaged(int damage_) {};

	//キャラクタ共通メソッド
	//めり込まない移動処理
	virtual  void  CheckMove(ML::Vec2& est_);
	//足元接触判定
	bool  CheckFoot();
	//頭上接触判定
	virtual  bool  CheckHead();
	//正面接触判定（サイドビューゲーム専用）
	virtual  bool  CheckFront_LR();
	//正面足元チェック（サイドビューゲーム専用）
	virtual  bool  CheckFrontFoot_LR();
	//モーションを更新（変更なしの場合	false)
	bool  UpdateMotion(Motion  nm_);

	//	アニメーション情報構造体
	struct DrawInfo {
		ML::Box2D		draw, src;
		ML::Color		color;
	};
	//攻撃情報
	struct AttackInfo {
		int	power;
		int	hit;
		int element;
		//その他必要に応じて
	};

	//接触時の応答処理（これ自体はダミーのようなモノ）
	virtual void Received(BChara* from_, AttackInfo at_);
	//接触判定
	virtual bool CheckHit(const ML::Box2D& hit_);

protected:
	virtual void Think() {}
	virtual void Move() {}
	ML::Vec2 MoveSet(int key);
};
