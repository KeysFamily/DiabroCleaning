#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace  PlayerStatusShop
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("SystemMenu");	//�O���[�v��
	const  string  defName("PlayerStatusShop");	//�^�X�N��
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
		DG::Image::SP imgProgress;	//�w���p�̉摜
		OL::Size2D imgProgressSize;	//�w���p�̉摜�̃T�C�Y
		DG::Font::SP fontDisplay;	//�X�e�[�^�X���\���p�t�H���g
		OL::Size2D fontDisplaySize;	//�X�e�[�^�X���\���p�t�H���g�̃T�C�Y
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask, public MyUI::SelectableObject
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
		string displayStr;		//�X�e�[�^�X��
		vector<int> price;		//�e���z
		vector<int> addStatus;	//�e�ǉ���
		OL::Limit<int> currentStatus;	//�ǂ��܂ōw������Ă��邩

		ML::Vec2 pos;				//�S�̂̈ʒu
		ML::Vec2 displayPos;		//�X�e�[�^�X���̈ʒu
		ML::Vec2 progressBeginPos;	//�w���p�̉摜�̈ʒu
		float progressDistance;		//�w���p�̉摜�̊Ԋu

		float selectScale;		//�I�����̃T�C�Y���摜���傫���������Ƃ��Ɏg��

		int statusType;				//�X�e�[�^�X�̎��

		//���̍w���ɕK�v�ȋ��z��Ԃ�
		int GetPrice() const;
		//���݂̒ǉ��X�e�[�^�X���擾����
		int GetStatusAdd() const;
		//�w������
		bool Buy(int& money_);

		//�T�C�Y�ƈʒu��`����
		virtual ML::Box2D GetObjectSize() const override;
		//�^�[�Q�b�g����
		virtual void IsSelecting() override;
		//�{�^���������ꂽ��
		virtual void IsDown() override;
	};
}