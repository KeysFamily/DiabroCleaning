#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�^�C�g�����j���[
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  TitleMenu
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("title");	//�O���[�v��
	const  string  defName("Menu");	//�^�X�N��
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
		DG::Image::SP imgSelect;
		OL::Size2D imgSelectSize;
		DG::Font::SP fontTitle;
		OL::Size2D fontTitleSize;
		DG::Font::SP fontMain;
		OL::Size2D fontMainSize;
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
		
		void RunMenuAction(int actId_);
		void LoadMenuAction(const string& filePath_);
		struct MenuObject
		{
			int actionId;
			string text;
		};
		vector<MenuObject> menuObj;
		vector<int> menuDisplay;
		int selectingMenu;


		void ResetMenu();
		void AddMenu(int id_);
	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		ML::Vec2 pos;
		ML::Vec2 titlePos;
		ML::Vec2 mainBeginPos;
		string titleStr;
		float mainTextDistance;
		float selectObjDistance;
		
	};
}