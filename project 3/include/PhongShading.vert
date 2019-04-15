#version 150 compatibility

out Vertex{
	vec3 vertexEyeSpace;
	vec3 normal;
	vec2 _texcoord;
};

uniform sampler2D colorTexture;

void main(){
	gl_TexCoord[0] = gl_MultiTexCoord0;
    vertexEyeSpace = vec3(gl_ModelViewMatrix * gl_Vertex);
    normal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewMatrix * gl_Vertex;
}
