#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#define SC 1.5
#define REF_DEPTH 1
#define X 10
#define Y 10
#define WIDTH 1600
#define HEIGHT 1600
#define VECTOR_DOT(A,B) ((A).x*(B).x+(A).y*(B).y+(A).z*(B).z)
#define VECTOR_NORMALIZE(A) {float ll=1/sqrtf((A).x*(A).x+(A).y*(A).y+(A).z*(A).z);(A).x*=ll;(A).y*=ll;(A).z*=ll;}
#define VECTOR_LENGTH(A) (sqrtf((A).x*(A).x+(A).y*(A).y+(A).z*(A).z))
#define VECTOR_SQLENGTH(A) ((A).x*(A).x+(A).y*(A).y+(A).z*(A).z)

using namespace std;

class Vector3D
{
public:
	float x, y, z;
	Vector3D() { x = 0.0f; y = 0.0f; z = 0.0f; }
	Vector3D(float tmp_x, float tmp_y, float tmp_z) { x = tmp_x; y = tmp_y; z = tmp_z; }
	Vector3D operator - (Vector3D& V) { return Vector3D(x - V.x, y - V.y, z - V.z); }
	Vector3D operator -() { return Vector3D(-x, -y, -z); }
	Vector3D operator + (Vector3D& V) { return Vector3D(x + V.x, y + V.y, z + V.z); }
	Vector3D operator *(float f) { return Vector3D(x*f, y*f, z*f); }
};

struct RGBColor
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
};

class plane
{
public:
	Vector3D N;
	float D;
	plane() :N(0.0f, 0.0f, 0.0f), D(0.0f) {}
	plane(Vector3D a_Normal, float a_D) :N(a_Normal), D(a_D) {}
};

typedef Vector3D Color;

class Ray
{
public:
	Vector3D m_Origin, m_Direction;
	Ray() :m_Origin(Vector3D(0.0f, 0.0f, 0.0f)), m_Direction(Vector3D(0.0f, 0.0f, 0.0f)) {}
	Ray(Vector3D& O, Vector3D& D)
	{
		m_Origin = O; m_Direction = D;
	}
};

RGBColor test[WIDTH][HEIGHT];

class Module
{
public:
	Color Module_Color;
	float Module_Ambient_reflection, Module_Diffuse_reflection, Module_Specular_reflection;
	bool Module_Light;
	float Module_reflection;
	float Module_mirror;
	Module() :Module_Color(Color(0.5f, 0.5f, 0.5f)),
		Module_Ambient_reflection(0.2f),
		Module_Diffuse_reflection(0.9f),
		Module_Specular_reflection(0.5f),
		Module_Light(false),
		Module_reflection(-1.0f),
		Module_mirror(-1.0f)
	{}
	bool isLight() { return Module_Light; }
	virtual bool Intersect(Ray& my_Ray, float& a_Dist) = 0;
	virtual Vector3D getNormal(Vector3D & P) = 0;
};

class Sphere : public Module
{
private:
	Vector3D m_Centre;
	float m_Radius;

public:
	Sphere() :m_Centre(Vector3D(0.0f, 0.0f, 0.0f)), m_Radius(1.0f) {}
	Sphere(Vector3D& C, float R) :m_Centre(C), m_Radius(R) {}
	Vector3D getNormal(Vector3D& P) { return (P - m_Centre); }
	Vector3D getCentre() { return m_Centre; }
	bool Intersect(Ray& my_Ray, float& a_Dist)
	{
		Vector3D tmp_v1 = my_Ray.m_Origin - m_Centre;
		Vector3D tmp_v2 = my_Ray.m_Direction;
		float b = 2 * VECTOR_DOT(tmp_v1, tmp_v2);
		float a = VECTOR_SQLENGTH(tmp_v2);
		float c = VECTOR_SQLENGTH(tmp_v1) - m_Radius*m_Radius;
		float det = b*b - 4 * a*c;
		if (det < 0) return false;
		float sqdet = sqrtf(det);
		float t_small = (-b - sqdet) / (2 * a);
		float t_big = (-b + sqdet) / (2 * a);

		if (t_small < 0.0f) return false;
		Vector3D tmp_v3 = (my_Ray.m_Direction)*t_small;

		float dis = VECTOR_LENGTH(tmp_v3);
		if (dis>a_Dist) return false;
		a_Dist = dis;
		return true;
	}
};

class Plane : public Module
{
private:
	plane m_Plane;
	float S_x, B_x, S_y, B_y, S_z, B_z;
	char Verticle_plane;

public:
	Plane(Vector3D& a_N, float a_DD, float s_x, float b_x, float s_y, float b_y, float s_z, float b_z, char verti) :m_Plane(plane(a_N, a_DD)),
		S_x(s_x), B_x(b_x), S_y(s_y), B_y(b_y), S_z(s_z), B_z(b_z), Verticle_plane(verti)
	{}
	Vector3D getNormal(Vector3D& P) { return m_Plane.N; }
	bool Intersect(Ray& my_Ray, float& a_Dist)
	{
		Vector3D DIR = my_Ray.m_Direction;
		Vector3D NOR = m_Plane.N;
		Vector3D ORI = my_Ray.m_Origin;
		float deno = VECTOR_DOT(NOR, DIR);
		if (abs(deno) < 1e-6) return false;

		float ss = VECTOR_DOT(ORI, NOR);
		ss += m_Plane.D;
		ss = -ss;
		ss = ss / deno;
		if (ss < 0.0f) return false;

		Vector3D tmp_v = (my_Ray.m_Direction)*ss;
		float dis = VECTOR_LENGTH(tmp_v);

		if (dis>a_Dist) return false;

		Vector3D tmp_v1 = my_Ray.m_Origin + (my_Ray.m_Direction)*ss;
		switch (Verticle_plane)
		{
		case 'x': {
			if (tmp_v1.z > B_z || tmp_v1.z < S_z) return false;
			if (tmp_v1.y > B_y || tmp_v1.y < S_y) return false;
			break;
		}
		case 'y': {
			if (tmp_v1.z > B_z || tmp_v1.z < S_z) return false;
			if (tmp_v1.x > B_x || tmp_v1.x < S_x) return false;
			break;
		}
		case 'z': {
			if (tmp_v1.y > B_y || tmp_v1.y < S_y) return false;
			if (tmp_v1.x > B_x || tmp_v1.x < S_x) return false;
			break;
		}
		case 'o':
		{
			float res = (tmp_v1.y - S_y)*(tmp_v1.y - S_y) + (tmp_v1.z - S_z)*(tmp_v1.z - S_z);
			if (res>B_x*B_x) return false;

		}
		default:
			break;
		}
		a_Dist = dis;
		return true;
	}
};


class Cylinder : public Module
{
private:
	Vector3D v_Centre;
	float v_Radius;
	float S_x, B_x;
public:
	Cylinder(Vector3D & C, float R, float s_x, float b_x) : v_Centre(C), v_Radius(R), S_x(s_x), B_x(b_x) {}
	Vector3D getNormal(Vector3D& P) { return Vector3D(0, P.y - v_Centre.y, P.z - v_Centre.z); }
	bool Intersect(Ray& my_Ray, float& a_Dist)
	{
		Vector3D tmp_v1 = my_Ray.m_Origin - v_Centre;
		tmp_v1.x = 0.0f;
		Vector3D tmp_v2 = my_Ray.m_Direction;
		tmp_v2.x = 0.0f;

		float b = 2 * VECTOR_DOT(tmp_v1, tmp_v2);
		float a = VECTOR_SQLENGTH(tmp_v2);
		float c = VECTOR_SQLENGTH(tmp_v1) - v_Radius*v_Radius;
		float det = b*b - 4 * a*c;

		if (det < 0) return false;
		float sqdet = sqrtf(det);
		float t_small = (-b - sqdet) / (2 * a);
		float t_big = (-b + sqdet) / (2 * a);

		if (t_small<0.0f) return false;

		Vector3D tmp_v3 = (my_Ray.m_Direction)*t_small;
		float dis = VECTOR_LENGTH(tmp_v3);
		if (dis>a_Dist) return false;

									 
		Vector3D vv = my_Ray.m_Direction;
		VECTOR_NORMALIZE(vv)
			Vector3D intersectPoint = my_Ray.m_Origin + vv * dis;
		if (intersectPoint.x<S_x || intersectPoint.x>B_x) return false;

		a_Dist = dis;
		return true;
	}
};

class Scene
{
private:
	int Num_Modules = 19;
	Module **My_Module;
public:
	Scene()
	{
		My_Module = new Module*[30];
		//light source
		My_Module[0] = new Sphere(Vector3D(8.0f, -0.5f, -8.0f), 0.001f);
		My_Module[0]->Module_Light = true;

		My_Module[1] = new Sphere(Vector3D(9.0f, 0.0f, 1.5f), 0.001f);
		My_Module[1]->Module_Light = true;

		//the mirror sphere
		My_Module[2] = new Sphere(Vector3D(-0.4f, 0.1f, 1.0f), 0.6f);
		My_Module[2]->Module_Ambient_reflection = 0.0f;
		My_Module[2]->Module_Diffuse_reflection = 0.0f;
		My_Module[2]->Module_Specular_reflection = 0.0f;
		My_Module[2]->Module_Color = Vector3D(1, 1, 1);
		My_Module[2]-> Module_reflection = 0.5f;

		//the yellow sphere
		My_Module[3] = new Sphere(Vector3D(0.3f, -1.0f, 0.5f), 0.3f);
		My_Module[3]->Module_Ambient_reflection = 0.1f;
		My_Module[3]->Module_Diffuse_reflection = 0.55f;
		My_Module[3]->Module_Specular_reflection = 0.7f;
		My_Module[3]->Module_Color = Vector3D(0.9, 0.9, 0.1);

		//the red sphere
		My_Module[4] = new Sphere(Vector3D(0.0f, -0.3f, 2.6f), 1.0f);
		My_Module[4]->Module_Ambient_reflection = 0.1f;
		My_Module[4]->Module_Diffuse_reflection = 0.85f;
		My_Module[4]->Module_Specular_reflection = 0.1f;
		My_Module[4]->Module_Color = Vector3D(0.8, 0, 0);

		//the purple sphere
		My_Module[5] = new Sphere(Vector3D(-0.8f, 0.0f, -0.2f), 0.2f);
		My_Module[5]->Module_Ambient_reflection = 0.1f;
		My_Module[5]->Module_Diffuse_reflection = 0.85f;
		My_Module[5]->Module_Specular_reflection = 0.1f;
		My_Module[5]->Module_Color = Vector3D(0.8, 0, 0.8);

		//background
		My_Module[6] = new  Plane(Vector3D(1, 0, 0), 1.0, 0, 0, -100, 100, -100, 100, 'x');
		My_Module[6]->Module_Color = Vector3D(0.7, 0.7, 0.7);
		My_Module[6]->Module_Ambient_reflection = 0.2f;
		My_Module[6]->Module_Diffuse_reflection = 0.5f;
		My_Module[6]->Module_Specular_reflection = 0.2f;

		My_Module[7] = new  Plane(Vector3D(0, -1, 0), 5.0f, -1.0f, 10.0f, 0, 0, -100, 100, 'y');
		My_Module[7]->Module_Color = Vector3D(0.7, 0.7, 0.7);
		My_Module[7]->Module_Ambient_reflection = 0.2f;
		My_Module[7]->Module_Diffuse_reflection = 0.5f;
		My_Module[7]->Module_Specular_reflection = 0.1f;

		My_Module[8] = new  Plane(Vector3D(0, 1, 0), 5.0f, -1.0f, 10.0f, 0, 0, -100, 100, 'y');
		My_Module[8]->Module_Color = Vector3D(0.7, 0.7, 0.7);
		My_Module[8]->Module_Ambient_reflection = 0.2f;
		My_Module[8]->Module_Diffuse_reflection = 0.5f;
		My_Module[8]->Module_Specular_reflection = 0.1f;

		My_Module[9] = new  Plane(Vector3D(0, 0, -1), 5.5f, -1.0f, 10.0f, -10, 10, 0, 0, 'z');
		My_Module[9]->Module_Color = Vector3D(0.7, 0.7, 0.7);
		My_Module[9]->Module_Ambient_reflection = 0.2f;
		My_Module[9]->Module_Diffuse_reflection = 0.5f;
		My_Module[9]->Module_Specular_reflection = 0.1f;

		My_Module[10] = new  Plane(Vector3D(0, 0, 1), 10.0f, -100.0f, 100.0f, -100, 100, 0, 0, 'z');
		My_Module[10]->Module_Color = Vector3D(0.7, 0.7, 0.7);
		My_Module[10]->Module_Ambient_reflection = 0.2f;
		My_Module[10]->Module_Diffuse_reflection = 0.5f;
		My_Module[10]->Module_Specular_reflection = 0.01f;

		My_Module[11] = new  Plane(Vector3D(-1.0, 0, 0), 10.0f, 0.0f, 0.0f, -100, 100, -100, 100, 'x');
		My_Module[11]->Module_Color = Vector3D(0.7, 0.7, 0.7);
		My_Module[11]->Module_Ambient_reflection = 0.2f;
		My_Module[11]->Module_Diffuse_reflection = 0.5f;
		My_Module[11]->Module_Specular_reflection = 0.01f;

		//the cube
		My_Module[12] = new Plane(Vector3D(0, 0, -1), 0.1f, -1, 0, 0.8, 1.8, 0, 0, 'z');
		My_Module[12]->Module_Color = Vector3D(0, 1, 0);
		My_Module[12]->Module_Ambient_reflection = 0.1f;
		My_Module[12]->Module_Diffuse_reflection = 0.55f;
		My_Module[12]->Module_Specular_reflection = 0.4f;

		My_Module[13] = new Plane(Vector3D(0, 0, 1), -1.1f, -1, 0, 0.8, 1.8, 0, 0, 'z');
		My_Module[13]->Module_Color = Vector3D(0, 1, 0);
		My_Module[13]->Module_Ambient_reflection = 0.1f;
		My_Module[13]->Module_Diffuse_reflection = 0.55f;
		My_Module[13]->Module_Specular_reflection = 0.4f;

		My_Module[14] = new Plane(Vector3D(0, 1, 0), -0.8f, -1, 0, 0, 0, 0.1, 1.1, 'y');
		My_Module[14]->Module_Color = Vector3D(0, 1, 0);
		My_Module[14]->Module_Ambient_reflection = 0.1f;
		My_Module[14]->Module_Diffuse_reflection = 0.55f;
		My_Module[14]->Module_Specular_reflection = 0.4f;

		My_Module[15] = new Plane(Vector3D(0, -1, 0), 1.8f, -1, 0, 0, 0, 0.1, 1.1, 'y');
		My_Module[15]->Module_Color = Vector3D(0, 1, 0);
		My_Module[15]->Module_Ambient_reflection = 0.1f;
		My_Module[15]->Module_Diffuse_reflection = 0.55f;
		My_Module[15]->Module_Specular_reflection = 0.4f;

		My_Module[16] = new Plane(Vector3D(1, 0, 0), 0.0f, 0, 0, 0.8, 1.8, 0.1, 1.1, 'x');
		My_Module[16]->Module_Color = Vector3D(0, 1, 0);
		My_Module[16]->Module_Ambient_reflection = 0.1f;
		My_Module[16]->Module_Diffuse_reflection = 0.55f;
		My_Module[16]->Module_Specular_reflection = 0.4f;

		//the cylinder
		My_Module[17] = new Cylinder(Vector3D(-1, -1, 0.5), 0.5f, -1.0f, 0.0f);
		My_Module[17]->Module_Color = Vector3D(0, 0, 1);
		My_Module[17]->Module_Ambient_reflection = 0.1f;
		My_Module[17]->Module_Diffuse_reflection = 0.55f;
		My_Module[17]->Module_Specular_reflection = 0.1f;

		My_Module[18] = new Plane(Vector3D(1, 0, 0), 0.0f, 0.0f, 0.5f, -1.0f, 0, 0.5f, 0, 'o');
		My_Module[18]->Module_Color = Vector3D(0, 0, 1);
		My_Module[18]->Module_Ambient_reflection = 0.1f;
		My_Module[18]->Module_Diffuse_reflection = 0.55f;
		My_Module[18]->Module_Specular_reflection = 0.1f;
	}
	~Scene()
	{
		for (int i = 0; i < Num_Modules; ++i) delete[] My_Module[i];
		delete My_Module;
	}
	int getNum_Modules() { return Num_Modules; }
	Module * getThing(int Index) { return My_Module[Index]; }
};

class World
{
public:
	Scene* my_Scene;
	float my_WindowX1, my_WindowX2, my_WindowY1, my_WindowY2;
	float m_Dx, m_Dy, m_StartX, m_StartY;

	World()
	{
		my_Scene = new Scene();
		my_WindowX1 = 3.0f / SC; my_WindowX2 = -3.0f / SC; my_WindowY1 = 3.0f / SC; my_WindowY2 = -3.0f / SC;
		m_Dx = (my_WindowX1 - my_WindowX2) / WIDTH;
		m_Dy = (my_WindowY1 - my_WindowY2) / HEIGHT;
		m_StartX = my_WindowX2;
		m_StartY = my_WindowY2;
	}
	~World() { delete my_Scene; }
	void RayTracing(Ray& my_Ray, Color &my_Color, int my_Depth, int my_Mirror)
	{
		if (my_Depth > REF_DEPTH) return;
		Vector3D intersectPoint;
		float nearest_distance = 100.0f;
		float tmp_distance = 100.f;
		Module * nearest_thing = 0;
		Module * tmp_thing = 0;
		int shade = 1;
		for (int i = 0; i < my_Scene->getNum_Modules(); ++i)
		{
			tmp_thing = my_Scene->getThing(i);
			if (tmp_thing->isLight()) continue;
			if (tmp_thing->Intersect(my_Ray, tmp_distance))
			{
				if (tmp_distance < nearest_distance)
				{
					nearest_distance = tmp_distance;
					nearest_thing = tmp_thing;
				}
			}
		}
		if (!nearest_thing)
		{
			my_Color.x = 0.9;my_Color.y = 0.9;my_Color.z = 0.8;
			return;
		}
		//ambient
		my_Color = (nearest_thing->Module_Color) * nearest_thing->Module_Ambient_reflection;
		for (int i = 0; i < my_Scene->getNum_Modules(); ++i)
		{
			Module* th = my_Scene->getThing(i);
			if (!th->isLight()) continue;

			//light
			Vector3D vv = my_Ray.m_Direction;
			VECTOR_NORMALIZE(vv)
				intersectPoint = my_Ray.m_Origin + vv * nearest_distance;//Intersect
			//diffuse
			Vector3D L = ((Sphere*)th)->getCentre() - intersectPoint;
			float L_length = VECTOR_LENGTH(L);
			Vector3D minus_L = Vector3D(0, 0, 0) - L;
			//distance
			VECTOR_NORMALIZE(minus_L)
				Ray light_Ray = Ray(((Sphere*)th)->getCentre(), minus_L);
			//shadow
			float dist_important = 1000.f;
			for (int k = 0; k<my_Scene->getNum_Modules(); ++k)
			{
				dist_important = 1000.f;
				Module *tth = my_Scene->getThing(k);
				if (tth->isLight()) continue;
				if (tth == nearest_thing) continue;

				if (tth->Intersect(light_Ray, dist_important))
				{
					if (dist_important < L_length)
					{
						shade = 0; break;//
					}
				}
			}
			float tt = (1.0f) / (0.1 + L_length*0.01 + L_length*L_length*0.005);
			float Fatt = tt>1.0f ? 1.0f : tt;
			VECTOR_NORMALIZE(L);
			Vector3D N = (nearest_thing)->getNormal(intersectPoint);
			VECTOR_NORMALIZE(N)
				float dotproduct = VECTOR_DOT(L, N);
			if (dotproduct > 0)
			{
				my_Color = my_Color + (nearest_thing->Module_Color)*Fatt*dotproduct*nearest_thing->Module_Diffuse_reflection*shade;
				if (my_Color.x > 1) my_Color.x = 1;
				if (my_Color.y > 1) my_Color.y = 1;
				if (my_Color.z > 1) my_Color.z = 1;

			}
			//specular
			Vector3D R = N*2.0f*dotproduct - L;
			Vector3D V = my_Ray.m_Direction;
			V.x = -V.x; V.y = -V.y; V.z = -V.z;
			VECTOR_NORMALIZE(V)
				float dotproduct2 = VECTOR_DOT(R, V);
			if (dotproduct2 > 0)
			{
				float tmp_spec = powf(dotproduct2, 3);
				my_Color = my_Color + Vector3D(1, 1, 1)*tmp_spec*Fatt*nearest_thing->Module_Specular_reflection*shade;
				if (my_Color.x > 1) my_Color.x = 1;
				if (my_Color.y > 1) my_Color.y = 1;
				if (my_Color.z > 1) my_Color.z = 1;
			}
			//refletion
			float thing_reflection = nearest_thing-> Module_reflection;
			if (thing_reflection > 0) {
				Vector3D NN = (nearest_thing)->getNormal(intersectPoint);
				VECTOR_NORMALIZE(NN)
					float sdf = VECTOR_DOT(V, N);//
				Vector3D XXx = N*sdf * 2 - V;//
				Ray ref_ray(intersectPoint + NN*0.00001f, XXx);
				Color rcol(0, 0, 0);
				RayTracing(ref_ray, rcol, my_Depth + 1, 0);
				my_Color = my_Color + rcol*thing_reflection;
				if (my_Color.x > 1) my_Color.x = 1;
				if (my_Color.y > 1) my_Color.y = 1;
				if (my_Color.z > 1) my_Color.z = 1;
			}
		}
	}
	void Render()
	{
		//point of view
		Vector3D eye(0.5, 0, -2.5f);
		for (int yy = 0; yy < HEIGHT; ++yy)
		{
			m_StartX = my_WindowX2;
			for (int xx = 0; xx < WIDTH; ++xx)
			{
				Color c(0xEE, 0xE8, 0xCD);
				Vector3D dir = Vector3D(m_StartX, m_StartY, 0) - eye;
				Ray r(eye, dir);
				RayTracing(r, c, 0, 0);
				test[xx][yy].R = (char)(255 * c.x);
				test[xx][yy].G = (char)(255 * c.y);
				test[xx][yy].B = (char)(255 * c.z);
				m_StartX += m_Dx;
			}
			m_StartY += m_Dy;
		}
	}
};

struct tagFOO
{
	unsigned short bfType;
};

struct tagBITMAPFILEHEADER
{
	unsigned bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned bfOffBits;
};


struct tagBITMAPINFOHEADER
{
	unsigned header_size;
	unsigned width;
	unsigned height;
	unsigned short biplanes;
	unsigned short bitCount;
	unsigned biCompression;
	unsigned image_size;
	unsigned hres;
	unsigned vres;
	unsigned ncolors;
	unsigned nimpcolors;
};

void main()
{
	//.bmp output settings
	tagFOO bmptagfoo;
	tagBITMAPFILEHEADER bmpfileheader;
	tagBITMAPINFOHEADER bmpinfoheader;

	bmptagfoo.bfType = 0x4d42;
	bmpfileheader.bfSize = WIDTH * HEIGHT * 3 + 54;
	bmpfileheader.bfReserved1 = 0;
	bmpfileheader.bfReserved2 = 0;
	bmpfileheader.bfOffBits = 0x36;
	bmpinfoheader.header_size = 0x28;
	bmpinfoheader.width = WIDTH;
	bmpinfoheader.height = HEIGHT;
	bmpinfoheader.biplanes = 1;
	bmpinfoheader.bitCount = 0x18;
	bmpinfoheader.biCompression = 0;
	bmpinfoheader.image_size = HEIGHT * WIDTH * 3;
	bmpinfoheader.vres = 0;
	bmpinfoheader.hres = 0;
	bmpinfoheader.ncolors = 0;
	bmpinfoheader.nimpcolors = 0;
	cout << "Rending...Please be patient..." << endl;
	//Module and render
	World* myWorld = new World();
	myWorld->Render();
	//output the result into the .bmp file
	FILE *fp = fopen("rendering_results.bmp", "wb");
	if (!fp) return;
	fwrite(&bmptagfoo, sizeof(tagFOO), 1, fp);
	fwrite(&bmpfileheader, sizeof(tagBITMAPFILEHEADER), 1, fp);
	fwrite(&bmpinfoheader, sizeof(tagBITMAPINFOHEADER), 1, fp);
	fwrite(test, WIDTH * HEIGHT * 3, 1, fp);
	fclose(fp);
	cout << "Rending complete!" << endl;
	system("pause");
}