#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//マップ
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "MapStruct.h"

namespace  Map
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Map");	//グループ名
	const  string  defName("Map");	//タスク名
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
		DG::Image::SP img;
		int chipSize;		//元画像の1マスのサイズ
		int drawSize;		//ゲームで描画するときの1マスのサイズ
		int turnNum;		//元画像で1行にいくつのタイルがあるか

		DG::Font::SP debugFont;	//デバッグ用のフォント
		bool drawObject;		//オブジェクトマップを表示するか
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
		//追加したい変数・メソッドはここに追加する
		//マップチップデータ
		struct MapData
		{
			int width;
			int height;
			vector<vector<int>> chipdata;

			//読み込み処理
			bool Load(const string& mapName_);
		};
		//背景データ
		struct BackMapData
		{
			DG::Image::SP img;	//画像
			OL::Size2D imgSize;	//画像サイズ
			float scale;
			float moveScale;	//カメラ移動によって画像が動く量
			bool infinity;		//画像を無限に続かせるか
		};
		//坂道判定データ
		struct SlopeData
		{
			ML::Vec2 slopeVec;		//坂の方向
			float    slopeHeight;	//坂開始時の高さ
		};

		BackMapData backMap;	//背景
		vector<MapData> drawMap;		//描画用チップデータ
		MapData			ObjectMap;		//当たり判定用チップデータ
		MapData			GenerateMap;	//エンティティ用チップデータ
		map<int, SlopeData> slopeData;	//坂の情報のマップ配列
		map<int, Map::SpikeData> spikeData;	//ダメージ床データ
		ML::Box2D			hitBase;	//ピクセル単位のマップサイズを持つ
		bool				visited;	//一度訪れたか
		int					depth;		//深度
		string				folderPath;	//マップデータフォルダへのパス
		int					depthInLevel;	//1レベルにおける深度
		int					depthInLevel_Conn;	//1レベルにおける深度（通路）

		//あたり判定
		bool  CheckHit(const  ML::Box2D& hit_);
		bool  CheckHit(const ML::Vec2& pos_);

		//坂との当たり判定
		ML::Vec2 CheckSlope(const ML::Box2D& hit_);
		//そのチップ内に坂が存在するか
		bool  CheckSlopeOnChip(const ML::Box2D& hit_);
		bool  CheckSlopeOnChip(const ML::Vec2& pos_);

		//すり抜ける床判定
		bool CheckFallGround(const ML::Box2D& hit_);

		//ダメージ床判定
		Map::SpikeData CheckSpike(const ML::Box2D& hit_);

		//マップ読み込み
		bool LoadMap(const string& folderPath_);

		//マップ保存
		bool SaveMap();

		//坂判定読み込み
		bool LoadSlope(const string& filepath_);

		//ダメージ床読み込み
		bool LoadSpike(const string& filepath_);

		//出口判定
		MapDir CheckExit(const ML::Box2D& hit_);

		//マップ移動時のプレイヤーの座標
		ML::Vec2 GetPlayerEnterPos(const MapDir& mapDirection_);

		//プレイヤーのスタート地点の座標
		ML::Vec2 GetPlayerStartPos()const;

		//マップ外を見せないようにカメラを位置調整する
		void  AdjustCameraPos();

	private:
		bool  CheckHitTo(const  ML::Box2D& hit_, int chipNum_);//あたり判定
		bool CheckHitTo(const ML::Vec2& pos_, int chipNum_);//座標との当たり判定
		//背景の描画
		void DrawBackMap();
		void DrawBackSubUD(const ML::Box2D& draw_, const ML::Box2D src_, const ML::Rect& bg_, const ML::Rect& gSc_);

		void SetEnemyOnMap();


	};
}