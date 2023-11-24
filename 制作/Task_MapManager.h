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
	private:

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

			virtual string Generate() { return ""; };
		};

		enum class MapEnter
		{
			Up = 0,
			Left
		};
		enum class MapExit
		{
			Non = 0,
			Right,
			Down,
		};

		class Map : public MapObject
		{
		private:
			MapEnter enter;	//入口
			MapExit exit;		//出口
			int depth;		//マップの深度

		public:

			Map(MapEnter enter_, MapExit exit_, int depth_)
				:enter(enter_)
				,exit(exit_)
				,depth(depth_)
			{
			}

			string Generate() override
			{
				return "1";
			}
		};

		//通路
		class Connect : public MapObject
		{
		private:
			MapEnter enter;
			MapExit exit;
			MapExit exitSub;
		public:
			Connect(MapEnter enter_, MapExit exit_, MapExit exitSub_ = MapExit::Non)
				:enter(enter_)
				,exit(exit_)
				,exitSub(exitSub_)
			{
			}

			string Generate() override
			{
				return "2";
			}

		};

	public:
		//追加したい変数・メソッドはここに追加する
		int bossDepth;
		unsigned int mapSeed;		//マップ生成のシード値
		MapObject* map[20][20];


		void Generate();
		void GenerateMap(int x_, int y_, int depth_, int depthRest_, MapEnter enter_);
		void Destroy();		//消滅時の処理
	};
}