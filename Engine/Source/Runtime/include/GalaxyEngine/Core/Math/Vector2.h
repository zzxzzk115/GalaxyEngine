//
// Vector2.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Core/Macro.h"

#include <cmath>

namespace Galaxy::Math
{
    struct Vector2
    {
        float X;
        float Y;

        Vector2() = default;
        Vector2(float x, float y) : X(x), Y(y) {}
        Vector2(const Vector2& other)
        {
            X = other.X;
            Y = other.Y;
        }

        inline Vector2 operator + (const Vector2& other) const
        {
            return { X + other.X, Y + other.Y };
        }

        inline Vector2 operator - (const Vector2& other) const
        {
            return { X - other.X, Y - other.Y };
        }

        inline float operator * (const Vector2& other) const
        {
            return DotProduct(other);
        }

        inline Vector2 operator * (float scalar) const
        {
            return { X * scalar, Y * scalar };
        }

        inline Vector2 operator / (float scalar) const
        {
            // divided by zero!
            if (std::abs(scalar) < FlOAT_NUMBER_EPSILON)
            {
                throw std::runtime_error("Mathematical error: Divided by zero!");
            }
            return { X / scalar, Y / scalar };
        }

        inline bool operator == (const Vector2& other) const
        {
            return X == other.X && Y == other.Y;
        }

        inline float GetLength() const
        {
            return std::hypot(X, Y);
        }

        inline Vector2 Normalized() const
        {
            if (abs(X) < FlOAT_NUMBER_EPSILON && abs(Y) < FlOAT_NUMBER_EPSILON)
            {
                return {X, Y};
            }
            float length = GetLength();
            return {X / length, Y / length };
        }

        inline float DotProduct(const Vector2& other) const
        {
            return X * other.X + Y * other.Y;
        }

        inline void static Normalize(Vector2& v)
        {
            v = v.Normalized();
        }

        const static Vector2 Zero;
        const static Vector2 One;
    };
} // namespace Galaxy::Math