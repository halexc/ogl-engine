#include "Utils.h"

void Utils::LoadTextureFromImage(GLuint * inTexID, const char * path)
{
	glGenTextures(1, inTexID);
	*inTexID = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, *inTexID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
}
