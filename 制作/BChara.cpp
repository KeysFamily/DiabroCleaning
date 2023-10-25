//-------------------------------------------------------------------
// キャラクタ汎用スーパークラス
// 作　成　者:全体
// TODO:↓いれば下も書いてください
// 編　集　者:
// 作成年月日:2023/07/05
// 概　　　要:キャラクター関係のクラスに継承させるためのクラス
//-------------------------------------------------------------------
#include "BChara.h"
#include  "MyPG.h"

ML::Vec2 BChara::MoveSet(int key)
{
	//0, 1, 2,  3,  4,  5,  6, 7, 8, 9,10,
	int table[] = { -1,90,270,-1,180,135,225,-1, 0,45,315 };
	float spd = 10.5f;
	ML::Vec2 est;
	//0は特殊条件
	if (key == 0)
	{
		est.x = 0.f;
		est.y = 0.f;
	}
	else
	{
		est.x = (float)cos(table[key] * D3DX_PI / 180.f) * spd;
		est.y = (float)-sin(table[key] * D3DX_PI / 180.f) * spd;
	}
	return est;
}
//接触判定
bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//接触時の応答処理（これ自体はダミーのようなモノ）
void BChara::Received(BChara* from_, int attack_)
{
	ML::MsgBox("Received 実装されていません");
}

ML::Box2D BChara::CallHitBox() const
{
	return hitBase.OffsetCopy(this->pos);
}