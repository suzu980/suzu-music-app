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
		vec4 texelColor = texture(audioFreq, freqMap); 
		if (texelColor.r > fragTexCoord.y){
			finalColor = vec4(1,0,0,1);
		}else{
			finalColor = vec4(0.0);
		}
		


}
