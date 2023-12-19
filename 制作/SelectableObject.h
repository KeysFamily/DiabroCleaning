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
		SelectableObject* GetNext_Up() const
		{
			return nextObj[0];
		}
		SelectableObject* GetNext_Down() const
		{
			return nextObj[1];
		}
		SelectableObject* GetNext_Left() const
		{
			return nextObj[2];
		}
		SelectableObject* GetNext_Right() const
		{
			return nextObj[3];
		}

		//セッタ
		void SetNext_Up(SelectableObject* nextObjU)
		{
			nextObj[0] = nextObjU;
		}
		void GetNext_Down(SelectableObject* nextObjD)
		{
			nextObj[1] = nextObjD;
		}
		void GetNext_Left(SelectableObject* nextObjL)
		{
			nextObj[2] = nextObjL;
		}
		void GetNext_Right(SelectableObject* nextObjR)
		{
			nextObj[3] = nextObjR;
		}
	};
}