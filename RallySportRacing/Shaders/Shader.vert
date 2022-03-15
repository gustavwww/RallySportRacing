#version 420

// Input vertex data, different for all executions of shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 colorIn;
layout(location = 2) in vec3 normals_modelspace;
layout(location = 3) in vec2 texCoordIn;

// Constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

// Outputs for fragment shader.
out vec3 vertexPosition_viewspace;
out vec3 normal_viewspace;
out vec3 color;
out vec2 texCoord;

void main(){
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
  //Set vertexPositions to view space.
  vertexPosition_viewspace = (modelViewMatrix * vec4(vertexPosition_modelspace, 1.0)).xyz;
  //Set normals to view space.
  normal_viewspace = (normalMatrix * vec4(normals_modelspace, 0.0)).xyz;
  
  texCoord = texCoordIn;
  color = colorIn;
}