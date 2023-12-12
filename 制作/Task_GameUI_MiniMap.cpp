//?------------------------------------------------------
//�^�X�N��:�~�j�}�b�v
//��@���@��:�y�c����
//TODO:��������Ή��L�֋L�q
//�ҁ@�W�@��:
//�쐬�N����:2023/11/25
//�T�@�@�@�v:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GameUI_MiniMap.h"
#include  "Task_MapManager.h"
#include  "Task_MapTransition.h"

namespace  MiniMap
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgBG = DG::Image::Create("./data/map/image/UI/BackGround.png");
		this->imgBGSize.Set(256, 256);
		this->imgChip = DG::Image::Create("./data/map/image/UI/MapChip.png");
		this->imgChipSize.Set(32, 32);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgBG.reset();
		this->imgChip.reset();
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
		this->pos.x = ge->screenWidth - this->res->imgBGSize.w / 2;
		this->pos.y = this->res->imgBGSize.h / 2;
		this->cameraPos = ML::Vec2(0, 0);
		this->screenSize.Set(240, 216);
		this->screenOfs = ML::Vec2(0, 12);
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
		int speed = 1;
		if (inp.RStick.BU.on)
			this->cameraPos.y -= speed;
		if (inp.RStick.BD.on)
			this->cameraPos.y += speed;
		if (inp.RStick.BR.on)
			this->cameraPos.x += speed;
		if (inp.RStick.BL.on)
			this->cameraPos.x -= speed;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�w�i�`��
		ML::Box2D draw = OL::setBoxCenter(this->res->imgBGSize);
		draw.Offset(this->pos);
		ML::Box2D src(0, 0, this->res->imgBGSize.w, this->res->imgBGSize.h);
		this->res->imgBG->Draw(draw, src);

		//�}�b�v�`�b�v�`��
		ML::Box2D screen = OL::setBoxCenter(this->screenSize);
		screen.Offset(this->pos + this->screenOfs);
		for (int y = 0; y < 30; ++y) {
			for (int x = 0; x < 30; ++x) {
				draw = ML::Box2D(0, 0, this->res->imgChipSize.w, this->res->imgChipSize.h);
				src = draw;
				draw.x += screen.x + x * this->res->imgChipSize.w;
				draw.y += screen.y + y * this->res->imgChipSize.h;
				draw.Offset(-cameraPos);
				this->SetChip(src, x, y);
				this->SetToScreen(draw, src, screen);

				this->res->imgChip->Draw(draw, src);
			}
		}
	}
	//-------------------------------------------------------------------
	//�֐���`
	//-------------------------------------------------------------------
	//�}�b�v�̌������摜�̈ʒu�ԍ��ɕύX
	int Object::MapDirToImgPosNum(const Map::MapDir& mapDirection_)
	{
		switch (mapDirection_)
		{
		case Map::MapDir::Up:
			return 0;
			break;
		case Map::MapDir::Down:
			return 2;
			break;
		case Map::MapDir::Right:
			return 1;
			break;
		case Map::MapDir::Left:
			return 1;
			break;
		case Map::MapDir::Non:
			return 0;
			break;
		}
	}

	//-------------------------------------------------------------------
	//�`�b�v�̐ݒ�
	void Object::SetChip(ML::Box2D& src_, int x_, int y_)
	{
		auto mapMng = ge->GetTask<MapManager::Object>("MapManager");
		if (!mapMng) 
		{
			return;
		}
		if (x_ == 0 && y_ == 0)
		{
			src_.x = this->res->imgChipSize.w * 3;
			src_.y = this->res->imgChipSize.h * 2;
		}

		//�}�b�v
		MapManager::Object::Area* map = dynamic_cast<MapManager::Object::Area*>(mapMng->map[y_][x_]);
		if (map)
		{
			src_.x = MapDirToImgPosNum(map->GetExit()) * this->res->imgChipSize.w;
			src_.y = this->res->imgChipSize.h * 2
				+ MapDirToImgPosNum(map->GetEnter()) * this->res->imgChipSize.h;
		}

		//�ʘH
		MapManager::Object::Connect* connect = dynamic_cast<MapManager::Object::Connect*>(mapMng->map[y_][x_]);
		if (connect)
		{
			src_.x = (MapDirToImgPosNum(connect->GetExit()) + MapDirToImgPosNum(connect->GetExitSub())) * this->res->imgChipSize.w;
			src_.y = MapDirToImgPosNum(connect->GetEnter()) * this->res->imgChipSize.h;
		}
	}
	//-------------------------------------------------------------------
	//�`�b�v���X�N���[�����Ɏ��߂�
	void Object::SetToScreen(ML::Box2D& drawBox_, ML::Box2D& srcBox_, const ML::Box2D& screen_)
	{
		//�X�N���[��
		ML::Rect s = {
			screen_.x,
			screen_.y,
			screen_.x + screen_.w,
			screen_.y + screen_.h
		};
		//�`��
		ML::Rect d = {
			drawBox_.x,
			drawBox_.y,
			drawBox_.x + drawBox_.w,
			drawBox_.y + drawBox_.h
		};

		if (screen_.Hit(drawBox_)) {
			//�摜���X�N���[������͂ݏo�Ă���ꍇ�A
			//�o�Ă��镔���͕`�悳���Ȃ�����
			//��
			if (s.left > d.left) {
				drawBox_.x += s.left - d.left;
				drawBox_.w -= s.left - d.left;
				srcBox_.x += s.left - d.left;	//�͂ݏo���Ă��镔�������̃f�[�^�͈̔͂ɍ��킹�Ă���v�Z
				srcBox_.w -= s.left - d.left;
			}
			//��
			if (s.top > d.top) {
				drawBox_.y += s.top - d.top;
				drawBox_.h -= s.top - d.top;
				srcBox_.y += s.top - d.top;
				srcBox_.h -= s.top - d.top;
			}
			//�E
			if (s.right < d.right) {
				drawBox_.w -= d.right - s.right;
				srcBox_.w -= d.right - s.right;
			}
			//��
			if (s.bottom < d.bottom) {
				drawBox_.h -= d.bottom - s.bottom;
				srcBox_.h -= d.bottom - s.bottom;
			}
		}
		//���S�ɃX�N���[���O�ɏo�Ă�����T�C�Y��0��
		else {
			drawBox_.w = 0;
			drawBox_.h = 0;
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