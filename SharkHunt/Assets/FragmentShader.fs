#version 330

// uniform GLuint texCoordID;
// uniform GLuint texID;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
    vec2 uv_ES;       // Eye-space uv
	LightSource light;
} fs_in;

out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

uniform sampler2D sampler;

vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
	diffuse = material.kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }

    return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

void main() {
    vec3 lighting = phongModel(fs_in.position_ES, fs_in.normal_ES);
    // Only include texture coords if the model uses uv coords
    // if (fs_in.uv_ES) {
        vec3 texCoord = texture(sampler, fs_in.uv_ES).rgb;
        fragColour = vec4(lighting * texCoord, 1.0);
    // } else {
    //     fragColour = vec4(lighting, 1.0);
    // }
}
