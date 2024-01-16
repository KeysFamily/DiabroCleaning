#include "OriginalLibrary.h"

using json = nlohmann::json;
//-------------------------------------------------------------------
// 0329�y�c�I���W�i�����C�u����
// ��@���@��:22CI0329�@�y�c����
// �쐬�N����:2023/07/05
// �T�@�@�@�v:�����������������邽�߂̏�����g�ݍ��񂾃��C�u����
//------------------------------------------------------------------
namespace OL
{
	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	//�T�C�Y�\����
	//�f�t�H���g�R���X�g���N�^
	Size2D::Size2D() :
		w(0),
		h(0)
	{}
	//�R���X�g���N�^
	Size2D::Size2D(int w_, int h_) :
		w(w_),
		h(h_)
	{}
	//�T�C�Y�ݒ�
	void Size2D::Set(int w_, int h_) {
		this->w = w_;
		this->h = h_;
	}
	//���Z�q��`
	Size2D operator*(const Size2D& sizeL_, float valueR_) {
		Size2D result;
		result.w = (int)(sizeL_.w * valueR_);
		result.h = (int)(sizeL_.h * valueR_);
		return result;
	}
	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	//�A�j���[�V�����\����
	//�R���X�g���N�^
	Animation::Animation(const Size2D& size_, int animDistance_)
		:imgSize(size_)
		, imgPos()
		, animDistance(animDistance_)
	{}
	//�R���X�g���N�^(�t�@�C������̓ǂݍ���)
	Animation::Animation(const std::string& filePath_)
		:imgSize()
		,imgPos()
		,animDistance(0)
	{
		ifstream ifs(filePath_);

		//�T�C�Y
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
		//�A�j���[�V�����̊Ԋu
		if (!ifs)
		{
			return;
		}
		ifs >> this->animDistance;

		//�A�j���[�V�����Ŏg�p����ʒu�̒ǉ�
		while (ifs)
		{
			ML::Point pos;
			ifs >> pos.x;

			//�ǂݍ��߂Ȃ��Ȃ�����I��
			if (!ifs)
			{
				return;
			}
			ifs >> pos.y;

			this->imgPos.push_back(pos);
		}

		ifs.close();
	}
	//����
	Animation::SP Animation::Create(const Size2D& size_, int animDistance_)
	{
		return shared_ptr<Animation>(new Animation(size_, animDistance_));
	}
	Animation::SP Animation::Create(const string& filePath_)
	{
		return shared_ptr<Animation>(new Animation(filePath_));
	}
	//�����Ŏ󂯎�����t���[�����ɑΉ������A�j���[�V�����摜��Ԃ�
	ML::Vec2 Animation::GetPos(int frames_) const
	{
		int animCnt = frames_ % (this->animDistance * this->imgPos.size());
		ML::Point imgPosNow = this->imgPos.at(animCnt / this->animDistance);
		ML::Vec2 srcPos(imgSize.w * imgPosNow.x, imgSize.h * imgPosNow.y);
		return srcPos;
	}
	//�����Ŏ󂯎�����t���[�����ɑΉ������A�j���[�V�����摜��Ԃ�
	ML::Box2D Animation::GetSrcBox(int frames_) const
	{
		int animCnt = frames_ % (this->animDistance * this->imgPos.size());
		ML::Point imgPosNow = this->imgPos.at(animCnt / this->animDistance);
		return ML::Box2D(imgSize.w * imgPosNow.x, imgSize.h * imgPosNow.y, imgSize.w, imgSize.h);
	}
	//�`��p��`�����炤
	ML::Box2D Animation::GetDrawBox() const
	{
		return setBoxCenter(this->imgSize);
	}

	//�ʒu�ǉ�
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

	//�摜�T�C�Y�ݒ�
	void Animation::SetSize(const Size2D& size_)
	{
		this->imgSize.Set(size_.w, size_.h);
	}
	//�摜�T�C�Y�ݒ�
	void Animation::SetDistance(int distance_)
	{
		this->animDistance = distance_;
	}
	//�A�j���[�V�����̉摜�����󂯎��
	int Animation::GetAnimLength()const
	{
		return (int)this->imgPos.size();
	}
	//�A�j���[�V�������ꏄ����܂ł̃t���[������
	int Animation::GetAnimCountMax() const
	{
		return GetAnimLength() * animDistance;
	}
	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	//�V�[���\���̊Ǘ��\����
	//�R���X�g���N�^
	SceneActionManager::SceneActionManager(SceneAction* initAction_)
		:playingAction(initAction_)
	{
		playingAction->Start();
	}
	//����
	SceneActionManager* SceneActionManager::Create(SceneAction* initAction_)
	{
		return new SceneActionManager(initAction_);
	}
	//�V�[���̍X�V����
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
	//�f�X�g���N�^
	SceneActionManager::~SceneActionManager()
	{
		delete this->playingAction;
	}
	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	//����ȊO�̊֐�
	//�{�b�N�X�𒆐S�P�ʂō��W�ړ��ł���悤�ɂ���
	ML::Box2D setBoxCenter(int w_, int h_)
	{
		return ML::Box2D(w_ / -2, h_ / -2, w_, h_);
	}
	
	ML::Box2D setBoxCenter(Size2D size_)
	{
		return ML::Box2D(size_.w / -2, size_.h / -2, size_.w, size_.h);
	}
	
	//�{�b�N�X����`�P�ʂɕϊ�
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

	//�{�b�N�X�̒��S���W�����߂�
	ML::Vec2 BoxCenterPos(const ML::Box2D& box_)
	{
		return ML::Vec2(box_.w / 2, box_.h / 2);
	}

	//�x�N�g������]������
	ML::Vec2 RotateVec(const ML::Vec2& vec_, float angleRad_)
	{
		ML::Vec2 result;
		float length = vec_.Length();
		float vecAngleRad = atan2(vec_.y, vec_.x);
		result.x = cos(vecAngleRad + angleRad_) * length;
		result.y = sin(vecAngleRad + angleRad_) * length;
		return result;
	}

	//json�t�@�C����ǂݍ���
	json LoadJsonFile(const string& filePath_)
	{
		json js;
		std::ifstream fin(filePath_);
		if (!fin)
		{
			return js;
		}
		//JSON�t�@�C���ǂݍ���
		fin >> js;
		//�t�@�C���ǂݍ��ݏI��
		fin.close();

		return js;
	}
	//json�t�@�C����ǂݍ���
	void SaveJsonFile(const json& js_, const string& filePath_)
	{
		ofstream ofs(filePath_);
		ofs << js_.dump(4);
	}
}