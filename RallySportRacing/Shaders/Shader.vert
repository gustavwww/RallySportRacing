#version 420

// Input vertex data, different for all executions of shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Constant for the whole mesh.
uniform mat4 MVP;

// Output data, for each fragment.
layout(location = 1) in vec3 vertexColor;
out vec3 fragmentColor;

void main(){
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

  fragmentColor = vertexColor;
}