#version 330


in vec2 fragTexCoord;
in vec4 fragColor;


out vec4 finalColor;

uniform vec2 windowres;

void main()
{
		vec2 uv = fragTexCoord * 2.0 - 1.0;
		uv.x *= windowres.x / windowres.y;
		float d = length(uv);
		//vec2 uv = fragTexCoord;
		finalColor = vec4(d,d,d, 1.0);
}
