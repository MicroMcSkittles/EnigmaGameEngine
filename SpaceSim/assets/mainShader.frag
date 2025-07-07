#version 460 core
out vec4 FragColor;

uniform sampler2D RasterFrame;
uniform sampler2D RasterDepthMap;
uniform sampler2D SceneData;

uniform vec2 Resolution;
uniform vec3 CameraPosition;
uniform mat4 InvProjection;
uniform mat4 InvView;

in vec2 TexCoord;

struct Ray {
    vec3 origin;
    vec3 direction;
};
struct RayHit {
    bool hit;
    float dist;
    vec3 point;
    vec3 normal;
    int iterationCount;
    int hitIndex;
};

const int SceneHeaderSize = 1;
const int SphereStructSize = 7;
const int PositionOffset = 0;
const int RadiusOffset = 3;
const int ColorOffset = 4;

const float nearPlane = 0.1;
const float farPlane = 100000;

float GetSphereCount() {
    return texelFetch(SceneData, ivec2(0,0), 0).r;
}
float ReadFloat(inout int pointer) {
    float rslt = texelFetch(SceneData, ivec2(pointer,0), 0).r;
    pointer += 1;
    return rslt;
}
vec3 GetSpherePosition(int index) {
    int pointer = index * SphereStructSize + SceneHeaderSize + PositionOffset;
    vec3 position;
    position.x = ReadFloat(pointer);
    position.y = ReadFloat(pointer);
    position.z = ReadFloat(pointer);
    return position;
}
float GetSphereRadius(int index) {
    int pointer = index * SphereStructSize + SceneHeaderSize + RadiusOffset;
    float radius = ReadFloat(pointer);
    return radius;
}
vec3 GetSphereColor(int index) {
    int pointer = index * SphereStructSize + SceneHeaderSize + ColorOffset;
    vec3 color;
    color.x = ReadFloat(pointer);
    color.y = ReadFloat(pointer);
    color.z = ReadFloat(pointer);
    return color;
}

// Blending Functions =================================
float smoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0, 1);
    return mix(d2,d1,h) - k*h*(1-h);
}
vec3 opTwist(vec3 p )
{
    const float k = 0.05; // or some other amount
    float c = cos(k*p.y);
    float s = sin(k*p.y);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xz,p.y);
    return q;
}

// SDFs ===============================================
float sdfSphere(vec3 point, float radius) {
    return length(point) - radius;
}
float sdfBox(vec3 point, vec3 b) {
    vec3 q = abs(point) - b;
    return length(max(q, 0)) + min(max(q.x, max(q.y, q.z)), 0);
}
float sdBoxFrame( vec3 p, vec3 b, float e )
{
    p = abs(p  )-b;
    vec3 q = abs(p+e)-e;
    return min(min(
        length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
        length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
        length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

float SDFtoWorld(vec3 point, inout RayHit rayHit) {
    //float closest = sdfSphere(point - GetSpherePosition(0), GetSphereRadius(0));

    float closestDist = 3.4028235e+38;
    
    int sphereCount = int(GetSphereCount());
    for (int i = 0; i < sphereCount; i++) {
        vec3 position = GetSpherePosition(i);
        float radius = GetSphereRadius(i);

        // Funcky thingy
        //float distB = sdfBox(point - position - vec3(radius* 0.15, radius* 0.15, radius* 0.15), vec3(radius* 0.15, radius* 0.15, radius* 0.15));
        //float distS = sdfSphere(point - position + radius * 0.1, radius * 0.1);
        //float distBF = sdBoxFrame(point - position, vec3(radius, radius, radius), radius * 0.1);
        //float dist = min(min(distBF, distS),distB);

        float dist = sdfSphere(point - position, radius);

        if(dist < closestDist) {
            closestDist = dist;
            rayHit.hitIndex = i;
        }
    }

    return closestDist;
}

// Magic normal function, idk how it works
vec3 CalculateNormal(vec3 point) {
    const float h = 1e-5;
    const vec2 k = vec2(1,-1);
    RayHit temp;
    return normalize(
        k.xyy * SDFtoWorld(point + k.xyy * h, temp) +
        k.yyx * SDFtoWorld(point + k.yyx * h, temp) +
        k.yxy * SDFtoWorld(point + k.yxy * h, temp) +
        k.xxx * SDFtoWorld(point + k.xxx * h, temp) 
    );
}

void CastRay(Ray ray, inout RayHit rayHit) {

    // Configure rayHit struct
    rayHit.hit = false;
    rayHit.dist = 0;
    rayHit.iterationCount = 0;

    // March ray
    for(int i = 0; i < 125; i++) {
        rayHit.point = ray.origin + ray.direction * rayHit.dist;
        float dist = SDFtoWorld(rayHit.point, rayHit);
        rayHit.dist += dist;
        rayHit.iterationCount += 1;

        // Exit loop if hit or if ray is to far from camera
        if (dist < 0.001) {
            rayHit.hit = true;
            rayHit.normal = CalculateNormal(rayHit.point);
            break;
        }
        if(rayHit.dist > farPlane) {
            rayHit.dist = 3.4028235e+38;
            break;
        }
    }

}

void ApplyLighting(RayHit rayHit, inout vec3 color) {
    const vec3 lightColor = vec3(1,1,1);
    //const vec3 lightPosition = vec3(10,10,10);
    //vec3 lightDirection = normalize(lightPosition - rayHit.point);
    const vec3 lightDirection = normalize(vec3(0.85,1.05,1));
    //vec3 lightDirection = normalize(GetSpherePosition(1) - rayHit.point);

    float ambientStrength = 0.1;
    vec3 ambient = lightColor * ambientStrength;

    float diff = max(dot(rayHit.normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.1;
    vec3 viewDirection = normalize(CameraPosition - rayHit.point);
    vec3 reflectDirection = reflect(-lightDirection, rayHit.normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 sphereColor = GetSphereColor(rayHit.hitIndex);

    Ray shadowRay;
    shadowRay.origin = rayHit.point + (rayHit.normal * 0.01);
    shadowRay.direction = lightDirection;

    RayHit shadowRayHit;
    CastRay(shadowRay, shadowRayHit);

    if (shadowRayHit.hit)
         color = (ambient * sphereColor);
    else
        color = (specular + diffuse + ambient) * sphereColor;

}

vec3 render(in vec2 uvCoord) {
    // Create ray
    Ray ray;
    ray.origin = CameraPosition;

    vec4 target = InvProjection * vec4(uvCoord.x, uvCoord.y, 1, 1);
    ray.direction = vec3(InvView * vec4(normalize(vec3(target) / target.w), 0));

    // Cast ray
    RayHit rayHit;
    CastRay(ray, rayHit);

    float rasterDepth = texture(RasterDepthMap, TexCoord).r;
    rasterDepth = (2 * nearPlane * farPlane) / (farPlane + nearPlane - (rasterDepth * 2.0 - 1.0) * (farPlane - nearPlane));

    // Calculate color
    vec3 color = vec3(0, 0, 0);

    if (rayHit.dist > rasterDepth) color = texture(RasterFrame, TexCoord).rgb;
    else if (rayHit.hit) {
        ApplyLighting(rayHit, color);
    }

    return color;
}

// Fix this function
vec2 getUV(vec2 offset) {
    vec2 uvCoord = (TexCoord * 2 - 1);
    uvCoord.x += offset.x * (1 / Resolution.x);
    uvCoord.y += offset.y * (1 / Resolution.y);
    return uvCoord;
}
vec3 renderAAx4() {
    vec4 e = vec4(0.125, -0.125, 0.375, -0.375);
    vec3 colorAA = render(getUV(e.xz)) + render(getUV(e.yw)) + render(getUV(e.wx)) + render(getUV(e.zy));
    return colorAA / 4.0;
}

void main() {
    
    // Convert TexCoord into normalized uv coords
    vec2 uvCoord = (TexCoord * 2 - 1);

    vec3 color = render(uvCoord);
    //vec3 color = renderAAx4();

    color = pow(color, vec3(0.7575));
    FragColor = vec4(color, 1);
    //FragColor = texture(SceneData, TexCoord);
    //FragColor = texture(RasterFrame, TexCoord);
    //FragColor = vec4(uvCoord, 0,1);
}