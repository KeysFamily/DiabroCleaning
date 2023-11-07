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
	//���E�̌����i2D�����_�Q�[����p�j
	enum class Angle_LR { Left, Right };
	Angle_LR	angle_LR;
	WP			target;

	//�L�����N�^�̍s����ԃt���O
	enum class Motion
	{
		Unnon = -1,	//	����(�g���܂���j
		Stand,		//	��~
		Walk,		//	���s
		Attack,		//	�U��
		Jump,		//	�W�����v
		Fall,		//	����
		TakeOff,	//	��ї��u��
		Landing,	//	���n
		Crouch,		//  ���Ⴊ��
		CrouchWalk,	//	���Ⴊ�݂Ȃ���ړ�
		Turn,		//	�����]��
		Bound,		//	�e����΂���Ă�
		Lose,		//  ���Œ�
	};
	Motion			motion;			//	���݂̍s���������t���O
	int				animCnt;		//�@�A�j���[�V�����J�E���^
	float			jumpPow;		//	�W�����v����
	float			maxFallSpeed;	//	�����ő呬�x
	float			gravity;		//	�t���[���P�ʂ̉��Z��
	float			maxSpeed;		//	���E�����ւ̈ړ��̉��Z��
	float			addSpeed;		//	���E�����ւ̈ړ��̉��Z��
	float			crouchSpeed;	//	���Ⴊ�݂Ȃ���ړ��̉��Z��
	float			decSpeed;		//	���E�����ւ̈ړ��̌�����
	int				unHitTime;		//�@���G����

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
		, angle_LR(Angle_LR::Right)
		, motion(Motion::Stand)
		, animCnt(0)
		, jumpPow(0.f)
		, maxFallSpeed(0.f)
		, gravity(0.f)
		, maxSpeed(0.f)
		, addSpeed(0.f)
		, crouchSpeed(0)
		, decSpeed(0.f)
		, unHitTime(0)
	{
	}
	virtual  ~BChara() {}

	ML::Box2D CallHitBox() const;
	virtual void Damaged(int damage_) {};

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	virtual  void  CheckMove(ML::Vec2& est_);
	//�����ڐG����
	bool  CheckFoot();
	//����ڐG����
	virtual  bool  CheckHead();
	//���ʐڐG����i�T�C�h�r���[�Q�[����p�j
	virtual  bool  CheckFront_LR();
	//���ʑ����`�F�b�N�i�T�C�h�r���[�Q�[����p�j
	virtual  bool  CheckFrontFoot_LR();
	//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
	bool  UpdateMotion(Motion  nm_);

	//	�A�j���[�V�������\����
	struct DrawInfo {
		ML::Box2D		draw, src;
		ML::Color		color;
	};
	//�U�����
	struct AttackInfo {
		int	power;
		int	hit;
		int element;
		//���̑��K�v�ɉ�����
	};

	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
	virtual void Received(BChara* from_, AttackInfo at_);
	//�ڐG����
	virtual bool CheckHit(const ML::Box2D& hit_);

protected:
	virtual void Think() {}
	virtual void Move() {}
	ML::Vec2 MoveSet(int key);
};
