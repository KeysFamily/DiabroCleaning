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
		Player::Object::Create(true);

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