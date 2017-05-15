#version 330 core


out vec4 frag_color;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const int NUM_CASCADES = 3;

in VS_OUT {
	vec3 model_pos;
	vec3 normal;
	vec2 tex_coords;
	vec4 light_pos[NUM_CASCADES];
	float obj_depth;
} vs_in;

uniform DirLight dir_light;
uniform sampler2D shadow_maps[NUM_CASCADES];
uniform float cascade_ends[NUM_CASCADES];

uniform vec3 light_pos;//light position
uniform vec3 view_pos;//camera pos

#define SHADOW_CALCULATION(pos_in_light_space, i) {\
	vec3 proj_coords = pos_in_light_space.xyz / pos_in_light_space.w;\
	proj_coords = proj_coords * 0.5 + 0.5;\
	/*if ((i ==0 ) && !(proj_coords.x <= 0 || proj_coords.x >= 1.0 || proj_coords.y <= 0 || proj_coords.y >= 1.0)) {\
		ambient = vec3(0.0, proj_coords.z, proj_coords.z);\
	}*/\
	\
	float closest_depth = texture(shadow_maps[i], proj_coords.xy).r;\
	\
	float current_depth = proj_coords.z;\
	\
	vec3 normal = normalize(vs_in.normal);\
	vec3 rev_dir_light = normalize(-dir_light.direction);\
	\
	float bias = 0.001*tan(acos(dot(normal, rev_dir_light)));\
	bias = clamp(bias, 0,	0.01);\
	\
	shadow = 0.0;\
	vec2 texel_size = 1.0 / textureSize(shadow_maps[i], 0) / 5.0;\
	for(int x = -4; x <= 4; ++x)\
	{\
		for(int y = -4; y <= 4; ++y)\
		{\
			float pcf_pepth = texture(shadow_maps[i], proj_coords.xy + vec2(x, y) * texel_size).r;\
			shadow += ((current_depth - bias) >= pcf_pepth) ? 1.0 : 0.0;\
		}\
	}\
	shadow /= 81.0;\
	if (proj_coords.z > 1.0) {\
		/*light_color = vec3(0.4, 0.0, 0.4);*/\
		shadow = 0.0;\
	}\
}

void main()
{           
	vec3 normal = normalize(vs_in.normal);
	//vec3 light_color = 1.2*vec3(0.1745, 0.01175, 0.01175);
	// Ambient
	vec3 ambient = vec3(0.1745, 0.01175, 0.01175);
	// Diffuse
	vec3 rev_dir_light = normalize(light_pos - vs_in.model_pos);
	float diffuse_coeff = max(dot(rev_dir_light, normal), 0.0);
	vec3 diffuse = diffuse_coeff * vec3(0.61424, 0.04136, 0.04136);
	// Specular
	float spec_coeff = 0.0;
	vec3 view_dir = normalize(view_pos - vs_in.model_pos);
	vec3 halfway_dir = normalize(rev_dir_light + view_dir);
	bool light_side = dot(rev_dir_light, normal) > 0.0;
	if (light_side) {
		spec_coeff = pow(max(dot(normal, halfway_dir), 0.0), 0.6);
	}
	vec3 specular = spec_coeff * vec3(0.727811, 0.626959, 0.626959);
	
	float shadow = 0.0;
	
	if (vs_in.obj_depth < cascade_ends[0]) {
		
		SHADOW_CALCULATION(vs_in.light_pos[0], 0)
		//ambient = vec3(0.4, 0.0, 0.0);
	} else if (vs_in.obj_depth < cascade_ends[1]) {
		SHADOW_CALCULATION(vs_in.light_pos[1], 1)
		//ambient = vec3(0.0, 0.4, 0.0);
	} else if (vs_in.obj_depth < cascade_ends[2]) {
		SHADOW_CALCULATION(vs_in.light_pos[2], 2)
		//ambient = vec3(0.0, 0.0, 0.4);
	} else {
		//ambient = vec3(0.0, 1.0, 1.0);
		shadow = 0.0;
	}
	
	shadow = min(shadow, 0.65);
	frag_color = vec4(ambient + (1.0 - shadow)*(specular + diffuse), 1.0);	
}


