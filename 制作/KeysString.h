#pragma once

#include <string>

using StringLength = size_t;	//�������J�E���^(string::size()�Ƃ͕ʕ�)

namespace KeysString
{
	class Strings
	{
		std::string str;

		/// <summary>
		/// �������S�p�������ǂ����`�F�b�N����
		/// </summary>
		/// <param name="c">char�^����</param>
		/// <returns>�S�p�����Ȃ�true</returns>
		bool SJISMultiCheck(unsigned char c) const;
	public:
		Strings()
			: str()
		{}
		Strings(const std::string str_)
			: str(str_)
		{}

		~Strings() {}

		/// <summary>
		/// ��������Ԃ�(std::string::size()�Ƃ͕ʕ�)
		/// </summary>
		/// <returns>������</returns>
		StringLength Length() const;

		/// <summary>
		/// size_�������̕�����Ԃ�
		/// </summary>
		/// <param name="size_">�T�C�Y</param>
		/// <returns></returns>
		std::string SubStr(const StringLength size_ = 1);




	};

}