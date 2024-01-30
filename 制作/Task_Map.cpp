//-------------------------------------------------------------------
//�}�b�v
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map.h"

#include  "Task_Player.h"
#include  "Task_EnemyManager.h"

#include  "Task_ItemTrsBox.h"	

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
		this->depth = 1;
		this->visited = true;
		this->folderPath = "";
		this->depthInLevel = 10;
		auto resultSlope = this->LoadSlope("./data/map/slopesData.json");
		auto resultSpike = this->LoadSpike("./data/map/spikesData.json");
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
	bool Object::LoadMap(const string& folderPath_)
	{
		json js = OL::LoadJsonFile(folderPath_ + "/mapData.json");

		string mapName = js["mapName"];
		Map::MapType mapType = (Map::MapType)((int)js["mapType"]);
		this->depth = js["depth"];
		this->visited = js["visited"];

		this->folderPath = folderPath_;
		js.clear();

		//�w�i�}�b�v�ǂݍ���

		int layerNum = 0;
		js = OL::LoadJsonFile("./data/map/backGroundData.json");
		this->backMap.img.reset();
		if (mapType == Map::MapType::Connect)
		{
			int level = this->depth / this->depthInLevel;
			int levelCnt = 0;
			for (auto& bmd : js["connects"])
			{
				if (levelCnt >= level)
				{
					this->backMap.img = DG::Image::Create(bmd["imgFilePath"]);
					this->backMap.imgSize.w = bmd["imgWidth"];
					this->backMap.imgSize.h = bmd["imgHeight"];
					this->backMap.scale = bmd["scale"];
					this->backMap.moveScale = bmd["moveScale"];
					this->backMap.infinity = bmd["infinity"];
					break;
				}
				else if (levelCnt >= js["connects"].size() - 1)
				{
					this->backMap.img = DG::Image::Create(bmd["imgFilePath"]);
					this->backMap.imgSize.w = bmd["imgWidth"];
					this->backMap.imgSize.h = bmd["imgHeight"];
					this->backMap.scale = bmd["scale"];
					this->backMap.moveScale = bmd["moveScale"];
					this->backMap.infinity = bmd["infinity"];
					break;
				}
				++levelCnt;
			}
		}
		else if (mapType == Map::MapType::Map)
		{
			int level = this->depth / this->depthInLevel;
			int levelCnt = 0;
			for (auto& bmd : js["backGrounds"])
			{
				if (levelCnt >= level)
				{
					this->backMap.img = DG::Image::Create(bmd["imgFilePath"]);
					this->backMap.imgSize.w = bmd["imgWidth"];
					this->backMap.imgSize.h = bmd["imgHeight"];
					this->backMap.scale = bmd["scale"];
					this->backMap.moveScale = bmd["moveScale"];
					this->backMap.infinity = bmd["infinity"];
					break;
				}
				else if (levelCnt >= js["backGrounds"].size() - 1)
				{
					this->backMap.img = DG::Image::Create(bmd["imgFilePath"]);
					this->backMap.imgSize.w = bmd["imgWidth"];
					this->backMap.imgSize.h = bmd["imgHeight"];
					this->backMap.scale = bmd["scale"];
					this->backMap.moveScale = bmd["moveScale"];
					this->backMap.infinity = bmd["infinity"];
					break;
				}
				++levelCnt;
			}
		}
		else if (mapName == "map_start")
		{
			json& bmd = js["start"];
			this->backMap.img = DG::Image::Create(bmd["imgFilePath"]);
			this->backMap.imgSize.w = bmd["imgWidth"];
			this->backMap.imgSize.h = bmd["imgHeight"];
			this->backMap.scale = bmd["scale"];
			this->backMap.moveScale = bmd["moveScale"];
			this->backMap.infinity = bmd["infinity"];
		}
		else if (mapName == "map_goal")
		{
			json& bmd = js["goal"];
			this->backMap.img = DG::Image::Create(bmd["imgFilePath"]);
			this->backMap.imgSize.w = bmd["imgWidth"];
			this->backMap.imgSize.h = bmd["imgHeight"];
			this->backMap.scale = bmd["scale"];
			this->backMap.moveScale = bmd["moveScale"];
			this->backMap.infinity = bmd["infinity"];
		}


		//�`��}�b�v�ǂݍ���
		drawMap.clear();

		layerNum = 0;
		while (layerNum < 10)
		{
			MapData layer;

			//�ǂݍ��ރ}�b�v�������Ȃ�����I��
			if (layer.Load("./data/map/" + mapName + "/" + mapName + "_R" + to_string(layerNum) + ".csv")
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

		if (this->ObjectMap.Load("./data/map/" + mapName + "/" + mapName + "_H.csv")
			 == false)
		{
			return false;
		}

		//�G���e�B�e�B�}�b�v�ǂݍ���
		this->GenerateMap.chipdata.clear();

		if (this->GenerateMap.Load("./data/map/" + mapName + "/" + mapName + "_gens.csv")
			== false)
		{
			return false;
		}	

		//�G����
		this->SetEnemyOnMap();
		
		//�����ς݃A�C�e���ǂݍ���
		this->boxOpenData.clear();

		ifstream ifs(this->folderPath + "openedBox.txt");
		while (!ifs)
		{
			ML::Point mapPos;
			ifs >> mapPos.x;
			if(!ifs )
			{
				break;
			}

			ifs >> mapPos.y;

			this->boxOpenData.push_back(mapPos);
		}


		//�A�C�e������
		this->SetItemOnMap();



		//�����蔻���`�ݒ�
		this->hitBase = ML::Box2D(0, 0, this->ObjectMap.width * this->res->drawSize, this->ObjectMap.height * this->res->drawSize);

		return true;
	}


	//�}�b�v�ǂݍ���
	bool Object::SaveMap()
	{
		json js = OL::LoadJsonFile(this->folderPath + "mapData.json");
		js["visited"] = true;
		
		ofstream ofs(this->folderPath + "openedBox.txt");
		for (auto& boxPos : this->boxOpenData)
		{
			ofs << boxPos.x << ' ';
			ofs << boxPos.y << endl;
		}
		ofs.close();

		OL::SaveJsonFile(js, this->folderPath + "mapData.json");
		return true;
	}
	//-------------------------------------------------------------------
	//��f�[�^�̓ǂݍ���
	bool Object::LoadSlope(const string& filepath_)
	{
		json js;
		std::ifstream fin(filepath_);
		if (!fin) { return false; }
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
		return true;
	}
	//-------------------------------------------------------------------
	//�_���[�W���f�[�^�̓ǂݍ���
	bool Object::LoadSpike(const string& filepath_)
	{
		json js = OL::LoadJsonFile(filepath_);
		for (auto& ji : js)
		{
			spikeData[ji["chipNum"]] = SpikeData();
			spikeData[ji["chipNum"]].damage = ji["damage"];
		}
		return true;

	}
	//-------------------------------------------------------------------
	//�����蔻��
	bool  Object::CheckHit(const  ML::Box2D& hit_)
	{
		return this->CheckHitTo(hit_, 0);
	}
	//-------------------------------------------------------------------
	//�����蔻��
	bool  Object::CheckHit(const  ML::Vec2& pos_)
	{
		return this->CheckHitTo(pos_, 0);
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
	//���̃`�b�v���ɍ₪���݂��邩
	bool Object::CheckSlopeOnChip(const ML::Box2D& hit_)
	{
		for (auto& sp : this->slopeData)
		{
			if (CheckHitTo(hit_, sp.first) == true)
			{
				return true;
			}
		}
		return false;
	}
	//���̃`�b�v���ɍ₪���݂��邩
	bool Object::CheckSlopeOnChip(const ML::Vec2& pos_)
	{
		for (auto& sp : this->slopeData)
		{
			if (CheckHitTo(pos_, sp.first) == true)
			{
				return true;
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//���蔲���鏰����
	bool  Object::CheckFallGround(const  ML::Box2D& hit_)
	{
		return this->CheckHitTo(hit_, 9);
	}
	//-------------------------------------------------------------------
	//�_���[�W������
	SpikeData Object::CheckSpike(const ML::Box2D& hit_)
	{
		for (auto& sp : this->spikeData)
		{
			if (CheckHitTo(hit_, sp.first) == true)
			{
				return sp.second;
			}
		}
		SpikeData notSpike;
		notSpike.damage = -1;
		return notSpike;
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
	// �v���C���[�̃X�^�[�g�n�_�̍��W
	ML::Vec2 Object::GetPlayerStartPos() const
	{
		int enterChip = 7;

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
	bool Object::CheckHitTo(const ML::Vec2& pos_, int chipNum_)
	{
		int mapX = pos_.x / this->res->drawSize;
		int mapY = pos_.y / this->res->drawSize;

		return this->ObjectMap.chipdata[mapY][mapX] == chipNum_;
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
		ML::Vec2 mapCenter(this->hitBase.w / 2, this->hitBase.h / 2);
		ML::Vec2 cameraPos(ge->camera2D.x, ge->camera2D.y);
		cameraPos += OL::BoxCenterPos(ge->camera2D);
		ML::Box2D draw = OL::setBoxCenter(this->backMap.imgSize * this->backMap.scale);
		draw.Offset(ge->GetScreenCenter());
		draw.Offset((mapCenter - cameraPos) * this->backMap.moveScale);
		ML::Box2D src(0, 0, this->backMap.imgSize.w, this->backMap.imgSize.h);

		//�X�N���[���ƃ}�b�v�͈̔͂�p��
		ML::Rect  c = OL::BoxToRect(ge->GetScreenBox());
		ML::Rect  bg = OL::BoxToRect(draw);

		//�L���̏ꍇ
		if (this->backMap.infinity == false)
		{
			//�X�N���[���̈ʒu�𒲐�
			if (c.right > bg.right){ draw.x = c.right - draw.w; }
			if (c.bottom > bg.bottom) { draw.y = c.bottom - draw.h; }
			if (c.left < bg.left) { draw.x = c.left; }
			if (c.top < bg.top) { draw.y = c.top; }
			//�}�b�v���X�N���[����菬�����ꍇ
			if (draw.w < ge->screenWidth) { draw.x = c.left; }
			if (draw.h < ge->screenHeight){ draw.y = c.top; }
		}
		//�����̏ꍇ
		else
		{
			//�㉺����
			this->DrawBackSubUD(draw, src, bg, c);
			ML::Box2D subDraw = draw;
			ML::Box2D subSrc = src;
			ML::Rect subBg = bg;
			//�E����
			while (c.right > subBg.right)
			{
				subDraw.x = subDraw.x + draw.w;
				subDraw.w = min(c.right - subBg.right, draw.w);
				subSrc.w = subDraw.w / this->backMap.scale;
				this->backMap.img->Draw(subDraw, subSrc);
				subBg = OL::BoxToRect(subDraw);
				//�㉺����
				this->DrawBackSubUD(subDraw, subSrc, subBg, c);
			}
			subDraw = draw;
			subSrc = src;
			subBg = bg;
			//������
			while (c.left < subBg.left)
			{
				subDraw.x = max(c.left, subDraw.x - draw.w);
				subDraw.w = min(subBg.left - c.left, draw.w);
				subSrc.x = (draw.w - subDraw.w) / this->backMap.scale;
				subSrc.w = subDraw.w / this->backMap.scale;
				this->backMap.img->Draw(subDraw, subSrc);
				subBg = OL::BoxToRect(subDraw);
				//�㉺����
				this->DrawBackSubUD(subDraw, subSrc, subBg, c);
			}
		}

		this->backMap.img->Draw(draw, src);

	}
	void Object::DrawBackSubUD(const ML::Box2D& draw_, const ML::Box2D src_, const ML::Rect& bg_, const ML::Rect& gSc_)
	{
		//�����
		ML::Box2D subDraw = draw_;
		ML::Box2D subSrc = src_;
		ML::Rect subBg = bg_;
		while (gSc_.top < subBg.top)
		{
			subDraw.y = max(gSc_.top, subDraw.y - draw_.h);
			subDraw.h = min(subBg.top - gSc_.top, draw_.h);
			subSrc.y = (draw_.h - subDraw.h) / this->backMap.scale;;
			subSrc.h = subDraw.h / this->backMap.scale;
			this->backMap.img->Draw(subDraw, subSrc);
			subBg = OL::BoxToRect(subDraw);
		}

		//������
		subDraw = draw_;
		subSrc = src_;
		subBg = bg_;
		while (gSc_.bottom > subBg.bottom)
		{
			subDraw.y = subDraw.y + draw_.h;
			subDraw.h = min(gSc_.bottom - subBg.bottom, draw_.h);
			subSrc.h = subDraw.h / this->backMap.scale;
			this->backMap.img->Draw(subDraw, subSrc);
			subBg = OL::BoxToRect(subDraw);
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
	//-------------------------------------------------------------------
	//�G�̐���
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
					
					em->SpawnEnemyNum(en, epos, this->depth / this->depthInLevel);
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//�A�C�e���̐���
	void Object::AddOpenedBox(ML::Point pos_)
	{
		this->boxOpenData.push_back(pos_);
	}
	//-------------------------------------------------------------------
	//�A�C�e���̐���
	void Object::SetItemOnMap()
	{
		ge->KillAll_G("item");

		json js = OL::LoadJsonFile("./data/Item/ItemRate.json");
		auto& itRate = js["Rate"].at(this->depth / this->depthInLevel);

		//�`�b�v��T��
		for (int y = 0; y < this->GenerateMap.height; ++y)
		{
			for (int x = 0; x < this->GenerateMap.width; ++x)
			{
				bool opened = false;
				for (auto& mapPos : this->boxOpenData)
				{
					if (mapPos.x == x && mapPos.y == y)
					{
						opened = true;
					}
				}

				if (opened)
				{
					continue;
				}

				int in = this->GenerateMap.chipdata[y][x];
				//en�͈̔͂�0�`6�͈̔͂Ƃ���B
				if (in == 12) {
					auto box = ItemTrsBox::Object::Create(true);
					box->pos.x = x * this->res->drawSize;
					box->pos.y = y * this->res->drawSize;
					ML::Point mapPos;
					mapPos.x = x;
					mapPos.y = y;
					box->BoxInit(
						mapPos,
						itRate["coinMin"],
						itRate["coinMax"],
						itRate["levelMin"],
						itRate["levelMax"]
					);
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