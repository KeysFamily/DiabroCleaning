#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:マップ管理
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "MapStruct.h"
#include "Task_MapTransition.h"

namespace MapManager
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("MapManager");	//グループ名
	const  string  defName("MapManager");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
		
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:

		class MapObject
		{
		public:
			string mapName;

			enum class MapType
			{
				Empty = 0,
				Map,
				Connect,
			};
			MapType mapType;
			bool visited;

			MapObject(const string& mapName_ = "")
				:mapName(mapName_)
				, mapType(MapType::Empty)
				, visited(false)
			{}

			virtual void Generate() { return; };
		};

		class Area : public MapObject
		{
		private:
			Map::MapDir enter;	//入口
			Map::MapDir exit;		//出口
			int depth;		//マップの深度

		public:

			Area(Map::MapDir enter_, Map::MapDir exit_, int depth_)
				:enter(enter_)
				, exit(exit_)
				, depth(depth_)
			{
			}

			void Generate() override
			{
				string genMapName = "map_";
				string mapDirStr[4] = { "Up", "Down", "Right", "Left" };

				genMapName += mapDirStr[(int)enter];
				if (exit == Map::MapDir::Non)
				{
					genMapName += mapDirStr[(int)enter];
				}
				else
				{
					genMapName += mapDirStr[(int)exit];
				}

				genMapName += "_" + to_string(rand() % 3 + 1);

				this->mapName = genMapName;
			}

			//ゲッタ
			Map::MapDir GetEnter() { return enter; }
			Map::MapDir GetExit() { return exit; }
			int GetDepth() { return depth; }
		};

		//通路
		class Connect : public MapObject
		{
		private:
			Map::MapDir enter;
			Map::MapDir exit;
			Map::MapDir exitSub;
		public:
			Connect(Map::MapDir enter_, Map::MapDir exit_, Map::MapDir exitSub_ = Map::MapDir::Non)
				:enter(enter_)
				, exit(exit_)
				, exitSub(exitSub_)
			{
			}

			void Generate() override
			{
				string genMapName = "pass_";
				string mapDirStr[4] = { "Up", "Down", "Right", "Left" };

				genMapName += mapDirStr[(int)enter];

				if (exitSub != Map::MapDir::Non)
				{
					if (exit == Map::MapDir::Down)
					{
						genMapName += mapDirStr[(int)exitSub] + mapDirStr[(int)exit];
					}
					else
					{
						genMapName += mapDirStr[(int)exit] + mapDirStr[(int)exitSub];
					}
				}
				else
				{
					genMapName += mapDirStr[(int)exit];
				}

				this->mapName = genMapName;
			}

			//ゲッタ
			Map::MapDir GetEnter() { return enter; }
			Map::MapDir GetExit() { return exit; }
			Map::MapDir GetExitSub() { return exitSub; }

		};

	public:
		//追加したい変数・メソッドはここに追加する
		int bossDepth;
		unsigned int mapSeed;		//マップ生成のシード値
		MapObject* map[30][30];
		ML::Point currentPos;		//現在のマップ
		
		Map::MapDir moveMapDir;		//マップ移動時の方向
		MapTransition::Object::SP mapTransition;	//マップトランジションへのポインタ

		//マップのロードに使用する列挙体
		//ロード
		void MoveMap(const Map::MapDir& mapDirection_);

	private:
		void Generate();
		void GenerateMap(int x_, int y_, int depth_, int depthRest_, Map::MapDir enter_);
		void Destroy();		//消滅時の処理

		void MoveMapUpDate();

	};
}