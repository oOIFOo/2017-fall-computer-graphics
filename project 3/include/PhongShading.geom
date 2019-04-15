#version 150 compatibility

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 85 ) out;

in Vertex{
	vec3 vertexEyeSpace;
	vec3 normal;    
	vec2 _texcoord;
}vertex[];

out	vec3 vertexEyeSpace;
out	vec3 normal;
out	vec2 _texcoord;
	
uniform int level;
uniform sampler2D colorTexture;

vec4 in_pos[5];
vec4 Centroid = vec4(0.0, 1.0, 0.0, 1.0);

void get_attr(int node1, int node2, int node3, int node4,int level){
	vec4 c = gl_ModelViewMatrix * Centroid;
	vertexEyeSpace = (vertex[node1].vertexEyeSpace + vertex[node2].vertexEyeSpace + vertex[node3].vertexEyeSpace + vertex[node4].vertexEyeSpace) / 4;
	normal = (vertex[node1].normal + vertex[node2].normal + vertex[node3].normal + vertex[node4].normal) / 4;
	_texcoord = (vertex[node1]._texcoord + vertex[node2]._texcoord + vertex[node3]._texcoord + vertex[node1]._texcoord) / 4;
	gl_TexCoord[0] = (gl_in[node1].gl_TexCoord[0] + gl_in[node2].gl_TexCoord[0] + gl_in[node3].gl_TexCoord[0] + gl_in[node4].gl_TexCoord[0]);

	vec4 tmp = (in_pos[node1] + in_pos[node2] + in_pos[node3] + in_pos[node4])/4 - c;

	if(level == 0)
		gl_Position = gl_ProjectionMatrix * ((in_pos[node1] + in_pos[node2] + in_pos[node3] + in_pos[node4]) / 4);
	else if(level == 1)
		gl_Position = gl_ProjectionMatrix * (vec4(normalize(tmp)) + c);
}

void main(){
	float length = 1.0f;
	
	for(int i = 0; i < gl_in.length(); i++){
			in_pos[i] = gl_in[i].gl_Position;
	}
		
	if(level == 0){
		for(int i = 0; i < gl_in.length(); i++){
			get_attr(i,i,i,i,0);
			EmitVertex();
		}
		EndPrimitive();
	}
	else if(level == 1){
		get_attr(0,0, 1,1, 1);
		EmitVertex();
		
		get_attr(1,1, 1,1, 1);
		EmitVertex();
	
		get_attr(1,1, 2,2, 1);
		EmitVertex();
		
		EndPrimitive();
		
		get_attr(0,0, 0,0, 1);
		EmitVertex();
		
		get_attr(0,0, 1,1, 1);
		EmitVertex();
		
		get_attr(0,0, 2,2, 1);
		EmitVertex();
		
		get_attr(1,1, 2,2, 1);
		EmitVertex();
		
		get_attr(2,2, 2,2, 1);
		EmitVertex();
		
		EndPrimitive();
	}
	else if(level == 2){
		get_attr(1,1, 1,1, 1);
		EmitVertex();
		
		get_attr(0,1, 1,1, 1);
		EmitVertex();
	
		get_attr(1,1, 1,2, 1);
		EmitVertex();
		
		EndPrimitive();
/**************************************/
		
		get_attr(0,0, 1,1, 1);
		EmitVertex();
		
		get_attr(0,1, 1,1, 1);
		EmitVertex();
		
		get_attr(0,1, 1,2, 1);
		EmitVertex();
		
		get_attr(1,1, 1,2, 1);
		EmitVertex();
		
		get_attr(1,1, 2,2, 1);
		EmitVertex();
		
		EndPrimitive();
/**************************************/
		
		get_attr(0,0, 0,1, 1);
		EmitVertex();
		
		get_attr(0,0, 1,1, 1);
		EmitVertex();
		
		get_attr(0,1, 0,2, 1);
		EmitVertex();
		
		get_attr(0,1, 1,2, 1);
		EmitVertex();
		
		get_attr(0,2, 1,2, 1);
		EmitVertex();
		
		get_attr(1,1, 2,2, 1);
		EmitVertex();

		get_attr(1,2, 2,2, 1);
		EmitVertex();

		EndPrimitive();           
/**************************************/
		get_attr(0,0, 0,0, 1);
		EmitVertex();
		
		get_attr(0,0, 0,1, 1);
		EmitVertex();
		
		get_attr(0,0, 0,2, 1);
		EmitVertex();
		
		get_attr(0,1, 0,2, 1);
		EmitVertex();
		
		get_attr(0,0, 2,2, 1);
		EmitVertex();
		
		get_attr(0,2, 1,2, 1);
		EmitVertex();

		get_attr(0,2, 2,2, 1);
		EmitVertex();

		get_attr(1,2, 2,2, 1);
		EmitVertex();

		get_attr(2,2, 2,2, 1);
		EmitVertex();

		EndPrimitive();
	}
}