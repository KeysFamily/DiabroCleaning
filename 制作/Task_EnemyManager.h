#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:敵管理タスク
//作　成　者:22CI0333 長谷川勇一朗
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:2023/12/1
//概　　　要:敵を管理する
//?------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "BEnemy.h"

namespace  EnemyManager
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("EnemyManager");	//グループ名
	const  string  defName("Normal");	//タスク名

	struct EnemyData {	//敵情報
		float hp;
		float jumpPow;
		float maxSpeed;
		float addSpeed;
		float decSpeed;

		int unHitTime;
		int dropMoney;
		int attackPow;
	};

	struct EnemyStatusRate {//敵ステータス倍率
		float hpRate;		//体力倍率
		float speedRate;	//速度倍率
		float moneyRate;	//報酬倍率
		float attackRate;	//攻撃力倍率
	};
	//---------------------------------------------------------
	// 敵ステータス倍率について
	// Rateを管理する配列の中に敵ごとに管理する。
	// 
	//---------------------------------------------------------


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

		
		std::map<string, EnemyData> enemyDatas;
		std::vector<string> enemyNames;

		std::map<int, std::map<string, EnemyStatusRate>> stateRates;


		std::map<string, function<BEnemy::SP(bool)>> enemyInits;
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
		std::vector<BResource::SP> residentResource;

		void SpawnEnemyNum(int enemyNum_, ML::Vec2 pos_, int depth_ = 1);
		void SpawnEnemyName(string name_, ML::Vec2 pos_, int depth_ = 1);

		void KillAllEnemys();

	};
}