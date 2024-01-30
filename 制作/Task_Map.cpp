//-------------------------------------------------------------------
//マップ
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
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/map/image/map96.png");
		this->chipSize = 96;	//チップ画像のサイズ
		this->drawSize = 64;	//描画するサイズ
		this->turnNum = 64;		//画像の1行に含まれるチップの種類

		this->debugFont = DG::Font::Create("non", 8, 16);	//数字フォント
		this->drawObject = false;	//オブジェクトチップの数字描画
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->render2D_Priority[1] = 0.9f;
		this->depth = 1;
		this->visited = true;
		this->folderPath = "";
		this->depthInLevel = 10;
		auto resultSlope = this->LoadSlope("./data/map/slopesData.json");
		auto resultSpike = this->LoadSpike("./data/map/spikesData.json");
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//背景の描画
		this->DrawBackMap();
		
		//マップチップの描画
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

		//オブジェクトマップの描画（デバッグ用）
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
	//その他のメソッド

	//マップ読み込み
	bool Object::LoadMap(const string& folderPath_)
	{
		json js = OL::LoadJsonFile(folderPath_ + "/mapData.json");

		string mapName = js["mapName"];
		Map::MapType mapType = (Map::MapType)((int)js["mapType"]);
		this->depth = js["depth"];
		this->visited = js["visited"];

		this->folderPath = folderPath_;
		js.clear();

		//背景マップ読み込み

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


		//描画マップ読み込み
		drawMap.clear();

		layerNum = 0;
		while (layerNum < 10)
		{
			MapData layer;

			//読み込むマップが無くなったら終了
			if (layer.Load("./data/map/" + mapName + "/" + mapName + "_R" + to_string(layerNum) + ".csv")
				 == false)
			{
				break;
			}

			this->drawMap.push_back(layer);
			++layerNum;
		}

		//マップが一つもなければfalse
		if (drawMap.empty())
		{
			return false;
		}

		//オブジェクトマップ読み込み
		this->ObjectMap.chipdata.clear();

		if (this->ObjectMap.Load("./data/map/" + mapName + "/" + mapName + "_H.csv")
			 == false)
		{
			return false;
		}

		//エンティティマップ読み込み
		this->GenerateMap.chipdata.clear();

		if (this->GenerateMap.Load("./data/map/" + mapName + "/" + mapName + "_gens.csv")
			== false)
		{
			return false;
		}	

		//敵生成
		this->SetEnemyOnMap();
		
		//生成済みアイテム読み込み
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


		//アイテム生成
		this->SetItemOnMap();



		//当たり判定矩形設定
		this->hitBase = ML::Box2D(0, 0, this->ObjectMap.width * this->res->drawSize, this->ObjectMap.height * this->res->drawSize);

		return true;
	}


	//マップ読み込み
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
	//坂データの読み込み
	bool Object::LoadSlope(const string& filepath_)
	{
		json js;
		std::ifstream fin(filepath_);
		if (!fin) { return false; }
		//JSONファイル読み込み
		fin >> js;
		//ファイル読み込み終了
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
	//ダメージ床データの読み込み
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
	//あたり判定
	bool  Object::CheckHit(const  ML::Box2D& hit_)
	{
		return this->CheckHitTo(hit_, 0);
	}
	//-------------------------------------------------------------------
	//あたり判定
	bool  Object::CheckHit(const  ML::Vec2& pos_)
	{
		return this->CheckHitTo(pos_, 0);
	}
	//-------------------------------------------------------------------
	//坂とのあたり判定
	ML::Vec2 Object::CheckSlope(const ML::Box2D& hit_)
	{
		ML::Vec2 result(0, 0);

		//プレイヤーの当たり判定
		ML::Rect  r = { hit_.x, hit_.y, hit_.x + hit_.w, hit_.y + hit_.h };
		//矩形がマップ外に出ていたらサイズを変更する
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

		//ループ範囲調整
		int sx, sy, ex, ey;
		sx = r.left / this->res->drawSize;
		sy = r.top / this->res->drawSize;
		ex = (r.right - 1) / this->res->drawSize;
		ey = (r.bottom - 1) / this->res->drawSize;

		//範囲内の障害物を探す
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				//全ての坂と判定させる
				for (auto& slope : slopeData) {
					//当たったマスが坂だったら
					if (this->ObjectMap.chipdata[y][x] == slope.first) {
						//チップの座標（左上）
						ML::Vec2 chipPos(x * this->res->drawSize, y * this->res->drawSize);
						//さらにチップの坂の部分と当たっているか判定
						if (slope.second.slopeVec.y < 0) {
							//坂に乗っていなかったら判定しない
							if (r.bottom > y * this->res->drawSize + this->res->drawSize)
							{
								continue;
							}
							//右上
							if (slope.second.slopeVec.x > 0) {
								//坂に乗っていなかったら判定しない
								if (r.right > x * this->res->drawSize + this->res->drawSize)
								{
									continue;
								}
								//坂が開始する地点の座標（ゲーム座標）
								ML::Vec2 slopeBegin(chipPos.x, chipPos.y + this->res->drawSize - 1 - slope.second.slopeHeight);
								//プレイヤーの当たり判定右端のx座標の、坂の高さ（ゲーム座標）
								float rbheight = slope.second.slopeVec.y * abs((r.right - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//プレイヤーを坂の上に乗せるために必要な移動距離（最大値は坂の最高値）
								float moveResult = min(-(slope.second.slopeVec.y + slope.second.slopeHeight), rbheight - r.bottom);
								if (moveResult < 0)
								{
									//値の小さいほうを採用する
									result.y = min(moveResult, result.y);
								}
							}
							//左上
							if (slope.second.slopeVec.x < 0) {
								//坂に乗っていなかったら判定しない
								if (r.left < x * this->res->drawSize)
								{
									continue;
								}
								//坂が開始する地点の座標（ゲーム座標）
								ML::Vec2 slopeBegin(chipPos.x + this->res->drawSize - 1, chipPos.y + this->res->drawSize - 1 - slope.second.slopeHeight);
								//プレイヤーの当たり判定左端のx座標の、坂の高さ（ゲーム座標）
								float lbheight = slope.second.slopeVec.y * abs((r.left - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//プレイヤーを坂の上に乗せるために必要な移動距離（最大値は坂の最高値）
								float moveResult = min(-(slope.second.slopeVec.y + slope.second.slopeHeight), lbheight - r.bottom);
								if (moveResult < 0)
								{
									//値の小さいほうを採用する
									result.y = min(moveResult, result.y);
								}
							}
						}
						else if (slope.second.slopeVec.y > 0) {
							//坂に乗っていなかったら判定しない
							if (r.top < y * this->res->drawSize)
							{
								continue;
							}
							//右下
							if (slope.second.slopeVec.x > 0) {
								//坂に乗っていなかったら判定しない
								if (r.right > x * this->res->drawSize + this->res->drawSize)
								{
									continue;
								}
								//坂が開始する地点の座標（ゲーム座標）
								ML::Vec2 slopeBegin(chipPos.x, chipPos.y + slope.second.slopeHeight);
								//プレイヤーの当たり判定右端のx座標の、坂の高さ（ゲーム座標ではなく、ローカル座標）
								float rtheight = slope.second.slopeVec.y * abs((r.right - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//プレイヤーを坂の上に乗せるために必要な移動距離（最大値は坂の最高値）
								float moveResult = min(slope.second.slopeVec.y + slope.second.slopeHeight, rtheight - r.top);
								if (moveResult > 0)
								{
									//上がる指示がない場合のみ実行（変更可）
									if (result.y >= 0) {
										//値の大きいほうを採用する
										result.y = min(moveResult, result.y);
									}
								}
							}
							//左下
							if (slope.second.slopeVec.x < 0) {
								//坂に乗っていなかったら判定しない
								if (r.left < x * this->res->drawSize)
								{
									continue;
								}
								//坂が開始する地点の座標（ゲーム座標）
								ML::Vec2 slopeBegin(chipPos.x + this->res->drawSize - 1, chipPos.y - slope.second.slopeHeight);
								//プレイヤーの当たり判定左端のx座標の、坂の高さ（ゲーム座標ではなく、ローカル座標）
								float lbheight = slope.second.slopeVec.y * abs((r.left - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//プレイヤーを坂の上に乗せるために必要な移動距離（最大値は坂の最高値）
								float moveResult = min(slope.second.slopeVec.y + slope.second.slopeHeight, lbheight - r.top);
								if (moveResult > 0)
								{
									//上がる指示がない場合のみ実行（変更可）
									if (result.y >= 0) {
										//値の大きいほうを採用する
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
	//そのチップ内に坂が存在するか
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
	//そのチップ内に坂が存在するか
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
	//すり抜ける床判定
	bool  Object::CheckFallGround(const  ML::Box2D& hit_)
	{
		return this->CheckHitTo(hit_, 9);
	}
	//-------------------------------------------------------------------
	//ダメージ床判定
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
	//マップ移動時のプレイヤーの座標
	ML::Vec2 Object::GetPlayerEnterPos(const MapDir& mapDirection_)
	{
		//番号は8番～上,下,右,左の順で準備されている
		int enterChip = 8 + (int)mapDirection_;
		OL::Size2D mapSize(GenerateMap.width, GenerateMap.height);

		//チップを探す
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
	// プレイヤーのスタート地点の座標
	ML::Vec2 Object::GetPlayerStartPos() const
	{
		int enterChip = 7;

		OL::Size2D mapSize(GenerateMap.width, GenerateMap.height);

		//チップを探す
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
	//出口判定
	MapDir Object::CheckExit(const ML::Box2D& hit_)
	{
		//4方向
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
	//チップを指定する当たり判定
	bool Object::CheckHitTo(const ML::Box2D& hit_, int chipNum_)
	{
		ML::Rect  r = { hit_.x, hit_.y, hit_.x + hit_.w, hit_.y + hit_.h };
		//矩形がマップ外に出ていたらサイズを変更する
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

		//ループ範囲調整
		int sx, sy, ex, ey;
		sx = r.left / this->res->drawSize;
		sy = r.top / this->res->drawSize;
		ex = (r.right - 1) / this->res->drawSize;
		ey = (r.bottom - 1) / this->res->drawSize;

		//範囲内の障害物を探す
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
	//マップ外を見せないようにカメラを位置調整する
	void  Object::AdjustCameraPos()
	{
		//カメラとマップの範囲を用意
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

		//カメラの位置を調整
		if (c.right > m.right) { ge->camera2D.x = m.right - ge->camera2D.w; }
		if (c.bottom > m.bottom) { ge->camera2D.y = m.bottom - ge->camera2D.h; }
		if (c.left < m.left) { ge->camera2D.x = m.left; }
		if (c.top < m.top) { ge->camera2D.y = m.top; }
		//マップがカメラより小さい場合
		if (this->hitBase.w < ge->camera2D.w) { ge->camera2D.x = m.left; }
		if (this->hitBase.h < ge->camera2D.h) { ge->camera2D.y = m.top; }
	}
	//-------------------------------------------------------------------
	//マップ外を見せないようにカメラを位置調整する
	void  Object::DrawBackMap()
	{
		ML::Vec2 mapCenter(this->hitBase.w / 2, this->hitBase.h / 2);
		ML::Vec2 cameraPos(ge->camera2D.x, ge->camera2D.y);
		cameraPos += OL::BoxCenterPos(ge->camera2D);
		ML::Box2D draw = OL::setBoxCenter(this->backMap.imgSize * this->backMap.scale);
		draw.Offset(ge->GetScreenCenter());
		draw.Offset((mapCenter - cameraPos) * this->backMap.moveScale);
		ML::Box2D src(0, 0, this->backMap.imgSize.w, this->backMap.imgSize.h);

		//スクリーンとマップの範囲を用意
		ML::Rect  c = OL::BoxToRect(ge->GetScreenBox());
		ML::Rect  bg = OL::BoxToRect(draw);

		//有限の場合
		if (this->backMap.infinity == false)
		{
			//スクリーンの位置を調整
			if (c.right > bg.right){ draw.x = c.right - draw.w; }
			if (c.bottom > bg.bottom) { draw.y = c.bottom - draw.h; }
			if (c.left < bg.left) { draw.x = c.left; }
			if (c.top < bg.top) { draw.y = c.top; }
			//マップがスクリーンより小さい場合
			if (draw.w < ge->screenWidth) { draw.x = c.left; }
			if (draw.h < ge->screenHeight){ draw.y = c.top; }
		}
		//無限の場合
		else
		{
			//上下方向
			this->DrawBackSubUD(draw, src, bg, c);
			ML::Box2D subDraw = draw;
			ML::Box2D subSrc = src;
			ML::Rect subBg = bg;
			//右方向
			while (c.right > subBg.right)
			{
				subDraw.x = subDraw.x + draw.w;
				subDraw.w = min(c.right - subBg.right, draw.w);
				subSrc.w = subDraw.w / this->backMap.scale;
				this->backMap.img->Draw(subDraw, subSrc);
				subBg = OL::BoxToRect(subDraw);
				//上下方向
				this->DrawBackSubUD(subDraw, subSrc, subBg, c);
			}
			subDraw = draw;
			subSrc = src;
			subBg = bg;
			//左方向
			while (c.left < subBg.left)
			{
				subDraw.x = max(c.left, subDraw.x - draw.w);
				subDraw.w = min(subBg.left - c.left, draw.w);
				subSrc.x = (draw.w - subDraw.w) / this->backMap.scale;
				subSrc.w = subDraw.w / this->backMap.scale;
				this->backMap.img->Draw(subDraw, subSrc);
				subBg = OL::BoxToRect(subDraw);
				//上下方向
				this->DrawBackSubUD(subDraw, subSrc, subBg, c);
			}
		}

		this->backMap.img->Draw(draw, src);

	}
	void Object::DrawBackSubUD(const ML::Box2D& draw_, const ML::Box2D src_, const ML::Rect& bg_, const ML::Rect& gSc_)
	{
		//上方向
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

		//下方向
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
	//読み込み処理
	bool Object::MapData::Load(const string& filePath_)
	{
		//描画マップ読み込み
		ifstream ifs(filePath_);
		if (!ifs)
		{
			return false;
		}

		//読み込み
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
	//敵の生成
	void Object::SetEnemyOnMap() {
		auto em = ge->GetTask<EnemyManager::Object>(EnemyManager::defGroupName, EnemyManager::defName);
		if (em == nullptr)return;
		em->KillAllEnemys();
		//チップを探す
		for (int y = 0; y < this->GenerateMap.height; ++y)
		{
			for (int x = 0; x < this->GenerateMap.width; ++x)
			{
				int en = this->GenerateMap.chipdata[y][x];
				//enの範囲は0～6の範囲とする。
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
	//アイテムの生成
	void Object::AddOpenedBox(ML::Point pos_)
	{
		this->boxOpenData.push_back(pos_);
	}
	//-------------------------------------------------------------------
	//アイテムの生成
	void Object::SetItemOnMap()
	{
		ge->KillAll_G("item");

		json js = OL::LoadJsonFile("./data/Item/ItemRate.json");
		auto& itRate = js["Rate"].at(this->depth / this->depthInLevel);

		//チップを探す
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
				//enの範囲は0～6の範囲とする。
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



	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
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
	//リソースクラスの生成
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