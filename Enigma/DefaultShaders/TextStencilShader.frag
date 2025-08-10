#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in float CharID;

// TextData memory layout
// glyph size
// contour count
//
// Note: All contours are back to back
// contour start
// contour end
// 
// Note: All points are back to back   
// point x
// point y
uniform sampler2D TextData;

uniform int StencilID;

const float Epsilon = 0.001;

const int HeaderSize  = 2; // GlyphSize, ContourCount
const int ContourSize = 2; // Start, End
const int PointSize   = 2; // X, Y
const int BezierSize  = 4; // p0, p1, p2

struct Contour {
    int start;
    int end;
    int bezierCount;
};
struct Bezier {
    vec2 p0;
    vec2 p1;
    vec2 p2;
};

vec2 MinPoint;
vec2 MaxPoint;
vec2 CurrentPoint;

int CharOffset;
int PointsOffset;
int ContourCount;

vec3 test;

bool equalf(float f1, float f2) {
    if(f1 < f2 + Epsilon && f1 > f2 - Epsilon) {
        return true;
    }
    return false;
}

float ReadDataF(int offset) {
    return texelFetch(TextData, ivec2(offset,0), 0).r;
}
int ReadDataI(int offset) {
    return int(texelFetch(TextData, ivec2(offset,0), 0).r);
}

void CalcOffsets() {
    // TODO: try to pack the character offsets at the start of the data
    // so that you could just use the CharID to get the offset
    // Character offset
    CharOffset = 0;
    int i = int(CharID);
    while (i != 0) {
        CharOffset += ReadDataI(CharOffset);
        i -= 1;
    }
    CharOffset += HeaderSize;
    // Contour count
    ContourCount = ReadDataI(CharOffset - 1);

    CurrentPoint = TexCoord.xy * 2 - 1;
    //CurrentPoint.x = (TexCoord.x * (MaxPoint.x - MinPoint.x) - MinPoint.x);
    //CurrentPoint.y = (TexCoord.y * (MaxPoint.y - MinPoint.y) - MinPoint.y);

    // Points offset
    PointsOffset = CharOffset + (ContourCount * ContourSize);

}
void ReadContour(inout Contour contour, int index) {
    int offset = CharOffset + index * ContourSize;
    contour.start = ReadDataI(offset); offset += 1;
    contour.end = ReadDataI(offset);
    contour.bezierCount = int(ceil((contour.end - contour.start - 1.0) / 2.0));
}
void ReadBezier(inout Bezier bezier, Contour contour, int index) {
    int offset = PointsOffset + contour.start * 2 + index * BezierSize;
    bezier.p0.x = ReadDataF(offset); offset += 1;
    bezier.p0.y = ReadDataF(offset); offset += 1;
    bezier.p1.x = ReadDataF(offset); offset += 1;
    bezier.p1.y = ReadDataF(offset); offset += 1;
    if(index == contour.bezierCount - 1) {
        offset = PointsOffset + contour.start * 2;
    }
    bezier.p2.x = ReadDataF(offset); offset += 1;
    bezier.p2.y = ReadDataF(offset);
}

bool TestBezier(Bezier bezier, float t, inout vec2 point) {
    if (t < 0.0 || t >= 1.0) return false;
    vec2 L0 = mix(bezier.p0, bezier.p1, t);
    vec2 L1 = mix(bezier.p1, bezier.p2, t);
    vec2 Q0 = mix(L0, L1, t);
    if (Q0.x > CurrentPoint.x) {
        point = Q0;
        return true;
    }
    return false;
}

const float intersectEpsilon = 1e-16;
vec2 previousIntersectPoint0;
vec2 previousIntersectPoint1;
void CountBezierIntersections(inout int count, Contour contour, Bezier bezier) {
    // Don't check bezier if all points are behind the current pixel
    if(bezier.p0.x < CurrentPoint.x && bezier.p1.x < CurrentPoint.x && bezier.p2.x < CurrentPoint.x) return;

    float a = bezier.p0.y - (2.0 * bezier.p1.y) + bezier.p2.y;
    float b = 2.0 * (bezier.p1.y - bezier.p0.y);
    float c = bezier.p0.y - CurrentPoint.y;

    if (equalf(a, 0.0)) {
        float t = -c / b;
        vec2 point;
        if (TestBezier(bezier, t, point)) {
            float dist = min(length(point - previousIntersectPoint0), length(point - previousIntersectPoint1));
            if (dist * dist > intersectEpsilon) count += 1;
            previousIntersectPoint0 = point;
            previousIntersectPoint1 = point;
        }
        return;
    }

    float discriminant = sqrt((b * b) - (4.0 * a * c));
    if (discriminant < 15e-4) return;

    float t1 = (-b + discriminant) / (2.0 * a);
    float t2 = (-b - discriminant) / (2.0 * a);

    vec2 point;
    if (TestBezier(bezier, t1, point)) {
        float dist = min(length(point - previousIntersectPoint0), length(point - previousIntersectPoint1));
        if (dist * dist > intersectEpsilon) count += 1;
        previousIntersectPoint0 = point;
    }
    if (TestBezier(bezier, t2, point)) {
        float dist = min(length(point - previousIntersectPoint0), length(point - previousIntersectPoint1));
        if (dist * dist > intersectEpsilon) count += 1;
        previousIntersectPoint1 = point;
    }
}
void CountContourIntersections(inout int count, Contour contour) {
    for(int i = 0; i < contour.bezierCount; i++) {
        Bezier bezier;
        ReadBezier(bezier, contour, i);
        CountBezierIntersections(count, contour, bezier);
    }
}

void main() {

    float stencil = 1;

    previousIntersectPoint0 = vec2(0);
    previousIntersectPoint1 = vec2(0);
    CalcOffsets();

    int intersections = 0;
    for(int i = 0; i < ContourCount; i++) {
        Contour contour;
        ReadContour(contour, i);
        CountContourIntersections(intersections, contour);
    }

    //if ((intersections % 2) == 0) stencil = 0;
    if ((intersections % 2) == 0) discard;

    // Pack stencil data to texture
    int rslts = int(floor(stencil * 0xFF));
    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);
    FragColor = vec4(rslt);
}