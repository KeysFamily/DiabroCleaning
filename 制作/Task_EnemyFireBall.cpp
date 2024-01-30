//?------------------------------------------------------
//�^�X�N��:�΋�
//��@���@��:22CI0306 �� �� ��
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:22CI0333 ���J�� �E��N
//�쐬�N����:
//�T�@�@�@�v:�΋�(�{�X�U��)
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyFireBall.h"

//#include  "Task_Effect00.h"

namespace  EnemyFireBall
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/effect/fireball.png");
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = OL::setBoxCenter(52, 52);
		this->pos = ML::Vec2(0, 0);
		this->power = 5.0f;
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
		this->moveCnt++;
		this->animCnt++;
		if (this->CheckFront_LR()) { ge->CreateEffect(8, this->pos);  this->Kill(); }
		this->pos += this->moveVec;
#if false
		auto enemys = ge->GetTasks<BChara>("Enemy");
		for (auto it = enemys->begin();
			it != enemys->end();
			++it) {
			if ((*it)->CheckHit(this->hitBase.OffsetCopy(this->pos))) {
				ge->CreateEffect(89, (*it)->pos);
				BChara::AttackInfo at = { this->power, 0, 0 };
				(*it)->Received(this, at);
				ge->CreateEffect(8, this->pos);
				this->Kill();
				break;
			}
		}
#endif
		if (ge->qa_Player == nullptr)return;
		if (ge->qa_Player->CheckHit(this->CallHitBox())) {
			ge->CreateEffect(89, this->pos);
			BChara::AttackInfo at = { this->power, 0, 0 };
			ge->qa_Player->Received(this, at);
			ge->CreateEffect(8, this->pos);
			this->Kill();
		}

		
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src, di.color);
	}

	//--------------------------------------------------------------------
	// �΋��ݒ�
	void Object::Setting(ML::Vec2 setPos_, float speed_, float rad_, float AttackPow_) {
		this->pos = setPos_;
		this->moveVec.x = speed_ * cos(rad_);
		this->moveVec.y = speed_ * sin(rad_);
		this->power = AttackPow_;
	}

	//--------------------------------------------------------------------
	// �A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1.0f, 0.1f, 1.0f, 2.0f);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-26,-26,52,52), ML::Box2D(0, 0, 26, 26), defColor },			//0
			{ ML::Box2D(-26,-26,52,52), ML::Box2D(26, 0, 26, 26), defColor },			//1
			{ ML::Box2D(-26,-26,52,52), ML::Box2D(26*2, 0, 26, 26), defColor },			//2
		};
		BChara::DrawInfo  rtv;
		int  work;
		work = this->animCnt / 6;
		work %= 3;
		rtv = imageTable[work];

		return rtv;
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