#version 330 core


out vec4 frag_color;

struct DirLight {
    vec3 direction;

};

struct Material {

		vec3 ambient;
		vec3 specular;
		vec3 diffuse;
		float shininess;
};

const int NUM_CASCADES = 3;

in VS_OUT {
	vec3 model_pos;
	vec3 normal;
	vec2 tex_coords;
	vec4 light_pos[NUM_CASCADES];
	float obj_depth;
} vs_in;

in vec2 TexCoords;

in vec4 points_edge[8];
in vec4 l_coord;

uniform DirLight dir_light;
uniform sampler2D shadow_maps[NUM_CASCADES];
uniform float cascade_ends[NUM_CASCADES];

uniform vec3 light_pos;//light position
uniform vec3 view_pos;//camera pos

uniform sampler2D texture_sampler;
uniform Material material;
uniform bool has_texture; 

#define SHADOW_CALCULATION(pos_in_light_space, i) {\
	vec3 proj_coords = pos_in_light_space.xyz / pos_in_light_space.w;\
	proj_coords = proj_coords * 0.5 + 0.5;\
	/*if ((i ==2 ) && !(proj_coords.x <= 0.0 || proj_coords.x >= 1.0 || proj_coords.y <= 0.0 || proj_coords.y >= 1.0)) {*/\
		/*ambient = vec3(1.0, 1.0, 1.0);*/\
	/*}*/\
	\
	float closest_depth = texture(shadow_maps[i], proj_coords.xy).r;\
	\
	float current_depth = proj_coords.z;\
	\
	vec3 normal = normalize(vs_in.normal);\
	vec3 rev_dir_light = normalize(-dir_light.direction);\
	\
	float bias = 0.0002*sin(acos(dot(normal, rev_dir_light)));\
	bias = clamp(bias, 0,	0.01);\
	\
	shadow = 0.0;\
	vec2 texel_size = 1.0 / textureSize(shadow_maps[i], 0);\
	for(int x = -0; x <= 0; ++x)\
	{\
		for(int y = -0; y <= 0; ++y)\
		{\
			float pcf_pepth = texture(shadow_maps[i], proj_coords.xy + vec2(x, y) * texel_size).r;\
			shadow += ((current_depth - bias) >= pcf_pepth) ? 1.0 : 0.0;\
			/*shadow = 1.0;*/\
		}\
	}\
	/*shadow /= 1.0;*/\
	if (proj_coords.z > 1.0) {\
		/*light_color = vec3(0.4, 0.0, 0.4);*/\
		shadow = 0.0;\
	}\
}

void main()
{           
	vec3 light_color = vec3(1.0, 1.0, 1.0);
	vec3 color = vec3(1.0, 1.0, 1.0);
	if (has_texture) {
		color = texture(texture_sampler, TexCoords).rgb;
		//color = light_color * 10 * TexCoords.y;
	}
	
	vec3 normal = normalize(vs_in.normal);
	//vec3 light_color = 1.2*vec3(0.1745, 0.01175, 0.01175);
	// Ambient
	vec3 ambient = material.ambient*light_color;//vec3(0.1745, 0.01175, 0.01175);
	//ambient = vec3(0.1, 0.1, 0.1);
	// Diffuse
	vec3 rev_dir_light = normalize(light_pos - vs_in.model_pos);
	float diffuse_coeff = max(dot(rev_dir_light, normal), 0.1); // 0.1 is reflecting light
	vec3 diffuse = diffuse_coeff*material.diffuse;// * vec3(0.61424, 0.04136, 0.04136);
	// Specular
	float spec_coeff = 0.0;
	vec3 view_dir = normalize(view_pos - vs_in.model_pos);
	vec3 halfway_dir = normalize(rev_dir_light + view_dir);
	bool light_side = dot(rev_dir_light, normal) > 0.0;
	if (light_side) {
		spec_coeff = pow(max(dot(normal, halfway_dir), 0.0), 0.6);
	}
	vec3 specular = spec_coeff*material.specular; //* vec3(0.727811, 0.626959, 0.626959);
	
	float shadow = 0.0;
	
 	if (vs_in.obj_depth < cascade_ends[0]) {
  		//ambient = vec3(1.0, 0.0, 0.0);
 		SHADOW_CALCULATION(vs_in.light_pos[0], 0)
 		
 	} else if (vs_in.obj_depth < cascade_ends[1]) {
  		//ambient = vec3(0.0, 1.0, 0.0);
 		SHADOW_CALCULATION(vs_in.light_pos[1], 1)
 		
 	} else if (vs_in.obj_depth < cascade_ends[2]) {
  		//ambient = vec3(0.0, 0.0, 1.0);
 		SHADOW_CALCULATION(vs_in.light_pos[2], 2)
 		
 	} else {
 		ambient = vec3(1.0, 1.0, 0.0);
 		shadow = 0.0;
 	}
	
// 	for(int i = 4; i < 8; i++) {
// 		vec3 lc = l_coord.xyz / l_coord.w;//vec3(l_coord.xy / l_coord.w, 0.0);
// 		vec3 pe = points_edge[i].xyz / points_edge[i].w;//vec3(points_edge[i].xy / points_edge[i].w, 0.0);
// 		if(distance(lc, pe) < 0.1)
// 			ambient = vec3(0.0, 1.0, 1.0);
// 	}
// 	for(int i = 0; i < 4; i++) {
// 		vec3 lc = vec3(l_coord.xy / l_coord.w, 0.0);
// 		vec3 pe = vec3(points_edge[i].xy / points_edge[i].w, 0.0);
// 		if(distance(lc, pe) < 0.05)
// 			ambient = vec3(1.0, 1.0, 1.0);
// 	}
	
	float shadow_max = 0.65;
	if (shadow > shadow_max) {
		frag_color = vec4((ambient + shadow_max * diffuse)*color, 1.0);
	} else {
		frag_color = vec4((ambient + (1.0 - shadow_max) * (diffuse + specular))*color, 1.0);
	}
}


