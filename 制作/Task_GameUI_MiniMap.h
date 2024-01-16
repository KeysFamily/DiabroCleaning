#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�~�j�}�b�v
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2023/11/25
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  MiniMap
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("GameUI");	//�O���[�v��
	const  string  defName("MiniMap");	//�^�X�N��
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
		DG::Image::SP imgBG;
		OL::Size2D imgBGSize;
		DG::Image::SP imgChip;
		OL::Size2D imgChipSize;


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
		enum MapChipType
		{
			NON			= 0b00000000,
			NOMAP		= 0b10000000,
			VISITED		= 0b01000000,
			ISCONNECT	= 0b00100000,
			ISOTHER		= 0b00010000,
			UP			= 0b00001000,
			DOWN		= 0b00000100,
			RIGHT		= 0b00000010,
			LEFT		= 0b00000001,
		};

		ML::Vec2 pos;
		ML::Vec2 cameraPos;
		OL::Size2D screenSize;
		ML::Vec2 screenOfs;
		vector<vector<int>> mapData;
		int mapSizeMax;


		//�}�b�v�̌������摜�̈ʒu�ԍ��ɕύX
		void ResizeMap(int size_);
		void LoadData(const std::string& folderPath_);
		int MapDirToImgPosNum(const Map::MapDir& mapDirection_);
		void SetVisit(int x_, int y_);

		MapChipType ConvertToMCT(int mapDirection_);
		void SetChip(ML::Box2D& src_, int x_, int y_);
		void SetToScreen(ML::Box2D& drawBox_, ML::Box2D& srcBox_, const ML::Box2D& screen_);
	};
}