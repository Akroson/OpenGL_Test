#include <math.h>

#define Pi32 3.14159f

struct mat4
{
	union
	{
		float E[4][4];
	};
};

struct v2
{
	union
	{
		struct
		{
			float x, y;
		};
		float E[2];
	};
};

struct v3
{
	union
	{
		struct
		{
			float x, y, z;
		};
		float E[3];
	};
};

struct v4
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float E[4];
	};
};

inline v3
V3(float x, float y, float z)
{
	v3 Result;

	Result.x = x;
	Result.y = y;
	Result.z = z;

	return Result;
}

inline v3
operator+(v3 A, v3 B)
{
	v3 Result;

	Result.x = A.x + B.x;
	Result.y = A.y + B.y;
	Result.z = A.z + B.z;

	return Result;
}

inline v3
operator-(v3 A, v3 B)
{
	v3 Result;

	Result.x = A.x - B.x;
	Result.y = A.y - B.y;
	Result.z = A.z - B.z;

	return Result;
}

inline v3
operator*(v3 A, float B)
{
	v3 Result;

	Result.x = A.x * B;
	Result.y = A.y * B;
	Result.z = A.z * B;

	return Result;
}

inline void
operator+=(v3 &A, v3 B)
{
	A.x += B.x;
	A.y += B.y;
	A.z += B.z;
}

inline void
operator-=(v3 &A, v3 B)
{
	A.x -= B.x;
	A.y -= B.y;
	A.z -= B.z;
}

inline v3
operator-(v3 A)
{
	v3 Result;

	Result.x = -A.x;
	Result.y = -A.y;
	Result.z = -A.z;

	return Result;
}

mat4
operator*(mat4 &A, mat4 &B)
{
	mat4 Result;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float sum = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				sum += A.E[k][i] * B.E[j][k];
			}

			Result.E[j][i] = sum;
		}
	}

	return Result;
}

inline v3
cross(v3 A, v3 B)
{
	v3 Result;

	Result.x = (A.y * B.z) - (A.z * B.y);
	Result.y = (A.z * B.x) - (A.x * B.z);
	Result.z = (A.x * B.y) - (A.y * B.x);

	return Result;
}

inline float
dot(v3 A, v3 B)
{
	float Result = A.x*B.x + A.y*B.y + A.z*B.z;
	return Result;
}

inline float
LengthSq(v3 A)
{
	float Result = dot(A, A);
	return Result;
}

inline float
Length(v3 A)
{
	float Result = sqrtf(LengthSq(A));
	return Result;
}

inline v3
normalize(v3 A)
{
	v3 Result;
	
	float InvLength = 1.0f / Length(A);
	Result = A * InvLength;

	return Result;
}

inline mat4
Identity(void)
{
	mat4 Result = {};

	Result.E[0][0] = 1.0f;
	Result.E[1][1] = 1.0f;
	Result.E[2][2] = 1.0f;
	Result.E[3][3] = 1.0f;

	return Result;
}

mat4
LookAt(v3 Pos, v3 Dir, v3 Up)
{
	mat4 Result = Identity();

	v3 Z = normalize(Dir - Pos);
	v3 X = normalize(cross(Z, Up));
	v3 Y = normalize(cross(X, Z));

	Result.E[0][0] = X.x;
	Result.E[1][0] = X.y;
	Result.E[2][0] = X.z;

	Result.E[0][1] = Y.x;
	Result.E[1][1] = Y.y;
	Result.E[2][1] = Y.z;
	
	Result.E[0][2] = -Z.x;
	Result.E[1][2] = -Z.y;
	Result.E[2][2] = -Z.z;

	Result.E[3][0] = -dot(X, Pos);
	Result.E[3][1] = -dot(Y, Pos);
	Result.E[3][2] = dot(Z, Pos);

	return Result;
}

mat4
Perspective(float Fov, float Aspect, float zNear, float zFar)
{
	mat4 Result = {};

	float ZRange = zFar - zNear;
	float tanHalfFov = tanf(Fov * 0.5f);
	
	Result.E[0][0] = 1.0f / (tanHalfFov * Aspect);
	Result.E[1][1] = 1.0f / tanHalfFov;
	Result.E[2][2] = -(zNear + zFar) / ZRange;
	Result.E[3][2] = -(2.0f * zFar * zNear) / ZRange;
	Result.E[2][3] = -1.0f;

	return Result;
}

inline float
AngelToRadian(float Angel)
{
	float Result = Angel * Pi32 / 180.f;
	return Result;
}