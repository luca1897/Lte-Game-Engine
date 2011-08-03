/*
 * LTE Game Engine
 * Copyright (C) 2006-2008 SiberianSTAR <haxormail@gmail.com>
 * http://www.ltestudios.com
 *  
 * The LTE Game Engine is based on Irrlicht 1.0
 * Irrlicht Engine is Copyright (C) 2002-2006 Nikolaus Gebhardt
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */
 
#ifndef __SHAD_INFO_H__
#define __SHAD_INFO_H__

struct Vertex
{
	float x, y, z;

	Vertex()
	{
	}

	Vertex(const engine::core::vector3df& v) : x(v.X), y(v.Y), z(v.Z)
	{
	}

	Vertex(const Vertex& v) : x(v.x), y(v.y), z(v.z)
	{
	}

	Vertex(float x, float y, float z) : x(x), y(y), z(z)
	{
	}

	Vertex& operator = (const Vertex& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	Vertex operator + (const Vertex& v) const
	{
		return Vertex(v.x+x,v.y+y,v.z+z);
	}

	Vertex& operator - (void)
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	Vertex operator - (const Vertex& v) const
	{
		return Vertex(x-v.x,y-v.y,z-v.z);
	}

	Vertex operator * (float scalar) const
	{
		return Vertex(x * scalar, y * scalar, z * scalar);
	}

	float dot(const Vertex& v) const
	{
		return (x*v.x) + (y*v.y) + (z*v.z);
	}

	Vertex cross(const Vertex& v)
	{
		return Vertex(
		               (y * v.z) - (z * v.y),
		               (z * v.x) - (x * v.z),
		               (x * v.y) - (y * v.x));
	}

	Vertex normalize() const
	{
		float len = 1.0f / ::FastSqrt(dot(*this));
		return *this * len;
	}
};

struct ShadowVertex
{
	Vertex pos;
};

struct Triangle
{
	u16 i[3];
};

struct Plane
{
	Vertex normal;
	float dist;

	float distance(Vertex& v) const
	{
		return dist + normal.dot(v);
	}
};


struct Edge
{
	unsigned int i[2];
	Vertex normal;
};

#endif
