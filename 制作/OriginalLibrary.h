#pragma once
//-------------------------------------------------------------------
// 0329�y�c�I���W�i�����C�u����
// ��@���@��:22CI0329�@�y�c����
// �쐬�N����:2023/07/05
// �T�@�@�@�v:�����������������邽�߂̏�����g�ݍ��񂾃��C�u����
//------------------------------------------------------------------
#include "MyLib.h"
#include "nlohmann/json.hpp"

namespace OL
{
	//��������N���X
	template<class Type>
	struct Limit
	{
		Type vnow;		//���݂̒l
		Type vmin;		//�ŏ��l
		Type vmax;		//�ő�l
		/// <summary>
		/// �l���ꊇ�ݒ�
		/// </summary>
		/// <param name="now_">���݂̒l</param>
		/// <param name="min_">�ŏ��l</param>
		/// <param name="max_">�ő�l</param>
		void SetValues(Type now_, Type min_, Type max_) {
			this->vnow = now_;
			this->vmin = min_;
			this->vmax = max_;
		}

		/// <summary>
		/// ���݂̒l���ŏ���
		/// </summary>
		/// <returns>�ŏ��Ȃ�true</returns>
		bool IsMin() const {
			return this->vnow <= this->vmin;
		}

		/// <summary>
		/// ���݂̒l���ő傩
		/// </summary>
		/// <returns>�ő�Ȃ�true</returns>
		bool IsMax() const {
			return this->vnow >= this->vmax;
		}

		/// <summary>
		/// ���݂̊������擾
		/// </summary>
		/// <returns>���݂̊���</returns>
		float GetRate() const{
			return (float)(this->vnow - this->vmin) / (float)(this->vmax - this->vmin);
		}

		/// <summary>
		/// �l�̒ǉ��i�����t���j
		/// </summary>
		/// <param name="adds_">�ǉ�����l</param>
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
		/// �l�̐ݒ�i�����t���j
		/// </summary>
		/// <param name="sets_">�ݒ肷��l</param>
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

	//�T�C�Y�\����
	struct Size2D {
		int w;
		int h;

		Size2D();
		Size2D(int w_, int h_);
		void Set(int w_, int h_);

	};
	//���Z�q��`
	Size2D operator*(const Size2D& sizeL, float valueR_);
	
	//�A�j���[�V�����\����
	struct Animation {
		Animation(const Size2D& size_, int animDistance_);
		Animation(const std::string& filePath_);
		using SP = std::shared_ptr<Animation>;
		using WP = std::weak_ptr<Animation>;
		/// <summary>
		/// �A�j���[�V�����̐���
		/// </summary>
		/// <param name="size_">�摜�T�C�Y</param>
		/// <param name="animDistance_">�A�j���[�V�����̑����i�l���������قǑ����j</param>
		/// <returns></returns>
		static Animation::SP Create(const Size2D& size_, int animDistance_);
		
		/// <summary>
		/// �A�j���[�V�����̐���
		/// </summary>
		/// <param name="filePath_">�t�@�C���p�X</param>
		static Animation::SP Create(const std::string& filePath_);

		Size2D  imgSize;			//�摜�T�C�Y
		vector<ML::Point>	imgPos;	//�摜�̈ʒu
		int		animDistance;		//�A�j���[�V�����̑����i���t���[�����Ƃɐ؂�ւ�邩�j
		/// <summary>
		/// //�A�j���[�V�����摜�̒ǉ��i���l�͉摜�P�ʁB�s�N�Z���ł͂Ȃ��j
		/// </summary>
		/// <param name="pos_">���ォ�琔�����摜�ԍ�</param>
		void AddPos(const ML::Point& pos_);

		/// <summary>
		/// //�A�j���[�V�����摜�̒ǉ�
		/// </summary>
		/// <param name="x_">�����琔�����摜�ԍ�X</param>
		/// <param name="y_">�ォ�琔�����摜�ԍ�Y</param>
		void AddPos(int x_, int y_);


		/// <summary>
		/// �����Ŏ󂯎�����t���[�����ɑΉ������A�j���[�V�����摜��Ԃ�
		/// </summary>
		/// <param name="frames_">�o�߃t���[����</param>
		/// <returns>�t���[�����ɑΉ������A�j���[�V�����摜</returns>
		ML::Vec2 GetPos(int frames_) const;	

		/// <summary>
		/// �����Ŏ󂯎�����t���[�����ɑΉ������A�j���[�V�����摜��`��Ԃ�
		/// </summary>
		/// <param name="frames_">�o�߃t���[����</param>
		/// <returns>�t���[�����ɑΉ������A�j���[�V�����摜��`</returns>
		ML::Box2D GetSrcBox(int frames_) const;


		/// <summary>
		/// �`��p��`�����炤
		/// </summary>
		/// <returns>�`��p��`�i���_�����ς݁j</returns>
		ML::Box2D GetDrawBox() const;

		/// <summary>
		/// �T�C�Y�̐ݒ�
		/// </summary>
		/// <param name="size_">�T�C�Y</param>
		void SetSize(const Size2D& size_);

		/// <summary>
		/// �A�j���[�V�����̑����̐ݒ�
		/// </summary>
		/// <param name="distance_">�摜���؂�ւ��܂ł̃t���[����</param>
		void SetDistance(int distance_);

		/// <summary>
		/// �A�j���[�V�����̉摜�����󂯎��
		/// </summary>
		int GetAnimLength() const;			//�A�j���[�V�����̉摜�����󂯎��

		/// <summary>
		/// �A�j���[�V�������ꏄ����܂ł̃t���[�������󂯎��
		/// </summary>
		int GetAnimCountMax() const;		//�A�j���[�V�������ꏄ����܂ł̃t���[�������󂯎��
	};

	//�X�e�[�^�X�\����
	struct CharaStatus {
		Limit<int> hp;		//�̗�
		int	atk;			//�U����

		//�R���X�g���N�^
		CharaStatus()
			:hp()
			, atk(0)
		{
			this->hp.SetValues(0, 0, 0);
		}
	};

	//�V�[���\����
	struct SceneAction
	{
		virtual void Start() {};
		virtual void UpDate() {};
		virtual bool ChangeActionFlag() = 0;
		virtual SceneAction* EndAndCreateNext() = 0;
		virtual ~SceneAction() {};
	};
	//�V�[���\���̊Ǘ�
	struct SceneActionManager
	{
	private:
		SceneAction* playingAction;
	public:
		//�R���X�g���N�^
		SceneActionManager(SceneAction* initAction_);
		//����
		static SceneActionManager* Create(SceneAction* initAction_);
		//�V�[���̍X�V����
		void UpDateScene();
		//�f�X�g���N�^
		~SceneActionManager();
	};

	//�{�b�N�X�𒆐S��ō��W�ړ��ł���悤�ɂ���
	ML::Box2D setBoxCenter(int w_, int h_);
	ML::Box2D setBoxCenter(Size2D size_);

	//�{�b�N�X����`�P�ʂɕϊ�
	ML::Rect BoxToRect(const ML::Box2D& box_);

	//�{�b�N�X�̒��S���W�����߂�
	ML::Vec2 BoxCenterPos(const ML::Box2D& box_);

	//�x�N�g������]������
	ML::Vec2 RotateVec(const ML::Vec2& vec_, float angleRad_);

	//JSON�t�@�C����ǂݍ���
	nlohmann::json LoadJsonFile(const string& filePath_);
	//JSON�t�@�C����ۑ�����
	void SaveJsonFile(const json& js_, const string& filePath_);

}