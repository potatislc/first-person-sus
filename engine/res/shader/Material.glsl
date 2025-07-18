#include ENGINE_RES_PATH/shader/Light.glsl

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shine;
};

struct MaterialMap {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shine;
};

Material sampleMap(MaterialMap material, vec2 texCoords) {
    Material sampled;
    sampled.diffuse = texture(material.diffuse, texCoords).rgb;
    sampled.specular = texture(material.specular, texCoords).rgb;
    sampled.emission = texture(material.emission, texCoords).rgb;
    sampled.shine = material.shine;
    return sampled;
}

vec3 calcAmbient(vec3 materialAmbient, vec3 lightAmbient) {
    return materialAmbient * lightAmbient;
}

vec3 calcPhongDiffuse(vec3 materialDiffuse, vec3 lightDir, vec3 lightColor, vec3 normal) {
    float diff = max(dot(normal, lightDir), 0.0);
    return lightColor * (diff * materialDiffuse);
}

vec3 calcPhongSpecular(vec3 materialSpecular, float materialShine, vec3 lightDir, vec3 lightColor, vec3 normal, vec3 viewDir) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShine);
    return lightColor * (spec * materialSpecular);
}

vec3 phongLighting(Material material, PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 direction = normalize(light.position - fragPos);
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    return calcAmbient(material.ambient, light.ambient * attenuation) +
    calcPhongDiffuse(material.diffuse, direction, light.diffuse * attenuation, normal) +
    calcPhongSpecular(material.specular, material.shine, direction, light.specular * attenuation, normal, viewDir) +
    material.emission;
}