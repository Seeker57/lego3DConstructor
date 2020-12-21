#version 130

uniform vec4 color;
uniform sampler2D Tex1;

struct LightInfo {

    vec3 pos;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 objectColor;
};

uniform LightInfo lights[3];

in vec3 fragNormal;
in vec3 fragPos;
in vec4 fragColor;
in vec2 texCoord;

float Kconst = 1.0;
float Klinaer = 0.01;
float Kquad = 100;

out vec4 varColor;

void main(void) {

   vec3 result;
   vec4 texColor = texture2D( Tex1, texCoord);

   for (int i = 0; i < 3; i++ ) {

        vec3 lightDir = normalize(lights[i].pos - fragPos);   	//направление источника
        float power = max(dot(fragNormal, lightDir), 0.0);	//сила cвета по модели Ламберта
        vec3 diffuse = power * lights[i].diffuseColor;

        float distance = length(lights[i].pos - fragPos);
        float atten = 1.0 / (Kconst + Klinaer * distance + Kquad * distance * distance);

        if (i == 2)
                result += (lights[i].ambientColor * atten + diffuse * atten);
        else
                result += (lights[i].ambientColor + diffuse) * lights[i].objectColor;

   }

   result += color.xyz * texColor.xyz;
   varColor = vec4(result, 1.0f);
}
