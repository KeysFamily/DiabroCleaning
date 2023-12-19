#pragma once
#include "myLib.h"
namespace MyUI
{
	class SelectableObject
	{
	public:
		virtual ML::Box2D GetObjectSize() = 0;
		virtual void IsSelecting() {};
		virtual weak_ptr<SelectableObject> GetNext_Up();
		virtual weak_ptr<SelectableObject> GetNext_Down();
	};
}