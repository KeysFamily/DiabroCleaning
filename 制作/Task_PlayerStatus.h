#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�v���C���[�X�e�[�^�X���
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace PlayerStatusShop
{
	class Object;
}

namespace MyUI
{
	class SelectableObject;
}

namespace  PlayerStatus
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("SystemMenu");	//�O���[�v��
	const  string  defName("PlayerStatus");	//�^�X�N��
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
		DG::Image::SP imgBg;
		OL::Size2D imgBgSize;
		DG::Font::SP systemFont;
		OL::Size2D systemFontSize;
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
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		enum PStatus
		{
			ATK = 0,
			DEF,
			INT,
			SPD,
		};
		
		shared_ptr<PlayerStatusShop::Object> shops[4];
		int currentStatus;

		ML::Vec2 pos;
		ML::Vec2 shopOffset;
		float shopDistance;

		ML::Vec2 statusBeginPos;
		float statusDistance;

		MyUI::SelectableObject* currentShop;		//�I�𒆂̃V���b�v
		
		void SetDownObj(MyUI::SelectableObject* nextObj_);
		void SetRightObj(MyUI::SelectableObject* nextObj_);
	};
}