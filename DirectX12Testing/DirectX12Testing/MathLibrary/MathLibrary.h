#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#undef max
#undef min

#include <cstdlib> // std::rand, std::srand
#include <ctime>   // std::time

#include <algorithm>

#include "Transform.h"
#define RANDOMFLOAT rand() / (float)RAND_MAX
#define RANDOMDOUBLE rand() / (double)RAND_MAX

//namespace MathConstants
//{
//    constexpr float kEpsilon = 0.001f;
//}

class MathLibrary
{
    public:
        // Common Math Function
        static float RandomFloatInRange(float min, float max);
        template <typename T>
        static T Warprange(T val, T min, T max)
        {
                return val - (max - min) * floor(val / (max - min));
        }

        /* wrap x -> [0,max) */
        static double wrapMax(double x, double max)
        {
                /* integer math: `(max + x % max) % max` */
                return fmod(max + fmod(x, max), max);
        }
        /* wrap x -> [min,max) */
        static double wrapMinMax(double x, double min, double max)
        {
                return min + wrapMax(x - min, max - min);
        }

        static DirectX::XMVECTOR WrapPosition(const DirectX::XMVECTOR& pos,
                                              const DirectX::XMVECTOR& min,
                                              const DirectX::XMVECTOR& max);
        // Matrix Function
        static void              OrthoNormalize(DirectX::XMVECTOR normal, DirectX::XMVECTOR& tangent);
        static DirectX::XMMATRIX LookAt(DirectX::XMVECTOR vPos, DirectX::XMVECTOR tPos, DirectX::XMVECTOR up);
        static void              TurnTo(DirectX::XMMATRIX& matrix, DirectX::XMVECTOR targetPosition, float speed);

        // Vector Function
        static DirectX::XMVECTOR GetClosestPointFromLine(DirectX::XMVECTOR startPoint,
                                                         DirectX::XMVECTOR endPoint,
                                                         DirectX::XMVECTOR point);
        static DirectX::XMVECTOR GetClosestPointFromLineClamped(DirectX::XMVECTOR startPoint,
                                                                DirectX::XMVECTOR endPoint,
                                                                DirectX::XMVECTOR point);

        static DirectX::XMVECTOR GetMidPointFromTwoVector(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateDistance(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateDistanceSq(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateDistanceIgnoreY(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateDistanceSqIgnoreY(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateVectorLength(DirectX::XMVECTOR vector);
        static float             VectorDotProduct(DirectX::XMVECTOR m, DirectX::XMVECTOR n);
        
        // Get random values
        static DirectX::XMVECTOR GetRandomPointInRadius(const DirectX::XMVECTOR& center, float innerRadius, float outerRadius);
        static DirectX::XMVECTOR GetRandomPointInArc(const DirectX::XMVECTOR& center,
                                                     const DirectX::XMVECTOR& forward,
                                                     const DirectX::XMVECTOR& up,
                                                     float                    maxAngle,
                                                     float                    minDistance,
                                                     float                    maxDistance);
        static DirectX::XMVECTOR GetRandomPointInRadius2D(const DirectX::XMVECTOR& center,
                                                          float                    innerRadius,
                                                          float                    outerRadius);

        static float CalculateAngleBetweenVectors(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b);

        static float             GetRandomFloat();
        static float             GetRandomFloatInRange(float min, float max);
        static int               GetRandomIntInRange(int min, int max);
        static double            GetRandomDouble();
        static double            GetRandomDoubleInRange(double min, double max);
        static DirectX::XMVECTOR GetRandomVector();
        static DirectX::XMVECTOR GetRandomVectorInRange(float min, float max);
        static DirectX::XMVECTOR GetRandomUnitVector();
        static DirectX::XMVECTOR GetRandomUnitVector2D();
        static DirectX::XMFLOAT4 GetRandomColorInRange(DirectX::XMFLOAT2 red,
                                                       DirectX::XMFLOAT2 green,
                                                       DirectX::XMFLOAT2 blue,
                                                       DirectX::XMFLOAT2 alpha);
        static DirectX::XMFLOAT4 GetRandomColor();
        static DirectX::XMFLOAT3 MultiplyFLoat3(DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b);
        inline static float      GetSign(float val)
        {
                return (val == 0) ? 0.0f : (val > 0 ? 1.0f : -1.0f);
        }

        inline static bool QuadraticFormula(const float a,
                                            const float b,
                                            const float c,
                                            float&      r1, // first
                                            float&      r2  // and second roots
        )
        {
                const float q = b * b - 4 * a * c;
                if (q >= 0)

                {


                        const float sq = sqrtf(q);
                        const float d  = 1 / (2 * a);
                        r1             = (-b + sq) * d;
                        r2             = (-b - sq) * d;
                        return true; // real roots
                }

                else

                {
                        return false; // complex roots
                }
        };

        static DirectX::XMFLOAT3 NormalizeEulerAngles(const DirectX::XMFLOAT3& euler);

        static float NormalizeAngle(float a0);
        static float ShortestAngle(float a0, float a1);
        static float LerpAngle(float a0, float a1, float t);
        static float MoveTowardsAngle(float a0, float a1, float speed);

        static float             MoveTowards(const float a, const float b, const float speed);
        static DirectX::XMVECTOR MoveTowards(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const float speed);
        static DirectX::XMVECTOR MoveVectorColorTowards(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const float speed);

        static double SmoothStart2(double x);
        static double SmoothStop2(double x);

        static double SmoothStart3(double x);
        static double SmoothStop3(double x);

        static double SmoothStart4(double x);
        static double SmoothStop4(double x);

        static double SmoothStartN(double x, double power);
        static double SmoothStopN(double x, double power);

        static double SmoothMix(double a, double b, double blend);
        static double SmoothCrossfade(double a, double b, double time);

        template <typename T>
        static T lerp(T v0, T v1, T t)
        {
                return fma(t, v1, fma(-t, v0, v0));
        }

        template <typename T>
        static T clamp(const T& n, const T& lower, const T& upper)
        {
                return std::max(lower, std::min(n, upper));
        }

        static float saturate(float n)
        {
                return std::max(0.0f, std::min(n, 1.0f));
        }

        template <typename T>
        static void NormalizeArray(size_t size, T* data)
        {
                T sum = 0.0f;
                for (int i = 0; i < size; ++i)
                        sum += data[i];

                for (int i = 0; i < size; ++i)
                        data[i] /= sum;
        }

        template <typename T>
        static void NormalizeValues(T& a, T& b)
        {
                if (a == 0 && b == 0)
                {
                        a = 0.0f;
                        b = 0.0f;
                        return;
                }

                T sum = a + b;
                a /= sum;
                b /= sum;
        }

        template <typename T>
        static void AddNumberToVector(DirectX::XMVECTOR& vector, T number)
        {
                float x = DirectX::XMVectorGetX(vector) + number;
                float y = DirectX::XMVectorGetY(vector) + number;
                float z = DirectX::XMVectorGetZ(vector) + number;
                vector  = DirectX::XMVectorSet(x, y, z, DirectX::XMVectorGetW(vector));
        }

};