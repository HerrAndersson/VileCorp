#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

class SYSTEM_EXPORT Example {
private:
	int mValue;
public:
	Example(int value);
	int getValue()const;

};
