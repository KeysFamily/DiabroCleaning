#pragma once
//-------------------------------------------------------------------
// 0329土田オリジナルライブラリ
// 作　成　者:22CI0329　土田誠也
// 作成年月日:2023/07/05
// 概　　　要:処理を効率化させるための処理を組み込んだライブラリ
//------------------------------------------------------------------
#include "MyLib.h"
#include "nlohmann/json.hpp"

namespace OL
{
	//上限下限クラス
	template<class Type>
	struct Limit
	{
		Type vnow;		//現在の値
		Type vmin;		//最小値
		Type vmax;		//最大値
		/// <summary>
		/// 値を一括設定
		/// </summary>
		/// <param name="now_">現在の値</param>
		/// <param name="min_">最小値</param>
		/// <param name="max_">最大値</param>
		void SetValues(Type now_, Type min_, Type max_) {
			this->vnow = now_;
			this->vmin = min_;
			this->vmax = max_;
		}

		/// <summary>
		/// 現在の値が最小か
		/// </summary>
		/// <returns>最小ならtrue</returns>
		bool IsMin() const {
			return this->vnow <= this->vmin;
		}

		/// <summary>
		/// 現在の値が最大か
		/// </summary>
		/// <returns>最大ならtrue</returns>
		bool IsMax() const {
			return this->vnow >= this->vmax;
		}

		/// <summary>
		/// 現在の割合を取得
		/// </summary>
		/// <returns>現在の割合</returns>
		float GetRate() const{
			return (float)(this->vnow - this->vmin) / (float)(this->vmax - this->vmin);
		}

		/// <summary>
		/// 値の追加（制限付き）
		/// </summary>
		/// <param name="adds_">追加する値</param>
		void Addval(Type adds_) {
			this->vnow = this->vnow + adds_;
			if (this->vnow <= this->vmin) {
				this->vnow = this->vmin;
			}
			else if (this->vnow >= this->vmax) {
				this->vnow = this->vmax;
			}
		}

		/// <summary>
		/// 値の設定（制限付き）
		/// </summary>
		/// <param name="sets_">設定する値</param>
		void Setval(Type sets_) {
			this->vnow = sets_;
			if (this->vnow <= this->vmin) {
				this->vnow = this->vmin;
			}
			else if (this->vnow >= this->vmax) {
				this->vnow = this->vmax;
			}
		}
	};

	//サイズ構造体
	struct Size2D {
		int w;
		int h;

		Size2D();
		Size2D(int w_, int h_);
		void Set(int w_, int h_);

	};
	//演算子定義
	Size2D operator*(const Size2D& sizeL, float valueR_);
	
	//アニメーション構造体
	struct Animation {
		Animation(const Size2D& size_, int animDistance_);
		Animation(const std::string& filePath_);
		using SP = std::shared_ptr<Animation>;
		using WP = std::weak_ptr<Animation>;
		/// <summary>
		/// アニメーションの生成
		/// </summary>
		/// <param name="size_">画像サイズ</param>
		/// <param name="animDistance_">アニメーションの速さ（値が小さいほど早い）</param>
		/// <returns></returns>
		static Animation::SP Create(const Size2D& size_, int animDistance_);
		
		/// <summary>
		/// アニメーションの生成
		/// </summary>
		/// <param name="filePath_">ファイルパス</param>
		static Animation::SP Create(const std::string& filePath_);

		Size2D  imgSize;			//画像サイズ
		vector<ML::Point>	imgPos;	//画像の位置
		int		animDistance;		//アニメーションの速さ（何フレームごとに切り替わるか）
		/// <summary>
		/// //アニメーション画像の追加（数値は画像単位。ピクセルではない）
		/// </summary>
		/// <param name="pos_">左上から数えた画像番号</param>
		void AddPos(const ML::Point& pos_);

		/// <summary>
		/// //アニメーション画像の追加
		/// </summary>
		/// <param name="x_">左から数えた画像番号X</param>
		/// <param name="y_">上から数えた画像番号Y</param>
		void AddPos(int x_, int y_);


		/// <summary>
		/// 引数で受け取ったフレーム数に対応したアニメーション画像を返す
		/// </summary>
		/// <param name="frames_">経過フレーム数</param>
		/// <returns>フレーム数に対応したアニメーション画像</returns>
		ML::Vec2 GetPos(int frames_) const;	

		/// <summary>
		/// 引数で受け取ったフレーム数に対応したアニメーション画像矩形を返す
		/// </summary>
		/// <param name="frames_">経過フレーム数</param>
		/// <returns>フレーム数に対応したアニメーション画像矩形</returns>
		ML::Box2D GetSrcBox(int frames_) const;


		/// <summary>
		/// 描画用矩形をもらう
		/// </summary>
		/// <returns>描画用矩形（原点調整済み）</returns>
		ML::Box2D GetDrawBox() const;

		/// <summary>
		/// サイズの設定
		/// </summary>
		/// <param name="size_">サイズ</param>
		void SetSize(const Size2D& size_);

		/// <summary>
		/// アニメーションの速さの設定
		/// </summary>
		/// <param name="distance_">画像が切り替わるまでのフレーム数</param>
		void SetDistance(int distance_);

		/// <summary>
		/// アニメーションの画像数を受け取る
		/// </summary>
		int GetAnimLength() const;			//アニメーションの画像数を受け取る

		/// <summary>
		/// アニメーションが一巡するまでのフレーム数を受け取る
		/// </summary>
		int GetAnimCountMax() const;		//アニメーションが一巡するまでのフレーム数を受け取る
	};

	//ステータス構造体
	struct CharaStatus {
		Limit<int> hp;		//体力
		int	atk;			//攻撃力

		//コンストラクタ
		CharaStatus()
			:hp()
			, atk(0)
		{
			this->hp.SetValues(0, 0, 0);
		}
	};

	//シーン構造体
	struct SceneAction
	{
		virtual void Start() {};
		virtual void UpDate() {};
		virtual bool ChangeActionFlag() = 0;
		virtual SceneAction* EndAndCreateNext() = 0;
		virtual ~SceneAction() {};
	};
	//シーン構造体管理
	struct SceneActionManager
	{
	private:
		SceneAction* playingAction;
	public:
		//コンストラクタ
		SceneActionManager(SceneAction* initAction_);
		//生成
		static SceneActionManager* Create(SceneAction* initAction_);
		//シーンの更新処理
		void UpDateScene();
		//デストラクタ
		~SceneActionManager();
	};

	//ボックスを中心基準で座標移動できるようにする
	ML::Box2D setBoxCenter(int w_, int h_);
	ML::Box2D setBoxCenter(Size2D size_);

	//ボックスを矩形単位に変換
	ML::Rect BoxToRect(const ML::Box2D& box_);

	//ボックスの中心座標を求める
	ML::Vec2 BoxCenterPos(const ML::Box2D& box_);

	//ベクトルを回転させる
	ML::Vec2 RotateVec(const ML::Vec2& vec_, float angleRad_);

	//JSONファイルを読み込む
	nlohmann::json LoadJsonFile(const string& filePath_);
	//JSONファイルを保存する
	void SaveJsonFile(const json& js_, const string& filePath_);

}