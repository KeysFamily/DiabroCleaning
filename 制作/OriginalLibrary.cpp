#include "OriginalLibrary.h"

using json = nlohmann::json;
//-------------------------------------------------------------------
// 0329土田オリジナルライブラリ
// 作　成　者:22CI0329　土田誠也
// 作成年月日:2023/07/05
// 概　　　要:処理を効率化させるための処理を組み込んだライブラリ
//------------------------------------------------------------------
namespace OL
{
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
	//サイズ構造体
	//デフォルトコンストラクタ
	Size2D::Size2D() :
		w(0),
		h(0)
	{}
	//コンストラクタ
	Size2D::Size2D(int w_, int h_) :
		w(w_),
		h(h_)
	{}
	//サイズ設定
	void Size2D::Set(int w_, int h_) {
		this->w = w_;
		this->h = h_;
	}
	//演算子定義
	Size2D operator*(const Size2D& sizeL_, float valueR_) {
		Size2D result;
		result.w = (int)(sizeL_.w * valueR_);
		result.h = (int)(sizeL_.h * valueR_);
		return result;
	}
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
	//アニメーション構造体
	//コンストラクタ
	Animation::Animation(const Size2D& size_, int animDistance_)
		:imgSize(size_)
		, imgPos()
		, animDistance(animDistance_)
	{}
	//コンストラクタ(ファイルからの読み込み)
	Animation::Animation(const std::string& filePath_)
		:imgSize()
		,imgPos()
		,animDistance(0)
	{
		ifstream ifs(filePath_);

		//サイズ
		if (!ifs) 
		{
			return;
		}
		ifs >> this->imgSize.w;
		if (!ifs)
		{
			return;
		}
		ifs >> this->imgSize.h;
		//アニメーションの間隔
		if (!ifs)
		{
			return;
		}
		ifs >> this->animDistance;

		//アニメーションで使用する位置の追加
		while (ifs)
		{
			ML::Point pos;
			ifs >> pos.x;

			//読み込めなくなったら終了
			if (!ifs)
			{
				return;
			}
			ifs >> pos.y;

			this->imgPos.push_back(pos);
		}

		ifs.close();
	}
	//生成
	Animation::SP Animation::Create(const Size2D& size_, int animDistance_)
	{
		return shared_ptr<Animation>(new Animation(size_, animDistance_));
	}
	Animation::SP Animation::Create(const string& filePath_)
	{
		return shared_ptr<Animation>(new Animation(filePath_));
	}
	//引数で受け取ったフレーム数に対応したアニメーション画像を返す
	ML::Vec2 Animation::GetPos(int frames_) const
	{
		int animCnt = frames_ % (this->animDistance * this->imgPos.size());
		ML::Point imgPosNow = this->imgPos.at(animCnt / this->animDistance);
		ML::Vec2 srcPos(imgSize.w * imgPosNow.x, imgSize.h * imgPosNow.y);
		return srcPos;
	}
	//引数で受け取ったフレーム数に対応したアニメーション画像を返す
	ML::Box2D Animation::GetSrcBox(int frames_) const
	{
		int animCnt = frames_ % (this->animDistance * this->imgPos.size());
		ML::Point imgPosNow = this->imgPos.at(animCnt / this->animDistance);
		return ML::Box2D(imgSize.w * imgPosNow.x, imgSize.h * imgPosNow.y, imgSize.w, imgSize.h);
	}
	//描画用矩形をもらう
	ML::Box2D Animation::GetDrawBox() const
	{
		return setBoxCenter(this->imgSize);
	}

	//位置追加
	void Animation::AddPos(const ML::Point& pos_)
	{
		this->imgPos.push_back(pos_);
	}
	void Animation::AddPos(int x_, int y_)
	{
		ML::Point pos;
		pos.x = x_;
		pos.y = y_;
		this->imgPos.push_back(pos);
	}

	//画像サイズ設定
	void Animation::SetSize(const Size2D& size_)
	{
		this->imgSize.Set(size_.w, size_.h);
	}
	//画像サイズ設定
	void Animation::SetDistance(int distance_)
	{
		this->animDistance = distance_;
	}
	//アニメーションの画像数を受け取る
	int Animation::GetAnimLength()const
	{
		return (int)this->imgPos.size();
	}
	//アニメーションが一巡するまでのフレーム数を
	int Animation::GetAnimCountMax() const
	{
		return GetAnimLength() * animDistance;
	}
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
	//シーン構造体管理構造体
	//コンストラクタ
	SceneActionManager::SceneActionManager(SceneAction* initAction_)
		:playingAction(initAction_)
	{
		playingAction->Start();
	}
	//生成
	SceneActionManager* SceneActionManager::Create(SceneAction* initAction_)
	{
		return new SceneActionManager(initAction_);
	}
	//シーンの更新処理
	void SceneActionManager::UpDateScene()
	{
		playingAction->UpDate();
		if (playingAction->ChangeActionFlag() == true)
		{
			auto next = playingAction->EndAndCreateNext();
			next->Start();
			delete playingAction;
			playingAction = next;
		}
	}
	//デストラクタ
	SceneActionManager::~SceneActionManager()
	{
		delete this->playingAction;
	}
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
	//それ以外の関数
	//ボックスを中心単位で座標移動できるようにする
	ML::Box2D setBoxCenter(int w_, int h_)
	{
		return ML::Box2D(w_ / -2, h_ / -2, w_, h_);
	}
	
	ML::Box2D setBoxCenter(Size2D size_)
	{
		return ML::Box2D(size_.w / -2, size_.h / -2, size_.w, size_.h);
	}
	
	//ボックスを矩形単位に変換
	ML::Rect BoxToRect(const ML::Box2D& box_)
	{
		ML::Rect result = {
			box_.x,
			box_.y,
			box_.x + box_.w,
			box_.y + box_.h
		};
		return result;
	}

	//ボックスの中心座標を求める
	ML::Vec2 BoxCenterPos(const ML::Box2D& box_)
	{
		return ML::Vec2(box_.w / 2, box_.h / 2);
	}

	//ベクトルを回転させる
	ML::Vec2 RotateVec(const ML::Vec2& vec_, float angleRad_)
	{
		ML::Vec2 result;
		float length = vec_.Length();
		float vecAngleRad = atan2(vec_.y, vec_.x);
		result.x = cos(vecAngleRad + angleRad_) * length;
		result.y = sin(vecAngleRad + angleRad_) * length;
		return result;
	}

	//jsonファイルを読み込む
	json LoadJsonFile(const string& filePath_)
	{
		json js;
		std::ifstream fin(filePath_);
		if (!fin)
		{
			return js;
		}
		//JSONファイル読み込み
		fin >> js;
		//ファイル読み込み終了
		fin.close();

		return js;
	}
	//jsonファイルを読み込む
	void SaveJsonFile(const json& js_, const string& filePath_)
	{
		ofstream ofs(filePath_);
		ofs << js_.dump(4);
	}
}