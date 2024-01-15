//?------------------------------------------------------
//�^�X�N��:���j���[��ʊǗ�
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "sound.h"
#include  "Task_SystemMenu.h"
#include  "Task_PlayerStatus.h"
#include  "Task_SystemMenuMessageWindow.h"
#include  "Task_SystemMenuSelectObject.h"
#include  "Task_PlayerStatusShop.h"
#include  "Task_ShopStaff.h"
#include  "Task_SkillSelect.h"
#include  "Task_SkillShop.h"
#include  "Task_SystemMenuMoneyDisplay.h"
#include  "Task_SystemMenuBackButton.h"


namespace  SystemMenu
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/Menu/backGround.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
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
		this->render2D_Priority[1] = 1.0f;
		//�ʒu�ݒ�
		this->pos = ML::Vec2(0, -1080);
		this->skillPos = ML::Vec2(1400, 465);
		this->statusPos = ML::Vec2(500, 560);
		this->backPos = ML::Vec2(1650, 1000);
		this->msgPos = ML::Vec2(750, 1000);
		this->moneyPos = ML::Vec2(420, 130);
		//�I������
		this->finishedAppear = false;
		this->finishFlag = true;
		//�Q�[���p�t�@�C���𐶐����Ă���
		this->CreateNewFile();

		//���^�X�N�̐���
		this->status = PlayerStatus::Object::Create(true);
		this->message = SystemMenuMessageWindow::Object::Create(true);
		this->skill = SkillSelect::Object::Create(true);
		this->moneyDp = SystemMenuMoneyDisplay::Object::Create(true);
		this->back = SystemMenuBackButton::Object::Create(true);
		
		this->SetPos();
		this->status->ShopUpdate();
		this->skill->ShopUpDate();

		//�I���ʒu�ݒ�
		skill->SetLeftObj(status->currentShop);
		skill->SetDownObj(back.get());
		status->SetRightObj(skill->currentShop);
		status->SetDownObj(back.get());
		back->SetNext_Up((skill->shops.end() - 1)->get());

		//�ŏ��͖����ɂ��Ă���
		this->SuspendMenu(true);


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
		this->SetPos();
		if (finishFlag == true)
		{
			this->DisappearUpdate();
			return;
		}
		if (finishedAppear == false)
		{
			this->AppearUpDate();
			return;
		}

		//�I������
		auto inp = ge->in1->GetState();
		if (inp.ST.down) 
		{
			this->FinishMenu();
		}


		this->skill->SetLeftObj(this->status->currentShop);
		this->status->SetRightObj(this->skill->currentShop);
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, 1920, 1080);
		ML::Box2D src(0,0,1024,1024);
		draw.Offset(this->pos);
		this->res->imgBg->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//���̑��̊֐�
	//�ʒu�ݒ�
	void Object::SetPos()
	{
		this->status->pos = this->pos + this->statusPos;
		this->message->pos = this->pos + this->msgPos;
		this->skill->pos = this->pos + this->skillPos;
		this->moneyDp->pos = this->pos + this->moneyPos;
		this->back->pos = this->pos + this->backPos;
	}

	//���j���[�I�u�W�F�N�g������
	void Object::SuspendMenu(bool f_)
	{
		if (true)
		{
			this->menuObj = ge->GetTasks<BTask>("SystemMenu");
		}
		for (auto& obj : *(this->menuObj))
		{
			obj->Suspend(f_);
		}
	}

	//�o������
	void Object::AppearUpDate()
	{
		this->pos.y = easing::GetPos("menuAp");
		if (easing::GetState("menuAp") == easing::EQ_STATE::EQ_END)
		{
			auto sobj = SystemMenuSelectObject::Object::Create(true);
			sobj->sto = status->shops[0].get();

			this->finishedAppear = true;
		}

	}
	//���ŏ���
	void Object::DisappearUpdate()
	{
		this->pos.y = easing::GetPos("menuDisap");
		if (easing::GetState("menuDisap") == easing::EQ_STATE::EQ_END)
		{
			this->Suspend(true);
		}
	}

	//���j���[��ʊJ�n
	void Object::StartMenu()
	{
		//�ʒu�ݒ�
		this->pos = ML::Vec2(0, -1080);
		this->skillPos = ML::Vec2(1400, 465);
		this->statusPos = ML::Vec2(500, 560);
		this->backPos = ML::Vec2(1650, 1000);
		this->msgPos = ML::Vec2(750, 1000);
		this->moneyPos = ML::Vec2(420, 130);
		//�I������
		this->finishedAppear = false;
		this->finishFlag = false;
		//�C�[�W���O�ݒ�
		easing::Create("menuAp", easing::EASINGTYPE::BOUNCEOUT, this->pos.y, 0, 30);
		easing::Start("menuAp");
		//�T�E���h�ݒ�
		bgm::LoadFile("bgmMenu", "./data/sound/bgm/shop.mp3");
		bgm::Play("bgmMenu");
		se::LoadFile("shopin", "./data/sound/se/shopin.wav");
		se::Play("shopin");
		//�֘A�^�X�N�L����
		this->SuspendMenu(false);
		this->SetPos();
		this->message->SetMessage("enter");
	}

	//���j���[��ʏI��
	void Object::FinishMenu()
	{
		ge->KillAll_GN("SystemMenu", "SelectObject");
		this->SaveToFile();
		easing::Create("menuDisap", easing::EASINGTYPE::CUBICOUT, this->pos.y, -1080, 30);
		easing::Start("menuDisap");
		bgm::Stop("bgmMenu");
		se::Play("shopin");
		this->finishFlag = true;
	}

	//�V�K�t�@�C���쐬
	void Object::CreateNewFile()
	{
		CopyFile("./data/inGame/template/pData_skill.json", "./data/inGame/run/pData_skill.json", false);
		CopyFile("./data/inGame/template/pData_status.json", "./data/inGame/run/pData_status.json", false);
	}

	//�t�@�C���ɃZ�[�u
	void Object::SaveToFile()
	{
		json js;
		std::ifstream fin("./data/inGame/run/pData_skill.json");
		if (!fin)
		{
			return;
		}
		//JSON�t�@�C���ǂݍ���
		fin >> js;
		//�t�@�C���ǂݍ��ݏI��
		fin.close();


		for (auto& ji : js["pData_skill"])
		{
			for (auto& sk : this->skill->shops)
			{
				if (ji["name"] == sk->shopData.skillName)
				{
					ji["isBought"] = sk->currentState == SkillShop::Object::State::BOUGHT;
				}
			}
		}

		ofstream fout("./data/inGame/run/pData_skill.json");
		fout << js.dump(4);

		fout.close();

		return;
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