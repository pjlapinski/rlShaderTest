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
uniform vec2 _resolution;
uniform float _outlineThickness;
uniform vec4 _edgeColor;

out vec4 _outColor;


float robertsCross(vec3 samples[4]) {
    vec3 diff1 = samples[1] - samples[2];
    vec3 diff2 = samples[0] - samples[3];

    return sqrt(dot(diff1, diff1) + dot(diff2, diff2));
}

bool is_edge() {
    vec2 texelSize = vec2(1. / _resolution.x,  1. / _resolution.y);
    float halfWidthF = floor(_outlineThickness * 0.5);
    float halfWidthC = ceil(_outlineThickness * 0.5);
    vec2 uvs[4];
    uvs[0] = _fTexCoord + texelSize * vec2(halfWidthF, halfWidthC) * vec2(-1., 1.);
    uvs[1] = _fTexCoord + texelSize * vec2(halfWidthC, halfWidthC) * vec2(1., 1.);
    uvs[2] = _fTexCoord + texelSize * vec2(halfWidthF, halfWidthF) * vec2(-1., -1.);
    uvs[3] = _fTexCoord + texelSize * vec2(halfWidthC, halfWidthF) * vec2(1., -1.);
    vec3 samples[4];
    for (int i = 0; i < 4; i++) {
        samples[i] = texture(_tex0, uvs[i]).xyz;
    }
    float edge = robertsCross(samples);
    float threshold = 1. / 10.;
    return edge > threshold;
}

void main(void) {
    vec4 t = texture(_tex0, _fTexCoord);
    int edge = is_edge() ? 1 : 0;
    _outColor = t * vec4(1 - edge) + _edgeColor * vec4(edge);
    _outColor.a = 1.;
}
