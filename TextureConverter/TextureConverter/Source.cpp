#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

bool GetFilenamesInDirectory(const char* wfolder, std::vector<std::string> &listToFill)
{
	bool result = false;
	std::wstring dot = L".", dotdot = L"..";
	if (wfolder != nullptr)
	{
		std::string folder = wfolder;
		std::wstring search_path(folder.begin(),folder.end());
		search_path.append(L"*");
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			result = true;
			do {
				std::wstring wpath = fd.cFileName;
				std::string path = folder + std::string(wpath.begin(), wpath.end());
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					listToFill.push_back(path);
				}
				else
				{
					if (wpath != dot && wpath != dotdot)
					{
						GetFilenamesInDirectory((path + "/").data(), listToFill);
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}
	return result;
}

//args = destinationfolder, ddsfolder, assetsfolders...-> -ToDDS filetypes
int main(int argc, char* argv[])
{
	std::cout << "--------------Texture Converter Running--------------" << std::endl;
	std::vector<std::string> dirstr;
	std::map<std::string, int> convertTypes;
	std::vector<std::vector<std::string>> files;
	bool ignore = false;
	for (int i = 1; i < argc; i++)
	{
		std::string src = argv[i];
		for (int c = 0; c < src.size(); c++)
		{
			if (src[c] == '\\')
			{
				src[c] = '/';
			}
		}
		if (argv[i] == std::string("-ToDDS"))
		{
			ignore = true;
			std::cout << std::endl << "Types to convert" << std::endl;
			continue;
		}
		if (ignore)
		{
			if (src[0] == '.')
			{
				src.erase(src.begin());
			}
			convertTypes[src] = src.size();
			std::cout << '.' << src << std::endl;
			continue;
		}
		dirstr.push_back(src);
		files.push_back(std::vector<std::string>());
		
		if (!GetFilenamesInDirectory(dirstr.back().data(), files.back()))
		{
			std::cout << "TextureConverter stopped: Searchpath " << dirstr.back().data() << " was bad" << std::endl;
			return 0;
		}
		for (unsigned f = 0; f < files.back().size(); f++)
		{
			files.back()[f] = files.back()[f].substr(dirstr.back().size());
		}
		std::cout << "Directory: " << dirstr.back() << std::endl << "Files found: " << files.back().size() << std::endl;
	}

	std::string cmd = R"(texconv.exe -f BC7_UNORM -pow2 -bcmax -fl 11.1 ")";
	for(size_t d = 2; d < dirstr.size(); d++)
	{
		for (size_t i = 0; i < files[d].size(); i++)
		{
			bool found = false, toDDS = false;
			unsigned extlength = 0, dds = 0;

			for (auto types : convertTypes)
			{
				if (types.first == files[d][i].substr(files[d][i].size() - types.second))
				{
					toDDS = true;
					extlength = types.second;
				}
			}

			if (toDDS)
			{
				for (; dds < files[1].size(); dds++)
				{
					if (files[d][i].substr(0, files[d][i].size() - extlength) == files[1][dds].substr(0, files[1][dds].size() - 3))
					{
						found = true;
						break;
					}
				}
			}

			std::string dest = dirstr[0], src;
			if (toDDS && found)
			{
				dest += files[1][dds];
				src = dirstr[1] + files[1][dds];
				CopyFile(std::wstring(src.begin(), src.end()).data(), std::wstring(dest.begin(), dest.end()).data(), false);
			}
			else if (toDDS)
			{
				system((cmd + dirstr[d] + files[d][i] + R"(")").data());
				std::string pos = dirstr[d] + files[d][i].substr(0, files[d][i].size() - extlength) + "dds";
				dest += files[d][i].substr(0, files[d][i].size() - extlength) + "dds";
				src = dirstr[1] + files[d][i].substr(0, files[d][i].size() - extlength) + "dds";
				MoveFile(std::wstring(pos.begin(), pos.end()).data(), std::wstring(src.begin(), src.end()).data());
				CopyFile(std::wstring(src.begin(), src.end()).data(), std::wstring(dest.begin(), dest.end()).data(), false);
			}
			else
			{
				dest += files[d][i];
				src = dirstr[d] + files[d][i];
				CopyFile(std::wstring(src.begin(), src.end()).data(), std::wstring(dest.begin(), dest.end()).data(), false);
			}
		}
	}
}