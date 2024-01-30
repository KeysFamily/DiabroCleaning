//?------------------------------------------------------
//�^�X�N��:���b�Z�[�W��
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SystemMenuMessageWindow.h"
#include  "Task_ShopStaff.h"

namespace  SystemMenuMessageWindow
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/ShopStaff/BackGround.png");
		this->imgBgSize.Set(1248, 160);
		this->fontMsg = DG::Font::Create("�l�r �S�V�b�N", 24, 48);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->fontMsg.reset();
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
		this->render2D_Priority[1] = 0.6f;
		this->pos = ML::Vec2(750, 1000);
		this->staffPos = ML::Vec2(-510, -10);
		this->shopStaff = ShopStaff::Object::Create(true);
		this->shopStaff->pos = this->pos + this->staffPos;

		this->appearMessageCount = 0;
		this->messageStartPos = ML::Vec2(-360, -30);
		this->displayStr = "";
		this->SetMessage("enter");

		this->messageColor = ML::Color(1, 0.2f, 0.2f, 0.2f);
		this->outlineColor = ML::Color(1, 1, 1, 1);

		//this->outlineColor = ML::Color(1, 1, 1, 1);
		//this->messageColor = ML::Color(1, 1.0f, 0.2f, 0.3f);
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
		this->shopStaff->pos = this->pos + this->staffPos;

		this->currentMessagePos.Setval(this->appearMessageCount / appearMessageDistance);
		this->displayStr = this->messageStr.substr(0, this->currentMessagePos.vnow);

		++this->appearMessageCount;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw = OL::setBoxCenter(this->res->imgBgSize);
		ML::Box2D src(0, 0, this->res->imgBgSize.w, this->res->imgBgSize.h);
		draw.Offset(this->pos);
		this->res->imgBg->Draw(draw, src);

		draw = ML::Box2D(0, 0, 1000, 1000);
		draw.Offset(this->pos + this->messageStartPos);
		this->res->fontMsg->DrawF(draw, this->displayStr, DG::Font::x4, this->messageColor, this->outlineColor);
	}
	//-------------------------------------------------------------------
	// ���̑��̊֐�
	//���b�Z�[�W�̐ݒ�
	void Object::SetMessage(const string& fileName_)
	{
		ifstream ifs(string("./data/staff_message/") + fileName_ + ".txt");

		if (!ifs)
		{
			return;
		}

		int distance = 0;
		ifs >> distance;

		if (!ifs)
		{
			return;
		}

		int motion = 0;
		ifs >> motion;

		if (!ifs)
		{
			return;
		}

		ifs >> this->messageStr;

		this->appearMessageDistance = distance;
		this->currentMessagePos.SetValues(0, 0, static_cast<int>(this->messageStr.size()));
		this->shopStaff->SetAnimation((ShopStaff::Motion)motion);
		this->appearMessageCount = 0;

		ifs.close();
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