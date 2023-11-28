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
	int searchCnt;			//���G�J�E���g
	int notFoundPlayerCnt;	//�v���C�����T���Ȃ������Ƃ�	

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BEnemy()
		: searchCnt(0)
		, notFoundPlayerCnt(0)
	{
	}
	virtual  ~BEnemy() {}


protected:
	void InputJsonFile(string fileName_);

	virtual DrawInfo Anim();	//�A�j���[�V��������
	virtual bool SearchPlayer(int distX_ = 0, int distY_ = 0);//Player���G

	bool Attack_Std(string gn_, BChara::AttackInfo at_, ML::Box2D AttackHit_);				//�U�����ʏ���
	void UpDate_Std();													//�X�V���ʏ���
	void Render_Std(const DG::Image::SP& img_);							//�`�拤�ʏ���

	void DropCoins(unsigned int dropNum_);
};
