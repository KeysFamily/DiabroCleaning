#pragma once

namespace Map
{
	enum class MapDir
	{
		Non = -1,
		Up = 0,
		Down,
		Right,
		Left
	};


	class MapFunc 
	{
	public:
		static MapDir ReverseMapDir(const MapDir& mapDirection_)
		{
			switch (mapDirection_)
			{
			case MapDir::Non:
				return MapDir::Non;
				break;
			case MapDir::Up:
				return MapDir::Down;
				break;
			case MapDir::Down:
				return MapDir::Up;
				break;
			case MapDir::Right:
				return MapDir::Left;
				break;
			case MapDir::Left:
				return MapDir::Right;
				break;
			}
			return MapDir::Non;
		}

	};

}