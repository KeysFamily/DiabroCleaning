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

namespace MiniMap
{
	class Object;
}

namespace MapTransition
{
	class Object;
}

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
		//追加したい変数・メソッドはここに追加する
		int bossDepth;
		unsigned int mapSeed;									//マップ生成のシード値
		float generateSubRate;									//分岐を作る確率
		int	  subDepthMax;										//外れの道の深度最大
		int   depthMax;											//最下層
		Map::MapObject* map[Map::MAPSIZE_MAX][Map::MAPSIZE_MAX];//マップデータ
		int mapid[Map::MAPSIZE_MAX][Map::MAPSIZE_MAX];			//マップデータ（省データ版）
		ML::Point currentPos;									//現在のマップ
		string saveFolderPath;									//保存先のパス
		Map::MapDir moveMapDir;									//マップ移動時の方向
		shared_ptr<MapTransition::Object> mapTransition;		//マップトランジションへのポインタ
		shared_ptr<MiniMap::Object> minimap;					//ミニマップへのポインタ
		bool generated;											//すでに生成済みか


		void GenerateMap(int x_, int y_, int depth_, int depthRest_, Map::MapDir enter_, bool setSub_ = false);
		bool GetSubFlag(int connX_, int connY_);
		void GenerateSub();
		void Destroy();		//消滅時の処理

		void MoveMapUpDate();
	public:
		//マップのロードに使用する列挙体
		//ロード
		void MoveMap(const Map::MapDir& mapDirection_);
		void SetMaxDepth(int depth_);
		bool Generate();

	};
}