//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_PlayerScore.h"

namespace  PlayerScore
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgResultScroll = DG::Image::Create("./data/ending/羊皮紙.png");
		this->fontData = DG::Font::Create("HGP創英角ﾎﾟｯﾌﾟ体", 15, 30);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgResultScroll.reset();
		this->fontData.reset();
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
		
		this->DrawCnt = 0;

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
		this->DrawCnt++;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(120,500,600,600);

		ML::Box2D drawScoroll(50, 450, 500, 500);
		ML::Box2D srcScoroll(0, 0, 1282, 962);

		ostringstream oss;
		
		oss << "　　　　　　　戦績　　　　　　" << endl << endl;
		oss << "クリアタイム[s]：" << ge->GameCnt << endl << endl;						//Player->surviveTime
		oss << "敵を倒した数：" << ge->TotalEnemyKill << endl<<endl;							//Enemy側
		oss << "敵に与えたダメージ：" << ge->TotalDamage << endl << endl;						//Enemy側
		oss << "獲得したコインの枚数：" << ge->TotalGetCoinCnt << endl<<endl;					//Item_coin側
		oss << "消費したコインの枚数：" << ge->TotalUsedCoinCnt << endl;						//shop側
		//oss << "クリアタイム[s]：" << 111111111 << endl << endl;
		//oss << "クリアタイム[s]：" << 111111111 << endl << endl;
		//oss << "クリアタイム[s]：" << 111111111 << endl << endl;
		//oss << "クリアタイム[s]：" << 111111111 << endl << endl;
		//oss << "クリアタイム[s]：" << 111111111 << endl;
		this->res->imgResultScroll->Draw(drawScoroll, srcScoroll);
		this->res->fontData->Draw(draw, oss.str(),ML::Color(1,0,0,0));		

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