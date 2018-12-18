#ifndef FILEDIALOG_IS_INCLUDED
#define FILEDIALOG_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class FileDialogOption
{
public:
	enum MODE
	{
		MODE_OPEN,
		MODE_SAVE
	};
	MODE mode;
	FileDialogOption()
	{
		mode=MODE_OPEN;
	}
};
std::string SelectFile(const FileDialogOption &opt);

/* } */
#endif
