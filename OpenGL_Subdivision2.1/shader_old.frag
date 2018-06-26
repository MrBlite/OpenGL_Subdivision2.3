#version 430
out vec4 color;

void main()
{
	vec3 objectColor = vec3(0.3f, 0.2f, 0.55f);//模型颜色
	vec3 lightColor = vec3(1.0f,1.0f,1.0f);//光照颜色
	float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor;
    color = vec4(result, 1.0);
	//color = vec4(0.3f, 0.2f, 0.55f, 1.0f);
}
