#pragma warning(disable:4996)
#pragma once
//?------------------------------------------------------
//タスク名:敵Boss
//作　成　者:22CI0333 長谷川勇一朗
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:2024/1/9
//概　　　要:敵ボス
//?------------------------------------------------------
#include "BEnemy.h"

namespace  EnemyBoss
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Enemy");	//グループ名
	const  string  defName("Boss");		//タスク名
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
		DG::Image::SP img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BEnemy
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
		enum BossMotion {
			Unnon = -1,	// 無効(使えません）
			Stand,		// N停止
			Teleport,	// テレポート
			CoolTime,	// A追尾
			Attack,
			Attack1,	// A攻撃1 ボスの杖から火の魔法を出す
			Attack2,	// A攻撃2 プレイヤーの足元に魔法を出し1秒後に攻撃をする
			Attack3,	// A攻撃3 ボスから半径３マス以内に範囲攻撃をする。
			Attack4,	// A攻撃4 ボスから半径5マス以内の敵が召喚可能なランダムな位置に敵を召喚する。
			Lose,		// 消滅中
		};

		void Think()override;			//思考＆状況判断
		void Move()override;			//モーションに対応した処理
		BEnemy::DrawInfo Anim()override;//アニメーション制御

		//接触時の応答処理(必ず受け身の処理として実装する)
		void Received(BEnemy* from_, AttackInfo at_);
	};
}