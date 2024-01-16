#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�V�X�e�����j���[ �Z���N�g�N���X
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2023/12/20
//�T�@�@�@�v:�V�X�e���̑I���̃I�u�W�F�N�g
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace  SystemMenuSelectObject
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("SystemMenu");	//�O���[�v��
	const  string  defName("SelectObject");	//�^�X�N��
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//���L����ϐ��͂����ɒǉ�����
		DG::Image::SP img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
	//�ύX�s����������������������������������������������������
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
		void  UpDate()			override;//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF()		override;//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	//�ύX������������������������������������������������������
	public:
		MyUI::SelectableObject* sto;
		ML::Box2D selectBox;
		float viewRate;	//�\�����[�g �P�ʂ�[�{]
	private:
		int animCnt;

		//�C�[�W���O�֘A
		bool isEasing;
		unsigned long easingCnt;
		void easingSet(ML::Box2D beforeBox_, ML::Box2D afterBox_, float beforeRate_ = 1.0f, float AfterRate_ = 1.0f, int easingFrame_ = 60);
		string enx, eny, enw, enh, enr;
	};
}