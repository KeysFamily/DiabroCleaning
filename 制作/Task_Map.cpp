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
		auto  pl = Player::Object::Create(true);
		pl->pos.x = ge->screen2DWidth / 2;
		pl->pos.y = ge->screen2DHeight * 2 / 3;

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
		int chipsize = 96;
		int drawSize = 64;
		int srcturn = 64;

		for (auto& layer : this->drawMap)
		{
			for (int y = 0; y < layer.height; ++y)
			{
				for (int x = 0; x < layer.width; ++x)
				{
					ML::Box2D draw(drawSize * x, drawSize * y, drawSize, drawSize);
					draw.Offset(-this->testCam);
					ML::Box2D src(
						chipsize * (layer.chipdata[y][x] % srcturn),
						chipsize * (layer.chipdata[y][x] / srcturn),
						chipsize,
						chipsize);

					this->res->img->Draw(draw, src);
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
			ifstream ifs("./data/map/" + mapName_ + "/" + mapName_ + "_R" + to_string(layerNum) + ".csv");
			if (!ifs)
			{
				break;
			}

			this->drawMap.push_back(MapData());

			//読み込み
			ifs >> this->drawMap[layerNum].width;
			ifs >> this->drawMap[layerNum].height;
			for (int y = 0; y < drawMap[layerNum].height; ++y)
			{
				this->drawMap[layerNum].chipdata.push_back(vector<int>());
				for (int x = 0; x < drawMap[layerNum].width; ++x)
				{
					this->drawMap[layerNum].chipdata[y].push_back(0);
					ifs >> this->drawMap[layerNum].chipdata[y][x];
				}
			}

			++layerNum;
		}

		if (drawMap.empty())
		{
			return false;
		}

		//オブジェクトマップ読み込み
		ifstream ifs("./data/map/" + mapName_ + "/" + mapName_ + "_H.csv");
		if (!ifs)
		{
			return false;
		}
		//読み込み
		ifs >> this->ObjectMap.width;
		ifs >> this->ObjectMap.height;
		for (int y = 0; y < ObjectMap.height; ++y)
		{
			this->ObjectMap.chipdata.push_back(vector<int>());
			for (int x = 0; x < ObjectMap.width; ++x)
			{
				this->ObjectMap.chipdata[y].push_back(0);
				ifs >> this->ObjectMap.chipdata[y][x];
			}
		}

		return true;
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
		sx = r.left / 32;
		sy = r.top / 32;
		ex = (r.right - 1) / 32;
		ey = (r.bottom - 1) / 32;

		//範囲内の障害物を探す
		for (int y = sy; y <= ey; ++y) {
			for (int x = sx; x <= ex; ++x) {
				if (8 <= this->arr[y][x]) {
					return true;
				}
			}
		}
		return false;
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