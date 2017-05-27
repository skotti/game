#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[3];
    float ClipSpacePosz;
} fs_in;

uniform DirLight dirLight;
uniform sampler2D shadowMaps[3];
uniform float cascadeEnds[3];

uniform vec3 lightPos;
uniform vec3 viewPos;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float ShadowCalculation(vec4 fragPosLightSpace, int index)
{

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap[i], projCoords.xy).r; 

    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-dirLight.direction);//lightPos - fs_in.FragPos);
    //float bias =  max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float bias = 0.005*tan(acos(dot(normal, lightDir))); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0,0.01);


    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[i], 0)/5;
    for(int x = -4; x <= 4; ++x)
    {
        for(int y = -4; y <= 4; ++y)
        {
            float pcfDepth = texture(shadowMap[i], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 81.0*2;
    

    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return (shadow);
}

void main()
{           
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.4);
    // Ambient
    vec3 ambient = 0.2*vec3(0.4);
    // Diffuse
    vec3 lightDir = normalize(-dirLight.direction);//ightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    float spec = 0.0;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * lightColor;
    
    float shadow = 0.0;
    for (int i = 0; i < 3; i++) {
        if (vs_in.ClipSpacePosz <= cascadeEnds[i])
            shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    }
    shadow = min(shadow, 0.65);
    FragColor = vec4(ambient + (1-shadow)*(specular + diffuse), 1.0);
}


