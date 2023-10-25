#pragma once
#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
// �L�����N�^�ėp�X�[�p�[�N���X
// ��@���@��:�S��
// TODO:������Ή��������Ă�������
// �ҁ@�W�@��:
// �쐬�N����:2023/07/05
// �T�@�@�@�v:�L�����N�^�[�֌W�̃N���X�Ɍp�������邽�߂̃N���X
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//�ύX������������������������������������������������������
	//�L�����N�^���ʃ����o�ϐ�
	ML::Vec2    pos;		//�L�����N�^�ʒu
	ML::Box2D   hitBase;	//�����蔻��͈�
	ML::Box2D   map_hitBase;//�}�b�v�����蔻��
	ML::Vec2	moveVec;	//�ړ��x�N�g��
	int			moveCnt;	//�s���J�E���^
	float       hp;         //�̗�
	float		maxHp;		//�ő�̗�
	float		speed;      //�ړ��X�s�[�h
	//�����i2D���_�j
	float angle;

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, map_hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		, hp(1.f)
		, maxHp(1.f)
		, speed(0.f)
		, angle(0.f)
	{
	}
	virtual  ~BChara() {}
	ML::Box2D CallHitBox() const;
	virtual void Damaged(int damage_) {};

	//�U�����
	struct AttackInfo {
		int	power;
		int	hit;
		int element;
		//���̑��K�v�ɉ�����
	};

	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
	virtual void Received(BChara* from_, int attack_);
	//�ڐG����
	virtual bool CheckHit(const ML::Box2D& hit_);

protected:
	virtual void Think() {}
	virtual void Move() {}
	ML::Vec2 MoveSet(int key);
};
