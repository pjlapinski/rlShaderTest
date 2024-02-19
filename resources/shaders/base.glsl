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

void main() {
    _outColor = texture(_tex0, _fTexCoord);
    _outColor = vec4(_outColor.rgb, abs(sin(_time)));
}
