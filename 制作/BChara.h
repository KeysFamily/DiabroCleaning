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
	{
	}
	virtual  ~BChara() {}
	ML::Box2D CallHitBox() const;
	virtual void Damaged(int damage_) {};

	//攻撃情報
	struct AttackInfo {
		int	power;
		int	hit;
		int element;
		//その他必要に応じて
	};

	//接触時の応答処理（これ自体はダミーのようなモノ）
	virtual void Received(BChara* from_, int attack_);
	//接触判定
	virtual bool CheckHit(const ML::Box2D& hit_);

protected:
	virtual void Think() {}
	virtual void Move() {}
	ML::Vec2 MoveSet(int key);
};
