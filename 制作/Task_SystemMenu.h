#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:メニュー画面管理
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace PlayerStatus
{
	class Object;
}
namespace SystemMenuMessageWindow
{
	class Object;
}
namespace SkillSelect
{
	class Object;
}
namespace SystemMenuMoneyDisplay
{
	class Object;
}
namespace SystemMenuBackButton
{
	class Object;
}


namespace  SystemMenu
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("SystemMenu");	//グループ名
	const  string  defName("System");	//タスク名
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
		void SetPos();
		void AppearUpDate();
		void DisappearUpdate();

	public:
		//追加したい変数・メソッドはここに追加する
		ML::Vec2 pos;
		ML::Vec2 skillPos;
		ML::Vec2 statusPos;
		ML::Vec2 backPos;
		ML::Vec2 msgPos;
		ML::Vec2 moneyPos;

		shared_ptr<PlayerStatus::Object> status;
		shared_ptr<SystemMenuMessageWindow::Object> message;
		shared_ptr<SkillSelect::Object> skill;
		shared_ptr<SystemMenuMoneyDisplay::Object> moneyDp;
		shared_ptr<SystemMenuBackButton::Object> back;

		bool finishedAppear;	//出現が終了したか
		bool finishFlag;		//終了するか

		//終了判定
		void FinishMenu();
	};
}