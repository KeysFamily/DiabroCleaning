#include "KeysString.h"

namespace KeysString
{

	bool Strings::SJISMultiCheck(unsigned char c) const {
		return ((c >= 0x81) && (c <= 0x9f)) || ((c >= 0xe0) && (c <= 0xfc));
	}

	StringLength Strings::Length() const {
		StringLength retSize_ = 0;
		for (int i = 0; i < str.size();) {
			if (SJISMultiCheck(str[i])) {
				i += 2;
			}
			else {
				i++;
			}
			retSize_++;
		}
		return retSize_;
	}

	void Strings::SetString(const std::string str_) {
		this->str = str_;
	}

	std::string Strings::SubStr(const StringLength size_) {
		StringLength sizeCnt = 0;
		int cnt = 0;
		StringLength strSize = size_ <= this->Length() ? size_ : this->Length();

		if (str.size() == 0) return str;
		while (cnt <= strSize) {
			if (SJISMultiCheck(str[sizeCnt])) {
				sizeCnt += 2;
			}
			else {
				sizeCnt++;
			}
			cnt++;
		}
		return str.substr(0, sizeCnt);
	}
}