#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:ミニマップ
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:2023/11/25
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  MiniMap
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("GameUI");	//グループ名
	const  string  defName("MiniMap");	//タスク名
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
		DG::Image::SP imgBG;
		OL::Size2D imgBGSize;
		DG::Image::SP imgChip;
		OL::Size2D imgChipSize;


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
		enum MapChipType
		{
			NON			= 0b00000000,
			NOMAP		= 0b10000000,
			VISITED		= 0b01000000,
			ISCONNECT	= 0b00100000,
			ISOTHER		= 0b00010000,
			UP			= 0b00001000,
			DOWN		= 0b00000100,
			RIGHT		= 0b00000010,
			LEFT		= 0b00000001,
		};

		ML::Vec2 pos;
		ML::Vec2 cameraPos;
		OL::Size2D screenSize;
		ML::Vec2 screenOfs;
		vector<vector<int>> mapData;
		int mapSizeMax;


		//マップの向きを画像の位置番号に変更
		void ResizeMap(int size_);
		void LoadData(const std::string& folderPath_);
		int MapDirToImgPosNum(const Map::MapDir& mapDirection_);
		void SetVisit(int x_, int y_);

		MapChipType ConvertToMCT(int mapDirection_);
		void SetChip(ML::Box2D& src_, int x_, int y_);
		void SetToScreen(ML::Box2D& drawBox_, ML::Box2D& srcBox_, const ML::Box2D& screen_);
	};
}