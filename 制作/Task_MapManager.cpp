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
#include  "Task_MapTransition.h"
#include  "Task_Sprite.h"

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
		ge->printToDebugFile(to_string(mapSeed),1);
		this->Generate();

		this->moveMapDir = Map::MapDir::Non;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		this->Destroy();


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		MoveMapUpDate();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}
	//-------------------------------------------------------------------
	//その他のメソッド
	//生成全般
	void Object::Generate()
	{
		for (int y = 0; y < 30; ++y)
		{
			for (int x = 0; x < 30; ++x)
			{
				map[y][x] = nullptr;
			}
		}

		map[0][0] = new MapObject("map_start");

		map[0][1] = new Object::Connect(Map::MapDir::Left, Map::MapDir::Right);

		this->GenerateMap(2, 0, 2, 6, Map::MapDir::Left);
		//生成
		for (int y = 0; y < 30; ++y)
		{
			for (int x = 0; x < 30; ++x)
			{
				if (map[y][x] != nullptr)
				{
					map[y][x]->Generate();
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//1マップ生成処理
	void Object::GenerateMap(int x_, int y_, int depth_, int depthRest_, Map::MapDir enter_)
	{
		enum GenerateDir
		{
			Right = 0,
			Down,
			RightDown,
		};

		//最下層なら次の生成処理は行わない
		if (depthRest_ <= 1)
		{
			map[y_][x_] = new Area(enter_, Map::MapDir::Non, depth_);
			return;
		}

		//生成不可な場所を探す（生成する方向にマップ・もしくは通路があるか判定）
		int cantGeneratesTotal = 0;
		cantGeneratesTotal += map[y_][x_ + 2] != nullptr || map[y_][x_ + 1] != nullptr;
		cantGeneratesTotal += map[y_ + 1][x_ + 1] != nullptr || map[y_ + 1][x_] != nullptr && map[y_][x_ + 1] != nullptr;
		cantGeneratesTotal += map[y_ + 2][x_] != nullptr || map[y_ + 1][x_] != nullptr;

		//生成可能な場所がなければ次の生成処理は行わない
		if (cantGeneratesTotal >= 3)
		{
			map[y_][x_] = new Area(enter_, Map::MapDir::Non, depth_);
			return;
		}

		//生成の種類（右・下・斜め）
		int generateTypes = 3;

		//分岐させるか
		bool generateSub = rand() % 2 && cantGeneratesTotal < 1 && map[y_ + 1][x_ + 1] == nullptr;
		//分岐が有効なら、右か下を最初に生成（斜めは確定なので）
		if (generateSub)
		{
			--generateTypes;
		}

		int genX = 0;
		int genY = 0;
		int conX = 0;
		int conY = 0;
		Map::MapDir enterDir;
		Map::MapDir exitDir;

		Map::MapDir connectEnter = Map::MapDir::Up;
		Map::MapDir connectExit = Map::MapDir::Right;
		Map::MapDir connectExitSub = Map::MapDir::Non;

		//生成が完了するまでループする
		bool finishedGenerate = true;
		while (finishedGenerate)
		{
			//生成の方向をランダムで決定
			int generatePos = rand() % generateTypes;

			//生成の方向に応じて値を調整
			switch (generatePos)
			{
			case GenerateDir::Right:
				if (map[y_][x_ + 2] != nullptr || map[y_][x_ + 1] != nullptr)
				{
					continue;
				}
				genX = 2;
				genY = 0;
				conX = 1;
				conY = 0;
				enterDir = Map::MapDir::Left;
				connectEnter = Map::MapDir::Left;
				exitDir = Map::MapDir::Right;
				connectExit = Map::MapDir::Right;
				break;
			case GenerateDir::Down:
				if (map[y_ + 2][x_] != nullptr || map[y_ + 1][x_] != nullptr)
				{
					continue;
				}
				genX = 0;
				genY = 2;
				conX = 0;
				conY = 1;
				enterDir = Map::MapDir::Up;
				connectEnter = Map::MapDir::Up;
				exitDir = Map::MapDir::Down;
				connectExit = Map::MapDir::Down;
				break;
			case GenerateDir::RightDown:
				if (map[y_ + 1][x_ + 1] != nullptr || map[y_ + 1][x_] != nullptr && map[y_][x_ + 1] != nullptr)
				{
					continue;
				}
				genX = 1;
				genY = 1;
				conX = 1;
				conY = 0;
				enterDir = Map::MapDir::Up;
				connectEnter = Map::MapDir::Left;
				exitDir = Map::MapDir::Right;
				connectExit = Map::MapDir::Down;
				break;
			}


			//次の位置を生成
			GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);

			//分岐が有効なら斜めも生成
			if (generateSub)
			{
				genX = 1;
				genY = 1;
				if (enterDir == Map::MapDir::Up)
				{
					enterDir = Map::MapDir::Left;
					connectExitSub = Map::MapDir::Right;
				}
				else
				{
					enterDir = Map::MapDir::Up;
					connectExitSub = Map::MapDir::Down;
				}
				GenerateMap(x_ + genX, y_ + genY, depth_ + 1, depthRest_ - 1, enterDir);
			}
			map[y_][x_] = new Area(enter_, exitDir, depth_);
			map[y_ + conY][x_ + conX] = new Connect(connectEnter, connectExit, connectExitSub);

			finishedGenerate = false;
		}
	}
	//-------------------------------------------------------------------
	//ロード
	void Object::MoveMap(const Map::MapDir& mapDirection_)
	{
		if (this->moveMapDir != Map::MapDir::Non)
		{
			return;
		}

		this->moveMapDir = mapDirection_;
		this->mapTransition = MapTransition::Object::Create(true);
		this->mapTransition->Appear(Map::MapFunc::ReverseMapDir(mapDirection_));
	}

	void Object::MoveMapUpDate()
	{
		if (this->moveMapDir == Map::MapDir::Non)
		{
			return;
		}

		if (this->mapTransition == nullptr)
		{
			return;
		}

		if (this->mapTransition->CheckFinishedAppear() == false)
		{
			return;
		}

		switch (this->moveMapDir)
		{
		case Map::MapDir::Up:
			--currentPos.y;
			break;
		case Map::MapDir::Down:
			++currentPos.y;
			break;
		case Map::MapDir::Left:
			--currentPos.x;
			break;
		case Map::MapDir::Right:
			++currentPos.x;
			break;
		}

		ge->qa_Map->LoadMap(map[currentPos.y][currentPos.x]->mapName);
		ge->qa_Player->pos = ge->qa_Map->GetPlayerEnterPos(Map::MapFunc::ReverseMapDir(moveMapDir));
		auto camera = ge->GetTask<Sprite::Object>("Sprite");
		camera->MoveImmediately();

		this->mapTransition->Disappear();

		this->moveMapDir = Map::MapDir::Non;
		this->mapTransition = nullptr;
	}

	//-------------------------------------------------------------------
	//消滅時の処理
	void Object::Destroy()
	{
		for (int y = 0; y < 30; ++y)
		{
			for (int x = 0; x < 30; ++x)
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
