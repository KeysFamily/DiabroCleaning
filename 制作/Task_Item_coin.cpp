//?------------------------------------------------------
//�^�X�N��:Task_Item_coin
//��@���@��:����@���u
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:11/15
//�T�@�@�@�v:coin�̌��f�[�^
//------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map.h"
#include  "Task_Player.h"
#include  "Task_Item_coin.h"
#include  "sound.h"
#include  "Task_Map.h"

namespace  Item_coin
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/coin.png");

		//22ci0308
		//se
		se::LoadFile("se_get_coin", "./data/sound/se/se_select2.wav");
		//
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
		this->render2D_Priority[1] = 0.4f;
		this->hitBase = OL::setBoxCenter(32, 32);
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��
		this->decSpeed = 0.05f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->maxFallSpeed = 11.0f;	//�ő嗎�����x
		this->angle = ML::ToRadian((float)(rand() % 360));
		this->moveVec = ML::Vec2(cos(angle) * 4, sin(angle) * 4);
		this->motion = Stand;

		this->pos.x = 1300;
		this->pos.y = 500;

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->CreateEffect(58, this->pos);

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

		this->moveCnt++;
		this->animCnt++;
		if (this->unHitTime > 0) { this->unHitTime--; }

		this->Think();
		this->Move();

		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//��ʓ��ɃR�C�������邩
		auto map = ge->qa_Map;
		ML::Vec2 map_size (map->ObjectMap.width, map->ObjectMap.height);
		this->out_coin(map_size.x,map_size.y);
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(di.draw, di.src);
		ge->debugRect(this->CallHitBox(), 4, -ge->camera2D.x, -ge->camera2D.y);
	}
	
	//-----------------------------------------------------------------
	void Object::Think()
	{
		auto inp = ge->in1->GetState();
		int nm = this->motion;
		switch (nm)
		{
		case Motion::Stand:
			if (inp.B2.down)
			{
				nm = Motion::Suction;
			}
			break;
		case Motion::Fall:
			if (this->CheckFoot() == true)
			{
				{ nm = Motion::Stand; }
			}
			break;
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);

	}

	//------------------------------------------------------------------
	void Object::Move()
	{
		switch (this->motion) {
		default:
			//�d�͉���
			switch (this->motion) {
			default:
				//�㏸���������͑����ɒn�ʂ�����
				if (this->moveVec.y < 0 ||
					this->CheckFoot() == false) {
					this->fallSpeed = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
					this->moveVec.y = this->fallSpeed;
				}
				//�n�ʂɐڐG���Ă���
				else {
					this->fallSpeed = min(this->moveVec.y - this->gravity, this->maxFallSpeed);
					this->moveVec.y -= this->fallSpeed*1.7;
				}


				break;
				//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
			case Motion::Unnon:	break;
			}
			//�ړ����x����
			switch (this->motion) {
			default:
				if (this->CheckFoot() == true) {
					if (this->moveVec.x < 0) {
						this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
					}
					else {
						this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
					}
				}
				break;

				//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
			case Motion::Bound:
			case Motion::Unnon:	break;
			}

			switch (this->motion)
			{
			case Motion::Suction:
				//auto pl = ge->GetTask<Player::Object>("Player");
				this->target = ge->qa_Player;
				if (auto  tg = this->target.lock()) {
					//�^�[�Q�b�g�ւ̑��΍��W�����߂�
					ML::Vec2  toVec = tg->pos - this->pos;

					float speed = 20;
					auto vec = toVec.Normalize();
					vec *= speed;
					//�^�[�Q�b�g�ɂT���߂Â�
					//this->pos += toVec * 0.05f;

					this->pos += vec;
				}
				break;
			}
		}
	}

	//-------------------------------------------------------------------
	//�A�j���[�V��������
	BChara::DrawInfo Object::Anim()
	{
		ML::Color defColor(1, 1, 1, 1);

		BChara::DrawInfo imageTable[] = {
			//draw                   src
			{this->hitBase,ML::Box2D(6,6,20,20),defColor},
			{this->hitBase,ML::Box2D(40,6,16,20),defColor},
			{this->hitBase,ML::Box2D(76,6,8,20),defColor},
			{this->hitBase,ML::Box2D(40,6,16,20),defColor},
		};
		BChara::DrawInfo  rtv;
		int work;
		switch (this->motion)
		{
		default:
			work = this->animCnt/5;
			work %= 4;
			rtv = imageTable[work]; 
			break;
		}

		return rtv;
	}
	//-----------------------------------------------------------------------------
	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�ȃ��m�j
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		from_->balanceMoney += 1;
		this->Kill();
		se::Stop("se_get_coin");
		se::Play("se_get_coin");
		//this->UpdateMotion(Motion::Bound);
		//from_�͍U�����Ă�������A�J�E���^�[�Ȃǂŋt�Ƀ_���[�W��^���������g��
	}

	//------------------------------------------------------------------------------
	//��ʊO��coin���o�����������
	void Object::out_coin(int x_,int y_)
	{
		ML::Box2D drawsize(0, 0, x_ * 64, y_ * 64);
		if ( drawsize.y>= this->pos.y || drawsize.h <= this->pos.y ||
			drawsize.x >= this->pos.x || drawsize.w <= this->pos.x) {
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