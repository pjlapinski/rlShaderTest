/* Vert */
in vec3 _vPos;
in vec2 _vTexCoord;

uniform mat4 mvp;

out vec2 _fTexCoord;

void main(void) {
    _fTexCoord = _vTexCoord;
    gl_Position = mvp * vec4(_vPos, 1.);
}

/* Frag */
in vec2 _fTexCoord;

uniform sampler2D _tex0;

out vec4 _outColor;

vec2 resolution = vec2(1600., 900.);

void main(void) {
    float stepU = 1. / resolution.x;
    float stepV = 1. / resolution.y;

    vec4 center = texture(_tex0, _fTexCoord);
    vec4 right = texture(_tex0, _fTexCoord + vec2(stepU, 0.));
    vec4 bottom = texture(_tex0, _fTexCoord + vec2(0., stepV));

    float dFdX = length(right - center) / stepU;
    float dFdY = length(bottom - center) / stepU;

    float edge = dFdX + dFdY;

    vec4 canny = vec4(edge);
    _outColor = texture(_tex0, _fTexCoord) - canny;
    _outColor.a = 1.;
}
