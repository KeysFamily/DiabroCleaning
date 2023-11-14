//-------------------------------------------------------------------
// 敵汎用スーパークラス
// 作　成　者:22CI0333 長谷川勇一朗
// TODO:↓いれば下も書いてください
// 編　集　者:
// 作成年月日:2023/11/08
// 概　　　要:敵関係のクラスに継承させるためのクラス
//-------------------------------------------------------------------
#include  "BEnemy.h"
#include  "MyPG.h"
#include  "Task_Map.h"

//-----------------------------------------------------------------------------
//モーションを更新（変更なしの場合	false)
bool  BEnemy::UpdateMotion(Motion  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
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
bool  BEnemy::CheckHead()
{
	//あたり判定を基にして頭上矩形を生成
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return false; }//マップが無ければ判定しない(出来ない）
	return map->CheckHit(head);
}
//-----------------------------------------------------------------------------
//めり込まない移動処理
void BEnemy::CheckMove(ML::Vec2& e_)
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
		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//移動をキャンセル
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//足元接触判定
bool  BEnemy::CheckFoot()
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
	return map->CheckHit(foot);
}
//-----------------------------------------------------------------------------
//正面接触判定（サイドビューゲーム専用）
bool  BEnemy::CheckFront_LR()
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
bool  BEnemy::CheckFrontFoot_LR()
{
	//あたり判定を基にして矩形を生成(とりあえず、縦幅と横幅１になった矩形を用意する）
	ML::Box2D  frontFoot(this->hitBase.x,
		this->hitBase.y + this->hitBase.w,
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
bool BEnemy::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//-----------------------------------------------------------------------------
//接触時の応答処理（これ自体はダミーのようなモノ）
void BEnemy::Received(BChara* from_, AttackInfo at_)
{
	ML::MsgBox("Received 実装されていません");
}

ML::Box2D BEnemy::CallHitBox() const
{
	return hitBase.OffsetCopy(this->pos);
}

BEnemy::DrawInfo BEnemy::Anim() {
	DrawInfo di{ ML::Box2D(),ML::Box2D(),ML::Color(1.0f,1.0f,1.0f,1.0f) };
	return di;
}

//-----------------------------------------------------------------------------
// 共通の攻撃処理
bool BEnemy::Attack_Std(string gn_, BChara::AttackInfo at_) {
	ML::Box2D me = this->CallHitBox();
	auto targets = ge->GetTasks<BChara>(gn_);
	for (auto it = targets->begin(); it != targets->end(); ++it) {
		if ((*it)->CheckHit(me)) {
			//(*it)->Received(this, at_);
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// 共通の更新処理
void BEnemy::UpDate_Std() {
	++this->moveCnt;
	++this->animCnt;
	if (this->unHitTime > 0) { --this->unHitTime; }
	this->Think();
	this->Move();
	ML::Vec2 est = this->moveVec;
	this->CheckMove(est);
}

//-----------------------------------------------------------------------------
// 共通の描画処理
void BEnemy::Render_Std(const DG::Image::SP& img_) {
	DrawInfo di = this->Anim();
	di.draw.Offset(this->pos);
	di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
	img_->Draw(di.draw, di.src, di.color);
}