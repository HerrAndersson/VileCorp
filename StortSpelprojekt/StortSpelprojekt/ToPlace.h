#pragma once

struct ToPlace
{
	ToPlace()
	{
		_type = System::TRAP;
		_subType = 0;
		_name = "trap_proto";
		ResetTemps();
	}
	void ResetTemps()
	{
		_goldCost = -1;
		_blueprintID = 1;
	}
	System::Type _type;
	int _subType;
	std::string _name;
	int _goldCost;
	int _blueprintID;
};