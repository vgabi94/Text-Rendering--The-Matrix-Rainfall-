#version 330 core

struct Light
{
	vec3 position;
	vec3 direction; // pointing from light source!
	vec3 color;
	
	float cutOff;
	float outerCutOff;
	
	float ambient;
	float specular;
	float shininess;
	
	float constant;
	float linear;
	float quadratic;
	
	int type; // 0 = point ; 1 = directional; 2 = spotlight
	bool blinn; // use Blinn model?
	bool atten; // has attenuation?
	bool off; // is it off?
};

vec4 illumination(in Light light, in vec3 eyePos, in vec3 FragPos, in vec3 Normal)
{
	if (light.off)
		return vec4(0.0);
	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(eyePos - FragPos);
	vec3 lightDir;
	float spec = 0;
	
	// Directional Light
	if (light.type == 1)
		lightDir = light.direction;
	else
		lightDir = normalize(light.position - FragPos);
	
	// Ambient
	vec3 ambient = light.ambient * light.color;
	
	// Diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.color;
	
	// Specular
	if (light.blinn) 
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(norm, halfwayDir), 0.0), light.shininess); 
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, norm);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shininess);
	}
	vec3 specular = light.specular * spec * light.color;
	
	// Spotlight with soft edges
	if (light.type == 2)
	{
		float theta = dot(lightDir, -normalize(light.direction));
		float eps = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / eps, 0.0f, 1.0f);
		diffuse *= intensity;
		specular *= intensity;
	}
	
	// Attenuation
	if (light.atten)
	{
		float dist = distance(light.position, FragPos);
		float att = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
		ambient *= att;
		diffuse *= att;
		specular *= att;
	}
	
	return vec4(ambient + diffuse + specular, 1.0);
}