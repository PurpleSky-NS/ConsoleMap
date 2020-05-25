#include "MapCom.h"
class MainSurface :public Surface
{
public:

	MainSurface()
	{
		Add(new MapCom);
	}

};
#include <fstream>
void StartFrame()
{
	SurfaceManager::GetInstance().Start(new MainSurface);
}
