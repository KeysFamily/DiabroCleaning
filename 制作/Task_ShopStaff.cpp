//?------------------------------------------------------
//�^�X�N��:�V���b�v�X��
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_ShopStaff.h"

namespace  ShopStaff
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgStaff = DG::Image::Create("./data/image/Menu/ShopStaff/ShopStaff.png");
		this->imgStaffSize.Set(128, 128);
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/Idle.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/walk.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/run.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/jump.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/squat.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/magic.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/surprised.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/die.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/die2.txt"));
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgStaff.reset();
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
		this->render2D_Priority[1] = 0.4f;
		this->pos = ML::Vec2(0, 0);
		this->animCnt = 0;
		this->currentAnim = Motion::IDLE;
		this->loopAnim = true;
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
		++this->animCnt;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->loopAnim == true && this->animCnt > this->res->animStaff[this->currentAnim]->GetAnimCountMax())
		{
			this->animCnt = 0;
		}
		ML::Box2D draw = this->res->animStaff[this->currentAnim]->GetDrawBox();
		ML::Box2D src = this->res->animStaff[this->currentAnim]->GetSrcBox(this->animCnt);
		draw.Offset(this->pos);
		this->res->imgStaff->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	// ���̑��̊֐�
	//�A�j���[�V�����J�n
	void Object::SetAnimation(Motion motion_, bool loop_)
	{
		this->currentAnim = motion_;
		this->animCnt = 0;
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