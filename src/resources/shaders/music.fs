#version 330


in vec2 fragTexCoord;
in vec4 fragColor;


out vec4 finalColor;

uniform vec2 windowres;
uniform sampler2D audioFreq;

void main()
{
		vec2 freqMap = fragTexCoord;
		vec2 spectMap = fragTexCoord;
		freqMap.y *= 0.5;
		freqMap.y -= 0.5; 
		spectMap.y *= 0.5;
		float t = fragTexCoord.x/1.0;
		float tbg = fragTexCoord.y/1.0;
		vec4 texelColor = texture(audioFreq, freqMap); 
		vec4 spectColor = texture(audioFreq, spectMap); 
		vec4 bg = vec4(0.12,0.12 ,0.18, 1);
		vec3 color1 = vec3(0.95,0.54,0.65);
		vec3 color2 = vec3(0.53,0.7,0.98);
		vec4 transparent = vec4(0.0,0.0,0.0,0.0);
		vec3 mixedColor = mix(color1,color2,t);
		vec4 mixedbgColor = mix(bg,transparent,tbg);

			finalColor = vec4(mixedbgColor);
		if (spectColor.g > fragTexCoord.y*0.995 && spectColor.g < fragTexCoord.y*1.0){
			finalColor = vec4(0.8,0.83,0.95,1.0);
		}
		if (texelColor.r > fragTexCoord.y){
			finalColor = vec4(mixedColor,texelColor.r);
		}

}
