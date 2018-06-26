#pragma once
//#include "vec3.h"
#include<algorithm>
#include<math.h>
#include <iostream>
//#include "cuda_runtime.h"
//#include "vec2.h"
using namespace std;
class vec3
{
public:
	double x, y, z;
	vec3();
	vec3(const vec3 &v);
	vec3(double nx, double ny, double nz);
	//vec3(const vec2 &v, double a);
	vec3(double p[3]);

	bool operator ==(const vec3 &v) const;
	bool operator !=(const vec3 &v) const;
	vec3 &operator =(const vec3 &v);
	vec3 operator -() const;
	vec3 operator +(const vec3 &v) const;
	vec3 operator -(const vec3 &v) const;
	vec3 operator *(double a) const;
	vec3 operator /(double a) const;
	vec3 &operator +=(const vec3 &v);
	vec3 &operator -=(const vec3 &v);
	vec3 &operator +=(double a);
	vec3 &operator -=(double a);
	vec3 &operator *=(double a);
	vec3 &operator /=(double a);

	friend ostream & operator<<(ostream &out, vec3 &v)
	{
		out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
		return out;
	}
	vec3 normalize();
	vec3 computeNormal(vec3 a, vec3 b, vec3 c);
	double distance(const vec3  &a, const vec3 &b);
	double dot(const vec3 &a, const vec3 &b);
	static vec3 cross(const vec3 &a, const vec3 &b);


private:

};



vec3::vec3()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

vec3::vec3(const vec3 & v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

vec3::vec3(double nx, double ny, double nz)
{
	this->x = nx;
	this->y = ny;
	this->z = nz;
}

//vec3::vec3(const vec2 & v, double a)
//{
//	x = v.x;
//	y = v.y;
//	z = a;
//}

vec3::vec3(double p[3])
{
	x = p[0];
	y = p[1];
	z = p[2];
}

bool vec3::operator==(const vec3 & v) const
{
	return this->x == v.x&&this->y == v.y&&this->z == v.z;
}

bool vec3::operator!=(const vec3 & v) const
{
	return this->x != v.x || this->y != v.y || this->z != v.z;
}

vec3 & vec3::operator=(const vec3 & v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	return *this;
}

vec3 vec3::operator-() const
{
	return vec3(-this->x, -this->y, -this->z);
}

vec3 vec3::operator+(const vec3 & v) const
{
	return vec3(this->x + v.x, this->y + v.y, this->z + v.z);
}

vec3 vec3::operator-(const vec3 & v) const
{
	return vec3(this->x - v.x, this->y - v.y, this->z - v.z);
}

vec3 vec3::operator*(double a) const
{
	double x1 = x *a;
	double y1 = y * a;
	double z1 = z * a;
	return vec3(x1, y1, z1);
	//return vec3(this->x * a,this->y *a,this->z * a);
}

vec3 vec3::operator/(double a) const
{
	double t = 1.0f / a;
	return vec3(this->x * t, this->y * t, this->z * t);
}

vec3 & vec3::operator+=(const vec3 & v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

vec3 & vec3::operator-=(const vec3 & v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	return *this;
}

vec3 & vec3::operator+=(double a)
{
	this->x += a;
	this->y += a;
	this->z += a;
	return *this;

}

vec3 & vec3::operator-=(double a)
{
	this->x -= a;
	this->y -= a;
	this->z -= a;
	return *this;
}

vec3 & vec3::operator*=(double a)
{
	this->x *= a;
	this->y *= a;
	this->z *= a;
	return *this;
}

vec3 & vec3::operator/=(double a)
{
	double t = 1.0f / a;
	this->x *= t;
	this->y *= t;
	this->z *= t;
	return *this;
}

vec3 vec3::normalize()
{
	double temp = this->x *this->x + this->y * this->y + this->z *this->z;
	if (temp > 0.0f)
	{
		double t = 1.0f / sqrtf(temp);
		return vec3(x*t, y*t, z*t);
		//this->x *= t;
		//this->y *= t;
		//this->z *= t;
	}
}

inline vec3 vec3::computeNormal(vec3 a, vec3 b, vec3 c)
{
	return cross(c - a, b - a).normalize();
}

double vec3::distance(const vec3 & a, const vec3 & b)
{
	double nx = a.x - b.x;
	double ny = a.y - b.y;
	double nz = a.z - b.z;
	return sqrt(nx*nx + ny*ny + nz*nz);
}

double vec3::dot(const vec3 & a, const vec3 & b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 vec3::cross(const vec3 &a, const vec3 &b)
{
	double nx = a.y*b.z - a.z*b.y;
	double ny = a.z*b.x - a.x*b.z;
	double nz = a.x*b.y - a.y*b.x;
	return vec3(nx, ny, nz);
}


