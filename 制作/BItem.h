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
class BItem : public BChara
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BItem>		SP;
	typedef weak_ptr<BItem>		WP;
public:

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BItem()
	{
	}
	virtual  ~BItem() {}

	//�v���C���[�̃X�e�[�^�X��ς���
	virtual void GiftPlayer(BChara* pl_) {}
};
