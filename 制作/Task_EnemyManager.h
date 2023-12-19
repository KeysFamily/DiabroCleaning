#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�G�Ǘ��^�X�N
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2023/12/1
//�T�@�@�@�v:�G���Ǘ�����
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "BEnemy.h"

namespace  EnemyManager
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("EnemyManager");	//�O���[�v��
	const  string  defName("Normal");	//�^�X�N��

	struct EnemyData {	//�G���
		float hp;
		float jumpPow;
		float maxSpeed;
		float addSpeed;
		float decSpeed;

		int unHitTime;
		int dropMoney;
		int attackPow;
	};

	struct EnemyStatusRate {//�G�X�e�[�^�X�{��
		float hpRate;		//�̗͔{��
		float speedRate;	//���x�{��
		float moneyRate;	//��V�{��
		float attackRate;	//�U���͔{��
	};
	//---------------------------------------------------------
	// �G�X�e�[�^�X�{���ɂ���
	// Rate���Ǘ�����z��̒��ɓG���ƂɊǗ�����B
	// 
	//---------------------------------------------------------


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

		
		std::map<string, EnemyData> enemyDatas;
		std::vector<string> enemyNames;

		std::map<int, std::map<string, EnemyStatusRate>> stateRates;


		std::map<string, function<BEnemy::SP(bool)>> enemyInits;
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
		std::vector<BResource::SP> residentResource;

		void SpawnEnemyNum(int enemyNum_, ML::Vec2 pos_, int depth_ = 1);
		void SpawnEnemyName(string name_, ML::Vec2 pos_, int depth_ = 1);

		void KillAllEnemys();

	};
}