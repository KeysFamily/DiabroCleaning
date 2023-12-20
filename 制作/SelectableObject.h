#pragma once
#include "myLib.h"
namespace MyUI
{
	class SelectableObject
	{
		SelectableObject* nextObj[4];
	public:
		virtual ML::Box2D GetObjectSize() const = 0;
		virtual void IsSelecting() = 0;

		SelectableObject()
			:nextObj{ this,this,this,this }
		{
		}

		//ゲッタ
		virtual SelectableObject* GetNext_Up() const
		{
			return nextObj[0];
		}
		virtual SelectableObject* GetNext_Down() const
		{
			return nextObj[1];
		}
		virtual SelectableObject* GetNext_Left() const
		{
			return nextObj[2];
		}
		virtual SelectableObject* GetNext_Right() const
		{
			return nextObj[3];
		}

		//セッタ
		void SetNext_Up(SelectableObject* nextObjU)
		{
			nextObj[0] = nextObjU;
		}
		void SetNext_Down(SelectableObject* nextObjD)
		{
			nextObj[1] = nextObjD;
		}
		void SetNext_Left(SelectableObject* nextObjL)
		{
			nextObj[2] = nextObjL;
		}
		void SetNext_Right(SelectableObject* nextObjR)
		{
			nextObj[3] = nextObjR;
		}
	};
}