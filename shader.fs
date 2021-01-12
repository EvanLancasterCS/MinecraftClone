#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);//vec4(0.0, 1.0, 1.0, 1.0);//texture(texture1, TexCoord);// * vec4(ourColor, 1.0); //mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(ourColor, 1.0);
}