#pragma once

#include "core.h"
#include <string>
#include "entity.h"
#include "bitmap_font.h"

namespace wgl
{

class Mesh;

class Text : public Entity
{
private:
	std::string str;
	ref<BitmapFont> font;
	void generateMesh();

public:
	Text(std::string str, ref<BitmapFont> font, ref<Shader> shader);
	void draw() override;
	ref<BitmapFont> getFont();
	~Text();
	void setText(std::string text);
};

} // namespace wgl