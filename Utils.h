#pragma once

#include <glad\glad.h>
#include "SOIL2/SOIL2.h"
class Utils
{
public:
	
	/**	void LoadTexturePNG(int * inTexID, const char * path)
	*
	*	Parameters:
	*	- int * inTexID: Pointer to where the texture ID is to be
	*	stored.
	*	- const char * path: File path to the PNG file (as c-string)
	*
	*	Use:
	*	This function can be used to load a PNG file into an OpenGL
	*	texture.
	*/
	static void LoadTextureFromImage(GLuint * inTexID, const char * path);


	/** bool InitFBO(GLuint * fb, GLint * texId, GLsizei width, GLsizei height, GLint format)
	*
	*   Use: Generate Framebuffer Object.
	*/
	static bool GenerateDepthFBO(GLuint * fb, GLuint * texId, GLsizei width, GLsizei height);
};

