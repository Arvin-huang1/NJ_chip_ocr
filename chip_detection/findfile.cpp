#include "findfile.h"

//查找文件
void fileSearch(const char* path_dir, vector<string> &file_vec)
{
	char dirNew[256];
	strcpy_s(dirNew, 256, path_dir);
	strcat_s(dirNew, 256, "\\*.*");

	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1)
		return;

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;
			strcpy_s(dirNew, 256, path_dir);
			strcat_s(dirNew, 256, "\\");
			strcat_s(dirNew, 256, findData.name);

			fileSearch(dirNew, file_vec);
		}
		else
		{
			string str = findData.name;
			/*str.append("\\");
			str.append(findData.name);*/
			file_vec.push_back(str);
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);
}

//字符串分割
vector<string> SplitStrToVec(const string &s, const string &seperator)
{
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}
