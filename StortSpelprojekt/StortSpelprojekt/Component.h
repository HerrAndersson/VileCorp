#pragma once

#include <string>
/*
Noteringar:
IDs f�r komponenter i Traps och Units TODO: Zache/Aron
Alla komponenter ska �rva fr�n denna tomma basklass
Alla komponenter b�r sparas i ComponentList
*/

class Component
{
public:
	Component();
	~Component();

	virtual std::string GetInfo() = 0;
};

