#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:コントローラーガイド
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  GuideControll
{
	enum Button
	{
		X,
		A,
		Y,
		B,
		LStick,
		RStick,
		LTrigger,
		RTrigger,
		LButton,
		RButton,
		Select,
		Start
	};

	enum GuideType
	{
		Game,
		Menu
	};


	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("GuideControll");	//グループ名
	const  string  defName("GuideControll");	//タスク名
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
		OL::Size2D imgSize;
		DG::Image::SP imgBg;
		OL::Size2D imgBgSize;
		int imgCountLine;		//一行に存在する画像数
		DG::Font::SP font;
		OL::Size2D fontSize;
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
		struct GuideButton
		{
			Button buttonNum;
			string guideStr;
		};
		ML::Vec2 posRB;	//右端+下端の座標
		float distance;
		vector<GuideButton> guides[2];
		GuideType currentGuide;
		ML::Box2D GetButtonSrc(Button bt_);
		void Load(const string& folderPath_);
		void AddGuides(GuideType type_, Button bu_, const string& guideStr_);
	public:
		//追加したい変数・メソッドはここに追加する

		void SetGuide(GuideType type_) { this->currentGuide = type_; }
	};
}