// Playground: edit freely and `cmake --build build && ./build/sandbox`.
// The commented-out lines demonstrate operations that intentionally do not
// compile (restrict design) — uncomment one to see the error.
#include <cstdio>

#include <shader_math/shader_math.hpp>

static void hlsl_demo() {
    using namespace hlsl;

    float3x3 mat1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float3x3 mat2 = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    float3x3 h = mat1 * mat2;       // component-wise (Hadamard)
    float3x3 p = mul(mat1, mat2);   // true matrix product
    std::printf("hlsl: (mat1*mat2)[0][0]=%g  mul(mat1,mat2)[0][0]=%g\n",
                static_cast<double>(h[0][0]), static_cast<double>(p[0][0]));

    float3 v(1.0f, 2.0f, 3.0f);
    float3 row = mul(v, mat1);  // row vector * matrix
    float3 col = mul(mat1, v);  // matrix * column vector
    std::printf("hlsl: mul(v,M)=(%g %g %g)  mul(M,v)=(%g %g %g)\n",
                static_cast<double>(row[0]), static_cast<double>(row[1]),
                static_cast<double>(row[2]), static_cast<double>(col[0]),
                static_cast<double>(col[1]), static_cast<double>(col[2]));

    float4 q(1.0f, 2.0f, 3.0f, 4.0f);
    float3 sw = q.zyx;  // swizzle
    q.xy = q.yx;        // swizzle write with aliasing
    std::printf("hlsl: q.zyx=(%g %g %g)  q after xy=yx: (%g %g)\n",
                static_cast<double>(sw[0]), static_cast<double>(sw[1]),
                static_cast<double>(sw[2]), static_cast<double>(q[0]),
                static_cast<double>(q[1]));

    if (all(p == p)) std::printf("hlsl: comparisons reduce with all()/any()\n");

    // ---- these do not compile (uncomment to see the errors) ----
    // float3x4 a; float4x3 b; auto bad1 = a * b;     // '*' is same-shape only; use mul()
    // if (v == v) {}                                  // bool3 is not bool; use all()/any()
    // q.xx = float2(1, 2);                            // duplicate swizzle write
    // float2 t = v;                                   // truncation must be explicit
}

static void glsl_demo() {
    using namespace glsl;

    mat3 m = {1, 4, 7, 2, 5, 8, 3, 6, 9};  // column-major fill
    mat3 id(1.0f);                          // scalar = diagonal: identity
    vec3 v(1.0f, 2.0f, 3.0f);

    mat3 p = m * id;   // '*' is the matrix product
    vec3 mv = m * v;   // matrix * column vector
    vec3 vm = v * m;   // row vector * matrix
    std::printf("glsl: (m*id)[0][0]=%g  m*v=(%g %g %g)  v*m=(%g %g %g)\n",
                static_cast<double>(p[0][0]), static_cast<double>(mv[0]),
                static_cast<double>(mv[1]), static_cast<double>(mv[2]),
                static_cast<double>(vm[0]), static_cast<double>(vm[1]),
                static_cast<double>(vm[2]));

    mat3 h = matrixCompMult(m, m);  // component-wise lives in a named function
    std::printf("glsl: matrixCompMult(m,m)[0][0]=%g  det(m)=%g\n",
                static_cast<double>(h[0][0]), static_cast<double>(determinant(m)));

    if (m == m) std::printf("glsl: == is a single bool\n");

    // ---- these do not compile (uncomment to see the errors) ----
    // mat4x2 w; auto bad2 = m * w;   // inner dimensions do not match
    // auto bad3 = v < v;             // GLSL has no vector '<'; use lessThan()
}

int main() {
    hlsl_demo();
    glsl_demo();
    return 0;
}
