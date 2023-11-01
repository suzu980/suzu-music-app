#version 330


in vec2 fragTexCoord;
in vec4 fragColor;


out vec4 finalColor;

uniform vec2 windowres;
uniform sampler2D audioFreq;

void main()
{
		vec2 freqMap = fragTexCoord;
		freqMap.y *= 0.5;
		freqMap.y -= 0.5; 
		float t = fragTexCoord.x/1.0;
		vec4 texelColor = texture(audioFreq, freqMap); 
		vec3 color1 = vec3(0.95,0.54,0.65);
		vec3 color2 = vec3(0.53,0.7,0.98);
		vec3 mixedColor = mix(color1,color2,t);

		if (texelColor.r > fragTexCoord.y){
			finalColor = vec4(mixedColor,texelColor.r);
		}else{
			finalColor = vec4(0.0);
		}
		


}
