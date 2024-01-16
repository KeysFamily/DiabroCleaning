//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GameUI.h"
#include  "Task_Player.h"

namespace  GameUI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->coinImg = DG::Image::Create("./data/image/coin.png");
		this->num = DG::Image::Create("./data/image/font_number.png");
		this->magic = DG::Image::Create("./data/effect/magicSelect.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->coinImg.reset();
		this->num.reset();
		this->magic.reset();
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
		this->render2D_Priority[1] = 0.2f;
		this->coinPos = ML::Vec2(0, 0);
		this->numPos = ML::Vec2(0, 0);
		this->magicPos = ML::Vec2(0, 0);
		this->num = 0;
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
		auto player = ge->GetTask<BChara>("Player");
		this->num = player->balanceMoney;
		this->coinPos = ML::Vec2(32, 35);
		this->numPos = ML::Vec2(50,20);
		this->magicPos = ML::Vec2(224, 40);

		auto pl = ge->GetTask<Player::Object>("Player");
		this->magicSelect = pl->magicSelect + 1;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		char nums[7];
		sprintf(nums, "%06d", this->num);
		//8���̐����\��
		for (int i = 0; i < 6; ++i) {
			ML::Box2D draw(i * 20, 0, 20, 32);
			ML::Box2D src((nums[i] - '0') * 20, 32, 20, 32);
			this->res->num->Draw(draw.OffsetCopy(numPos), src);
		}
		ML::Box2D coin_draw(-20, -20, 40, 40);
		ML::Box2D coin_src(0, 0, 32, 32);
		this->res->coinImg->Draw(coin_draw.OffsetCopy(coinPos), coin_src);

		
		ML::Box2D magic_draw(-32, -32, 64, 64);
		ML::Box2D magic_src(0, this->magicSelect * 64, 64, 64);
		this->res->magic->Draw(magic_draw.OffsetCopy(magicPos), magic_src);
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