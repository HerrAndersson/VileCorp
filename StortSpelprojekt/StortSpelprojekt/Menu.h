#ifndef MENU_H
#define MENU_H

#include "UIHandler.h"

class Menu
{
private:
	UIHandler* _uiHandler;

public:
	Menu(UIHandler* uiHandler);
	~Menu();

	void Update();

};

#endif