#pragma once
#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
// �G�ėp�X�[�p�[�N���X
// ��@���@��:22CI0333 ���J��E��N
// TODO:������Ή��������Ă�������
// �ҁ@�W�@��:
// �쐬�N����:2023/11/08
// �T�@�@�@�v:�G�֌W�̃N���X�Ɍp�������邽�߂̃N���X
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "BChara.h"

class BEnemy : public BChara
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BEnemy>		SP;
	typedef weak_ptr<BEnemy>		WP;
public:
	//�ύX������������������������������������������������������
	//�L�����N�^���ʃ����o�ϐ�
	
	//int			moveCnt,preMoveCnt;	//�s���J�E���^
	//OL::Limit<float> hp;	//�̗�
	

	//�L�����N�^�̍s����ԃt���O
	enum Motion
	{
		Unnon = -1,	// ����(�g���܂���j
		Stand,		// N��~
		Walk,		// N���s
		Tracking,	// A�ǔ�
		Attack,		// A�U��
		//	Jump,		// �W�����v
		Fall,		// ����
		//	TakeOff,	// ��ї��u��
		Landing,	// ���n
		Turn,		// �t������
		Bound,	// �e����΂���Ă���
		Lose,		// ���Œ�
	};
	//Motion			motion,preMotion;			//	���݂̍s���������t���O
	//int				animCnt;		//�@�A�j���[�V�����J�E���^
	//float			jumpPow;		//	�W�����v����
	//float			maxFallSpeed;	//	�����ő呬�x
	//float			gravity;		//	�t���[���P�ʂ̉��Z��
	//float			maxSpeed;		//	���E�����ւ̈ړ��̉��Z��
	//float			addSpeed;		//	���E�����ւ̈ړ��̉��Z��
	//float			decSpeed;		//	���E�����ւ̈ړ��̌�����
	//int				unHitTime;		//�@���G����

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BEnemy()
	{
	}
	virtual  ~BEnemy() {}

	//ML::Box2D CallHitBox() const;
	//virtual void Damaged(int damage_) {};

	
	//bool  UpdateMotion(int  nm_);

	

	

protected:
	//virtual void Think() {}		//�v�l����
	//virtual void Move() {}		//���쏈��
	virtual DrawInfo Anim();	//�A�j���[�V��������
	


	bool Attack_Std(string gn_, BChara::AttackInfo at_);				//�U�����ʏ���
	void UpDate_Std();													//�X�V���ʏ���
	void Render_Std(const DG::Image::SP& img_);							//�`�拤�ʏ���
};
