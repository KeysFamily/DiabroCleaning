#pragma once
#include "GameEngine_Ver3_83.h"
#include "GameFileManager.h"
#include <fstream>
#include <sstream>

#define DEBUG_GAME
//#define DEBUG_ENEMY	//TODO: 敵デバッグ用(最後に消せ)


//ゲーム内全域で使用する構造体などを定義する
//-----------------------------------------------------------------------------------------------

#include "fpscounter.h"


#include "BChara.h"
#include "Task_Effect00.h"

#include "BEnemy.h"
#include "Task_Player.h"
#include "Task_Map.h"
#include "MapStruct.h"

#include "easing.h"




//-----------------------------------------------------------------------------------------------
namespace  MyPG
{
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//----------------------------------------------
	//カメラ基本形
	class Camera
	{
		Camera() { }
		Camera(
			const ML::Vec3& tg_,	//	被写体の位置
			const ML::Vec3& pos_,	//	カメラの位置
			const ML::Vec3& up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
			float				fov_,	//	視野角
			float				np_,	//	前クリップ平面（これより前は映らない）
			float				fp_,	//	後クリップ平面（これより後ろは映らない）
			float				asp_);	//	アスペクト比（画面の比率に合わせる　横÷縦）			
	public:
		//	ビュー情報（方向関連）
		ML::Vec3 target;			//	被写体の位置
		ML::Vec3 pos;			//	カメラの位置
		ML::Vec3 up;				//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
	//	射影情報（視野範囲関連）
		float fov;					//	視野角
		float nearPlane;			//	前クリップ平面（これより前は映らない）
		float forePlane;			//	後クリップ平面（これより後ろは映らない）
		float aspect;				//	アスペクト比（画面の比率に合わせる　横÷縦）
	//	行列情報
		ML::Mat4x4  matView, matProj;
		~Camera();
		using SP = shared_ptr<Camera>;
		//	カメラを生成する
		static SP Create(
			const ML::Vec3& tg_,	//	被写体の位置
			const ML::Vec3& pos_,	//	カメラの位置
			const ML::Vec3& up_,	//	カメラの上方向を示すベクトル（大体Ｙ＋固定）
			float				fov_,	//	視野角
			float				np_,	//	前クリップ平面（これより前は映らない）
			float				fp_,	//	後クリップ平面（これより後ろは映らない）
			float				asp_);	//	アスペクト比（画面の比率に合わせる　横÷縦）	
//	カメラの設定
		void UpDate();
	};
	//----------------------------------------------
	class MyGameEngine : public GameEngine
	{
	public:
		MyGameEngine();
		//ゲームエンジンに追加したものの初期化と開放
		bool Initialize(HWND wnd_);
		~MyGameEngine();
		//ゲームエンジンに追加したもののステップ処理
		void UpDate();

		//3DPG1対応により追加
			//2D描画環境のデフォルトパラメータ設定
		void Set2DRenderState(DWORD l_);
		//3D描画環境のデフォルトパラメータ設定
		void Set3DRenderState(DWORD l_);

		//ゲームエンジンに追加したいものは下に加える
		//----------------------------------------------
		MyPG::Camera::SP		camera[4];		//	カメラ

		XI::Mouse::SP  mouse;
		XI::GamePad::SP  in1, in2, in3, in4;	//取り合えず４本
		//２Ｄカメラ矩形
		ML::Box2D				camera2D;	//  ２Ｄスクロール制御用
		map<string, float> evFlags;

		GameFileManager filemg;		//ファイル管理クラス
		bool GameOverFlag = false; //ゲームオーバーフラグ
		bool GameClearFlag = false;//クリアフラグ
		bool unHitEnemy = false;//敵との当たり判定
		int GameCnt; //ゲームのプレイ時間
		int TotalEnemyKill; //プレイヤー敵を倒した総数
		int TotalDamage; //プレイヤーが敵に与えたダメージの総数
		int TotalUsedCoinCnt; //消費したコインの総数
		int TotalGetCoinCnt; //獲得したコインの総数

		int MaxGameOver;

		bool			Jump2Check;		//ジャンプ2判定
		bool			DestroyFlag; //破壊フラグ
		bool			AttackFlag; //攻撃フラグ

		bool			TransparentFlag; //透明フラグ
		//◇◇◇◇◇◇◇◇◇◇
		//以下22CI0333記述
		Map::Object::SP qa_Map;
		Player::Object::SP qa_Player;
		shared_ptr<vector<BEnemy::SP>> qa_Enemys;
		//◆◆◆◆◆◆◆◆◆◆
		//◇◇◇◇◇◇◇◇◇◇
		//以下22CI0329記述
		int debugPrintTimeCnt;

		ML::Box2D GetScreenBox() const;
		ML::Vec2  GetScreenCenter() const;
		//標準の描画処理
		static void DrawStd(const DG::Image::SP& img_, OL::Size2D size_, ML::Vec2 pos_);
		//デバッグ関数
		//param1:出力する文字列
		//param2:何フレームごとに出力するか

		void printToDebugFile(std::string debugstr_, int distance_);
		//◆◆◆◆◆◆◆◆◆◆


		//----------------------------------------------
		FPSCounter* c;

		int stage;

		Effect00::Object::WP CreateEffect(int no, ML::Vec2 pos);
		void Dbg_ToConsole(const char* str, ...);
		void Dbg_ToDisplay(int x, int y, const char* str, ...);
		void Dbg_ToTitle(const char* str, ...);
		void Dbg_ToBox(const char* str, ...);
		void Dbg_ToFileOut(const char* str, ...);

		//デバッグ矩形
		//ここから
		DG::Image::SP rectImage;

		enum DEBUGRECTMODE {
			RED, BLUE, GREEN, ORANGE,
			REDFRAME, BLUEFRAME, GREENFRAME, ORANGEFRAME,
		};
		struct DebugRectInfo {
			ML::Box2D	r;
			int			flag;
		};

		std::vector<DebugRectInfo> debugRectArray;
		//内部利用
		void debugRectLoad();
		void debugRectReset();
		void debugRectDraw();
		//表示したい矩形と表示モードを渡す
		//RED,BLUE,GREEN,ORANGEの矩形とFRAMEは枠のみ
		void debugRect(ML::Box2D r_, int flag = 4, int offsetx = 0, int offsety = 0);

		enum COUNTER_FLAGS {
			NONE,
			ACTIVE,//稼働中
			LIMIT,//終了直後
			END,//終了
		};

		struct COUNTER {
			COUNTER_FLAGS flag;
			int count;
		};
		std::map < std::string, COUNTER > counterMap;


		void InitCounter();
		void ActCounter();
		void AllEndCounter();
		COUNTER_FLAGS getCounterFlag(string str);
		void StartCounter(string str, int count);


	};

	//◇◇◇◇◇◇◇◇◇◇
	//以下22CI0329記述
	//オブジェクトへキャスト
	template<class Type>
	std::shared_ptr<Type> CastToObject(BTask::SP btask_)
	{
		return std::dynamic_pointer_cast<Type>(btask_);
	}
	//◆◆◆◆◆◆◆◆◆◆

}
extern MyPG::MyGameEngine* ge;


