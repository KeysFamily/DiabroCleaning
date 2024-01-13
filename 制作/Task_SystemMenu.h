#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:���j���[��ʊǗ�
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace PlayerStatus
{
	class Object;
}
namespace SystemMenuMessageWindow
{
	class Object;
}
namespace SkillSelect
{
	class Object;
}
namespace SystemMenuMoneyDisplay
{
	class Object;
}
namespace SystemMenuBackButton
{
	class Object;
}


namespace  SystemMenu
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("SystemMenu");	//�O���[�v��
	const  string  defName("System");	//�^�X�N��
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
		void SetPos();
		void AppearUpDate();
		void DisappearUpdate();

	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		ML::Vec2 pos;
		ML::Vec2 skillPos;
		ML::Vec2 statusPos;
		ML::Vec2 backPos;
		ML::Vec2 msgPos;
		ML::Vec2 moneyPos;

		shared_ptr<PlayerStatus::Object> status;
		shared_ptr<SystemMenuMessageWindow::Object> message;
		shared_ptr<SkillSelect::Object> skill;
		shared_ptr<SystemMenuMoneyDisplay::Object> moneyDp;
		shared_ptr<SystemMenuBackButton::Object> back;

		bool finishedAppear;	//�o�����I��������
		bool finishFlag;		//�I�����邩

		//�I������
		void FinishMenu();
	};
}