//?------------------------------------------------------
//�^�X�N��:�^�C�g�����j���[
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_TitleMenu.h"
#include  "Task_Title.h"

namespace  TitleMenu
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imgBg = DG::Image::Create("./data/image/title/selectBG.png");
		imgBgSize.Set(704, 416);
		imgSelect = DG::Image::Create("./data/image/title/selectObject.png");
		imgSelectSize.Set(32, 32);
		fontTitleSize.Set(24, 48);
		fontTitle = DG::Font::Create("�l�r �S�V�b�N", this->fontTitleSize.w, this->fontTitleSize.h);
		fontMainSize.Set(16, 32);
		fontMain = DG::Font::Create("�l�r �S�V�b�N", fontMainSize.w, fontMainSize.h);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->imgSelect.reset();
		this->fontTitle.reset();
		this->fontMain.reset();
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
		this->pos = ML::Vec2(ge->screenWidth / 2, ge->screenHeight / 4 * 3);
		this->titlePos = ML::Vec2(0, -110);
		this->mainBeginPos = ML::Vec2(0, -40);
		this->mainTextDistance = 20;
		this->selectObjDistance = 21;

		this->LoadMenuAction("./data/title/MenuData.txt");

		this->RunMenuAction(0);
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
		auto inp = ge->in1->GetState();

		if (inp.LStick.BU.down)
		{
			--this->selectingMenu;
			if (this->selectingMenu < 0)
			{
				this->selectingMenu = menuDisplay.size() - 1;
			}
		}
		if (inp.LStick.BD.down)
		{
			++this->selectingMenu;
			if (this->selectingMenu >= menuDisplay.size())
			{
				this->selectingMenu = 0;
			}
		}

		if (inp.B1.down)
		{
			this->RunMenuAction(menuDisplay[selectingMenu]);
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�w�i�\��
		ge->DrawStd(this->res->imgBg, this->res->imgBgSize, this->pos);

		//�^�C�g���\��
		float titleSizeW = this->res->fontTitleSize.w * this->titleStr.size();
		ML::Box2D draw = OL::setBoxCenter(titleSizeW, this->res->fontTitleSize.h);
		draw.Offset(this->pos + this->titlePos);
		this->res->fontTitle->Draw(draw, this->titleStr);
		
		//���j���[�\��
		int selectObjCnt = 0;
		ML::Vec2 mainDrawPos = this->mainBeginPos;
		for (auto& dp : this->menuDisplay)
		{
			float menuSizeW = this->res->fontMainSize.w * this->menuObj[dp].text.size();
			draw = OL::setBoxCenter(menuSizeW, this->res->fontMainSize.h);
			draw.Offset(this->pos + mainDrawPos);
			
			for (auto& menu : menuObj)
			{
				if (menu.actionId == dp)
				{
					this->res->fontMain->Draw(draw, menu.text);
				}

			}

			if(selectObjCnt == this->selectingMenu)
			{
				//�I���I�u�W�F�N�g�\��
				ML::Vec2 selectObjPos = this->pos;
				selectObjPos.x = draw.x - this->res->imgSelectSize.w - this->selectObjDistance;
				selectObjPos.y = draw.y + (draw.h / 2);
				ge->DrawStd(this->res->imgSelect, this->res->imgSelectSize, selectObjPos);
			}

			++selectObjCnt;
			mainDrawPos.y += this->res->fontMainSize.h + mainTextDistance;
		}


	}
	//-------------------------------------------------------------------
	//���j���[�f�[�^�ǂݍ���
	void Object::LoadMenuAction(const string& filePath_)
	{
		ifstream ifs(filePath_);
		while (ifs)
		{
			MenuObject obj;
			ifs >> obj.actionId;
			if (!ifs)
			{
				ifs.close();
				return;
			}
			ifs >> obj.text;
			this->menuObj.push_back(obj);
		}
		ifs.close();
	}
	//-------------------------------------------------------------------
	//���j���[�ꗗ
	void Object::RunMenuAction(int actId_)
	{
		switch (actId_)
		{
		case 0:
			this->ResetMenu();
			this->AddMenu(1);
			this->AddMenu(2);
			this->AddMenu(3);
			this->AddMenu(4);
			this->titleStr = "��Փx�I��";
			break;
		case 1:
		{
			ge->filemg.SetRunPath("./data/inGame", true);
			auto title = ge->GetTask<Title::Object>("title", "NoName");
			title->CreateGame(3);
			this->Kill();
		}
		break;
		case 2:
		{
			ge->filemg.SetRunPath("./data/inGame", true);
			auto title = ge->GetTask<Title::Object>("title", "NoName");
			title->CreateGame(5);
			this->Kill();
		}
		break;
		case 3:
		{
			ge->filemg.SetRunPath("./data/inGame", true);
			auto title = ge->GetTask<Title::Object>("title", "NoName");
			title->CreateGame(10);
			this->Kill();
		}
		break;
		case 4:
			this->Kill();
			break;
		default:
			break;
		}

	}
	//-------------------------------------------------------------------
	//���j���[�ꗗ���Z�b�g
	void Object::ResetMenu()
	{
		this->selectingMenu = 0;
		this->menuDisplay.clear();
	}
	//-------------------------------------------------------------------
	//���j���[�ꗗ�ɒǉ�
	void Object::AddMenu(int id_)
	{
		this->menuDisplay.push_back(id_);
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