#pragma once

#include <string>

using StringLength = size_t;	//文字数カウンタ(string::size()とは別物)

namespace KeysString
{
	class Strings
	{
		std::string str;

		/// <summary>
		/// 文字が全角文字かどうかチェックする
		/// </summary>
		/// <param name="c">char型文字</param>
		/// <returns>全角文字ならtrue</returns>
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
		/// 文字数を返す(std::string::size()とは別物)
		/// </summary>
		/// <returns>文字数</returns>
		StringLength Length() const;

		/// <summary>
		/// size_文字分の文字を返す
		/// </summary>
		/// <param name="size_">サイズ</param>
		/// <returns></returns>
		std::string SubStr(const StringLength size_ = 1);




	};

}