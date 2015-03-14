#pragma once
#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);