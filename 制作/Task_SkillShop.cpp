//?------------------------------------------------------
//�^�X�N��:�X�L���V���b�v
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SkillShop.h"
#include  "Task_Price.h"
#include  "Task_SystemMenuMessageWindow.h"

namespace  SkillShop
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgSkill = DG::Image::Create("./data/effect/magicSelect.png");
		this->imgSkillSize.Set(64, 64);
		this->imgPriceBG = DG::Image::Create("./data/image/Menu/Skill/priceBG.png");
		this->imgPriceBGSize.Set(232, 80);
		this->fontSkill = DG::Font::Create("�l�r �S�V�b�N", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgSkill.reset();
		this->fontSkill.reset();
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
		this->render2D_Priority[1] = 0.5f;
		this->pos = ML::Vec2(0, 0);
		this->skillImgPos = ML::Vec2(-300, 0);
		this->skillNamePos = ML::Vec2(-250, -30);
		this->pricePos = ML::Vec2(200, 0);
		this->objPrice = Price::Object::Create(true);
		this->objPrice->pos = this->pos + this->pricePos;

		this->shopData.price = 0;
		this->shopData.skillSrcOfs = 0;
		this->shopData.skillName = "";
		this->shopData.staffTalkFile = "";

		this->currentState = State::SALE;

		this->selectCount = 0;
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
		this->objPrice->pos = this->pos + this->pricePos;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�X�L�����̕`��
		ML::Box2D draw(0, 0, 1000, 1000);
		draw.Offset(this->pos + this->skillNamePos);
		this->res->fontSkill->Draw(draw, this->shopData.skillName);


		//�X�L���摜�̕`��
		draw = OL::setBoxCenter(this->res->imgSkillSize);
		ML::Box2D src(0, 0, this->res->imgSkillSize.w, this->res->imgSkillSize.h);
		draw.Offset(this->pos + this->skillImgPos);
		src.y += this->res->imgSkillSize.h * this->shopData.skillSrcOfs;
		this->res->imgSkill->Draw(draw, src);

		//���i�̔w�i�\��
		draw = OL::setBoxCenter(this->res->imgPriceBGSize);
		src = ML::Box2D(0, 0, this->res->imgPriceBGSize.w, this->res->imgPriceBGSize.h);
		draw.Offset(this->pos + this->pricePos);
		src.y += this->res->imgPriceBGSize.h * this->currentState;
		this->res->imgPriceBG->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//���̑��̊֐�
	void Object::SetShopData(const ShopData& shopData_)
	{
		this->shopData = shopData_;
		this->objPrice->SetPrice(this->shopData.price);
	}
	//SelectableObject�̃��\�b�h
	ML::Box2D Object::GetObjectSize() const
	{
		ML::Box2D box = OL::setBoxCenter(this->res->imgPriceBGSize);
		box.Offset(this->pos + this->pricePos);
		return box;
	}
	void Object::IsSelecting()
	{
		if (this->selectCount == 0)
		{
			auto msg = ge->GetTask<SystemMenuMessageWindow::Object>("SystemMenu", "MessageWindow");
			msg->SetMessage(this->shopData.staffTalkFile);
		}
		++this->selectCount;
	}
	void Object::FinishSelect()
	{
		this->selectCount = 0;
	}
	void Object::IsDown()
	{

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