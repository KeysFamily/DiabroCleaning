#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  Map
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("Map");	//�O���[�v��
	const  string  defName("Map");	//�^�X�N��
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
		int chipSize;		//���摜��1�}�X�̃T�C�Y
		int drawSize;		//�Q�[���ŕ`�悷��Ƃ���1�}�X�̃T�C�Y
		int turnNum;		//���摜��1�s�ɂ����̃^�C�������邩

		DG::Font::SP debugFont;	//�f�o�b�O�p�̃t�H���g
		bool drawObject;		//�I�u�W�F�N�g�}�b�v��\�����邩
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
		struct MapData
		{
			int width;
			int height;
			vector<vector<int>> chipdata;

			//�ǂݍ��ݏ���
			bool Load(const string& mapName_);
		};
		vector<MapData> drawMap;
		MapData			ObjectMap;

		struct SlopeData
		{
			ML::Vec2 slopeVec;		//��̕���
			float    slopeHeight;	//��J�n���̍���
		};
		map<int, SlopeData> slopeData;	//��̏��̃}�b�v�z��

		int  arr[100][100];
		ML::Box2D			hitBase;//�s�N�Z���P�ʂ̃}�b�v�T�C�Y������

		bool  CheckHit(const  ML::Box2D& hit_);//�����蔻��
		ML::Vec2 CheckSlope(const ML::Box2D& hit_);//��Ƃ̓����蔻��

		//�}�b�v�ǂݍ���
		//param1:��Փx�܂ł̖��O
		bool LoadMap(const string& mapName_);
		bool LoadSlope(const string& filepath_);


		//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu��������
		void  AdjustCameraPos();
	};
}