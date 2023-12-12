//-------------------------------------------------------------------
// �G�ėp�X�[�p�[�N���X
// ��@���@��:22CI0333 ���J��E��N
// TODO:������Ή��������Ă�������
// �ҁ@�W�@��:
// �쐬�N����:2023/11/08
// �T�@�@�@�v:�G�֌W�̃N���X�Ɍp�������邽�߂̃N���X
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "BEnemy.h"

#include  "Task_Item_coin.h"

#include  "nlohmann/json.hpp"
using json = nlohmann::json;

//-----------------------------------------------------------------------------
//�t�@�C���ǂݍ���
void BEnemy::InputJsonFile(string enemyName_) {
	//****************************************
	//�t�@�C������󂯕t�������
	//�E�̗�
	//�E�W�����v��
	//�E���E �ő呬�x
	//�E���E �����x
	//�E���E ������
	// 
	//�E���G����
	// 
	//�E�h���b�v���邨���̗�
	//�E�U����
	// 
	//****************************************
	ifstream f("./data/enemy/json/enemy.json");
	if (!f.is_open()) return;//�t�@�C���I�[�v���Ɏ��s
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
//�A�j���[�V��������
BChara::DrawInfo BEnemy::Anim() {
	BChara::DrawInfo di{ ML::Box2D(),ML::Box2D(),ML::Color(1.0f,1.0f,1.0f,1.0f)};
	return di;
}

//-----------------------------------------------------------------------------
//Player���G����
bool BEnemy::SearchPlayer(int distX_, int distY_) {
	ML::MsgBox("��������������Ă��܂���");
	return false;
}

//-----------------------------------------------------------------------------
// �R�C�����h���b�v����
void BEnemy::DropCoins(unsigned int dropNum_) {
	for (unsigned int i = 0; i < dropNum_; ++i) {
		auto coin = Item_coin::Object::Create(true);
		coin->pos = this->pos;
	}
}

//-----------------------------------------------------------------------------
// ���ʂ̍U������
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
// ���ʂ̍X�V����
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
// ���ʂ̕`�揈��
void BEnemy::Render_Std(const DG::Image::SP& img_) {
	DrawInfo di = this->Anim();
	di.draw.Offset(this->pos);
	di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
	img_->Draw(di.draw, di.src, di.color);
}
