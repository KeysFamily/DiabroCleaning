#pragma once
#include <random>
#include <vector>

/// <summary>
/// �T�@�@�@�v�F�ݒ肵�����I�m�������Ƃɒ��I���ʂ�Ԃ�
/// ��@���@�ҁF���J��E��N
/// �쐬�N�����F2023/6/28
/// </summary>

class RateLot
{
	std::vector<float> probabilities;
	std::discrete_distribution<std::size_t> dist;
	std::mt19937 engine;
public:
	/// <summary>
	/// �m�������߂�
	/// ���I�m���̊��������X�g�����ēn��
	/// </summary>
	/// <param name="dist_">���I�䗦�̃��X�g</param>
	RateLot(std::vector<float> dist_);

	~RateLot();

	/// <summary>
	/// �ݒ肵���m���Ɋ�Â��Ē��I����(�C���f�b�N�X)��Ԃ�
	/// </summary>
	/// <returns>0�`�R���X�g���N�^�����œn�����z��̗v�f��-1�̂����ꂩ�̒��I����</returns>
	std::size_t result();
};
