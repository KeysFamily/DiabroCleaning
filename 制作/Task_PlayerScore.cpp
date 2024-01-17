//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_PlayerScore.h"

namespace  PlayerScore
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgResultScroll = DG::Image::Create("./data/ending/�r�玆.png");
		this->fontData = DG::Font::Create("HGP�n�p�p�߯�ߑ�", 15, 30);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgResultScroll.reset();
		this->fontData.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();
		//���f�[�^������
		
		this->DrawCnt = 0;

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		this->DrawCnt++;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(120,500,600,600);

		ML::Box2D drawScoroll(50, 450, 500, 500);
		ML::Box2D srcScoroll(0, 0, 1282, 962);

		ostringstream oss;
		
		oss << "�@�@�@�@�@�@�@��с@�@�@�@�@�@" << endl << endl;
		oss << "�N���A�^�C��[s]�F" << ge->GameCnt << endl << endl;						//Player->surviveTime
		oss << "�G��|�������F" << ge->TotalEnemyKill << endl<<endl;							//Enemy��
		oss << "�G�ɗ^�����_���[�W�F" << ge->TotalDamage << endl << endl;						//Enemy��
		oss << "�l�������R�C���̖����F" << ge->TotalGetCoinCnt << endl<<endl;					//Item_coin��
		oss << "������R�C���̖����F" << ge->TotalUsedCoinCnt << endl;						//shop��
		//oss << "�N���A�^�C��[s]�F" << 111111111 << endl << endl;
		//oss << "�N���A�^�C��[s]�F" << 111111111 << endl << endl;
		//oss << "�N���A�^�C��[s]�F" << 111111111 << endl << endl;
		//oss << "�N���A�^�C��[s]�F" << 111111111 << endl << endl;
		//oss << "�N���A�^�C��[s]�F" << 111111111 << endl;
		this->res->imgResultScroll->Draw(drawScoroll, srcScoroll);
		this->res->fontData->Draw(draw, oss.str(),ML::Color(1,0,0,0));		

	}

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}