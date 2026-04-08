//
// Created by lbert on 2/9/2023.
// Optimized for Zygisk-Exsss by Gemini
//

#ifndef ZYGISKPG_UNITY_H
#define ZYGISKPG_UNITY_H

#include <codecvt>
#include <math.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include <algorithm>

struct Vector2
{
    union
    {
        struct
        {
            float X;
            float Y;
        };
        float data[2];
    };

    inline Vector2();
    inline Vector2(float data[]);
    inline Vector2(float value);
    inline Vector2(float x, float y);

    static inline Vector2 Zero();
    static inline Vector2 One();
    static inline Vector2 Right();
    static inline Vector2 Left();
    static inline Vector2 Up();
    static inline Vector2 Down();

    static inline float Angle(Vector2 a, Vector2 b);
    static inline Vector2 ClampMagnitude(Vector2 vector, float maxLength);
    static inline float Component(Vector2 a, Vector2 b);
    static inline float Distance(Vector2 a, Vector2 b);
    static inline float Dot(Vector2 lhs, Vector2 rhs);
    static inline Vector2 FromPolar(float rad, float theta);
    static inline Vector2 Lerp(Vector2 a, Vector2 b, float t);
    static inline Vector2 LerpUnclamped(Vector2 a, Vector2 b, float t);
    static inline float Magnitude(Vector2 v);
    static inline Vector2 Max(Vector2 a, Vector2 b);
    static inline Vector2 Min(Vector2 a, Vector2 b);
    static inline Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta);
    static inline Vector2 Normalized(Vector2 v);
    static inline void OrthoNormalize(Vector2 &normal, Vector2 &tangent);
    static inline Vector2 Project(Vector2 a, Vector2 b);
    static inline Vector2 Reflect(Vector2 vector, Vector2 planeNormal);
    static inline Vector2 Reject(Vector2 a, Vector2 b);
    static inline Vector2 RotateTowards(Vector2 current, Vector2 target, float maxRadiansDelta, float maxMagnitudeDelta);
    static inline Vector2 Scale(Vector2 a, Vector2 b);
    static inline Vector2 Slerp(Vector2 a, Vector2 b, float t);
    static inline Vector2 SlerpUnclamped(Vector2 a, Vector2 b, float t);
    static inline float SqrMagnitude(Vector2 v);
    static inline void ToPolar(Vector2 vector, float &rad, float &theta);

    inline struct Vector2& operator+=(const float rhs);
    inline struct Vector2& operator-=(const float rhs);
    inline struct Vector2& operator*=(const float rhs);
    inline struct Vector2& operator/=(const float rhs);
    inline struct Vector2& operator+=(const Vector2 rhs);
    inline struct Vector2& operator-=(const Vector2 rhs);
};

// =======================================================
// 🚨 WAJIB DEKLARASI MATEMATIKA DI SINI (SEBELUM RUMUS) 🚨
// =======================================================
inline Vector2 operator-(Vector2 rhs);
inline Vector2 operator+(Vector2 lhs, const float rhs);
inline Vector2 operator-(Vector2 lhs, const float rhs);
inline Vector2 operator*(Vector2 lhs, const float rhs);
inline Vector2 operator/(Vector2 lhs, const float rhs);
inline Vector2 operator+(const float lhs, Vector2 rhs);
inline Vector2 operator-(const float lhs, Vector2 rhs);
inline Vector2 operator*(const float lhs, Vector2 rhs);
inline Vector2 operator/(const float lhs, Vector2 rhs);
inline Vector2 operator+(Vector2 lhs, const Vector2 rhs);
inline Vector2 operator-(Vector2 lhs, const Vector2 rhs);
inline bool operator==(const Vector2 lhs, const Vector2 rhs);
inline bool operator!=(const Vector2 lhs, const Vector2 rhs);


// =======================================================
// ⚙️ IMPLEMENTASI VECTOR2 (RUMUS) ⚙️
// =======================================================

inline Vector2::Vector2() : X(0), Y(0) {}
inline Vector2::Vector2(float data[]) : X(data[0]), Y(data[1]) {}
inline Vector2::Vector2(float value) : X(value), Y(value) {}
inline Vector2::Vector2(float x, float y) : X(x), Y(y) {}

inline Vector2 Vector2::Zero() { return Vector2(0, 0); }
inline Vector2 Vector2::One() { return Vector2(1, 1); }
inline Vector2 Vector2::Right() { return Vector2(1, 0); }
inline Vector2 Vector2::Left() { return Vector2(-1, 0); }
inline Vector2 Vector2::Up() { return Vector2(0, 1); }
inline Vector2 Vector2::Down() { return Vector2(0, -1); }

inline float Vector2::Angle(Vector2 a, Vector2 b) {
    float v = Dot(a, b) / (Magnitude(a) * Magnitude(b));
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    return acos(v);
}

inline Vector2 Vector2::ClampMagnitude(Vector2 vector, float maxLength) {
    float length = Magnitude(vector);
    if (length > maxLength) vector *= maxLength / length;
    return vector;
}

inline float Vector2::Component(Vector2 a, Vector2 b) { return Dot(a, b) / Magnitude(b); }
inline float Vector2::Distance(Vector2 a, Vector2 b) { return Vector2::Magnitude(a - b); }
inline float Vector2::Dot(Vector2 lhs, Vector2 rhs) { return lhs.X * rhs.X + lhs.Y * rhs.Y; }

inline Vector2 Vector2::FromPolar(float rad, float theta) {
    Vector2 v;
    v.X = rad * cos(theta);
    v.Y = rad * sin(theta);
    return v;
}

inline Vector2 Vector2::Lerp(Vector2 a, Vector2 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return LerpUnclamped(a, b, t);
}

inline Vector2 Vector2::LerpUnclamped(Vector2 a, Vector2 b, float t) { return (b - a) * t + a; }
inline float Vector2::Magnitude(Vector2 v) { return sqrt(SqrMagnitude(v)); }

inline Vector2 Vector2::Max(Vector2 a, Vector2 b) {
    return Vector2(a.X > b.X ? a.X : b.X, a.Y > b.Y ? a.Y : b.Y);
}

inline Vector2 Vector2::Min(Vector2 a, Vector2 b) {
    return Vector2(a.X > b.X ? b.X : a.X, a.Y > b.Y ? b.Y : a.Y);
}

inline Vector2 Vector2::MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta) {
    Vector2 d = target - current;
    float m = Magnitude(d);
    if (m < maxDistanceDelta || m == 0) return target;
    return current + (d * maxDistanceDelta / m);
}

inline Vector2 Vector2::Normalized(Vector2 v) {
    float mag = Magnitude(v);
    if (mag == 0) return Vector2::Zero();
    return v / mag;
}

inline void Vector2::OrthoNormalize(Vector2 &normal, Vector2 &tangent) {
    normal = Normalized(normal);
    tangent = Reject(tangent, normal);
    tangent = Normalized(tangent);
}

inline Vector2 Vector2::Project(Vector2 a, Vector2 b) {
    float m = Magnitude(b);
    return Dot(a, b) / (m * m) * b;
}

inline Vector2 Vector2::Reflect(Vector2 vector, Vector2 planeNormal) { return vector - 2 * Project(vector, planeNormal); }
inline Vector2 Vector2::Reject(Vector2 a, Vector2 b) { return a - Project(a, b); }

inline Vector2 Vector2::RotateTowards(Vector2 current, Vector2 target, float maxRadiansDelta, float maxMagnitudeDelta) {
    float magCur = Magnitude(current);
    float magTar = Magnitude(target);
    float newMag = magCur + maxMagnitudeDelta * ((magTar > magCur) - (magCur > magTar));
    newMag = fmin(newMag, fmax(magCur, magTar));
    newMag = fmax(newMag, fmin(magCur, magTar));
    float totalAngle = Angle(current, target) - maxRadiansDelta;
    if (totalAngle <= 0) return Normalized(target) * newMag;
    else if (totalAngle >= M_PI) return Normalized(-target) * newMag;
    float axis = current.X * target.Y - current.Y * target.X;
    axis = axis / fabs(axis);
    if (!(1 - fabs(axis) < 0.00001)) axis = 1;
    current = Normalized(current);
    Vector2 newVector = current * cos(maxRadiansDelta) + Vector2(-current.Y, current.X) * sin(maxRadiansDelta) * axis;
    return newVector * newMag;
}

inline Vector2 Vector2::Scale(Vector2 a, Vector2 b) { return Vector2(a.X * b.X, a.Y * b.Y); }
inline Vector2 Vector2::Slerp(Vector2 a, Vector2 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return SlerpUnclamped(a, b, t);
}

inline Vector2 Vector2::SlerpUnclamped(Vector2 a, Vector2 b, float t) {
    float magA = Magnitude(a);
    float magB = Magnitude(b);
    a /= magA; b /= magB;
    float dot = Dot(a, b);
    dot = fmax(dot, -1.0); dot = fmin(dot, 1.0);
    float theta = acos(dot) * t;
    Vector2 relativeVec = Normalized(b - a * dot);
    Vector2 newVec = a * cos(theta) + relativeVec * sin(theta);
    return newVec * (magA + (magB - magA) * t);
}

inline float Vector2::SqrMagnitude(Vector2 v) { return v.X * v.X + v.Y * v.Y; }
inline void Vector2::ToPolar(Vector2 vector, float &rad, float &theta) {
    rad = Magnitude(vector); theta = atan2(vector.Y, vector.X);
}

inline struct Vector2& Vector2::operator+=(const float rhs) { X += rhs; Y += rhs; return *this; }
inline struct Vector2& Vector2::operator-=(const float rhs) { X -= rhs; Y -= rhs; return *this; }
inline struct Vector2& Vector2::operator*=(const float rhs) { X *= rhs; Y *= rhs; return *this; }
inline struct Vector2& Vector2::operator/=(const float rhs) { X /= rhs; Y /= rhs; return *this; }
inline struct Vector2& Vector2::operator+=(const Vector2 rhs) { X += rhs.X; Y += rhs.Y; return *this; }
inline struct Vector2& Vector2::operator-=(const Vector2 rhs) { X -= rhs.X; Y -= rhs.Y; return *this; }

inline Vector2 operator-(Vector2 rhs) { return rhs * -1; }
inline Vector2 operator+(Vector2 lhs, const float rhs) { return lhs += rhs; }
inline Vector2 operator-(Vector2 lhs, const float rhs) { return lhs -= rhs; }
inline Vector2 operator*(Vector2 lhs, const float rhs) { return lhs *= rhs; }
inline Vector2 operator/(Vector2 lhs, const float rhs) { return lhs /= rhs; }
inline Vector2 operator+(const float lhs, Vector2 rhs) { return rhs += lhs; }
inline Vector2 operator-(const float lhs, Vector2 rhs) { return rhs -= lhs; }
inline Vector2 operator*(const float lhs, Vector2 rhs) { return rhs *= lhs; }
inline Vector2 operator/(const float lhs, Vector2 rhs) { return rhs /= lhs; }
inline Vector2 operator+(Vector2 lhs, const Vector2 rhs) { return lhs += rhs; }
inline Vector2 operator-(Vector2 lhs, const Vector2 rhs) { return lhs -= rhs; }
inline bool operator==(const Vector2 lhs, const Vector2 rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
inline bool operator!=(const Vector2 lhs, const Vector2 rhs) { return !(lhs == rhs); }

// =======================================================
// 💎 MONO / IL2CPP STRUCTS 💎
// =======================================================

template <typename T>
struct monoArray {
    void* klass; void* monitor; void* bounds; int max_length; void* vector [1];
    int getLength() { return max_length; }
    T getPointer() { return (T)vector; }
};

typedef struct _monoString {
    void *klass; void *monitor; int length; char16_t chars[1];
    int getLength() { return length; }
    char16_t *getRawChars() { return chars; }
    std::string getString() {
        std::u16string u16 = std::u16string(chars);
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
    }
} monoString;

typedef struct _monoDecimal { void *klass; void *monitor; } monoDecimal;

template <typename T>
struct monoList {
    void *unk0; void *unk1; monoArray<T> *items; int size; int version;
    T getItems(){ return items->getPointer(); }
    int getSize(){ return size; }
};

template <typename K, typename V>
struct monoDictionary {
    void *unk0; void *unk1; monoArray<int **> *table; monoArray<void **> *linkSlots;
    monoArray<K> *keys; monoArray<V> *values; int touchedSlots; int emptySlot; int size;
    K getKeys(){ return keys->getPointer(); }
    V getValues(){ return values->getPointer(); }
    int getSize(){ return size; }
};

// =======================================================
// 🛡️ OBSCURED VALUES BYPASS (WAJIB INLINE) 🛡️
// =======================================================

inline int GetObscuredIntValue(uint64_t location){
    int cryptoKey = *(int *)location;
    int obfuscatedValue = *(int *)(location + 0x4);
    return obfuscatedValue ^ cryptoKey;
}

inline bool GetObscuredBoolValue(uint64_t location){
    int cryptoKey = *(int *)location;
    int obfuscatedValue = *(int *)(location + 0x4);
    return (bool)(obfuscatedValue ^ cryptoKey);
}

inline void SetObscuredIntValue(uint64_t location, int value){
    int cryptoKey = *(int *)location;
    *(int *)(location + 0x4) = value ^ cryptoKey;
}

inline void SetObscuredBoolValue(uint64_t location, bool value){
    int cryptoKey = *(int *)location;
    *(int *)(location + 0x4) = (int)value ^ cryptoKey;
}

inline float GetObscuredFloatValue(uint64_t location) {
    int cryptoKey = *(int *) location;
    int obfuscatedValue = *(int *) (location + 0x4);
    union { int i; float f; } IF;
    IF.i = obfuscatedValue ^ cryptoKey;
    return IF.f;
}

inline void SetObscuredFloatValue(uint64_t location, float value) {
    int cryptoKey = *(int *) location;
    union { int i; float f; } IF;
    IF.f = value;
    *(int *) (location + 0x4) = IF.i ^ cryptoKey;
}

#endif //ZYGISKPG_UNITY_H
