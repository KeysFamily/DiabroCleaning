//-------------------------------------------------------------------
// 敵汎用スーパークラス
// 作　成　者:22CI0333 長谷川勇一朗
// TODO:↓いれば下も書いてください
// 編　集　者:
// 作成年月日:2023/11/08
// 概　　　要:敵関係のクラスに継承させるためのクラス
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "BEnemy.h"

//-----------------------------------------------------------------------------
//アニメーション制御
BChara::DrawInfo BEnemy::Anim() {
	BChara::DrawInfo di{ ML::Box2D(),ML::Box2D(),ML::Color(1.0f,1.0f,1.0f,1.0f)};
	return di;
}

//-----------------------------------------------------------------------------
// 共通の攻撃処理
bool BEnemy::Attack_Std(string gn_, BChara::AttackInfo at_, ML::Box2D AttackHit_) {
	ML::Box2D me = this->CallHitBox();
	auto targets = ge->GetTasks<BChara>(gn_);
	for (auto it = targets->begin(); it != targets->end(); ++it) {
		if ((*it)->CheckHit(AttackHit_)) {
			(*it)->Received(this, at_);
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
	++this->searchCnt;

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