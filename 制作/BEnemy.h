#pragma once
#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
// 敵汎用スーパークラス
// 作　成　者:22CI0333 長谷川勇一朗
// TODO:↓いれば下も書いてください
// 編　集　者:
// 作成年月日:2023/11/08
// 概　　　要:敵関係のクラスに継承させるためのクラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "BChara.h"

class BEnemy : public BChara
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BEnemy>		SP;
	typedef weak_ptr<BEnemy>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数
	
	//int			moveCnt,preMoveCnt;	//行動カウンタ
	//OL::Limit<float> hp;	//体力
	

	//キャラクタの行動状態フラグ
	enum Motion
	{
		Unnon = -1,	// 無効(使えません）
		Stand,		// N停止
		Walk,		// N歩行
		Tracking,	// A追尾
		Attack,		// A攻撃
		//	Jump,		// ジャンプ
		Fall,		// 落下
		//	TakeOff,	// 飛び立つ瞬間
		Landing,	// 着地
		Turn,		// 逆を向く
		Bound,	// 弾き飛ばされている
		Lose,		// 消滅中
	};
	//Motion			motion,preMotion;			//	現在の行動を示すフラグ
	//int				animCnt;		//　アニメーションカウンタ
	//float			jumpPow;		//	ジャンプ初速
	//float			maxFallSpeed;	//	落下最大速度
	//float			gravity;		//	フレーム単位の加算量
	//float			maxSpeed;		//	左右方向への移動の加算量
	//float			addSpeed;		//	左右方向への移動の加算量
	//float			decSpeed;		//	左右方向への移動の減衰量
	//int				unHitTime;		//　無敵時間

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加する事★★
	BEnemy()
	{
	}
	virtual  ~BEnemy() {}

	//ML::Box2D CallHitBox() const;
	//virtual void Damaged(int damage_) {};

	
	//bool  UpdateMotion(int  nm_);

	

	

protected:
	//virtual void Think() {}		//思考処理
	//virtual void Move() {}		//動作処理
	virtual DrawInfo Anim();	//アニメーション制御
	


	bool Attack_Std(string gn_, BChara::AttackInfo at_);				//攻撃共通処理
	void UpDate_Std();													//更新共通処理
	void Render_Std(const DG::Image::SP& img_);							//描画共通処理
};
