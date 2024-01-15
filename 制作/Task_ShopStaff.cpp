//?------------------------------------------------------
//タスク名:ショップ店員
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_ShopStaff.h"

namespace  ShopStaff
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgStaff = DG::Image::Create("./data/image/Menu/ShopStaff/ShopStaff.png");
		this->imgStaffSize.Set(128, 128);
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/Idle.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/walk.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/run.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/jump.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/squat.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/magic.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/surprised.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/die.txt"));
		this->animStaff.push_back(OL::Animation::Create("./data/animation/ShopStaff/die2.txt"));
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgStaff.reset();
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
		this->render2D_Priority[1] = 0.4f;
		this->pos = ML::Vec2(0, 0);
		this->animCnt = 0;
		this->currentAnim = Motion::IDLE;
		this->loopAnim = true;
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
		++this->animCnt;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->loopAnim == true && this->animCnt > this->res->animStaff[this->currentAnim]->GetAnimCountMax())
		{
			this->animCnt = 0;
		}
		ML::Box2D draw = this->res->animStaff[this->currentAnim]->GetDrawBox();
		ML::Box2D src = this->res->animStaff[this->currentAnim]->GetSrcBox(this->animCnt);
		draw.Offset(this->pos);
		this->res->imgStaff->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	// その他の関数
	//アニメーション開始
	void Object::SetAnimation(Motion motion_, bool loop_)
	{
		this->currentAnim = motion_;
		this->animCnt = 0;
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