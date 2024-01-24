#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�΋�
//��@���@��:22CI0306 �� �� ��
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:22CI0333 ���J�� �E��N
//�쐬�N����:
//�T�@�@�@�v:�΋�(�{�X�U��)
//?------------------------------------------------------
#include "BChara.h"

namespace  EnemyFireBall
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("Magic");	//�O���[�v��
	const  string  defName("FireBall");		//�^�X�N��
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
		DG::Image::SP	img;
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
		float power;	//�_���[�W

		//�A�j���[�V��������
		void Setting(ML::Vec2 setPos_, float speed_, float rad_, float AttackPow_);

	private:
		BChara::DrawInfo  Anim();
	};
}