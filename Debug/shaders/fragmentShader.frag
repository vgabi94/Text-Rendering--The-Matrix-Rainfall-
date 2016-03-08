#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 color;

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
vec4 illumination(in Light light, in vec3 eyePos, in vec3 FragPos, in vec3 Normal);

uniform vec4 objColor;
uniform vec3 eyePos;
uniform sampler2D objTexture, matrixTexture;
uniform float boundary, boundary2;
uniform bool isText, isMatrix;
uniform bool menu;
uniform Light light, lamp1, lamp2, lamp3;
const float SCALE_FACTOR = 1.f / 0.02f;

void main()
{
	vec4 result;
	if (!menu)
	{
		vec4 result1 = illumination(light, eyePos, FragPos, Normal);
		vec4 result2 = illumination(lamp1, eyePos, FragPos, Normal);
		vec4 result3 = illumination(lamp2, eyePos, FragPos, Normal);
		vec4 result4 = illumination(lamp3, eyePos, FragPos, Normal);
		result = result1 + result2 + result3 + result4;
	}
	vec4 texel = texture(objTexture, TexCoord);
	
	if (isText)
	{
		if (!menu && (FragPos.x >= 25.f || FragPos.x <= -25.f))
			discard;
		if (texel.rgb == vec3(0.0))
			discard;
		texel = objColor;
	}
	else if (isMatrix)
	{
		if (FragPos.y > boundary2) 
			discard;
		else 
		{
			vec4 texel2 = texture(matrixTexture, TexCoord);
			float eps = 4.f * 0.02f;
			float eps2 = -10.f * 0.02f;
			float coef2 = clamp((FragPos.y - boundary2) / eps2, 0.0f, 1.0f);
			float coef = clamp((FragPos.y - boundary) / eps, 0.0f, 1.0f);
			texel = coef * texel + (1.f - coef) * texel2;	
			texel = coef2 * texel;
			if (texel.rgb == vec3(0.0))
				discard;
		}
	}
	
	if (menu || isMatrix)
		color = texel;
	else
		color = texel * result;
}