#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:火球
//作　成　者:22CI0306 王 功 健
//TODO:もしいれば下記へ記述
//編　集　者:22CI0333 長谷川 勇一朗
//作成年月日:
//概　　　要:火球(ボス攻撃)
//?------------------------------------------------------
#include "BChara.h"

namespace  EnemyFireBall
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Magic");	//グループ名
	const  string  defName("FireBall");		//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()		override;
		bool  Finalize()		override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
		//共有する変数はここに追加する
		DG::Image::SP	img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BChara
	{
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
		void  UpDate()		override;	//「実行」１フレーム毎に行う処理
		void  Render2D_AF()	override;	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
		//追加したい変数・メソッドはここに追加する
		//BCharaに含まれないモノのみここに追加する
		float power;	//ダメージ

		//アニメーション制御
		void Setting(ML::Vec2 setPos_, float speed_, float rad_, float AttackPow_);

	private:
		BChara::DrawInfo  Anim();
	};
}