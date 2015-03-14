#version 430

layout(location = 0) in vec3 vertexPosition_modelspace;
//uniform mat4 MVP;

//layout(location = 0) out vec3 color;
 out gl_PerVertex    {      vec4 gl_Position;    };
void main(){
 
    // Output position of the vertex, in clip space : MVP * position
   // vec4 v = vec4(vertexPosition_modelspace,1); // Transform an homogeneous 4D vector, remember ?
    gl_Position.xyz = vertexPosition_modelspace; //MVP * v;
    gl_Position.w = 1.0;
	//color = vec3(0.5,1.0,0.2);
}