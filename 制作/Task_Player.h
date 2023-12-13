#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�v���C���[
//-------------------------------------------------------------------
#include "BChara.h"

namespace  Player
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("Player");	//�O���[�v��
	const  string  defName("Player");		//�^�X�N��
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
	class  Object : public  BChara
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
			//BChara�Ɋ܂܂�Ȃ����m�݂̂����ɒǉ�����

		OL::Size2D cutSize;
		int imgTurn;
		OL::Limit<int> select;
		int drawScale;
		ML::Box2D initialHitBase;
		ML::Box2D crouchHitBase;

		void LoadImg();

		XI::GamePad::SP  controller;

		//�L�����N�^�̍s����ԃt���O
		enum Motion
		{
			Unnon = -1,	//	����(�g���܂���j
			Stand,		//	��~
			Walk,		//	���s
			Attack,		//	�U��1
			Attack2,	//	�U��2
			Attack3,	//	�U��3
			AirAttack,	//	�󒆍U��1
			AirAttack2,	//	�󒆍U��2
			AirAttack3,	//	�󒆍U��3
			AirAttack4,	//	�󒆍U��4
			Jump,		//	�W�����v
			Jump2,		//	��i�W�����v
			Fall,		//	����
			Fall2,		//	����(��i�W�����v��)
			TakeOff,	//	��ї��u��
			Landing,	//	���n
			Crouch,		//  ���Ⴊ��
			CrouchWalk,	//	���Ⴊ�݂Ȃ���ړ�
			Turn,		//	�����]��
			Bound,		//	�e����΂���Ă�
			Lose,		//  ���Œ�
			Dash,		//  �_�b�V��
			MagicAttack,//	���@
		};

		enum Magic
		{
			NoMagic = -1,	//  ����
			FireBall,
			WaterBlast,
			Thunder,
			Beam,
		};
		Magic magicSelect;

		//�v�l���󋵔��f(�X�e�[�^�X����j
		void  Think();
		//���[�V�����ɑΉ���������
		void  Move();
		//�A�j���[�V��������
		BChara::DrawInfo  Anim();
		//�ڐG���̉�������(�K���󂯐g�̏����Ƃ��Ď�������)
		void Received(BChara* from_, AttackInfo at_) override;

		//�{��
		ML::Box2D DrawScale(ML::Box2D& me ,const int drawScale);
		//�U������
		ML::Box2D attackBase;
		//�A���U���t���O
		bool attack2, attack3;
		//�󒆍U���t���O
		bool airattack;
		//��i�W�����v�t���O
		bool canJump;
		bool canDash;

		//�U����
		int power;
		//�U���͔{��
		float powerScale;

		//�U������
		void MakeAttack();

		//--------------------------------------
		//0329
		//�}�b�v�ړ�����
		OL::Limit<int> moveMapCoolTime;		//�}�b�v�ړ��̃N�[���^�C��
		void CheckMoveMap();				//�}�b�v�ړ�����
		bool CheckFallGround(float preY_, float estY_);				//���蔲���鏰����
		bool CheckFoot() override;
		//�߂荞�܂Ȃ��ړ�����
		virtual  void  CheckMove(ML::Vec2& e_) override;

		//--------------------------------------
	};
}