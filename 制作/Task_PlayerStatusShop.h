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
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace  PlayerStatusShop
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("SystemMenu");	//グループ名
	const  string  defName("PlayerStatusShop");	//タスク名
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
		DG::Image::SP imgProgress;	//購入用の画像
		OL::Size2D imgProgressSize;	//購入用の画像のサイズ
		DG::Font::SP fontDisplay;	//ステータス名表示用フォント
		OL::Size2D fontDisplaySize;	//ステータス名表示用フォントのサイズ
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask, public MyUI::SelectableObject
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
		string displayStr;		//ステータス名
		vector<int> price;		//各金額
		vector<int> addStatus;	//各追加量
		OL::Limit<int> currentStatus;	//どこまで購入されているか

		ML::Vec2 pos;				//全体の位置
		ML::Vec2 displayPos;		//ステータス名の位置
		ML::Vec2 progressBeginPos;	//購入用の画像の位置
		float progressDistance;		//購入用の画像の間隔

		float selectScale;		//選択時のサイズを画像より大きくしたいときに使う

		int statusType;				//ステータスの種類

		//次の購入に必要な金額を返す
		int GetPrice() const;
		//現在の追加ステータスを取得する
		int GetStatusAdd() const;
		//購入する
		bool Buy(int& money_);

		//サイズと位置を伝える
		virtual ML::Box2D GetObjectSize() const override;
		//ターゲット中か
		virtual void IsSelecting() override;
		//ボタンが押されたか
		virtual void IsDown() override;
	};
}