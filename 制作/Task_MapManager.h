#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�}�b�v�Ǘ�
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace MapManager
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("MapManager");	//�O���[�v��
	const  string  defName("MapManager");	//�^�X�N��
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
	private:

		class MapObject
		{
		public:
			string mapName;

			enum class MapType
			{
				Empty = 0,
				Map,
				Connect,
			};
			MapType mapType;
			bool visited;

			MapObject(const string& mapName_ = "")
				:mapName(mapName_)
				, mapType(MapType::Empty)
				, visited(false)
			{}

			virtual string Generate() { return ""; };
		};

		enum class MapEnter
		{
			Up = 0,
			Left
		};
		enum class MapExit
		{
			Non = 0,
			Right,
			Down,
		};

		class Map : public MapObject
		{
		private:
			MapEnter enter;	//����
			MapExit exit;		//�o��
			int depth;		//�}�b�v�̐[�x

		public:

			Map(MapEnter enter_, MapExit exit_, int depth_)
				:enter(enter_)
				,exit(exit_)
				,depth(depth_)
			{
			}

			string Generate() override
			{
				return "1";
			}
		};

		//�ʘH
		class Connect : public MapObject
		{
		private:
			MapEnter enter;
			MapExit exit;
			MapExit exitSub;
		public:
			Connect(MapEnter enter_, MapExit exit_, MapExit exitSub_ = MapExit::Non)
				:enter(enter_)
				,exit(exit_)
				,exitSub(exitSub_)
			{
			}

			string Generate() override
			{
				return "2";
			}

		};

	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		int bossDepth;
		unsigned int mapSeed;		//�}�b�v�����̃V�[�h�l
		MapObject* map[20][20];


		void Generate();
		void GenerateMap(int x_, int y_, int depth_, int depthRest_, MapEnter enter_);
		void Destroy();		//���Ŏ��̏���
	};
}