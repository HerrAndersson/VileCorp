#pragma once

#include <string>
/*
Noteringar:
IDs för komponenter i Traps och Units TODO: Zache/Aron
Alla komponenter ska ärva från denna tomma basklass
Alla komponenter bör sparas i ComponentList
*/

class Component
{
public:
	Component();
	~Component();

	virtual std::string GetInfo() = 0;
};

