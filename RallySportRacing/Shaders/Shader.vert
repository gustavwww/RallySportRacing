#version 420

// Input vertex data, different for all executions of shader.
layout(location = 0) in vec3 vertexPosition_worldSpace;
//layout(location = 1) in vec3 colorIn;
layout(location = 1) in vec3 normals_modelspace;
layout(location = 2) in vec2 texCoordIn;
layout(location = 3) in vec2 texCoord2In;
layout(location = 4) in mat4 instanceMatrix;


// Constant for the whole mesh.
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightMatrix;

// Outputs for fragment shader.
out vec3 vertexPosition_viewspace;
out vec3 normal_viewspace;
out vec2 texCoord;
out vec2 texCoord2;
out vec4 shadowMapCoord;

void main(){
  gl_Position = (projection * view * instanceMatrix) * vec4(vertexPosition_worldSpace, 1);
  //Set vertexPositions to view space.
  vertexPosition_viewspace = ((view * instanceMatrix) * vec4(vertexPosition_worldSpace, 1.0)).xyz;
  //Set normals to view space.
  normal_viewspace = (inverse(transpose(view * instanceMatrix)) * vec4(normals_modelspace, 0.0)).xyz;
  
  //ToDo uncomment when shadow map is implemented.
  shadowMapCoord = lightMatrix * vec4(vertexPosition_viewspace, 1.0);

  texCoord = texCoordIn;
  texCoord2 = texCoord2In;
}