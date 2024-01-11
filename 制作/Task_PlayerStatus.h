#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:プレイヤーステータス画面
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace PlayerStatusShop
{
	class Object;
}

namespace MyUI
{
	class SelectableObject;
}

namespace  PlayerStatus
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("SystemMenu");	//グループ名
	const  string  defName("PlayerStatus");	//タスク名
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
		DG::Image::SP imgBg;
		OL::Size2D imgBgSize;
		DG::Font::SP systemFont;
		OL::Size2D systemFontSize;
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
		enum PStatus
		{
			ATK = 0,
			DEF,
			INT,
			SPD,
		};
		
		shared_ptr<PlayerStatusShop::Object> shops[4];
		int currentStatus;

		ML::Vec2 pos;
		ML::Vec2 shopOffset;
		float shopDistance;

		ML::Vec2 statusBeginPos;
		float statusDistance;

		MyUI::SelectableObject* currentShop;		//選択中のショップ
		
		void SetDownObj(MyUI::SelectableObject* nextObj_);
		void SetRightObj(MyUI::SelectableObject* nextObj_);
	};
}