#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//�^�X�N��:�X�L���V���b�v
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace Price
{
	class Object;
}

namespace  SkillShop
{
	struct ShopData
	{
		int skillSrcOfs;		//�g�p����X�L���摜�̔ԍ�
		string skillName;		//�X�L����
		string staffTalkFile;	//�X���̃Z���t�t�@�C��
		int price;				//���i
	};
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("SystemMenu");	//�O���[�v��
	const  string  defName("SkillShop");	//�^�X�N��
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
		DG::Image::SP imgSkill;		//�X�L���摜
		OL::Size2D imgSkillSize;	//�X�L���摜�T�C�Y
		DG::Image::SP imgPriceBG;	//���i�w�i
		OL::Size2D imgPriceBGSize;	//���i�w�i�T�C�Y

		DG::Font::SP fontSkill;		//�X�L�����t�H���g

		
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
		bool Buy(int& money_);
	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		ML::Vec2 pos;			//���W
		ML::Vec2 skillImgPos;	//�X�L���摜�̈ʒu�̈ʒu
		ML::Vec2 skillNamePos;	//�X�L�����̈ʒu
		ML::Vec2 pricePos;		//���i�̈ʒu

		shared_ptr<Price::Object> objPrice;	//���i�I�u�W�F�N�g

		ShopData shopData;

		//���
		enum State
		{
			NON = -1,			//����
			SALE,				//�̔���
			BOUGHT,				//�w���ς�
			SELECTING			//�I��
		};

		int currentState;		//���݂̏��

		void SetShopData(const ShopData& shopData_);

		int selectCount;		//�I������Ă���Ƃ��̃J�E���g

		//SelectableObject�̃��\�b�h
		virtual ML::Box2D GetObjectSize() const;
		virtual void IsSelecting();
		virtual void FinishSelect();
		virtual void IsDown();
	};
}