
#version 330 core
in vec2 f_texcoords;
in vec4 f_color;

uniform vec2 u_texture_size;
uniform sampler2D u_texture0;
uniform float u_scale;

out vec4 final_color;
vec2 subpixel_aa(vec2 uv, vec2 texture_size, float zoom)
{
    vec2 pixel = uv * texture_size;
    return (floor(pixel) + 1.5 - clamp((1.0 - fract(pixel)) * zoom, 0.0, 1.0)) / texture_size;
}

void main()
{
	vec2 uv = subpixel_aa(f_texcoords, u_texture_size, u_scale);
	//vec2 uv = f_texcoords;

	vec4 color = texture(u_texture0, uv) * f_color;

	final_color = color;
}



