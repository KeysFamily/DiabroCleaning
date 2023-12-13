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

#include  "Task_Item_coin.h"

#include  "nlohmann/json.hpp"
using json = nlohmann::json;

//-----------------------------------------------------------------------------
//ファイル読み込み
void BEnemy::InputJsonFile(string enemyName_) {
	//****************************************
	//ファイルから受け付けるもの
	//・体力
	//・ジャンプ力
	//・左右 最大速度
	//・左右 加速度
	//・左右 減衰量
	// 
	//・無敵時間
	// 
	//・ドロップするお金の量
	//・攻撃力
	// 
	//****************************************
	ifstream f("./data/enemy/json/enemy.json");
	if (!f.is_open()) return;//ファイルオープンに失敗
	json data = json::parse(f);
	auto& e = data[enemyName_];

	int HP = e["hp"];
	this->hp.SetValues(HP, 0, HP);
	this->jumpPow = e["jumpPow"];
	this->maxSpeed = e["maxSpeed"];
	this->addSpeed = e["addSpeed"];
	this->decSpeed = e["decSpeed"];
	this->unHitTime = e["unHitTime"];
	int a = e["dropMoney"];
	int b = e["attack"];


	f.close();

}

//-----------------------------------------------------------------------------
//アニメーション制御
BChara::DrawInfo BEnemy::Anim() {
	BChara::DrawInfo di{ ML::Box2D(),ML::Box2D(),ML::Color(1.0f,1.0f,1.0f,1.0f)};
	return di;
}

//-----------------------------------------------------------------------------
//Player索敵制御
bool BEnemy::SearchPlayer(int distX_, int distY_) {
	ML::MsgBox("処理が実装されていません");
	return false;
}

//-----------------------------------------------------------------------------
// コインをドロップする
void BEnemy::DropCoins(unsigned int dropNum_) {
	for (unsigned int i = 0; i < dropNum_; ++i) {
		auto coin = Item_coin::Object::Create(true);
		coin->pos = this->pos;
	}
}

//-----------------------------------------------------------------------------
// 共通の攻撃処理
bool BEnemy::Attack_Std(string gn_, BChara::AttackInfo at_, ML::Box2D AttackHit_) {
	if (this->hp.vnow > 0) {
		ML::Box2D me = this->CallHitBox();
		auto targets = ge->GetTasks<BChara>(gn_);
		for (auto it = targets->begin(); it != targets->end(); ++it) {
			if ((*it)->CheckHit(AttackHit_)) {
				(*it)->Received(this, at_);
				return true;
			}
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
	if (ge->qa_Map != nullptr && !ge->qa_Map->hitBase.Hit(this->CallHitBox())) {
		this->Kill();
	}
}
//-----------------------------------------------------------------------------
// 共通の描画処理
void BEnemy::Render_Std(const DG::Image::SP& img_) {
	DrawInfo di = this->Anim();
	di.draw.Offset(this->pos);
	di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
	img_->Draw(di.draw, di.src, di.color);
}
