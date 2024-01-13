//?------------------------------------------------------
//タスク名:スキルショップ
//作　成　者:土田誠也
//TODO:もしいれば下記へ記述
//編　集　者:
//作成年月日:
//概　　　要:
//?------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SkillShop.h"
#include  "Task_Price.h"
#include  "Task_SystemMenuMessageWindow.h"

namespace  SkillShop
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgSkill = DG::Image::Create("./data/effect/magicSelect.png");
		this->imgSkillSize.Set(64, 64);
		this->imgPriceBG = DG::Image::Create("./data/image/Menu/Skill/priceBG.png");
		this->imgPriceBGSize.Set(232, 80);
		this->fontSkill = DG::Font::Create("ＭＳ ゴシック", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgSkill.reset();
		this->fontSkill.reset();
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
		this->render2D_Priority[1] = 0.5f;
		this->pos = ML::Vec2(0, 0);
		this->skillImgPos = ML::Vec2(-300, 0);
		this->skillNamePos = ML::Vec2(-250, -30);
		this->pricePos = ML::Vec2(200, 0);
		this->objPrice = Price::Object::Create(true);
		this->objPrice->pos = this->pos + this->pricePos;

		this->shopData.price = 0;
		this->shopData.skillSrcOfs = 0;
		this->shopData.skillName = "";
		this->shopData.staffTalkFile = "";

		this->currentState = State::SALE;

		this->selectCount = 0;
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
		this->objPrice->pos = this->pos + this->pricePos;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//スキル名の描画
		ML::Box2D draw(0, 0, 1000, 1000);
		draw.Offset(this->pos + this->skillNamePos);
		this->res->fontSkill->Draw(draw, this->shopData.skillName);


		//スキル画像の描画
		draw = OL::setBoxCenter(this->res->imgSkillSize);
		ML::Box2D src(0, 0, this->res->imgSkillSize.w, this->res->imgSkillSize.h);
		draw.Offset(this->pos + this->skillImgPos);
		src.y += this->res->imgSkillSize.h * this->shopData.skillSrcOfs;
		this->res->imgSkill->Draw(draw, src);

		//価格の背景表示
		draw = OL::setBoxCenter(this->res->imgPriceBGSize);
		src = ML::Box2D(0, 0, this->res->imgPriceBGSize.w, this->res->imgPriceBGSize.h);
		draw.Offset(this->pos + this->pricePos);
		src.y += this->res->imgPriceBGSize.h * this->currentState;
		this->res->imgPriceBG->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//その他の関数
	void Object::SetShopData(const ShopData& shopData_)
	{
		this->shopData = shopData_;
		this->objPrice->SetPrice(this->shopData.price);
	}
	//SelectableObjectのメソッド
	ML::Box2D Object::GetObjectSize() const
	{
		ML::Box2D box = OL::setBoxCenter(this->res->imgPriceBGSize);
		box.Offset(this->pos + this->pricePos);
		return box;
	}
	void Object::IsSelecting()
	{
		if (this->selectCount == 0)
		{
			auto msg = ge->GetTask<SystemMenuMessageWindow::Object>("SystemMenu", "MessageWindow");
			msg->SetMessage(this->shopData.staffTalkFile);
		}
		++this->selectCount;
	}
	void Object::FinishSelect()
	{
		this->selectCount = 0;
	}
	void Object::IsDown()
	{

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