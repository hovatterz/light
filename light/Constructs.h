#ifndef CONSTRUCTS_H
#define CONSTRUCTS_H

#include <iostream>
#include <math.h>

struct Color3f
{
	float r, g, b;

	Color3f();
	Color3f(float R, float G, float B);
};

class Point2i
{
public:
	int x, y;

	Point2i(int X, int Y);
	Point2i();
	
	bool operator==(const Point2i &other) const;
};

class Vec2f 
{
public:
	float x, y;

	Vec2f();
	Vec2f(float X, float Y);

	bool operator==(const Vec2f &other) const;
		
	Vec2f operator*(float scale) const;
	Vec2f operator/(float scale) const;
	Vec2f operator+(const Vec2f &other) const;
	Vec2f operator-(const Vec2f &other) const;
	Vec2f operator-() const;
		
	const Vec2f &operator*=(float scale);
	const Vec2f &operator/=(float scale);
	const Vec2f &operator+=(const Vec2f &other);
	const Vec2f &operator-=(const Vec2f &other);
		
	float magnitude() const;
	float magnitudeSquared() const;
	Vec2f normalize() const;
	float dot(const Vec2f &other) const;
	float cross(const Vec2f &other) const;
};

Vec2f operator*(float scale, const Vec2f &v);
std::ostream &operator<<(std::ostream &output, const Vec2f &v);

#endif
