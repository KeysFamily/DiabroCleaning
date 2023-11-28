//?------------------------------------------------------
//タスク名:ミニマップ
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:2023/11/25
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GameUI_MiniMap.h"
#include  "Task_MapManager.h"

namespace  MiniMap
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBG = DG::Image::Create("./data/map/image/UI/BackGround.png");
		this->imgBGSize.Set(256, 256);
		this->imgChip = DG::Image::Create("./data/map/image/UI/MapChip.png");
		this->imgChipSize.Set(32, 32);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBG.reset();
		this->imgChip.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->pos.x = ge->screenWidth - this->res->imgBGSize.w / 2;
		this->pos.y = this->res->imgBGSize.h / 2;
		this->cameraPos = ML::Vec2(0, 0);
		this->screenSize.Set(240, 216);
		this->screenOfs = ML::Vec2(0, 12);
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		int speed = 1;
		if (inp.RStick.BU.on)
			this->cameraPos.y -= speed;
		if (inp.RStick.BD.on)
			this->cameraPos.y += speed;
		if (inp.RStick.BR.on)
			this->cameraPos.x += speed;
		if (inp.RStick.BL.on)
			this->cameraPos.x -= speed;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//背景描画
		ML::Box2D draw = OL::setBoxCenter(this->res->imgBGSize);
		draw.Offset(this->pos);
		ML::Box2D src(0, 0, this->res->imgBGSize.w, this->res->imgBGSize.h);
		this->res->imgBG->Draw(draw, src);

		//マップチップ描画
		ML::Box2D screen = OL::setBoxCenter(this->screenSize);
		screen.Offset(this->pos + this->screenOfs);
		for (int y = 0; y < 30; ++y) {
			for (int x = 0; x < 30; ++x) {
				draw = ML::Box2D(0, 0, this->res->imgChipSize.w, this->res->imgChipSize.h);
				src = draw;
				draw.x += screen.x + x * this->res->imgChipSize.w;
				draw.y += screen.y + y * this->res->imgChipSize.h;
				draw.Offset(-cameraPos);
				this->SetChip(src, x, y);
				this->SetToScreen(draw, src, screen);

				this->res->imgChip->Draw(draw, src);
			}
		}
	}
	//-------------------------------------------------------------------
	//関数定義
	void Object::SetChip(ML::Box2D& src_, int x_, int y_)
	{
		auto mapMng = ge->GetTask<MapManager::Object>("MapManager");
		if (!mapMng) 
		{
			return;
		}
		if (x_ == 0 && y_ == 0)
		{
			src_.x = this->res->imgChipSize.w * 3;
			src_.y = this->res->imgChipSize.h * 2;
		}

		MapManager::Object::Map* map = dynamic_cast<MapManager::Object::Map*>(mapMng->map[y_][x_]);
		if (map)
		{
			src_.x = (int)map->GetExit() * this->res->imgChipSize.w;
			src_.y = this->res->imgChipSize.h * 2
				+ (int)map->GetEnter() * this->res->imgChipSize.h;
		}
		MapManager::Object::Connect* connect = dynamic_cast<MapManager::Object::Connect*>(mapMng->map[y_][x_]);
		if (connect)
		{
			src_.x = ((int)connect->GetExit() + (int)connect->GetExitSub()) * this->res->imgChipSize.w;
			src_.y = (int)connect->GetEnter() * this->res->imgChipSize.h;
		}
	}

	void Object::SetToScreen(ML::Box2D& drawBox_, ML::Box2D& srcBox_, const ML::Box2D& screen_)
	{
		//スクリーン
		ML::Rect s = {
			screen_.x,
			screen_.y,
			screen_.x + screen_.w,
			screen_.y + screen_.h
		};
		//描画
		ML::Rect d = {
			drawBox_.x,
			drawBox_.y,
			drawBox_.x + drawBox_.w,
			drawBox_.y + drawBox_.h
		};

		if (screen_.Hit(drawBox_)) {
			//画像がスクリーンからはみ出ている場合、
			//出ている部分は描画させない処理
			//左
			if (s.left > d.left) {
				drawBox_.x += s.left - d.left;
				drawBox_.w -= s.left - d.left;
				srcBox_.x += s.left - d.left;	//はみ出している部分を元のデータの範囲に合わせてから計算
				srcBox_.w -= s.left - d.left;
			}
			//上
			if (s.top > d.top) {
				drawBox_.y += s.top - d.top;
				drawBox_.h -= s.top - d.top;
				srcBox_.y += s.top - d.top;
				srcBox_.h -= s.top - d.top;
			}
			//右
			if (s.right < d.right) {
				drawBox_.w -= d.right - s.right;
				srcBox_.w -= d.right - s.right;
			}
			//下
			if (s.bottom < d.bottom) {
				drawBox_.h -= d.bottom - s.bottom;
				srcBox_.h -= d.bottom - s.bottom;
			}
		}
		//完全にスクリーン外に出ていたらサイズを0に
		else {
			drawBox_.w = 0;
			drawBox_.h = 0;
		}
	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}