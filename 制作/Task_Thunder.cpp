//?------------------------------------------------------
//�^�X�N��:
//��@���@��:
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Thunder.h"
#include  "BEnemy.h"

namespace  Thunder
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img1 = DG::Image::Create("./data/effect/thunder.png");
		this->img2 = DG::Image::Create("./data/effect/thunderhit.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img1.reset();
		this->img2.reset();
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
		this->hitBase = ML::Box2D(-24, -24, 48, 48);
		this->pos = ML::Vec2(0, 0);
		this->speed = 10.0f;
		this->power = 1.0f;
		this->cost = 10;
		this->motion = Motion::Start;
		this->diff = ML::Vec2(0, 0);
		this->searchEnemy = ML::Box2D(0, 0, 0, 0);
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
		this->Think();
		this->Move();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		if (this->motion == Motion::Start) {
			this->res->img1->Draw(di.draw, di.src);
		}
		if (this->motion == Motion::Infinite) {
			this->res->img1->Draw(di.draw, di.src);
		}
		if (this->motion == Motion::End) {
			this->res->img2->Draw(di.draw, di.src);
		}
		//ge->debugRect(this->searchEnemy.OffsetCopy(this->pos), 7, -ge->camera2D.x, -ge->camera2D.y);
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		int  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��
		switch (nm) {
		case Motion::Start:
			if (this->moveCnt > 21) {
				nm = Motion::Infinite;
			}
			if (true == this->CheckFront_LR()) { nm = Motion::End; }
			break;
		case Motion::Infinite:
			if (true == this->CheckFront_LR()) { nm = Motion::End; }
			break;
		case Motion::End:
			if (this->moveCnt > 15) { this->Kill(); }
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//���[�V�����ɑΉ���������
	void  Object::Move()
	{
		auto enemys = ge->GetTasks<BChara>("Enemy");
		this->pos += this->moveVec;
		switch (this->motion) {
		case Motion::Start:
			break;
		case Motion::Infinite:
			for (auto it = enemys->begin();
				it != enemys->end();
				++it) {
				
				if((*it)->CheckHit(this->searchEnemy.OffsetCopy(this->pos)))
				{diff = (*it)->pos - this->pos;

				float angRad = atan2(diff.y, diff.x);
				this->moveVec.x = this->speed * cos(angRad);
				this->moveVec.y = this->speed * sin(angRad); 
				}

				if ((*it)->CheckHit(this->hitBase.OffsetCopy(this->pos))) {
					ge->CreateEffect(91, (*it)->pos);
					BChara::AttackInfo at = { this->power, 0, 0 };
					(*it)->Received(this, at);
					this->motion = Motion::End;
				}
			}
			break;
		case Motion::End:
			this->moveVec = ML::Vec2(0, 0);
			break;
		}
	}
	//--------------------------------------------------------------------
	// �A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(0, 0, 48, 48), defColor },					//0
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48, 0, 48, 48), defColor },					//1
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 2, 0, 48, 48), defColor },				//2
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 3, 0, 48, 48), defColor },				//3
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 4, 0, 48, 48), defColor },				//4
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 5, 0, 48, 48), defColor },				//5
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 6, 0, 48, 48), defColor },				//6
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 7, 0, 48, 48), defColor },				//7
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 8, 0, 48, 48), defColor },				//8
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 9, 0, 48, 48), defColor },				//9
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 10, 0, 48, 48), defColor },				//10
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 11, 0, 48, 48), defColor },				//11
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 12, 0, 48, 48), defColor },				//12
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 13, 0, 48, 48), defColor },				//13
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 14, 0, 48, 48), defColor },				//14
			{ ML::Box2D(-24,-24,48,48), ML::Box2D(48 * 15, 0, 48, 48), defColor },				//15
			{ ML::Box2D(-16,-16,32,32), ML::Box2D(32 * 0, 0, 32, 32), defColor },				//16
			{ ML::Box2D(-16,-16,32,32), ML::Box2D(32 * 1, 0, 32, 32), defColor },				//17
			{ ML::Box2D(-16,-16,32,32), ML::Box2D(32 * 2, 0, 32, 32), defColor },				//18
			{ ML::Box2D(-16,-16,32,32), ML::Box2D(32 * 3, 0, 32, 32), defColor },				//19
			{ ML::Box2D(-16,-16,32,32), ML::Box2D(32 * 4, 0, 32, 32), defColor },				//20
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
		case Motion::Start:
			if (this->animCnt <= 3)work = 0;
			if (this->animCnt > 3 && this->animCnt <= 6)work = 1;
			if (this->animCnt > 6 && this->animCnt <= 9)work = 2;
			if (this->animCnt > 9 && this->animCnt <= 12)work = 3;
			if (this->animCnt > 9 && this->animCnt <= 12)work = 4;
			if (this->animCnt > 12 && this->animCnt <= 15)work = 5;
			if (this->animCnt > 15 && this->animCnt <= 18)work = 6;
			if (this->animCnt > 18)work = 7;
			rtv = imageTable[work];
			break;
		case Motion::Infinite:
			work = this->animCnt / 3;
			work %= 8;
			rtv = imageTable[work + 8];
			break;
		case Motion::End:
			if (this->animCnt <= 3)work = 0;
			if (this->animCnt > 3 && this->animCnt <= 6)work = 1;
			if (this->animCnt > 6 && this->animCnt <= 9)work = 2;
			if (this->animCnt > 9 && this->animCnt <= 12)work = 3;
			if (this->animCnt > 12)work = 4;
			rtv = imageTable[work + 16];
			break;
		}
		searchEnemy = ML::Box2D(0, -150, 400, 300);
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
			searchEnemy = ML::Box2D(-400, -150, 400, 300);
		}
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