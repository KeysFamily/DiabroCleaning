//-------------------------------------------------------------------
// �G�ėp�X�[�p�[�N���X
// ��@���@��:22CI0333 ���J��E��N
// TODO:������Ή��������Ă�������
// �ҁ@�W�@��:
// �쐬�N����:2023/11/08
// �T�@�@�@�v:�G�֌W�̃N���X�Ɍp�������邽�߂̃N���X
//-------------------------------------------------------------------
#include  "BEnemy.h"
#include  "MyPG.h"
#include  "Task_Map.h"

//-----------------------------------------------------------------------------
//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
bool  BEnemy::UpdateMotion(Motion  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
		this->preMotion = this->motion;
		this->preMoveCnt = this->moveCnt;
		this->motion = nm_;		//���[�V�����ύX
		this->moveCnt = 0;		//�s���J�E���^�N���A
		this->animCnt = 0;		//�A�j���[�V�����J�E���^�̃N���A
		return  true;
	}
}
//-----------------------------------------------------------------------------
//����ڐG����
bool  BEnemy::CheckHead()
{
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(head);
}
//-----------------------------------------------------------------------------
//�߂荞�܂Ȃ��ړ�����
void BEnemy::CheckMove(ML::Vec2& e_)
{
	//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j

	//�����ɑ΂���ړ�
	while (e_.x != 0) {
		float  preX = this->pos.x;
		if (e_.x >= 1) { this->pos.x += 1;		e_.x -= 1; }
		else if (e_.x <= -1) { this->pos.x -= 1;		e_.x += 1; }
		else { this->pos.x += e_.x;		e_.x = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.x = preX;		//�ړ����L�����Z��
			break;
		}
	}
	//�c���ɑ΂���ړ�
	while (e_.y != 0) {
		float  preY = this->pos.y;
		if (e_.y >= 1) { this->pos.y += 1;		e_.y -= 1; }
		else if (e_.y <= -1) { this->pos.y -= 1;		e_.y += 1; }
		else { this->pos.y += e_.y;		e_.y = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//�ړ����L�����Z��
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//�����ڐG����
bool  BEnemy::CheckFoot()
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D  foot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		this->hitBase.w,
		1);
	foot.Offset(this->pos);

	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return  false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	//�}�b�v�ƐڐG����
	return map->CheckHit(foot);
}
//-----------------------------------------------------------------------------
//���ʐڐG����i�T�C�h�r���[�Q�[����p�j
bool  BEnemy::CheckFront_LR()
{
	//�����蔻�����ɂ��ċ�`�𐶐�(�Ƃ肠�����A���������P�ɂȂ�����`��p�ӂ���j
	ML::Box2D  front(this->hitBase.x,
		this->hitBase.y,
		1,
		this->hitBase.h);
	//�L�����N�^�̕����ɂ���`�̈ʒu�𒲐�
	if (this->angle_LR == Angle_LR::Left) {
		front.Offset(-1, 0);//�����Ɉړ�
	}
	else {
		front.Offset(this->hitBase.w, 0);//�E���Ɉړ�
	}
	//���݂̈ʒu�ɍ��킹��
	front.Offset((int)this->pos.x, (int)this->pos.y);
	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return  false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	//�}�b�v�ƐڐG����
	return map->CheckHit(front);
}
//-----------------------------------------------------------------------------
//���ʑ����`�F�b�N�i�T�C�h�r���[�Q�[����p�j
bool  BEnemy::CheckFrontFoot_LR()
{
	//�����蔻�����ɂ��ċ�`�𐶐�(�Ƃ肠�����A�c���Ɖ����P�ɂȂ�����`��p�ӂ���j
	ML::Box2D  frontFoot(this->hitBase.x,
		this->hitBase.y + this->hitBase.w,
		1,
		1);
	//�L�����N�^�̕����ɂ���`�̈ʒu�𒲐�
	if (this->angle_LR == Angle_LR::Left) {
		frontFoot.Offset(-1, 0);//�����Ɉړ�
	}
	else {
		frontFoot.Offset(this->hitBase.w, 0);//�E���Ɉړ�
	}
	//���݂̈ʒu�ɍ��킹��
	frontFoot.Offset((int)this->pos.x, (int)this->pos.y);
	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return  false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	//�}�b�v�ƐڐG����
	return map->CheckHit(frontFoot);
}
//-----------------------------------------------------------------------------
//�ڐG����
bool BEnemy::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//-----------------------------------------------------------------------------
//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
void BEnemy::Received(BChara* from_, AttackInfo at_)
{
	ML::MsgBox("Received ��������Ă��܂���");
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
// ���ʂ̍U������
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
// ���ʂ̍X�V����
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
// ���ʂ̕`�揈��
void BEnemy::Render_Std(const DG::Image::SP& img_) {
	DrawInfo di = this->Anim();
	di.draw.Offset(this->pos);
	di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
	img_->Draw(di.draw, di.src, di.color);
}