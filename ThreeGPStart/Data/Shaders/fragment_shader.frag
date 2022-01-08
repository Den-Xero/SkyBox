#version 330

//uniform sampler2D sampler_tex;

uniform vec4 diffuse_colour;
uniform sampler2D JeepSamplerTex;

in vec2 varyingCoords;
in vec3 VaryingPosition;
//vec3 N = normalize(varying_colour);

out vec4 fragment_colour;

void main(void)
{
	vec3 LightColour = vec3(1,1,1);
	float LightRange = 100;
	float MatShininess = 200;
	vec3 VaryingNormals = vec3(0,1,0);

	float SpotlightFOV = 1.1;
	vec3 SpotlightDir = normalize(vec3(0,1,0));

	vec3 MatTexColour = texture(JeepSamplerTex, varyingCoords).rgb;
	
	vec3 MatAmibentColour = MatTexColour;
	vec3 MatSpecColour = MatTexColour;

	vec3 P = VaryingPosition;
	vec3 N = normalize(VaryingNormals);

	vec3 light_direction = vec3(-0.7, -0.4, 0);
//	vec3 PointLight_Position = vec3 (500, 500, 150);
//	vec3 SpotLightPosition = vec3 (1800,1800,150);
//	vec3 Pointlight_direction = P - PointLight_Position;
//	vec3 SpotlightDirection = P - SpotLightPosition;

	vec3 L = normalize(-light_direction);
	float DirectionLightIntensity = max(0, dot(L,N));

//	float LightDistance = distance(PointLight_Position, P);
//	float Attenuation = 1.0 - smoothstep(0,LightRange,LightDistance);
//
//	vec3 SL = normalize(-SpotlightDirection);
//	float SpotLightIntensity = max(0, dot(SL,N));
//
//	float FC = smoothstep(cos(0.5 * SpotlightFOV), 1, dot(SpotlightDirection, SpotlightDir));
//	FC *= Attenuation;

//	SpotLightIntensity *= FC;
//
//	vec3 PL = normalize(-Pointlight_direction);
//	float PointLightIntensity = max(0, dot(PL,N));
//
//	vec3 DifColour = MatTexColour * PL;
	vec3 AmbientLight = vec3(0.25, 0.25, 0.25);
	vec3 Ambient = AmbientLight * MatAmibentColour;


	//vec3 FinalTexColour = (((PointLightIntensity + DirectionLightIntensity + SpotLightIntensity) * MatTexColour + Ambient * Attenuation) * LightColour);
	vec3 FinalTexColour = ((DirectionLightIntensity * MatTexColour + Ambient) * LightColour);

	fragment_colour = vec4(FinalTexColour,1.0);
}