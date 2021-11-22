#include "bitmap_font.h"
#include <fstream>
#include <sstream>

namespace wgl
{

BitmapFont::BitmapFont(std::string path)
{
	readFontData(path);
	// path.substr(0, path.find('/'));
	texture = createScope<Texture>(path.substr(0, path.find_last_of('.')) + ".jpg");
	// texture = new Texture("resources/fonts/classic_console.jpg");
}

BitmapFont::~BitmapFont()
{
}

CharInfo BitmapFont::getCharInfo(char c)
{
	// std::map<char, CharInfo>::iterator it = characters.find(c);
	// if (it != characters.end())
	return (characters[c]);
}

std::vector<CharInfo> BitmapFont::getCharInfo(std::string s)
{
	std::vector<CharInfo> charInfos;

	for (char &c : s)
	{
		std::map<char, CharInfo>::iterator it = characters.find(c);
		if (it != characters.end())
			charInfos.push_back(characters[c]);
	}
	return (charInfos);
}

void BitmapFont::readFontData(std::string path)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line.compare(0, 4, "info") == 0)
			{
				fontName = "Font Name";
			}
			else if (line.compare(0, 6, "common") == 0)
			{
			}
			else if (line.compare(0, 4, "page") == 0)
			{
				int page = 0;
				char bitmapFile[128];
				sscanf(line.c_str(), "page id=%d file=\"%[^\"]", &page, &bitmapFile[0]);
				bitmapFilename = bitmapFile;
			}
			else if (line.compare(0, 5, "chars") == 0)
			{
				sscanf(line.c_str(), "chars count=%d", &charCount);
			}
			else if (line.compare(0, 4, "char") == 0)
			{
				CharInfo info;
				sscanf(line.c_str(),
					   "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d",
					   &info.id, &info.x, &info.y, &info.width, &info.height, &info.xOffset, &info.yOffset, &info.xAdvance);
				info.c = (char)info.id;
				characters[info.c] = info;
			}
		}
		file.close();
	}
	printf("read font: %s [bitmap: %s chars: %d]\n", path.c_str(), bitmapFilename.c_str(), charCount);
}

ref<Texture> BitmapFont::getTexture()
{
	return (texture);
}

} // namespace wgl