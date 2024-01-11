//?------------------------------------------------------
//�^�X�N��:�X�L���I��
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SkillSelect.h"
#include  "Task_SkillShop.h"

namespace  SkillSelect
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/Skill/BackGround.png");
		this->imgBgSize.Set(768, 864);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
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
		this->pos = ML::Vec2(1400, 465);
		this->shopBeginPos = ML::Vec2(0, -220);
		this->shopDistance = 96;
		this->currentShop = nullptr;

		this->LoadShopData("./data/SystemMenu/shopData.json");
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
		int idxShop = 0;
		for (auto& shop : shops)
		{
			//�V���b�v���W�ݒ�
			shop->pos = this->pos + this->shopBeginPos;
			shop->pos.y += this->shopDistance * idxShop;
			++idxShop;

			//���݂̃V���b�v���擾
			if (shop->selectCount > 0)
			{
				this->currentShop = shop.get();
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw = OL::setBoxCenter(this->res->imgBgSize);
		ML::Box2D src(0, 0, this->res->imgBgSize.w, this->res->imgBgSize.h);
		draw.Offset(this->pos);
		this->res->imgBg->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//���̑��̊֐�
	bool Object::LoadShopData(const string& filePath_)
	{
		json js;
		std::ifstream fin(filePath_);
		if (!fin) 
		{
			return false; 
		}
		//JSON�t�@�C���ǂݍ���
		fin >> js;
		//�t�@�C���ǂݍ��ݏI��
		fin.close();

		int idxShop = 0;	//�v�f�ԍ�
		//�V���b�v������
		shops.clear();

		for (auto& ji : js["skillShopMenu"])
		{
			//�V���b�v�ǉ�
			shops.push_back(SkillShop::Object::Create(true));

			//�f�[�^�ǂݍ���
			SkillShop::ShopData shopdata;
			shopdata.skillName = ji["name"];
			shopdata.staffTalkFile = ji["talkFileName"];
			shopdata.skillSrcOfs = ji["srcPosY"];
			shopdata.price = ji["price"];

			//�ǂݍ��񂾃f�[�^���V���b�v�ɔ��f
			shops[idxShop]->SetShopData(shopdata);

			//�V���b�v���W�ݒ�
			shops[idxShop]->pos = this->pos + this->shopBeginPos;
			shops[idxShop]->pos.y += this->shopDistance + idxShop;

			//�ړ���ݒ�
			if (idxShop != 0)
			{
				shops[idxShop - 1]->SetNext_Down(shops[idxShop].get());
				shops[idxShop]->SetNext_Up(shops[idxShop - 1].get());
			}

			++idxShop;
		}

		this->currentShop = shops[0].get();

		return true;

	}

	void Object::SetDownObj(MyUI::SelectableObject* nextObj_)
	{
		if (!this->shops.empty())
		{
			(*(shops.end() - 1))->SetNext_Down(nextObj_);
		}
	}
	void Object::SetLeftObj(MyUI::SelectableObject* nextObj_)
	{
		for (auto& shop : shops)
		{
			shop->SetNext_Left(nextObj_);
		}
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