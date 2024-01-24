//?------------------------------------------------------
//�^�X�N��:�Q�[���I�[�o�[���[�h�^�X�N
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_LoadGameOver.h"
#include  "Task_GameOverMenu.h"

namespace  LoadGameOver
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgBg = DG::Image::Create("./data/image/GameOverBG.png");
		this->imgText = DG::Image::Create("./data/image/TextGameOver.png");
		this->imgTextSize.Set(96, 64);
		this->imgTextSize = imgTextSize * 2;
		this->textResSize = 9;
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgBg.reset();
		this->imgText.reset();
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
		this->render2D_Priority[1] = 0.1f;
		this->textPos = ML::Vec2(ge->screenWidth / 2, 200);
		this->textDistance = 150;
		this->state = State::Invalid;
		this->textAppearDistance = 20;
		this->textAppearCnt = 0;

		easing::Create("gameOverBgAp", easing::EASINGTYPE::BOUNCEOUT, 0, ge->screenWidth / 2, 120);
		for (int i = 0; i < this->res->textResSize; ++i)
		{
			easing::Create("gameOverTxtAp" + to_string(i), easing::EASINGTYPE::SINEOUT, -this->res->imgTextSize.h, this->textPos.y, 30);
		}
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
		switch (this->state)
		{
		case State::Invalid:
			break;
		case State::FinishAppear:
			break;
		case State::Appear:
			this->AppearUpDate();
			break;
		case State::Disappear:
			this->DisappearUpDate();
			break;
		}

	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		float bgPos = easing::GetPos("gameOverBgAp");
		
		//�w�i�̕`��
		{
			ML::Box2D drawBase = OL::setBoxCenter(ge->screenWidth / 2, ge->screenHeight);
			ML::Box2D src(0, 0, 8, 8);

			ML::Box2D drawL = drawBase.OffsetCopy((float)ge->screenWidth / -4, (float)ge->screenHeight / 2);
			drawL.x += bgPos;
			this->res->imgBg->Draw(drawL, src);


			ML::Box2D drawR = drawBase.OffsetCopy((float)ge->screenWidth + (float)ge->screenWidth / 4, (float)ge->screenHeight / 2);
			drawR.x -= bgPos;
			this->res->imgBg->Draw(drawR, src);
		}

		//�����̕`��
		ML::Box2D draw = OL::setBoxCenter(this->res->imgTextSize);
		ML::Box2D src(0, 0, this->res->imgTextSize.w / 2, this->res->imgTextSize.h / 2);
		draw.x += this->textPos.x;
		draw.x -= (this->textDistance * (this->res->textResSize - 1) / 2);
		
		for (int i = 0; i < this->res->textResSize; ++i)
		{
			draw.y = easing::GetPos("gameOverTxtAp" + to_string(i));
			this->res->imgText->Draw(draw, src);

			draw.x += this->textDistance;
			src.x += this->res->imgTextSize.w / 2;
		}

		
	}
	//-------------------------------------------------------------------
	//�o��
	void Object::Appear()
	{
		if (this->state == State::Invalid)
		{
			easing::Start("gameOverBgAp");
			easing::Start("gameOverTxtAp0");
			this->state = State::Appear;
		}
	}
	//�o�������̍X�V
	void Object::AppearUpDate()
	{
		if (textAppearCnt >= textAppearDistance * this->res->textResSize)
		{
			if (easing::GetState("gameOverTxtAp" + to_string(this->res->textResSize - 1)) == easing::EQ_STATE::EQ_END)
			{
				GameOverMenu::Object::Create(true);
				this->state = State::FinishAppear;
			}
			return;
		}
		int currentChar = textAppearCnt / textAppearDistance;

		if (easing::GetState("gameOverTxtAp" + to_string(currentChar)) == easing::EQ_STATE::EQ_SET)
		{
			easing::Start("gameOverTxtAp" + to_string(currentChar));
		}


		++textAppearCnt;
	}

	//�o���������I��������
	bool Object::CheckFinishdAppear()
	{
		return this->state == State::FinishAppear;
	}

	//���ŏ����J�n
	void Object::Disappear()
	{
		float startPos = easing::GetPos("gameOverBgAp");
		easing::Create("gameOverBgAp", easing::EASINGTYPE::QUADOUT, startPos, 0, 120);
		easing::Start("gameOverBgAp");
		for (int i = 0; i < this->res->textResSize; ++i)
		{
			startPos = easing::GetPos("gameOverTxtAp" + to_string(i));

			easing::Create("gameOverTxtAp" + to_string(i), easing::EASINGTYPE::QUADOUT, startPos, -this->res->imgTextSize.h, 30);
			easing::Start("gameOverTxtAp" + to_string(i));
		}
		this->state = State::Disappear;
	}

	//���ŏ���
	void Object::DisappearUpDate()
	{
		if (easing::GetState("gameOverTxtAp" + to_string(this->res->textResSize - 1)) == easing::EQ_STATE::EQ_END)
		{
			this->Kill();
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