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
class BItem : public BChara
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BItem>		SP;
	typedef weak_ptr<BItem>		WP;
public:

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加する事★★
	BItem()
	{
	}
	virtual  ~BItem() {}

	//プレイヤーのステータスを変える
	virtual void GiftPlayer(BChara* pl_) {}
};
