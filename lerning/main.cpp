#include <iostream>
#include <fstream>
#include <SDL.h>
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>

using namespace std;

struct coordinate{
	float x,y,z;
	coordinate(float a,float b,float c) : x(a),y(b),z(c) {};
};

//for faces, it can contain triangles and quads as well, the four variable contain which is that
struct face{
	int facenum;
	bool four;
	int faces[4];
	face(int facen,int f1,int f2,int f3) : facenum(facen){  //constructor for triangle
		faces[0]=f1;
		faces[1]=f2;
		faces[2]=f3;
		four=false;
	}
	face(int facen,int f1,int f2,int f3,int f4) : facenum(facen){ //overloaded constructor for quad
		faces[0]=f1;
		faces[1]=f2;
		faces[2]=f3;
		faces[3]=f4;
		four=true;
	}
};

float angle=0.0;        //we rotate or object with angle degrees

int loadObject(const char* filename)
{
	vector<string*> coord;        //read every single line of the obj file as a string
	vector<coordinate*> vertex;
	vector<face*> faces;
	vector<coordinate*> normals;       //normal vectors for every face
	ifstream in(filename);     //open the .obj file
	if(!in.is_open())       //if not opened, exit with -1
	{
		std::cout << "Nor opened" << std::endl;
		return -1;
	}
	char buf[256];
	//read in every line to coord
	while(!in.eof())
	{
		in.getline(buf,256);
		coord.push_back(new std::string(buf));
	}
	//go through all of the elements of coord, and decide what kind of element is that
	for(int i=0;i<coord.size();i++)
	{
		if(coord[i]->c_str()[0]=='#')   //if it is a comment (the first character is #)
			continue;       //we don't care about that
		else if(coord[i]->c_str()[0]=='v' && coord[i]->c_str()[1]==' ') //if vector
		{
			float tmpx,tmpy,tmpz;
			sscanf(coord[i]->c_str(),"v %f %f %f",&tmpx,&tmpy,&tmpz);       //read in the 3 float coordinate to tmpx,tmpy,tmpz
			vertex.push_back(new coordinate(tmpx,tmpy,tmpz));       //and then add it to the end of our vertex list
		}else if(coord[i]->c_str()[0]=='v' && coord[i]->c_str()[1]=='n')        //if normal vector
		{
			float tmpx,tmpy,tmpz;   //do the same thing
			sscanf(coord[i]->c_str(),"vn %f %f %f",&tmpx,&tmpy,&tmpz);
			normals.push_back(new coordinate(tmpx,tmpy,tmpz));     
		}else if(coord[i]->c_str()[0]=='f')     //if face
		{
			int a,b,c,d,e;
			if(count(coord[i]->begin(),coord[i]->end(),' ')==3)     //if it is a triangle (it has 3 space in it)
			{
				sscanf(coord[i]->c_str(),"f %d//%d %d//%d %d//%d",&a,&b,&c,&b,&d,&b);
				faces.push_back(new face(b,a,c,d));     //read in, and add to the end of the face list
			}else{
				sscanf(coord[i]->c_str(),"f %d//%d %d//%d %d//%d %d//%d",&a,&b,&c,&b,&d,&b,&e,&b);
				faces.push_back(new face(b,a,c,d,e));   //do the same, except we call another constructor, and we use different pattern
			}
		}
	}
	//raw
	int num;        //the id for the list
	num=glGenLists(1);      //generate a unique
	glNewList(num,GL_COMPILE);      //and create it
	for(int i=0;i<faces.size();i++)
	{
		if(faces[i]->four)      //if it's a quad draw a quad
		{
			glBegin(GL_QUADS);
			//basically all I do here, is use the facenum (so the number of the face) as an index for the normal, so the 1st normal owe to the first face
			//I subtract 1 because the index start from 0 in C++
			glNormal3f(normals[faces[i]->facenum-1]->x,normals[faces[i]->facenum-1]->y,normals[faces[i]->facenum-1]->z);
			//draw the faces
			glVertex3f(vertex[faces[i]->faces[0]-1]->x,vertex[faces[i]->faces[0]-1]->y,vertex[faces[i]->faces[0]-1]->z);
			glVertex3f(vertex[faces[i]->faces[1]-1]->x,vertex[faces[i]->faces[1]-1]->y,vertex[faces[i]->faces[1]-1]->z);
			glVertex3f(vertex[faces[i]->faces[2]-1]->x,vertex[faces[i]->faces[2]-1]->y,vertex[faces[i]->faces[2]-1]->z);
			glVertex3f(vertex[faces[i]->faces[3]-1]->x,vertex[faces[i]->faces[3]-1]->y,vertex[faces[i]->faces[3]-1]->z);
			glEnd();
		}else{
			glBegin(GL_TRIANGLES);
			glNormal3f(normals[faces[i]->facenum-1]->x,normals[faces[i]->facenum-1]->y,normals[faces[i]->facenum-1]->z);
			glVertex3f(vertex[faces[i]->faces[0]-1]->x,vertex[faces[i]->faces[0]-1]->y,vertex[faces[i]->faces[0]-1]->z);
			glVertex3f(vertex[faces[i]->faces[1]-1]->x,vertex[faces[i]->faces[1]-1]->y,vertex[faces[i]->faces[1]-1]->z);
			glVertex3f(vertex[faces[i]->faces[2]-1]->x,vertex[faces[i]->faces[2]-1]->y,vertex[faces[i]->faces[2]-1]->z);
			glEnd();
		}
	}
	glEndList();
	//delete everything to avoid memory leaks
	for(int i=0;i<coord.size();i++)
		delete coord[i];
	for(int i=0;i<faces.size();i++)
		delete faces[i];
	for(int i=0;i<normals.size();i++)
		delete normals[i];
	for(int i=0;i<vertex.size();i++)
		delete vertex[i];
	return num;     //return with the id
}

int cube;

void init ()
{
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,640.0/480.0,1.0,500.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	cube=loadObject("test.obj");    //load the test.obj file
	glEnable(GL_LIGHTING);  //we enable lighting, to make the 3D object to 3D
	glEnable(GL_LIGHT0);
	float col[]={1.0,1.0,1.0,1.0};  //light color is white
	glLightfv(GL_LIGHT0,GL_DIFFUSE,col);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	float pos[]={-1.0,1.0,-2.0,1.0};        //set the position
	glLightfv(GL_LIGHT0,GL_POSITION,pos);
	glTranslatef(0.0,0.0,-5.0);
	glRotatef(angle,0.0,0.0,1.0);
	glCallList(cube);       //draw the 3D mesh
}

int main(int argc,char *args[]) {
	cout << "program is start now" << endl;
	if((SDL_Init(SDL_INIT_EVERYTHING)==-1)) { 
		cout << "Could not initialize SDL: "<<endl;
		return-1;
	}
	SDL_WM_SetCaption("hello",NULL);
	SDL_Surface* screen = SDL_SetVideoMode(640,480,32,SDL_SWSURFACE | SDL_OPENGL);
	bool running=true;
	UINT32 start;
	SDL_Event event;
	init();

	while(running)
	{
		start=SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				running=false;
				break;
			}
		}
		display();
		SDL_GL_SwapBuffers();
		angle+=0.5;
		if(angle>360)
			angle-=360;
		if(1000/30>(SDL_GetTicks()-start))
			SDL_Delay(1000/30-(SDL_GetTicks()-start));
	}

	system("pause");
	SDL_Quit();
return 0;
}