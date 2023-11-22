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
#include  "Task_Map.h"

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
//-----------------------------------------------------------------------------
//モーションを更新（変更なしの場合	false)
bool  BChara::UpdateMotion(int  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
		//ひとつ前の状態を保持
		this->preMotion = this->motion;
		this->preMoveCnt = this->moveCnt;

		this->motion = nm_;		//モーション変更
		this->moveCnt = 0;		//行動カウンタクリア
		this->animCnt = 0;		//アニメーションカウンタのクリア
		return  true;
	}
}
//-----------------------------------------------------------------------------
//頭上接触判定
bool  BChara::CheckHead()
{
	//あたり判定を基にして頭上矩形を生成
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return false; }//マップが無ければ判定しない(出来ない）
	return map->CheckHit(head)
		|| map->CheckSlope(head) != ML::Vec2(0,0);
}
//-----------------------------------------------------------------------------
//めり込まない移動処理
void BChara::CheckMove(ML::Vec2& e_)
{
	//マップが存在するか調べてからアクセス
	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return; }//マップが無ければ判定しない(出来ない）

	//横軸に対する移動
	while (e_.x != 0) {
		float  preX = this->pos.x;
		if (e_.x >= 1) { this->pos.x += 1;		e_.x -= 1; }
		else if (e_.x <= -1) { this->pos.x -= 1;		e_.x += 1; }
		else { this->pos.x += e_.x;		e_.x = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);

		//坂道判定
		ML::Vec2 move = map->CheckSlope(hit);
		ge->printToDebugFile(to_string(move.x) + "/" + to_string(move.y), 60);

		if (true == map->CheckHit(hit)) {
			this->pos.x = preX;		//移動をキャンセル
			break;
		}
	}
	//縦軸に対する移動
	while (e_.y != 0) {
		float  preY = this->pos.y;
		if (e_.y >= 1) { this->pos.y += 1;		e_.y -= 1; }
		else if (e_.y <= -1) { this->pos.y -= 1;		e_.y += 1; }
		else { this->pos.y += e_.y;		e_.y = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);

		//坂道判定
		ML::Vec2 move = map->CheckSlope(hit);
		ge->printToDebugFile(to_string(move.x) + "/" + to_string(move.y), 60);


		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//移動をキャンセル
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//足元接触判定
bool  BChara::CheckFoot()
{
	//あたり判定を基にして足元矩形を生成
	ML::Box2D  foot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		this->hitBase.w,
		1);
	foot.Offset(this->pos);

	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return  false; }//マップが無ければ判定しない(出来ない）
	//マップと接触判定
	return map->CheckHit(foot)
		|| map->CheckSlope(foot) != ML::Vec2(0, 0);
}
//-----------------------------------------------------------------------------
//正面接触判定（サイドビューゲーム専用）
bool  BChara::CheckFront_LR()
{
	//あたり判定を基にして矩形を生成(とりあえず、横幅だけ１になった矩形を用意する）
	ML::Box2D  front(this->hitBase.x,
		this->hitBase.y,
		1,
		this->hitBase.h);
	//キャラクタの方向により矩形の位置を調整
	if (this->angle_LR == Angle_LR::Left) {
		front.Offset(-1, 0);//左側に移動
	}
	else {
		front.Offset(this->hitBase.w, 0);//右側に移動
	}
	//現在の位置に合わせる
	front.Offset((int)this->pos.x, (int)this->pos.y);
	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return  false; }//マップが無ければ判定しない(出来ない）
	//マップと接触判定
	return map->CheckHit(front);
}
//-----------------------------------------------------------------------------
//正面足元チェック（サイドビューゲーム専用）
bool  BChara::CheckFrontFoot_LR()
{
	//あたり判定を基にして矩形を生成(とりあえず、縦幅と横幅１になった矩形を用意する）
	ML::Box2D  frontFoot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		1,
		1);
	//キャラクタの方向により矩形の位置を調整
	if (this->angle_LR == Angle_LR::Left) {
		frontFoot.Offset(-1, 0);//左側に移動
	}
	else {
		frontFoot.Offset(this->hitBase.w, 0);//右側に移動
	}
	//現在の位置に合わせる
	frontFoot.Offset((int)this->pos.x, (int)this->pos.y);
	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return  false; }//マップが無ければ判定しない(出来ない）
	//マップと接触判定
	return map->CheckHit(frontFoot);
}
//-----------------------------------------------------------------------------
//接触判定
bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//-----------------------------------------------------------------------------
//接触時の応答処理（これ自体はダミーのようなモノ）
void BChara::Received(BChara* from_, AttackInfo at_)
{
	ML::MsgBox("Received 実装されていません");
}

ML::Box2D BChara::CallHitBox() const
{
	return hitBase.OffsetCopy(this->pos);
}