//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map.h"

#include  "Task_Player.h"
#include  "Task_EnemyManager.h"

namespace  Map
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/map/image/map96.png");
		this->chipSize = 96;	//�`�b�v�摜�̃T�C�Y
		this->drawSize = 64;	//�`�悷��T�C�Y
		this->turnNum = 64;		//�摜��1�s�Ɋ܂܂��`�b�v�̎��

		this->debugFont = DG::Font::Create("non", 8, 16);	//�����t�H���g
		this->drawObject = false;	//�I�u�W�F�N�g�`�b�v�̐����`��
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
		this->render2D_Priority[1] = 0.9f;
		auto result = this->LoadMap("map_start");
		auto resultSlope = this->LoadSlope("./data/map/slopesData.json");

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
		//�w�i�̕`��
		this->DrawBackMap();
		
		//�}�b�v�`�b�v�̕`��
		for (auto& layer : this->drawMap)
		{
			for (int y = 0; y < layer.height; ++y)
			{
				for (int x = 0; x < layer.width; ++x)
				{
					ML::Box2D draw(this->res->drawSize * x, this->res->drawSize * y, this->res->drawSize, this->res->drawSize);
					draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
					ML::Box2D src(
						this->res->chipSize * (layer.chipdata[y][x] % this->res->turnNum),
						this->res->chipSize * (layer.chipdata[y][x] / this->res->turnNum),
						this->res->chipSize,
						this->res->chipSize);

					this->res->img->Draw(draw, src);
				}
			}
		}

		//�I�u�W�F�N�g�}�b�v�̕`��i�f�o�b�O�p�j
		if (this->res->drawObject == true)
		{
			for (int y = 0; y < this->ObjectMap.height; ++y)
			{
				for (int x = 0; x < this->ObjectMap.width; ++x)
				{
					ML::Box2D draw(this->res->drawSize * x, this->res->drawSize * y, this->res->drawSize, this->res->drawSize);
					draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
					ML::Box2D src(
						this->res->chipSize * (this->ObjectMap.chipdata[y][x] % this->res->turnNum),
						this->res->chipSize * (this->ObjectMap.chipdata[y][x] / this->res->turnNum),
						this->res->chipSize,
						this->res->chipSize);

					this->res->debugFont->Draw(draw, to_string(this->ObjectMap.chipdata[y][x]));
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//���̑��̃��\�b�h

	//�}�b�v�ǂݍ���
	bool Object::LoadMap(const string& mapName_)
	{

		//�w�i�}�b�v�ǂݍ���
		backMap.clear();

		int layerNum = 0;
		ifstream ifs("./data/map/" + mapName_ + "/" + mapName_ + "_BG.json");
		if (ifs.is_open())
		{
			json backMapData = json::parse(ifs);
			for (auto& bmd : backMapData["BackMap"])
			{
				BackMapData bglayer;
				bglayer.img = DG::Image::Create(bmd["imgFilePath"]);
				bglayer.imgSize.w = bmd["imgWidth"];
				bglayer.imgSize.h = bmd["imgHeight"];
				bglayer.moveScale = bmd["moveScale"];
				this->backMap.push_back(bglayer);
			}
			ifs.close();
		}

		//�`��}�b�v�ǂݍ���
		drawMap.clear();

		layerNum = 0;
		while (layerNum < 10)
		{
			MapData layer;

			//�ǂݍ��ރ}�b�v�������Ȃ�����I��
			if (layer.Load("./data/map/" + mapName_ + "/" + mapName_ + "_R" + to_string(layerNum) + ".csv")
				 == false)
			{
				break;
			}

			this->drawMap.push_back(layer);
			++layerNum;
		}

		//�}�b�v������Ȃ����false
		if (drawMap.empty())
		{
			return false;
		}

		//�I�u�W�F�N�g�}�b�v�ǂݍ���
		this->ObjectMap.chipdata.clear();

		if (this->ObjectMap.Load("./data/map/" + mapName_ + "/" + mapName_ + "_H.csv")
			 == false)
		{
			return false;
		}

		//�G���e�B�e�B�}�b�v�ǂݍ���
		this->GenerateMap.chipdata.clear();

		if (this->GenerateMap.Load("./data/map/" + mapName_ + "/" + mapName_ + "_gens.csv")
			== false)
		{
			return false;
		}	
		this->SetEnemyOnMap();
		


		//�����蔻���`�ݒ�
		this->hitBase = ML::Box2D(0, 0, this->ObjectMap.width * this->res->drawSize, this->ObjectMap.height * this->res->drawSize);

		return true;
	}

	//-------------------------------------------------------------------
	//��f�[�^�̓ǂݍ���
	bool Object::LoadSlope(const string& filepath_)
	{
		json js;
		std::ifstream fin(filepath_);
		if (!fin) { return json(); }
		//JSON�t�@�C���ǂݍ���
		fin >> js;
		//�t�@�C���ǂݍ��ݏI��
		fin.close();

		for (auto& ji : js)
		{
			slopeData[ji["chipNum"]] = SlopeData();
			slopeData[ji["chipNum"]].slopeHeight = this->res->drawSize * (float)ji["beginHeight"];
			slopeData[ji["chipNum"]].slopeVec.x = this->res->drawSize * (float)ji["vecX"];
			slopeData[ji["chipNum"]].slopeVec.y = this->res->drawSize * (float)ji["vecY"];
		}
		return false;
	}
	//-------------------------------------------------------------------
	//�����蔻��
	bool  Object::CheckHit(const  ML::Box2D& hit_)
	{
		return this->CheckHitTo(hit_, 0);
	}
	//-------------------------------------------------------------------
	//��Ƃ̂����蔻��
	ML::Vec2 Object::CheckSlope(const ML::Box2D& hit_)
	{
		ML::Vec2 result(0, 0);

		//�v���C���[�̓����蔻��
		ML::Rect  r = { hit_.x, hit_.y, hit_.x + hit_.w, hit_.y + hit_.h };
		//��`���}�b�v�O�ɏo�Ă�����T�C�Y��ύX����
		ML::Rect  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h
		};
		if (r.left < m.left) { r.left = m.left; }
		if (r.top < m.top) { r.top = m.top; }
		if (r.right > m.right) { r.right = m.right; }
		if (r.bottom > m.bottom) { r.bottom = m.bottom; }

		//���[�v�͈͒���
		int sx, sy, ex, ey;
		sx = r.left / this->res->drawSize;
		sy = r.top / this->res->drawSize;
		ex = (r.right - 1) / this->res->drawSize;
		ey = (r.bottom - 1) / this->res->drawSize;

		//�͈͓��̏�Q����T��
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				//�S�Ă̍�Ɣ��肳����
				for (auto& slope : slopeData) {
					//���������}�X���₾������
					if (this->ObjectMap.chipdata[y][x] == slope.first) {
						//�`�b�v�̍��W�i����j
						ML::Vec2 chipPos(x * this->res->drawSize, y * this->res->drawSize);
						//����Ƀ`�b�v�̍�̕����Ɠ������Ă��邩����
						if (slope.second.slopeVec.y < 0) {
							//��ɏ���Ă��Ȃ������画�肵�Ȃ�
							if (r.bottom > y * this->res->drawSize + this->res->drawSize)
							{
								continue;
							}
							//�E��
							if (slope.second.slopeVec.x > 0) {
								//��ɏ���Ă��Ȃ������画�肵�Ȃ�
								if (r.right > x * this->res->drawSize + this->res->drawSize)
								{
									continue;
								}
								//�₪�J�n����n�_�̍��W�i�Q�[�����W�j
								ML::Vec2 slopeBegin(chipPos.x, chipPos.y + this->res->drawSize - 1 - slope.second.slopeHeight);
								//�v���C���[�̓����蔻��E�[��x���W�́A��̍����i�Q�[�����W�j
								float rbheight = slope.second.slopeVec.y * abs((r.right - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//�v���C���[����̏�ɏ悹�邽�߂ɕK�v�Ȉړ������i�ő�l�͍�̍ō��l�j
								float moveResult = min(-(slope.second.slopeVec.y + slope.second.slopeHeight), rbheight - r.bottom);
								if (moveResult < 0)
								{
									//�l�̏������ق����̗p����
									result.y = min(moveResult, result.y);
								}
							}
							//����
							if (slope.second.slopeVec.x < 0) {
								//��ɏ���Ă��Ȃ������画�肵�Ȃ�
								if (r.left < x * this->res->drawSize)
								{
									continue;
								}
								//�₪�J�n����n�_�̍��W�i�Q�[�����W�j
								ML::Vec2 slopeBegin(chipPos.x + this->res->drawSize - 1, chipPos.y + this->res->drawSize - 1 - slope.second.slopeHeight);
								//�v���C���[�̓����蔻�荶�[��x���W�́A��̍����i�Q�[�����W�j
								float lbheight = slope.second.slopeVec.y * abs((r.left - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//�v���C���[����̏�ɏ悹�邽�߂ɕK�v�Ȉړ������i�ő�l�͍�̍ō��l�j
								float moveResult = min(-(slope.second.slopeVec.y + slope.second.slopeHeight), lbheight - r.bottom);
								if (moveResult < 0)
								{
									//�l�̏������ق����̗p����
									result.y = min(moveResult, result.y);
								}
							}
						}
						else if (slope.second.slopeVec.y > 0) {
							//��ɏ���Ă��Ȃ������画�肵�Ȃ�
							if (r.top < y * this->res->drawSize)
							{
								continue;
							}
							//�E��
							if (slope.second.slopeVec.x > 0) {
								//��ɏ���Ă��Ȃ������画�肵�Ȃ�
								if (r.right > x * this->res->drawSize + this->res->drawSize)
								{
									continue;
								}
								//�₪�J�n����n�_�̍��W�i�Q�[�����W�j
								ML::Vec2 slopeBegin(chipPos.x, chipPos.y + slope.second.slopeHeight);
								//�v���C���[�̓����蔻��E�[��x���W�́A��̍����i�Q�[�����W�ł͂Ȃ��A���[�J�����W�j
								float rtheight = slope.second.slopeVec.y * abs((r.right - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//�v���C���[����̏�ɏ悹�邽�߂ɕK�v�Ȉړ������i�ő�l�͍�̍ō��l�j
								float moveResult = min(slope.second.slopeVec.y + slope.second.slopeHeight, rtheight - r.top);
								if (moveResult > 0)
								{
									//�オ��w�����Ȃ��ꍇ�̂ݎ��s�i�ύX�j
									if (result.y >= 0) {
										//�l�̑傫���ق����̗p����
										result.y = min(moveResult, result.y);
									}
								}
							}
							//����
							if (slope.second.slopeVec.x < 0) {
								//��ɏ���Ă��Ȃ������画�肵�Ȃ�
								if (r.left < x * this->res->drawSize)
								{
									continue;
								}
								//�₪�J�n����n�_�̍��W�i�Q�[�����W�j
								ML::Vec2 slopeBegin(chipPos.x + this->res->drawSize - 1, chipPos.y - slope.second.slopeHeight);
								//�v���C���[�̓����蔻�荶�[��x���W�́A��̍����i�Q�[�����W�ł͂Ȃ��A���[�J�����W�j
								float lbheight = slope.second.slopeVec.y * abs((r.left - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//�v���C���[����̏�ɏ悹�邽�߂ɕK�v�Ȉړ������i�ő�l�͍�̍ō��l�j
								float moveResult = min(slope.second.slopeVec.y + slope.second.slopeHeight, lbheight - r.top);
								if (moveResult > 0)
								{
									//�オ��w�����Ȃ��ꍇ�̂ݎ��s�i�ύX�j
									if (result.y >= 0) {
										//�l�̑傫���ق����̗p����
										result.y = min(moveResult, result.y);
									}
								}
							}
						}
					}
				}
			}
		}
		return result;
	}
	//-------------------------------------------------------------------
	//���蔲���鏰����
	bool  Object::CheckFallGround(const  ML::Box2D& hit_)
	{
		return this->CheckHitTo(hit_, 9);
	}

	//-------------------------------------------------------------------
	//�}�b�v�ړ����̃v���C���[�̍��W
	ML::Vec2 Object::GetPlayerEnterPos(const MapDir& mapDirection_)
	{
		//�ԍ���8�ԁ`��,��,�E,���̏��ŏ�������Ă���
		int enterChip = 8 + (int)mapDirection_;
		OL::Size2D mapSize(GenerateMap.width, GenerateMap.height);

		//�`�b�v��T��
		for (int y = 0; y < mapSize.h; ++y)
		{
			for (int x = 0; x < mapSize.w; ++x)
			{
				while (GenerateMap.chipdata[y][x] == enterChip)
				{
					return ML::Vec2(x, y) * this->res->drawSize;
				}
			}
		}

		return ML::Vec2(0, 0);

	}
	//-------------------------------------------------------------------
	//�o������
	MapDir Object::CheckExit(const ML::Box2D& hit_)
	{
		//4����
		for (int i = 0; i < 4; ++i)
		{
			if (CheckHitTo(hit_, 13 + i))
			{
				return (MapDir)i;
			}
		}

		return MapDir::Non;
	}


	//-------------------------------------------------------------------
	//�`�b�v���w�肷�铖���蔻��
	bool Object::CheckHitTo(const ML::Box2D& hit_, int chipNum_)
	{
		ML::Rect  r = { hit_.x, hit_.y, hit_.x + hit_.w, hit_.y + hit_.h };
		//��`���}�b�v�O�ɏo�Ă�����T�C�Y��ύX����
		ML::Rect  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h
		};
		if (r.left < m.left) { r.left = m.left; }
		if (r.top < m.top) { r.top = m.top; }
		if (r.right > m.right) { r.right = m.right; }
		if (r.bottom > m.bottom) { r.bottom = m.bottom; }

		//���[�v�͈͒���
		int sx, sy, ex, ey;
		sx = r.left / this->res->drawSize;
		sy = r.top / this->res->drawSize;
		ex = (r.right - 1) / this->res->drawSize;
		ey = (r.bottom - 1) / this->res->drawSize;

		//�͈͓��̏�Q����T��
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				if (this->ObjectMap.chipdata[y][x] == chipNum_) {
					return true;
				}
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu��������
	void  Object::AdjustCameraPos()
	{
		//�J�����ƃ}�b�v�͈̔͂�p��
		ML::Rect  c = {
			ge->camera2D.x,
			ge->camera2D.y,
			ge->camera2D.x + ge->camera2D.w,
			ge->camera2D.y + ge->camera2D.h };
		ML::Rect  m = {
			this->hitBase.x,
			this->hitBase.y,
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y + this->hitBase.h };

		//�J�����̈ʒu�𒲐�
		if (c.right > m.right) { ge->camera2D.x = m.right - ge->camera2D.w; }
		if (c.bottom > m.bottom) { ge->camera2D.y = m.bottom - ge->camera2D.h; }
		if (c.left < m.left) { ge->camera2D.x = m.left; }
		if (c.top < m.top) { ge->camera2D.y = m.top; }
		//�}�b�v���J������菬�����ꍇ
		if (this->hitBase.w < ge->camera2D.w) { ge->camera2D.x = m.left; }
		if (this->hitBase.h < ge->camera2D.h) { ge->camera2D.y = m.top; }
	}
	//-------------------------------------------------------------------
	//�}�b�v�O�������Ȃ��悤�ɃJ�������ʒu��������
	void  Object::DrawBackMap()
	{
		if (this->backMap.empty())
		{
			return;
		}

		for (auto& bglayer : this->backMap)
		{
			ML::Vec2 mapCenter(this->hitBase.w / 2, this->hitBase.h / 2);
			ML::Vec2 cameraPos(ge->camera2D.x, ge->camera2D.y);
			cameraPos += OL::BoxCenterPos(ge->camera2D);
			ML::Box2D draw = OL::setBoxCenter(bglayer.imgSize);
			draw.Offset(ge->GetScreenCenter());
			draw.Offset((mapCenter - cameraPos) * bglayer.moveScale);


			//�X�N���[���ƃ}�b�v�͈̔͂�p��
			ML::Rect  c = OL::BoxToRect(ge->GetScreenBox());
			ML::Rect  bg = OL::BoxToRect(draw);

			//�X�N���[���̈ʒu�𒲐�
			if (c.right > bg.right) { draw.x = c.right - draw.w; }
			if (c.bottom > bg.bottom) { draw.y = c.bottom - draw.h; }
			if (c.left < bg.left) { draw.x = c.left; }
			if (c.top < bg.top) { draw.y = c.top; }
			//�}�b�v���X�N���[����菬�����ꍇ
			if (draw.w < ge->screenWidth) { draw.x = c.left; }
			if (draw.h < ge->screenHeight) { draw.y = c.top; }

			ML::Box2D src(0, 0, bglayer.imgSize.w, bglayer.imgSize.h);

			bglayer.img->Draw(draw, src);
		}

	}
	//-------------------------------------------------------------------
	//�ǂݍ��ݏ���
	bool Object::MapData::Load(const string& filePath_)
	{
		//�`��}�b�v�ǂݍ���
		ifstream ifs(filePath_);
		if (!ifs)
		{
			return false;
		}

		//�ǂݍ���
		ifs >> this->width;
		ifs >> this->height;
		for (int y = 0; y < this->height; ++y)
		{
			this->chipdata.push_back(vector<int>());
			for (int x = 0; x < width; ++x)
			{
				this->chipdata[y].push_back(0);
				ifs >> this->chipdata[y][x];
			}
		}

		return true;
	}

	void Object::SetEnemyOnMap() {
		auto em = ge->GetTask<EnemyManager::Object>(EnemyManager::defGroupName, EnemyManager::defName);
		if (em == nullptr)return;
		em->KillAllEnemys();
		//�`�b�v��T��
		for (int y = 0; y < this->GenerateMap.height; ++y)
		{
			for (int x = 0; x < this->GenerateMap.width; ++x)
			{
				int en = this->GenerateMap.chipdata[y][x];
				//en�͈̔͂�0�`6�͈̔͂Ƃ���B
				if (en >= 0 && en <= 6) {
					ML::Vec2 epos(
						x * this->res->drawSize,
						y * this->res->drawSize
					);
					
					em->SpawnEnemyNum(en, epos);
				}
			}
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