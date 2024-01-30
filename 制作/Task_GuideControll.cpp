//?------------------------------------------------------
//�^�X�N��:�R���g���[���[�K�C�h
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GuideControll.h"

namespace  GuideControll
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/UI/gamepadButton.png");
		this->imgSize.Set(32, 32);
		this->imgBg = DG::Image::Create("./data/image/UI/buttonBack.png");
		this->imgBgSize.Set(8, 8);
		this->imgCountLine = 4;
		this->fontSize.Set(16, 32);
		this->font = DG::Font::Create("�l�r �S�V�b�N", fontSize.w, fontSize.h);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img.reset();
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
		this->render2D_Priority[1] = 0.01f;

		this->posRB.x = static_cast<float>(ge->screenWidth);
		this->posRB.y = static_cast<float>(ge->screenHeight);
		this->distance = 20;
		this->currentGuide = GuideType::Game;
		this->Load("./data/guideControll");
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
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Vec2 imgPos;
		imgPos.x = this->posRB.x - this->res->imgSize.w / 2;
		imgPos.y = this->posRB.y - this->res->imgSize.h / 2;
		for (auto& gd : this->guides[this->currentGuide])
		{
			//�摜�̐ݒ�
			imgPos.x -= this->res->imgSize.w / 2;
			imgPos.x -= gd.guideStr.size() * this->res->fontSize.w;
			ML::Box2D draw = OL::setBoxCenter(this->res->imgSize);
			draw.Offset(imgPos);
			ML::Box2D src = GetButtonSrc(gd.buttonNum);

			//�����̐ݒ�
			ML::Box2D drawf = ML::Box2D(0, 0, this->res->fontSize.w * static_cast<int>(gd.guideStr.size()), this->res->fontSize.h);
			drawf.x = static_cast<int>(imgPos.x + this->res->imgSize.w / 2);
			drawf.y = static_cast<int>(this->posRB.y - this->res->fontSize.h);
			
			//�w�i�̐ݒ�
			ML::Box2D drawbg(0, 0, 0, 0);
			ML::Box2D srcbg(0, 0, this->res->imgBgSize.w, this->res->imgBgSize.h);
			drawbg.x = draw.x;
			drawbg.y = min(draw.y, drawf.y);
			drawbg.w = draw.w + drawf.w;
			drawbg.h = max(draw.h, drawf.h);

			//�`��
			this->res->imgBg->Draw(drawbg, srcbg);
			this->res->img->Draw(draw, src);
			this->res->font->DrawF(drawf, gd.guideStr, DG::Font::x4, ML::Color(1, 0, 0, 0), ML::Color(1, 1, 1, 1));

			imgPos.x -= this->distance;
		}


	}
	//-------------------------------------------------------------------
	//�K�C�h�ǉ�
	void Object::AddGuides(GuideType type_, Button bu_, const string& guideStr_)
	{
		GuideButton freshButton;
		freshButton.buttonNum = bu_;
		freshButton.guideStr = guideStr_;
		this->guides[type_].push_back(freshButton);
	}
	//-------------------------------------------------------------------
	//�ǂݍ��ݏ���
	void Object::Load(const string& folderPath_)
	{
		for (auto gd : this->guides)
		{
			gd.clear();
		}
		{
			ifstream ifs(folderPath_ + "/game.txt");
			while (ifs)
			{
				int btNum;
				ifs >> btNum;
				if (!ifs)
				{
					break;
				}
				string btStr;
				ifs >> btStr;
				AddGuides(GuideType::Game, (Button)btNum, btStr);
			}
			ifs.clear();
		}
		{
			ifstream ifs(folderPath_ + "/menu.txt");
			while (ifs)
			{
				int btNum;
				ifs >> btNum;
				if (!ifs)
				{
					break;
				}
				string btStr;
				ifs >> btStr;
				AddGuides(GuideType::Menu, (Button)btNum, btStr);
			}
			ifs.clear();
		}
	}
	//-------------------------------------------------------------------
	//�Ή�����{�^���摜�̋�`��Ԃ�
	ML::Box2D Object::GetButtonSrc(Button bt_)
	{
		return ML::Box2D(
			(bt_ % this->res->imgCountLine) * this->res->imgSize.w
			, (bt_ / this->res->imgCountLine) * this->res->imgSize.h
			, this->res->imgSize.w
			, this->res->imgSize.h
		);
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