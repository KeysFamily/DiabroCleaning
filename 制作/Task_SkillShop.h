#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:スキルショップ
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "SelectableObject.h"

namespace Price
{
	class Object;
}

namespace  SkillShop
{
	struct ShopData
	{
		int skillSrcOfs;		//使用するスキル画像の番号
		string skillName;		//スキル名
		string staffTalkFile;	//店員のセリフファイル
		int price;				//価格
	};
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("SystemMenu");	//グループ名
	const  string  defName("SkillShop");	//タスク名
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
		DG::Image::SP imgSkill;		//スキル画像
		OL::Size2D imgSkillSize;	//スキル画像サイズ
		DG::Image::SP imgPriceBG;	//価格背景
		OL::Size2D imgPriceBGSize;	//価格背景サイズ

		DG::Font::SP fontSkill;		//スキル名フォント

		
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
		bool Buy(int& money_);
	public:
		//追加したい変数・メソッドはここに追加する
		ML::Vec2 pos;			//座標
		ML::Vec2 skillImgPos;	//スキル画像の位置の位置
		ML::Vec2 skillNamePos;	//スキル名の位置
		ML::Vec2 pricePos;		//価格の位置

		shared_ptr<Price::Object> objPrice;	//価格オブジェクト

		ShopData shopData;

		//状態
		enum State
		{
			NON = -1,			//無効
			SALE,				//販売中
			BOUGHT,				//購入済み
			SELECTING			//選択中
		};

		int currentState;		//現在の状態

		void SetShopData(const ShopData& shopData_);

		int selectCount;		//選択されているときのカウント

		//SelectableObjectのメソッド
		virtual ML::Box2D GetObjectSize() const;
		virtual void IsSelecting();
		virtual void FinishSelect();
		virtual void IsDown();
	};
}