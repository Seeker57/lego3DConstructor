#version 130

in vec4 vertex;				//вершина
in vec3 normal;				//вектор нормали к вершине
in vec4 color;				//цвет вершины
in vec2 texture;			//вектор координат текстур
uniform mat4 matrix;			//результирующая матрица
uniform mat3 normalMatrix;		//матрица для умножения векторов нормали
uniform mat4 modelViewMatrix;		//видовая матрица (для расчета освещения)

//параметры, которые будут  переданы далее по конвейеру
out vec4 fragColor;		//цвет вершины
out vec3 fragNormal;		//вектор нормали
out vec3 fragPos;		//позиция вершины
out vec2 texCoord;		//координаты текстуры

void main(void) {

   gl_Position = matrix * vertex;

   fragColor = color;
   fragPos = vec4(modelViewMatrix * vertex).xyz;
   fragNormal = normalMatrix * normal;
   texCoord = texture;
}
