#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

bool GetFilenamesInDirectory(const wchar_t* wfolder, wchar_t* extension, std::vector<std::wstring> &listToFill)
{
	bool result = false;
	std::wstring dot = L".", dotdot = L"..", ext = extension;
	if (wfolder != nullptr && extension != nullptr)
	{
		std::ofstream testout;
		std::wstring search_path = wfolder;
		search_path.append(L"*");
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			result = true;
			do {
				std::wstring path = fd.cFileName;
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if(path.substr(path.size()-ext.size())==ext)
						listToFill.push_back(wfolder + path);
				}
				else
				{
					if (path != dot && path != dotdot)
					{
						GetFilenamesInDirectory((wfolder + path + L"/").data(), extension, listToFill);
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}
	return result;
}

//args = sourcefiles, destination
int main(int argc, char* argv[])
{
	std::cout << "Texture Converter Running" << std::endl;
	std::wstring wsrc;
	std::wstring wdst;
	std::string cmd = R"(texconv.exe -f BC7_UNORM -pow2 -bcmax -fl 11.1 )";
	{
		std::string src = argv[1];
		std::string dst = argv[2];
		wsrc = std::wstring(src.begin(), src.end());
		wdst = std::wstring(dst.begin(), dst.end());
		cmd += R"(")" + src;
	}
	std::vector<std::wstring> wfiles;
	if (!GetFilenamesInDirectory(wsrc.data(), L".png", wfiles))
		throw std::runtime_error("Searchpath was bad or empty");
	std::vector<std::wstring> wfilesdds;
	GetFilenamesInDirectory(wdst.data(), L".dds", wfilesdds);
	for (int i = 0; i < wfiles.size(); i++)
		wfiles[i] = wfiles[i].substr(wsrc.size());
	for (int i = 0; i < wfilesdds.size(); i++)
		wfilesdds[i] = wfilesdds[i].substr(wdst.size());
	for (size_t i = 0; i < wfiles.size(); i++)
	{
		bool found = false;
		for (std::wstring str : wfilesdds)
		{
			if (str.substr(0, str.size() - 4) == wfiles[i].substr(0, wfiles[i].size() - 4))
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::string out = cmd + std::string(wfiles[i].begin(), wfiles[i].end()) + R"(")";
			system(out.data());
			std::wstring test = ((wsrc + wfiles[i]).substr(0, wsrc.size() + wfiles[i].size() - 4) + L".dds");
			test = ((wdst + wfiles[i]).substr(0, wdst.size() + wfiles[i].size() - 4) + L".dds");
			MoveFile(((wsrc + wfiles[i]).substr(0, wsrc.size() + wfiles[i].size() - 4) + L".dds").data(),
				((wdst + wfiles[i]).substr(0, wdst.size() + wfiles[i].size() - 4) + L".dds").data());
		}
	}
}