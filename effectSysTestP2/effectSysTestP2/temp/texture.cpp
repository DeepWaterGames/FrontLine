#include "texture.h"

GLuint loadTexture(const char* filename)
{
	 GLuint id;
	 sf::Image Image;
	 Image.loadFromFile(filename);
	 glGenTextures(1, &id);

	 // Bind the texture object
	 glBindTexture(GL_TEXTURE_2D, id);

	 glTexImage2D(GL_TEXTURE_2D, 0, 4, Image.getSize().x, Image.getSize().y, 0,
	 GL_RGBA, GL_UNSIGNED_BYTE, Image.getPixelsPtr());
	 // Set the texture's stretching properties
	 /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	 return id;
}