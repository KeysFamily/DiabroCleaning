#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�GBoss
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2024/1/9
//�T�@�@�@�v:�G�{�X
//?------------------------------------------------------
#include "BEnemy.h"

namespace  EnemyBoss
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("Enemy");	//�O���[�v��
	const  string  defName("Boss");		//�^�X�N��
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()		override;
		bool  Finalize()		override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
	//�ύX������������������������������������������������������
		//���L����ϐ��͂����ɒǉ�����
		DG::Image::SP img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BEnemy
	{
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:

		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//�u�������v�^�X�N�������ɂP�񂾂��s������
		void  UpDate()		override;	//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF()	override;	//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	public:
	//�ύX������������������������������������������������������
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		enum BossMotion {
			Unnon = -1,	// ����(�g���܂���j
			Stand,		// N��~
			Teleport,	// �e���|�[�g
			CoolTime,	// A�ǔ�
			Attack,
			Attack1,	// A�U��1 �{�X�̏񂩂�΂̖��@���o��
			Attack2,	// A�U��2 �v���C���[�̑����ɖ��@���o��1�b��ɍU��������
			Attack3,	// A�U��3 �{�X���甼�a�R�}�X�ȓ��ɔ͈͍U��������B
			Attack4,	// A�U��4 �{�X���甼�a5�}�X�ȓ��̓G�������\�ȃ����_���Ȉʒu�ɓG����������B
			Lose,		// ���Œ�
		};

		void Think()override;			//�v�l���󋵔��f
		void Move()override;			//���[�V�����ɑΉ���������
		BEnemy::DrawInfo Anim()override;//�A�j���[�V��������

		//�ڐG���̉�������(�K���󂯐g�̏����Ƃ��Ď�������)
		void Received(BEnemy* from_, AttackInfo at_);
	};
}