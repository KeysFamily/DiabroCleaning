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

namespace MiniMap
{
	class Object;
}

namespace MapTransition
{
	class Object;
}

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
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		int bossDepth;
		unsigned int mapSeed;									//�}�b�v�����̃V�[�h�l
		float generateSubRate;									//��������m��
		int	  subDepthMax;										//�O��̓��̐[�x�ő�
		int   depthMax;											//�ŉ��w
		Map::MapObject* map[Map::MAPSIZE_MAX][Map::MAPSIZE_MAX];//�}�b�v�f�[�^
		int mapid[Map::MAPSIZE_MAX][Map::MAPSIZE_MAX];			//�}�b�v�f�[�^�i�ȃf�[�^�Łj
		ML::Point currentPos;									//���݂̃}�b�v
		string saveFolderPath;									//�ۑ���̃p�X
		Map::MapDir moveMapDir;									//�}�b�v�ړ����̕���
		shared_ptr<MapTransition::Object> mapTransition;		//�}�b�v�g�����W�V�����ւ̃|�C���^
		shared_ptr<MiniMap::Object> minimap;					//�~�j�}�b�v�ւ̃|�C���^
		bool generated;											//���łɐ����ς݂�


		void GenerateMap(int x_, int y_, int depth_, int depthRest_, Map::MapDir enter_, bool setSub_ = false);
		bool GetSubFlag(int connX_, int connY_);
		void GenerateSub();
		void Destroy();		//���Ŏ��̏���

		void MoveMapUpDate();
	public:
		//�}�b�v�̃��[�h�Ɏg�p����񋓑�
		//���[�h
		void MoveMap(const Map::MapDir& mapDirection_);
		void SetMaxDepth(int depth_);
		bool Generate();

	};
}