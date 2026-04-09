//
// Optimized for GymFlex PUBG (Unreal Engine Math)
// Tetap pakai nama Unity.h biar gak ngerusak struktur include lama
//

#ifndef ZYGISKPG_UNITY_H
#define ZYGISKPG_UNITY_H

#include <math.h>

// =======================================================
// 📐 VECTOR 2 (Untuk Koordinat Layar / 2D)
// =======================================================
struct Vector2 {
    float X;
    float Y;

    Vector2() : X(0), Y(0) {}
    Vector2(float x, float y) : X(x), Y(y) {}

    Vector2 operator+(const Vector2& v) const { return Vector2(X + v.X, Y + v.Y); }
    Vector2 operator-(const Vector2& v) const { return Vector2(X - v.X, Y - v.Y); }
    Vector2 operator*(float v) const { return Vector2(X * v, Y * v); }
    Vector2 operator/(float v) const { return Vector2(X / v, Y / v); }

    float Distance(Vector2 v) {
        return sqrt(pow(X - v.X, 2) + pow(Y - v.Y, 2));
    }
};

// =======================================================
// 🧊 VECTOR 3 (Untuk Koordinat Map PUBG / 3D)
// =======================================================
struct Vector3 {
    float X;
    float Y;
    float Z;

    Vector3() : X(0), Y(0), Z(0) {}
    Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    Vector3 operator+(const Vector3& v) const { return Vector3(X + v.X, Y + v.Y, Z + v.Z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(X - v.X, Y - v.Y, Z - v.Z); }
    Vector3 operator*(float v) const { return Vector3(X * v, Y * v, Z * v); }
    Vector3 operator/(float v) const { return Vector3(X / v, Y / v, Z / v); }

    float Distance(Vector3 v) {
        return sqrt(pow(X - v.X, 2) + pow(Y - v.Y, 2) + pow(Z - v.Z, 2));
    }
    
    float Length() {
        return sqrt(X * X + Y * Y + Z * Z);
    }
};

// =======================================================
// 🎯 FMATRIX (Matriks untuk World-to-Screen / W2S)
// WAJIB ADA untuk mengubah koordinat 3D musuh jadi 2D di layar
// =======================================================
struct FMatrix {
    float M[4][4];
};

struct FTransform {
    Vector3 Rotation;
    Vector3 Translation;
    Vector3 Scale3D;
};

#endif //ZYGISKPG_UNITY_H
