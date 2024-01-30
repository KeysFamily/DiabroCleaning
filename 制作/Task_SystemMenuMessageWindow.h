#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:メッセージ欄
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "KeysString.h"

namespace ShopStaff
{
	class Object;
}

namespace  SystemMenuMessageWindow
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("SystemMenu");	//グループ名
	const  string  defName("MessageWindow");	//タスク名
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
		DG::Font::SP fontMsg;
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
		ML::Vec2 pos;
		ML::Vec2 staffPos;
		shared_ptr<ShopStaff::Object> shopStaff;

		ML::Vec2 messageStartPos;			//メッセージ開始位置
		OL::Limit<int> currentMessagePos;	//現在表示している文字
		int appearMessageDistance;			//メッセージ表示の間隔フレーム数
		KeysString::Strings msg;
		string displayStr;					//画面に表示する文字
		int appearMessageCount;				//メッセージ表示用カウント

		ML::Color messageColor;
		ML::Color outlineColor;
		//メッセージの設定
		void SetMessage(const string& fileName_);
		//void SetMessage(const string& msg_, int distance_ = 10);
		
	};
}