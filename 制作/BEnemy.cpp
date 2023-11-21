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

//-----------------------------------------------------------------------------
//�A�j���[�V��������
BChara::DrawInfo BEnemy::Anim() {
	BChara::DrawInfo di{ ML::Box2D(),ML::Box2D(),ML::Color(1.0f,1.0f,1.0f,1.0f)};
	return di;
}

//-----------------------------------------------------------------------------
// ���ʂ̍U������
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
}
//-----------------------------------------------------------------------------
// ���ʂ̕`�揈��
void BEnemy::Render_Std(const DG::Image::SP& img_) {
	DrawInfo di = this->Anim();
	di.draw.Offset(this->pos);
	di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
	img_->Draw(di.draw, di.src, di.color);
}