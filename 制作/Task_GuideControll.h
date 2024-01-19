#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�R���g���[���[�K�C�h
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  GuideControll
{
	enum Button
	{
		X,
		A,
		Y,
		B,
		LStick,
		RStick,
		LTrigger,
		RTrigger,
		LButton,
		RButton,
		Select,
		Start
	};

	enum GuideType
	{
		Game,
		Menu
	};


	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("GuideControll");	//�O���[�v��
	const  string  defName("GuideControll");	//�^�X�N��
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
		OL::Size2D imgSize;
		DG::Image::SP imgBg;
		OL::Size2D imgBgSize;
		int imgCountLine;		//��s�ɑ��݂���摜��
		DG::Font::SP font;
		OL::Size2D fontSize;
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
		struct GuideButton
		{
			Button buttonNum;
			string guideStr;
		};
		ML::Vec2 posRB;	//�E�[+���[�̍��W
		float distance;
		vector<GuideButton> guides[2];
		GuideType currentGuide;
		ML::Box2D GetButtonSrc(Button bt_);
		void Load(const string& folderPath_);
		void AddGuides(GuideType type_, Button bu_, const string& guideStr_);
	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����

		void SetGuide(GuideType type_) { this->currentGuide = type_; }
	};
}