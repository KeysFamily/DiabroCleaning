//?------------------------------------------------------
//タスク名:マップ管理
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_MapManager.h"

namespace  MapManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		this->mapSeed = (unsigned int)time(NULL);
		srand(mapSeed);
		this->Destroy();


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
	}
	//-------------------------------------------------------------------
	//その他のメソッド
	void Object::Generate()
	{
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				map[y][x] = nullptr;
			}
		}

		map[0][0] = new MapObject("map_start");
		map[0][1] = new Object::Connect(MapEnter::Left, MapExit::Right);
		

	}

	void Object::GenerateMap(int x_, int y_, int depth_, int depthRest_, MapEnter enter_)
	{
		enum GenerateDir
		{
			Right = 0,
			Down,
			RightDown,
		};

		int generatePos = rand() % 3;
		bool generateSub = rand() % 2;

		if (generateSub && generatePos == 3)
		{
			--generatePos;
		}


		int genX;
		int genY;
		MapEnter enterDir;
		MapExit exitDir;
		
		switch (generatePos)
		{
		case GenerateDir::Right:
			genX = 2;
			genY = 0;
			enterDir = MapEnter::Left;
			exitDir = MapExit::Right;
			break;
		case GenerateDir::Down:
			genX = 0;
			genY = 2;
			enterDir = MapEnter::Up;
			exitDir = MapExit::Down;
			break;
		case GenerateDir::RightDown:
			genX = 1;
			genY = 1;
			enterDir = MapEnter::Up;
			exitDir = MapExit::Right;
			break;
		}
		
		GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
		if (generateSub)
		{
			genX = 1;
			genY = 1;
			if (enterDir == MapEnter::Up)
			{
				enterDir = MapEnter::Left;
			}
			else
			{
				enterDir = MapEnter::Up;
			}
			GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
		}


		map[y_][x_] = new Map(enter_, exitDir, depth_);
	}




	//消滅時の処理
	void Object::Destroy()
	{
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				delete map[y][x];
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

	//-------------------------------------------------------------------

}
