#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:
//作　成　者:
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "BItem.h"

namespace  Item
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("item");	//グループ名
	const  string  defName("item");		//タスク名
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
		DG::Image::SP ImgItem;
	};
	//-------------------------------------------------------------------
	class  Object : public  BItem
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

		enum Motion
		{
			Unnon = -1,	//	無効(使えません）
			Stand,		//	停止
			Fall,       //  落下
			Bound,		//	弾き飛ばされてる
			Lose,		//  消滅中
		};

		class item_buff  //格納するアイテムの効果
		{
		public:
			int Attack;     //攻撃力
			int Defense;    //防御力
			int Magic;      //魔法力
			int Speed;      //スピード
			int Time;       //持続時間
			int Item_pos;   //アイテムの種類
			int Power_step; //強さの段階
		};

		void InputJsonFile(string fileName_);
		void  Think();
		//モーションに対応した処理
		void  Move();
		//接触時の応答処理(必ず受け身の処理として実装する)
		void Received(BChara* from_, AttackInfo at_) override;
		//プレイヤーのステータスを変える
		void GiftPlayer(BChara* pl_) override;
		item_buff itemb;
		//デバック用
		XI::GamePad::SP controller;

		bool initialized;
		void ItemInit(ML::Vec2 pos_, int minLevel_, int maxLevel_);

		//画面外に出たらアイテムを消す処理
		void out_coin(int x, int y);
		
	};
}