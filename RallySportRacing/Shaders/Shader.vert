#version 420

// Input vertex data, different for all executions of shader.
layout(location = 0) in vec3 vertexPosition_worldSpace;
//layout(location = 1) in vec3 colorIn;
layout(location = 1) in vec3 normals_modelspace;
layout(location = 2) in vec2 texCoordIn;

// Constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform mat4 ligtSpaceMatrix;

// Outputs for fragment shader.
out vec3 vertexPosition_viewspace;
out vec3 normal_viewspace;
out vec2 texCoord;
out vec4 posLightSpace;

void main(){
  gl_Position = MVP * vec4(vertexPosition_worldSpace, 1);
  //Set vertexPositions to view space.
  vertexPosition_viewspace = (modelViewMatrix * vec4(vertexPosition_worldSpace, 1.0)).xyz;
  //Set normals to view space.
  normal_viewspace = (normalMatrix * vec4(normals_modelspace, 0.0)).xyz;
  
  //ToDo uncomment when shadow map is implemented.
  //posLightSpace = ligtSpaceMatrix * vec4(vertexPosition_worldSpace, 1.0);

  texCoord = texCoordIn;
}