#version 330

// blur shader

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D image;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 finalColor;

void main()
{
	vec3 result = texture(image, fragTexCoord).rgb;
	float brightness = dot(result.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.15)
		finalColor = vec4(result, 1.0);	
	else
		finalColor = vec4(0.0,0.0,0.0,1.0); 

}
