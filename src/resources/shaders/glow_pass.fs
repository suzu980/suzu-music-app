#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D original;
uniform sampler2D blurred;
uniform float exposure;

out vec4 finalColor;

void main()
{
		//const float gamma = 2.2;
    vec3 originalColor = texture(original, fragTexCoord).rgb;      
    vec3 bloomColor = texture(blurred, fragTexCoord).rgb;
    //originalColor += bloomColor; // additive blending
		originalColor = bloomColor+originalColor-(originalColor * bloomColor);
    // tone mapping
    finalColor = vec4(originalColor, 1.0);
}
