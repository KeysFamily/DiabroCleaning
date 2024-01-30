#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:���b�Z�[�W��
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "KeysString.h"

namespace ShopStaff
{
	class Object;
}

namespace  SystemMenuMessageWindow
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("SystemMenu");	//�O���[�v��
	const  string  defName("MessageWindow");	//�^�X�N��
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
		DG::Font::SP fontMsg;
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
		ML::Vec2 pos;
		ML::Vec2 staffPos;
		shared_ptr<ShopStaff::Object> shopStaff;

		ML::Vec2 messageStartPos;			//���b�Z�[�W�J�n�ʒu
		OL::Limit<int> currentMessagePos;	//���ݕ\�����Ă��镶��
		int appearMessageDistance;			//���b�Z�[�W�\���̊Ԋu�t���[����
		KeysString::Strings msg;
		string displayStr;					//��ʂɕ\�����镶��
		int appearMessageCount;				//���b�Z�[�W�\���p�J�E���g

		ML::Color messageColor;
		ML::Color outlineColor;
		//���b�Z�[�W�̐ݒ�
		void SetMessage(const string& fileName_);
		//void SetMessage(const string& msg_, int distance_ = 10);
		
	};
}