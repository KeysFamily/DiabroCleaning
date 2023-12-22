//?------------------------------------------------------
//�^�X�N��:�V�X�e�����j���[ �Z���N�g�N���X
//��@���@��:22CI0333 ���J��E��N
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2023/12/20
//�T�@�@�@�v:�V�X�e���̑I���̃I�u�W�F�N�g
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SystemMenuSelectObject.h"

#include  "easing.h"
#include  "randomLib.h"

namespace  SystemMenuSelectObject
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/UI/Spritesheet_UI_Flat_Select_01.png");
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
		this->animCnt = 0;
		this->viewRate = 1.0f;
		this->sto = nullptr;

		this->easingCnt = 0;
		this->isEasing = false;
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
		if (this->sto == nullptr)return;
		this->animCnt = (this->animCnt + 1) % 2095133040;

		auto inp = ge->in1->GetState();
		bool isAct = false;

		this->sto->IsSelecting();

		//if (this->sto == nullptr) goto debug;
		if (inp.B2.down && !this->isEasing) {
			this->sto->IsDown();
			isAct = true;
		}
		if (inp.LStick.BU.down) {
			this->sto = sto->GetNext_Up();
			isAct = true;
		}
		if (inp.LStick.BD.down) {
			this->sto = sto->GetNext_Down();
			isAct = true;
		}
		if (inp.LStick.BL.down) {
			this->sto = sto->GetNext_Left();
			isAct = true;
		}
		if (inp.LStick.BR.down) {
			this->sto = sto->GetNext_Right();
			isAct = true;
		}

		//debug:
		//if (inp.B4.down) {
		//	isAct = true;
		//	
		//}

		//				//�����Ŏ擾���ꂽ��`�͕ύX��̃T�C�Y
		//this->selectBox = this->sto->GetObjectSize();
		//�����T�C�Y���ύX���ꂽ��C�[�W���O���J�n����B
		if (isAct) {
			isAct = false;

			//ML::Box2D rndBox(
			//	GetRandom<int>(10, 500),
			//	GetRandom<int>(10, 500),
			//	GetRandom<int>(100, 500),
			//	GetRandom<int>(100, 500));
			//easingSet(this->selectBox, rndBox, 1.0f, 1.0f, 10);
			easingSet(this->selectBox, sto->GetObjectSize(), 1.0f,1.0f,10);


		}

		//�C�[�W���O���̏���
		if (isEasing) {
			ML::Box2D box;
			box.x = easing::GetPos(enx);
			box.y = easing::GetPos(eny);
			box.w = easing::GetPos(enw);
			box.h = easing::GetPos(enh);
			viewRate = easing::GetPos(enr);

			this->selectBox = box;
			if (easing::GetState(enx) == easing::EQ_STATE::EQ_END)isEasing = false;
		}
		
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		int size = 12 * this->viewRate;
		ML::Box2D UIBox = this->selectBox;
		//�摜��4�ɕ���(����A�E��A�����A�E��)
		ML::Box2D draw[4] = {
			ML::Box2D(UIBox.x                 , UIBox.y                 , size, size),
			ML::Box2D(UIBox.x + UIBox.w - size, UIBox.y                 , size, size),
			ML::Box2D(UIBox.x                 , UIBox.y + UIBox.h - size, size, size),
			ML::Box2D(UIBox.x + UIBox.w - size, UIBox.y + UIBox.h - size, size, size)
		};
		//�S�̂̑傫����32x32
		ML::Box2D src[4] = {
			ML::Box2D( 0, 0,12,12),
			ML::Box2D(20, 0,12,12),
			ML::Box2D( 0,20,12,12),
			ML::Box2D(20,20,12,12)
		};

		for (auto& s : src) { 
			int work = this->animCnt / 8;
			s.Offset((work % 4) * 32, 0);
		}
		
		//for (auto& d : draw) { d.Offset(-ge->camera2D.x, -ge->camera2D.y); }

		for (int i = 0; i < 4; ++i) {
			this->res->img->Draw(draw[i], src[i]);
		}
	}

	void Object::easingSet(ML::Box2D beforeBox_, ML::Box2D afterBox_, float beforeRate_, float AfterRate_, int easingFrame_) {
		isEasing = true;
		enx = "eas_x" + to_string(easingCnt);
		eny = "eas_y" + to_string(easingCnt);
		enw = "eas_w" + to_string(easingCnt);
		enh = "eas_h" + to_string(easingCnt);
		enr = "eas_r" + to_string(easingCnt);
		easingCnt++;

		easing::EASINGTYPE et = easing::EASINGTYPE::CUBICOUT;
		easing::Create(enx, et, beforeBox_.x, afterBox_.x, easingFrame_);
		easing::Start(enx);
		easing::Create(eny, et, beforeBox_.y, afterBox_.y, easingFrame_);
		easing::Start(eny);
		easing::Create(enw, et, beforeBox_.w, afterBox_.w, easingFrame_);
		easing::Start(enw);
		easing::Create(enh, et, beforeBox_.h, afterBox_.h, easingFrame_);
		easing::Start(enh);

		easing::Create(enr, et, beforeRate_, AfterRate_, easingFrame_);
		easing::Start(enr);
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