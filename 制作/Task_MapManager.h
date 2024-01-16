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
#include "MapStruct.h"
#include "Task_MapTransition.h"

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
	public:

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

			virtual void Generate() { return; };
		};

		class Area : public MapObject
		{
		private:
			Map::MapDir enter;	//����
			Map::MapDir exit;		//�o��
			int depth;		//�}�b�v�̐[�x

		public:

			Area(Map::MapDir enter_, Map::MapDir exit_, int depth_)
				:enter(enter_)
				, exit(exit_)
				, depth(depth_)
			{
			}

			void Generate() override
			{
				string genMapName = "map_";
				string mapDirStr[4] = { "Up", "Down", "Right", "Left" };

				genMapName += mapDirStr[(int)enter];
				if (exit == Map::MapDir::Non)
				{
					genMapName += mapDirStr[(int)enter];
				}
				else
				{
					genMapName += mapDirStr[(int)exit];
				}

				genMapName += "_" + to_string(rand() % 3 + 1);

				this->mapName = genMapName;
			}

			//�Q�b�^
			Map::MapDir GetEnter() { return enter; }
			Map::MapDir GetExit() { return exit; }
			int GetDepth() { return depth; }
		};

		//�ʘH
		class Connect : public MapObject
		{
		private:
			Map::MapDir enter;
			Map::MapDir exit;
			Map::MapDir exitSub;
		public:
			Connect(Map::MapDir enter_, Map::MapDir exit_, Map::MapDir exitSub_ = Map::MapDir::Non)
				:enter(enter_)
				, exit(exit_)
				, exitSub(exitSub_)
			{
			}

			void Generate() override
			{
				string genMapName = "pass_";
				string mapDirStr[4] = { "Up", "Down", "Right", "Left" };

				genMapName += mapDirStr[(int)enter];

				if (exitSub != Map::MapDir::Non)
				{
					if (exit == Map::MapDir::Down)
					{
						genMapName += mapDirStr[(int)exitSub] + mapDirStr[(int)exit];
					}
					else
					{
						genMapName += mapDirStr[(int)exit] + mapDirStr[(int)exitSub];
					}
				}
				else
				{
					genMapName += mapDirStr[(int)exit];
				}

				this->mapName = genMapName;
			}

			//�Q�b�^
			Map::MapDir GetEnter() { return enter; }
			Map::MapDir GetExit() { return exit; }
			Map::MapDir GetExitSub() { return exitSub; }

		};

	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		int bossDepth;
		unsigned int mapSeed;		//�}�b�v�����̃V�[�h�l
		MapObject* map[30][30];
		ML::Point currentPos;		//���݂̃}�b�v
		
		Map::MapDir moveMapDir;		//�}�b�v�ړ����̕���
		MapTransition::Object::SP mapTransition;	//�}�b�v�g�����W�V�����ւ̃|�C���^

		//�}�b�v�̃��[�h�Ɏg�p����񋓑�
		//���[�h
		void MoveMap(const Map::MapDir& mapDirection_);

	private:
		void Generate();
		void GenerateMap(int x_, int y_, int depth_, int depthRest_, Map::MapDir enter_);
		void Destroy();		//���Ŏ��̏���

		void MoveMapUpDate();

	};
}