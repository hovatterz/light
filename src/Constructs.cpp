#include "LTBL/Constructs.h"

Color3f::Color3f()
	: r(0.0f), g(0.0f), b(0.0f)
{
}

Color3f::Color3f(float R, float G, float B)
	: r(R), g(G), b(B)
{
}

Point2i::Point2i()
{
}

Point2i::Point2i(int X, int Y)
	: x(X), y(Y)
{
}

bool Point2i::operator==(const Point2i &other) const
{
	if(x == other.x && y == other.y)
		return true;

	return false;
}

Vec2f::Vec2f() 
{
}

Vec2f::Vec2f(float X, float Y)
	: x(X), y(Y)
{
}

bool Vec2f::operator==(const Vec2f &other) const 
{
	if(x == other.x && y == other.y)
		return true;

	return false;
}

Vec2f Vec2f::operator*(float scale) const 
{
	return Vec2f(x * scale, y * scale);
}

Vec2f Vec2f::operator/(float scale) const 
{
	return Vec2f(x / scale, y / scale);
}


Vec2f Vec2f::operator+(const Vec2f &other) const 
{
	return Vec2f(x + other.x, y + other.y);
}

Vec2f Vec2f::operator-(const Vec2f &other) const 
{
	return Vec2f(x - other.x, y - other.y);
}

Vec2f Vec2f::operator-() const
{
	return Vec2f(-x, -y);
}

const Vec2f &Vec2f::operator*=(float scale) 
{
	x *= scale;
	y *= scale;

	return *this;
}

const Vec2f &Vec2f::operator/=(float scale) 
{
	x /= scale;
	y /= scale;

	return *this;
}

const Vec2f &Vec2f::operator+=(const Vec2f &other) 
{
	x += other.x;
	y += other.y;

	return *this;
}

const Vec2f &Vec2f::operator-=(const Vec2f &other) 
{
	x -= other.x;
	y -= other.y;

	return *this;
}

float Vec2f::magnitude() const 
{
	return sqrt(x * x + y * y);
}

float Vec2f::magnitudeSquared() const 
{
	return x * x + y * y;
}

Vec2f Vec2f::normalize() const 
{
	float m = sqrt(x * x + y * y);
	return Vec2f(x / m, y / m);
}

float Vec2f::dot(const Vec2f &other) const 
{
	return x * other.x + y * other.y;
}

float Vec2f::cross(const Vec2f &other) const 
{
	return x * other.y - y * other.x;
}

Vec2f operator*(float scale, const Vec2f &v) 
{
	return v * scale;
}

std::ostream &operator<<(std::ostream &output, const Vec2f &v)
{
	std::cout << '(' << v.x << ", " << v.y << ')';
	return output;
}
