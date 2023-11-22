//-------------------------------------------------------------------
// �L�����N�^�ėp�X�[�p�[�N���X
// ��@���@��:�S��
// TODO:������Ή��������Ă�������
// �ҁ@�W�@��:
// �쐬�N����:2023/07/05
// �T�@�@�@�v:�L�����N�^�[�֌W�̃N���X�Ɍp�������邽�߂̃N���X
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
	//0�͓������
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
//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
bool  BChara::UpdateMotion(int  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
		//�ЂƂO�̏�Ԃ�ێ�
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
bool  BChara::CheckHead()
{
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask<Map::Object>(Map::defGroupName, Map::defName);
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(head)
		|| map->CheckSlope(head) != ML::Vec2(0,0);
}
//-----------------------------------------------------------------------------
//�߂荞�܂Ȃ��ړ�����
void BChara::CheckMove(ML::Vec2& e_)
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

		//�⓹����
		ML::Vec2 move = map->CheckSlope(hit);
		ge->printToDebugFile(to_string(move.x) + "/" + to_string(move.y), 60);

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

		//�⓹����
		ML::Vec2 move = map->CheckSlope(hit);
		ge->printToDebugFile(to_string(move.x) + "/" + to_string(move.y), 60);


		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//�ړ����L�����Z��
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//�����ڐG����
bool  BChara::CheckFoot()
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
	return map->CheckHit(foot)
		|| map->CheckSlope(foot) != ML::Vec2(0, 0);
}
//-----------------------------------------------------------------------------
//���ʐڐG����i�T�C�h�r���[�Q�[����p�j
bool  BChara::CheckFront_LR()
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
bool  BChara::CheckFrontFoot_LR()
{
	//�����蔻�����ɂ��ċ�`�𐶐�(�Ƃ肠�����A�c���Ɖ����P�ɂȂ�����`��p�ӂ���j
	ML::Box2D  frontFoot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
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
bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//-----------------------------------------------------------------------------
//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
void BChara::Received(BChara* from_, AttackInfo at_)
{
	ML::MsgBox("Received ��������Ă��܂���");
}

ML::Box2D BChara::CallHitBox() const
{
	return hitBase.OffsetCopy(this->pos);
}