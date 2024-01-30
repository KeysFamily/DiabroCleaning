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
#include "BItem.h"

namespace  Item
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("item");	//�O���[�v��
	const  string  defName("item");		//�^�X�N��
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()		override;
		bool  Finalize()		override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
	//�ύX������������������������������������������������������
		//���L����ϐ��͂����ɒǉ�����
		DG::Image::SP ImgItem;
	};
	//-------------------------------------------------------------------
	class  Object : public  BItem
	{
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
		void  UpDate()		override;	//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF()	override;	//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	public:
	//�ύX������������������������������������������������������
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		//BChara�Ɋ܂܂�Ȃ����m�݂̂����ɒǉ�����

		enum Motion
		{
			Unnon = -1,	//	����(�g���܂���j
			Stand,		//	��~
			Fall,       //  ����
			Bound,		//	�e����΂���Ă�
			Lose,		//  ���Œ�
		};

		class item_buff  //�i�[����A�C�e���̌���
		{
		public:
			int Attack;     //�U����
			int Defense;    //�h���
			int Magic;      //���@��
			int Speed;      //�X�s�[�h
			int Time;       //��������
			int Item_pos;   //�A�C�e���̎��
			int Power_step; //�����̒i�K
		};

		void InputJsonFile(string fileName_);
		void  Think();
		//���[�V�����ɑΉ���������
		void  Move();
		//�ڐG���̉�������(�K���󂯐g�̏����Ƃ��Ď�������)
		void Received(BChara* from_, AttackInfo at_) override;
		//�v���C���[�̃X�e�[�^�X��ς���
		void GiftPlayer(BChara* pl_) override;
		item_buff itemb;
		//�f�o�b�N�p
		XI::GamePad::SP controller;

		bool initialized;
		void ItemInit(ML::Vec2 pos_, int minLevel_, int maxLevel_);

		//��ʊO�ɏo����A�C�e������������
		void out_coin(int x, int y);
		
	};
}