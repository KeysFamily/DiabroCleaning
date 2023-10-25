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
//�ڐG����
bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
void BChara::Received(BChara* from_, int attack_)
{
	ML::MsgBox("Received ��������Ă��܂���");
}

ML::Box2D BChara::CallHitBox() const
{
	return hitBase.OffsetCopy(this->pos);
}