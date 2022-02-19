#version 420

// Input vertex data, different for all executions of shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
//ToDo make sure that normals in modelspace is added to layout location 2 and the uncomment all code related to normals in both vertex and fragment shader.
//layout(location = 2) in vec3 normals_modelspace;

// Constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

//Outputs for fragment shader.
out vec3 vertexPosition_viewspace;
out vec3 normal_viewspace;
out vec3 fragmentColor;

void main(){
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
  vertexPosition_viewspace = (modelViewMatrix * vec4(vertexPosition_modelspace, 1.0)).xyz;
  //normal_viewspace = (normalMatrix * vec4(normals_modelspace, 0.0)).xyz;
  fragmentColor = vertexColor;
}