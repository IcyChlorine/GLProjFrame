#include "Text.h"
#include "Application.h"

static bool freetype_initialized{false};
static FT_Library ft_lib; //TODO make these two local - seems no need to make them global var.
static FT_Face font;

#define NR_ASCII 128

struct glyph_geometry{
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;	
};

glyph_geometry glyph_data[NR_ASCII];

unsigned int font_texture{0};
unsigned int block_width{0};
unsigned int block_height{0};

Shader* text_shader{nullptr};

void show_glyph_cli(FT_GlyphSlot& glyph) {
	auto width = glyph->bitmap.width;
	auto height= glyph->bitmap.rows;
	printf("width = %d; height = %d;\n",width, height);
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			printf(glyph->bitmap.buffer[i*width+j]>0?"#":" ");
		}
		printf("\n");
	}
	system("pause");
}

void copy_bitmap(
	unsigned char* dst, unsigned int dst_width, unsigned int dst_height,
	unsigned char* src, unsigned int src_width, unsigned int src_height,
	unsigned int offset_width, unsigned int offset_height
) {
	//assert(offset_width+src_width<=dst_width && offset_height+src_height<=dst_height);
	for(int i=0; i<src_height; i++) {
		for(int j=0; j<src_width; j++) {
			dst[ (offset_height + i)*dst_width + (offset_width + j) ]
			= src[ i*src_width + j ];
		}
	}
}

void init_freetype() {
	FT_Library ft_lib;
	if(FT_Init_FreeType(&ft_lib)){
		error("Fail to initialize freetype library!\n");
		throw init_exception();
	}

	const char* TTF_FILENAME = "FiraCode-Regular.ttf";
	FT_Face font;
	if(FT_New_Face(ft_lib, TTF_FILENAME, 0, &font)){
		errorf("Fail to load font `%s`!\n",TTF_FILENAME);
		throw init_exception();
	}
	FT_Set_Pixel_Sizes(font, 0, 48);
	

	//show_glyph_cli(font->glyph);
	unsigned int max_advance{0}, max_height{0};
	//WARN: c must declared as unsigned char, or the loop won't stop due to overflow!
	for(unsigned char c=0; c<NR_ASCII; c++) { 
		if (FT_Load_Char(font, c, FT_LOAD_RENDER)){
  		  	error("FREETYTPE: Failed to load Glyph\n");
			throw exception();
		}
		FT_GlyphSlot& glyph = font->glyph; // alias
		glyph_data[c].size = glm::ivec2(
			glyph->bitmap.width, glyph->bitmap.rows
		);
		glyph_data[c].bearing = glm::ivec2(
			glyph->bitmap_left, glyph->bitmap_top
		);
		// advance原生的单位是1/64像素，将其转换为以像素为单位的值
		glyph_data[c].advance = glyph->advance.x>>6;

		if(glyph_data[c].advance>max_advance)
			max_advance = glyph_data[c].advance;
		if(glyph->bitmap.rows>max_height)
			max_height = glyph->bitmap.rows;
	}
	// 16 ch per row, 8 rows in total
	block_width = max_advance;
	block_height = max_height;
	unsigned int blanket_width = 16*block_width;
	unsigned int blanket_height = 8*block_height;

	unsigned char* ch_blanket = new unsigned char[ blanket_width*blanket_height ];
	memset((void*) ch_blanket, 0, blanket_width*blanket_height);
	for(unsigned char c=0; c<NR_ASCII; c++) {
		if (FT_Load_Char(font, c, FT_LOAD_RENDER)){
  		  	error("FREETYTPE: Failed to load Glyph\n");
			throw exception();
		}
		copy_bitmap(
			ch_blanket, blanket_width, blanket_height,
			font->glyph->bitmap.buffer, glyph_data[c].size[0], glyph_data[c].size[1],
			(c%16)*block_width, (c/16)*block_height
		);
	}

	glGenTextures(1, &font_texture);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE);
	glTexImage2D(
		GL_TEXTURE_2D,
		0, // no mipmap - level===0!
		GL_RED, // single byte memory layout
		blanket_width,
		blanket_height,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		ch_blanket
	);
	// These parameters have to be set, or texture doesn't work in ogl.
	// Seems there's no "default value" for these.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] ch_blanket;
	
	FT_Done_Face(font);
	FT_Done_FreeType(ft_lib);

}

Text::Text(string text, glm::ivec2 pos, float scale): 
	text{text}, pos{pos}
{
	if(!freetype_initialized) {
		init_freetype();
		freetype_initialized = true;
	}
	if(!text_shader)
		text_shader = new Shader("src\\shaders\\text.vert.glsl", "src\\shaders\\text.frag.glsl");
		//TODO 这么个shader怎么析构？？

	this->scale = scale;

	get_and_bind_vao(&VAO);
	get_and_bind_vbo(&VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, vert_data, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 
}

Text::~Text()
{

}

void Text::setText(string text) {
	this->text = text;
}
void Text::setPosition(glm::ivec2 pos) {
	this->pos = pos;
}
void Text::setPosition(unsigned int x, unsigned int y) {
	this->pos = glm::ivec2(x, y);
}

void Text::render() {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	text_shader->use();
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	int wnd_width, wnd_height;
	theApp->getWindow()->getSize(&wnd_width, &wnd_height);

	text_shader->setUniform("wnd_width", wnd_width*1.0f);
	text_shader->setUniform("wnd_height", wnd_height*1.0f);

	float x=pos.x, y=pos.y;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		auto glyph = glyph_data[*it];
		unsigned char c = *it;

		if(c=='\n'){//允许换行
			x = pos.x;
			y += block_height * scale;
			continue;
		}
		
		//xypos: 每个字符左上角的起点
		float xpos = x + glyph.bearing.x * scale;
		// the offset for ypos is a bit tricky, be careful!
		float ypos = y + (block_height-glyph.bearing.y) * scale;

		float w = block_width * scale; //glyph.size.x * scale;
		float h = block_height * scale; //glyph.size.y * scale;
		// update VBO for each character
		const float eps = 1e-3;
		float vertices[4][4] = {
			//pos from UL in pixel  |      col        |      row
			{ xpos,     ypos,       (c%16  )/16.0f    , (c/16  )/8.0f     },
			{ xpos,     ypos + h,   (c%16  )/16.0f    , (c/16+1)/8.0f-eps },            
			{ xpos + w, ypos + h,   (c%16+1)/16.0f-eps, (c/16+1)/8.0f-eps },      
			{ xpos + w, ypos,       (c%16+1)/16.0f-eps, (c/16  )/8.0f     },
		};
		
		// update content of VBO memory
		
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad

		/*glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//for debugging
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glEnable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += glyph.advance * scale;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}