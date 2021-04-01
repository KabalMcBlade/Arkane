#pragma once

#include "../Core/Defines.h"


AK_NAMESPACE_BEGIN

// These vertex layout are used when load from file (GLTF or native Arkane format .akmdl, which is not yet implemented)
// Does not require any optimized function (apart read from file, but is not matter of this class)
// This Vertex are afterwards pushed into a list, and the list, as buffer ( .data() ) will returned to the Vertex buffer to bind against Vulkan API
// So it is matter of memory: Note that even if I can send values smaller than float such uint16_t or even uint8_t, I'm trying to send the hugest value,
// to avoid any "visual" clamping.
// Also I describes more layout, because depending by the model loaded, you can need only a couple of set or the full set, so to optimize the CPU -> GPU
// the Vertex reflect these differences as well as the shader used. (So for every custom Vertex shader, be sure to reflect the proper Vertex layout)


// The sequence CANNOT BE CHANGE!
// The index of the vertex layout is written inside the native Arkane model format .akmdl


// Naming convention for Vertex Layout structures:
// P = Position Only
// C = Position and Color
// N = Position + Normal + Tex Coordinates 0
// T = Position + Normal + Tex Coordinates 0 + Tex Coordinates 1
// F = Full: Position + Normal + Tex Coordinates 0 + Tangent
// E = Extended: Position + Normal + Tex Coordinates 0 + Tex Coordinates 1 + Tangent
// S = Each of the above can have the suffix S which means skinned (weight and joints)
enum class EVertexLayout
{
	EVertexLayout_P = 0,
	EVertexLayout_C,
	EVertexLayout_N,
	EVertexLayout_T,
	EVertexLayout_F,
	EVertexLayout_E,

	EVertexLayout_P_S = 127,
	EVertexLayout_C_S,
	EVertexLayout_N_S,
	EVertexLayout_T_S,
	EVertexLayout_F_S,
	EVertexLayout_E_S
};



// Full size
// 12 + 12 + 16 + 8 + 8 + 16 + 16 + 8 = 96
// float			m_position[3];
// float			m_normal[3];
// float			m_tangent[4];
// float			m_texCoord0[2];
// float			m_texCoord1[2];
// float			m_color[4];
// float			m_weights[4];
// uint16_t			m_joints[4];


// GLTF CONVERSION
// TYPE						// INT TO FLOAT					// FLOAT TO INT
// 5120 (BYTE)				f = max(c / 127.0, -1.0)		c = round(f * 127.0)
// 5121 (UNSIGNED_BYTE)		f = c / 255.0					c = round(f * 255.0)
// 5122 (SHORT)				f = max(c / 32767.0, -1.0)		c = round(f * 32767.0)
// 5123 (UNSIGNED_SHORT)	f = c / 65535.0					c = round(f * 65535.0)



// Position Only
AK_MEMORY_ALIGNED struct Vertex_P
{
	float m_position[3];

	// Accessors helpers for reading from GLTF format
	void SetPosition(float _x, float _y, float _z)
	{
		m_position[0] = _x;
		m_position[1] = _y;
		m_position[2] = _z;
	}
};

// Position and Color
AK_MEMORY_ALIGNED struct Vertex_C : public Vertex_P
{
	float m_color[4];

	// Accessors helpers for reading from GLTF format
	void SetColor(float _r, float _g, float _b, float _a)
	{
		m_color[0] = _r;
		m_color[1] = _g;
		m_color[2] = _b;
		m_color[3] = _a;
	}

	void SetColor(uint16_t _r, uint16_t _g, uint16_t _b, uint16_t _a)
	{
		m_color[0] = static_cast<float>(_r) / 65535.0f;
		m_color[1] = static_cast<float>(_g) / 65535.0f;
		m_color[2] = static_cast<float>(_b) / 65535.0f;
		m_color[3] = static_cast<float>(_a) / 65535.0f;
	}

	void SetColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
	{
		m_color[0] = static_cast<float>(_r) / 255.0f;
		m_color[1] = static_cast<float>(_g) / 255.0f;
		m_color[2] = static_cast<float>(_b) / 255.0f;
		m_color[3] = static_cast<float>(_a) / 255.0f;
	}
};

// Position + Normal + Tex Coordinates 0
AK_MEMORY_ALIGNED struct Vertex_N : public Vertex_P
{
	float m_normal[3];
	float m_texCoord0[2];

	// Accessors helpers for reading from GLTF format
	void SetNormal(float _x, float _y, float _z)
	{
		m_normal[0] = _x;
		m_normal[1] = _y;
		m_normal[2] = _z;
	}

	void SetTexCoord0(float _u, float _v)
	{
		m_texCoord0[0] = _u;
		m_texCoord0[1] = _v;
	}

	void SetTexCoord0(uint16_t _u, uint16_t _v)
	{
		m_texCoord0[0] = static_cast<float>(_u) / 65535.0f;
		m_texCoord0[1] = static_cast<float>(_v) / 65535.0f;
	}

	void SetTexCoord0(uint8_t _u, uint8_t _v)
	{
		m_texCoord0[0] = static_cast<float>(_u) / 255.0f;
		m_texCoord0[1] = static_cast<float>(_v) / 255.0f;
	}
};

// Position + Normal + Tex Coordinates 0 + Tex Coordinates 1
AK_MEMORY_ALIGNED struct Vertex_T : public Vertex_N
{
	float m_texCoord1[2];

	// Accessors helpers for reading from GLTF format
	void SetTexCoord1(float _u, float _v)
	{
		m_texCoord1[0] = _u;
		m_texCoord1[1] = _v;
	}

	void SetTexCoord1(uint16_t _u, uint16_t _v)
	{
		m_texCoord1[0] = static_cast<float>(_u) / 65535.0f;
		m_texCoord1[1] = static_cast<float>(_v) / 65535.0f;
	}

	void SetTexCoord1(uint8_t _u, uint8_t _v)
	{
		m_texCoord1[0] = static_cast<float>(_u) / 255.0f;
		m_texCoord1[1] = static_cast<float>(_v) / 255.0f;
	}
};


// Full: Position + Normal + Tex Coordinates 0 + Tangent
AK_MEMORY_ALIGNED struct Vertex_F : public Vertex_N
{
	float m_tangent[4];

	// Accessors helpers for reading from GLTF format
	void SetTangent(float _x, float _y, float _z, float _w)
	{
		m_tangent[0] = _x;
		m_tangent[1] = _y;
		m_tangent[2] = _z;
		m_tangent[3] = _w;
	}
};


// Extended: Position + Normal + Tex Coordinates 0 + Tex Coordinates 1 + Tangent
AK_MEMORY_ALIGNED struct Vertex_E : public Vertex_T
{
	float m_tangent[4];

	// Accessors helpers for reading from GLTF format
	void SetTangent(float _x, float _y, float _z, float _w)
	{
		m_tangent[0] = _x;
		m_tangent[1] = _y;
		m_tangent[2] = _z;
		m_tangent[3] = _w;
	}
};


// Skinned
AK_MEMORY_ALIGNED struct SkinnedVertex
{
	float m_weights[4];
	uint16_t m_joints[4];

	// Accessors helpers for reading from GLTF format
	void SetWeights(float _x, float _y, float _z, float _w)
	{
		m_weights[0] = _x;
		m_weights[1] = _y;
		m_weights[2] = _z;
		m_weights[3] = _w;
	}

	void SetWeights(uint16_t _x, uint16_t _y, uint16_t _z, uint16_t _w)
	{
		m_weights[0] = static_cast<float>(_x) / 65535.0f;
		m_weights[1] = static_cast<float>(_y) / 65535.0f;
		m_weights[2] = static_cast<float>(_z) / 65535.0f;
		m_weights[3] = static_cast<float>(_w) / 65535.0f;
	}

	void SetWeights(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _w)
	{
		m_weights[0] = static_cast<float>(_x) / 255.0f;
		m_weights[1] = static_cast<float>(_y) / 255.0f;
		m_weights[2] = static_cast<float>(_z) / 255.0f;
		m_weights[3] = static_cast<float>(_w) / 255.0f;
	}

	void SetJoint(uint16_t _x, uint16_t _y, uint16_t _z, uint16_t _w)
	{
		m_joints[0] = _x;
		m_joints[1] = _y;
		m_joints[2] = _z;
		m_joints[3] = _w;
	}

	void SetJoint(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _w)
	{
		m_joints[0] = static_cast<uint16_t>(_x);
		m_joints[1] = static_cast<uint16_t>(_y);
		m_joints[2] = static_cast<uint16_t>(_z);
		m_joints[3] = static_cast<uint16_t>(_w);
	}
};



// Vertex_P + Skinned
AK_MEMORY_ALIGNED struct Vertex_P_S : public Vertex_P, public SkinnedVertex { };

// Vertex_C + Skinned
AK_MEMORY_ALIGNED struct Vertex_C_S : public Vertex_C, public SkinnedVertex { };

// Vertex_N + Skinned
AK_MEMORY_ALIGNED struct Vertex_N_S : public Vertex_N, public SkinnedVertex { };

// Vertex_P + Skinned
AK_MEMORY_ALIGNED struct Vertex_T_S : public Vertex_T, public SkinnedVertex { };

// Vertex_C + Skinned
AK_MEMORY_ALIGNED struct Vertex_F_S : public Vertex_F, public SkinnedVertex { };

// Vertex_N + Skinned
AK_MEMORY_ALIGNED struct Vertex_E_S : public Vertex_E, public SkinnedVertex { };


AK_NAMESPACE_END