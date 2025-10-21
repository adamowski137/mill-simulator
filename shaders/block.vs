#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNorm;
layout (location = 2) in vec2 aTex;

out vec3 vPos;
out vec3 vNorm;
out vec2 vTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  vec3 norm = aNorm.xyz;
  vec4 globalPos = model * aPos;
  vPos = globalPos.xyz;
  //vNorm = normalize(mat3(transpose(inverse(model))) * norm);
	vNorm = norm;
  vTex = aTex;
  gl_Position = projection * view * globalPos;
}


