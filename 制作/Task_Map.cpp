//-------------------------------------------------------------------
//マップ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Map.h"
#include  "Task_Player.h"

namespace  Map
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/map/map96.png");
		this->chipSize = 96;	//チップ画像のサイズ
		this->drawSize = 64;	//描画するサイズ
		this->turnNum = 64;		//画像の1行に含まれるチップの種類

		this->debugFont = DG::Font::Create("non", 8, 16);	//数字フォント
		this->drawObject = true;	//オブジェクトチップの数字描画
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
		auto result = this->LoadMap("map_start");
		this->testCam = ML::Vec2(0, 0);
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
		auto inp = ge->in1->GetState();
		int camSpeed = 10;
		if (inp.LStick.BU.on == true)
		{
			this->testCam.y -= camSpeed;
		}
		if (inp.LStick.BD.on == true)
		{
			this->testCam.y += camSpeed;
		}
		if (inp.LStick.BL.on == true)
		{
			this->testCam.x -= camSpeed;
		}
		if (inp.LStick.BR.on == true)
		{
			this->testCam.x += camSpeed;
		}

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
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
	bool Object::LoadMap(const string& mapName_)
	{
		//描画マップ読み込み
		int layerNum = 0;
		while (layerNum < 10)
		{
			MapData layer;

			//読み込むマップが無くなったら終了
			if (layer.Load("./data/map/" + mapName_ + "/" + mapName_ + "_R" + to_string(layerNum) + ".csv")
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
		if (this->ObjectMap.Load("./data/map/" + mapName_ + "/" + mapName_ + "_H.csv")
			 == false)
		{
			return false;
		}

		//生成マップ読み込み
		/*if (this->ObjectMap.Load("./data/map/" + mapName_ + "/" + mapName_ + "_gen.csv")
			== false)
		{
			return false;
		}*/


		//当たり判定矩形設定
		this->hitBase = ML::Box2D(0, 0, this->ObjectMap.width * this->res->drawSize, this->ObjectMap.height * this->res->drawSize);

		return true;
	}

	//-------------------------------------------------------------------
	//坂データの読み込み
	bool Object::LoadSlope(const string& filepath_)
	{
		json js;
		std::ifstream fin(filepath_);
		if (!fin) { return json(); }
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
		return false;
	}
	//-------------------------------------------------------------------
	//あたり判定
	bool  Object::CheckHit(const  ML::Box2D& hit_)
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
				if (this->ObjectMap.chipdata[y][x] == 0) {
					return true;
				}
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//坂とのあたり判定
	ML::Vec2 Object::CheckSlope(const ML::Box2D& hit_)
	{
		ML::Vec2 result(0, 0);

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
							//右上
							if (slope.second.slopeVec.x > 0) {
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
							//右下
							if (slope.second.slopeVec.x > 0) {
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
								//坂が開始する地点の座標（ゲーム座標）
								ML::Vec2 slopeBegin(chipPos.x + this->res->drawSize - 1, chipPos.y - slope.second.slopeHeight);
								//プレイヤーの当たり判定左端のx座標の、坂の高さ（ゲーム座標ではなく、ローカル座標）
								float lbheight = slope.second.slopeVec.y * abs((r.left - slopeBegin.x) / this->res->drawSize) + slopeBegin.y;
								//プレイヤーを坂の上に乗せるために必要な移動距離（最大値は坂の最高値）
								float moveResult = min(slope.second.slopeVec.y + slope.second.slopeHeight, lbheight - r.bottom);
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
		if (c.bottom > m.bottom) { ge->camera2D.y = m.bottom-ge->camera2D.h; }
		if (c.left < m.left) { ge->camera2D.x = m.left; }
		if (c.top < m.top) { ge->camera2D.y = m.top; }
		//マップがカメラより小さい場合
		if (this->hitBase.w < ge->camera2D.w) { ge->camera2D.x = m.left; }
		if (this->hitBase.h < ge->camera2D.h) { ge->camera2D.y = m.top; }
	}



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