//-------------------------------------------------------------------
//�G���f�B���O
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "sound.h"

#include  "Task_PlayerScore.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->Ending_clear_img = DG::Image::Create("./data/ending/Diobro_Cleaning_Game_clear.png");
		this->Ending_over_img = DG::Image::Create("./data/ending/Diobro_Cleaning_Game_over.png");
		this->Key_img = DG::Image::Create("./data/ending/PleaseKey.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->Ending_clear_img.reset();
		this->Ending_over_img.reset();
		this->Key_img.reset();
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
		this->KeyTime = 0;

		//�Q�[���N���A���ƃQ�[���I�[�o�[���̌��ʉ�
		bgm::LoadFile("bgm_GameClear", "./data/sound/bgm/GameClear.mp3");
		bgm::LoadFile("bgm_GameOver", "./data/sound/bgm/GameOver.mp3");

		if (ge->GameClearFlag == true) {
			bgm::Play("bgm_GameClear");
		}else{
			bgm::Play("bgm_GameOver");
		}

		//���^�X�N�̐���
		
		PlayerScore::Object::Create(true);
		
		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		bgm::Stop("bgm_GameClear");
		bgm::Stop("bgm_GameOver");

		ge->KillAll_G("�G���f�B���O");
		bgm::AllStop();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		this->KeyTime++;
		if (KeyTime % 60 < 30) this->KeyOnOff = true; 
		else this->KeyOnOff = false;

		auto inp = ge->in1->GetState();
		if (inp.ST.down && ge->getCounterFlag("End") != ge->ACTIVE) {
			ge->StartCounter("End", 45); //�t�F�[�h��90�t���[���Ȃ̂Ŕ�����45�Ő؂�ւ�
			ge->CreateEffect(98, ML::Vec2(0, 0));

		}
		if (ge->getCounterFlag("End") == ge->LIMIT) {
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, 1920, 1080);
		ML::Box2D src(0, 0, 1920, 1080);

		ML::Box2D KeyDraw((ge->screen2DWidth/2)-250,ge->screen2DHeight*3/4,500,150);
		ML::Box2D KeySrc(0,0,372,97);
		if (ge->GameClearFlag == true) 	this->res->Ending_clear_img->Draw(draw, src);	//�w�i��p�ӂ���
		else this->res->Ending_over_img->Draw(draw, src);   //�w�i��p�ӂ���
		
		if(this->KeyOnOff==true)this->res->Key_img->Draw(KeyDraw, KeySrc);
		
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