#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "MapStruct.h"

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
		//�}�b�v�`�b�v�f�[�^
		struct MapData
		{
			int width;
			int height;
			vector<vector<int>> chipdata;

			//�ǂݍ��ݏ���
			bool Load(const string& mapName_);
		};
		//�w�i�f�[�^
		struct BackMapData
		{
			DG::Image::SP img;	//�摜
			OL::Size2D imgSize;	//�摜�T�C�Y
			float scale;
			float moveScale;	//�J�����ړ��ɂ���ĉ摜��������
			bool infinity;		//�摜�𖳌��ɑ������邩
		};
		//�⓹����f�[�^
		struct SlopeData
		{
			ML::Vec2 slopeVec;		//��̕���
			float    slopeHeight;	//��J�n���̍���
		};

		BackMapData backMap;	//�w�i
		vector<MapData> drawMap;		//�`��p�`�b�v�f�[�^
		MapData			ObjectMap;		//�����蔻��p�`�b�v�f�[�^
		MapData			GenerateMap;	//�G���e�B�e�B�p�`�b�v�f�[�^
		map<int, SlopeData> slopeData;	//��̏��̃}�b�v�z��
		map<int, Map::SpikeData> spikeData;	//�_���[�W���f�[�^
		ML::Box2D			hitBase;	//�s�N�Z���P�ʂ̃}�b�v�T�C�Y������
		bool				visited;	//��x�K�ꂽ��
		int					depth;		//�[�x
		string				folderPath;	//�}�b�v�f�[�^�t�H���_�ւ̃p�X
		int					depthInLevel;	//1���x���ɂ�����[�x
		int					depthInLevel_Conn;	//1���x���ɂ�����[�x�i�ʘH�j

		//�����蔻��
		bool  CheckHit(const  ML::Box2D& hit_);
		bool  CheckHit(const ML::Vec2& pos_);

		//��Ƃ̓����蔻��
		ML::Vec2 CheckSlope(const ML::Box2D& hit_);
		//���̃`�b�v���ɍ₪���݂��邩
		bool  CheckSlopeOnChip(const ML::Box2D& hit_);
		bool  CheckSlopeOnChip(const ML::Vec2& pos_);

		//���蔲���鏰����
		bool CheckFallGround(const ML::Box2D& hit_);

		//�_���[�W������
		Map::SpikeData CheckSpike(const ML::Box2D& hit_);

		//�}�b�v�ǂݍ���
		bool LoadMap(const string& folderPath_);

		//�}�b�v�ۑ�
		bool SaveMap();

		//�┻��ǂݍ���
		bool LoadSlope(const string& filepath_);

		//�_���[�W���ǂݍ���
		bool LoadSpike(const string& filepath_);

		//�o������
		MapDir CheckExit(const ML::Box2D& hit_);

		//�}�b�v�ړ����̃v���C���[�̍��W
		ML::Vec2 GetPlayerEnterPos(const MapDir& mapDirection_);

		//�v���C���[�̃X�^�[�g�n�_�̍��W
		ML::Vec2 GetPlayerStartPos()const;

		//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu��������
		void  AdjustCameraPos();

	private:
		bool  CheckHitTo(const  ML::Box2D& hit_, int chipNum_);//�����蔻��
		bool CheckHitTo(const ML::Vec2& pos_, int chipNum_);//���W�Ƃ̓����蔻��
		//�w�i�̕`��
		void DrawBackMap();
		void DrawBackSubUD(const ML::Box2D& draw_, const ML::Box2D src_, const ML::Rect& bg_, const ML::Rect& gSc_);

		void SetEnemyOnMap();


	};
}