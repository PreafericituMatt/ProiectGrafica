///din sageti se muta camera pe baza coordonatelor x,y,z
///din mouse se fac rotatiile din x,y
///din click dreapta se intra in meniul de selectare a culorii masinii 
///apasand tasta ESC, programul se inchide

#include <stdlib.h> ///PENTRU EXIT(0)
#include "glut.h"///INLOCUITOR PENTRU GLU SI GL               
#include "glaux.h" ///PENTRU FUNCTIILE DE TEXTURARE (cele din laborator)
#include <math.h> ///PENTRU CALCULE


///DEFINESTE VALORILE POZITIEI INITIALE A CAMEREI SI A FERESTREI DE VIZIONARE
#define INIT_VIEW_X 0.0    
#define INIT_VIEW_Y 0.0
#define INIT_VIEW_Z -4.5
#define VIEW_LEFT -2.0
#define VIEW_RIGHT 2.0
#define VIEW_BOTTOM -2.0
#define VIEW_TOP 2.0
#define VIEW_NEAR 1.0
#define VIEW_FAR 200.0

///VARIABILE ILUMINARE
GLfloat AmbientLight[] = { 0.3,0.3,0.3,1.0 };
GLfloat DiffuseLight[] = { 0.8,0.8,0.8,1.0 };
GLfloat SpecularLight[] = { 1.0,1.0,1.0,1.0 };
GLfloat SpecRef[] = { 0.7,0.7,0.7,1.0 };
GLfloat pozSursa[] = { -50.0,50.0,100.0,1.0 };
GLint Shine = 128;

///VARIABILE PENTRU MISCAREA CAMEREI PRIN MOUSE SI SAGETI
GLint walkX = 0, walkY = 0;
GLint lookX = 0, lookY = 0, oldX = -1, oldY = -1;

///VARIABILE MENIU CULOARE
GLfloat r = 0, g = 0, b = 1;

///VARIABILE TEXTURA
GLuint IDtextura;
GLuint ID1;
const char* sir;

///VARIABILA UMBRA
GLint check;

///VARIABILE MUTARE 
int xr = -1, zr = -1;

///PROTOTIP TEXTURA
void incarca_textura(const char* s);




///FUNCTII UMBRE
void calcCoeficientiPlan(float P[3][3], float coef[4])
{
	float v1[3], v2[3];
	float length;
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	//calculeaza doi vectori din trei puncte
	v1[x] = P[0][x] - P[1][x];
	v1[y] = P[0][y] - P[1][y];
	v1[z] = P[0][z] - P[1][z];

	v2[x] = P[1][x] - P[2][x];
	v2[y] = P[1][y] - P[2][y];
	v2[z] = P[1][z] - P[2][z];


	//se calculeaza produsul vectorial al celor doi vectori
	//care reprezinta un al treilea vector perpendicular pe plan
	//ale carui componente sunt chiar coeficientii A,B,C din ecuatia planului

	coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
	coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
	coef[z] = v1[x] * v2[y] - v1[y] * v2[x];

	//normalizeaza vectorul
	length = (float)sqrt((coef[x] * coef[x]) + (coef[y] * coef[y]) + (coef[z] * coef[z]));
	coef[x] /= length;
	coef[y] /= length;
	coef[z] /= length;
}


void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4])
{
	GLfloat coefPlan[4];
	GLfloat temp;

	//determina coeficientii planului
	calcCoeficientiPlan(puncte, coefPlan);

	//determina si pe D
	coefPlan[3] = -((coefPlan[0] * puncte[2][0]) + (coefPlan[1] * puncte[2][1]) + (coefPlan[2] * puncte[2][2]));

	//temp=A*xL+B*yL+C*zL+D*w
	temp = coefPlan[0] * pozSursa[0] + coefPlan[1] * pozSursa[1] + coefPlan[2] * pozSursa[2] + coefPlan[3] * pozSursa[3];

	//prima coloana
	mat[0][0] = temp - pozSursa[0] * coefPlan[0];
	mat[1][0] = 0.0f - pozSursa[0] * coefPlan[1];
	mat[2][0] = 0.0f - pozSursa[0] * coefPlan[2];
	mat[3][0] = 0.0f - pozSursa[0] * coefPlan[3];
	//a doua coloana
	mat[0][1] = 0.0f - pozSursa[1] * coefPlan[0];
	mat[1][1] = temp - pozSursa[1] * coefPlan[1];
	mat[2][1] = 0.0f - pozSursa[1] * coefPlan[2];
	mat[3][1] = 0.0f - pozSursa[1] * coefPlan[3];
	//a treia coloana
	mat[0][2] = 0.0f - pozSursa[2] * coefPlan[0];
	mat[1][2] = 0.0f - pozSursa[2] * coefPlan[1];
	mat[2][2] = temp - pozSursa[2] * coefPlan[2];
	mat[3][2] = 0.0f - pozSursa[2] * coefPlan[3];
	//a patra coloana
	mat[0][3] = 0.0f - pozSursa[3] * coefPlan[0];
	mat[1][3] = 0.0f - pozSursa[3] * coefPlan[1];
	mat[2][3] = 0.0f - pozSursa[3] * coefPlan[2];
	mat[3][3] = temp - pozSursa[3] * coefPlan[3];
}




///FUNCTII OBIECTE//

///FUNCTIE TROTINETA
void trotineta()
{

	///TEAVA PT ROATA FATA
	glPushMatrix();
	if (check == 0)
		glColor3f(0.552941, 0.552941, 0.552941);
	else glColor3f(0, 0, 0);
	glTranslatef(1.5, 0.5, 2);
	glRotatef(-45, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	gluCylinder(gluNewQuadric(), 0.05, 0.05, 1, 100, 100);
	glPopMatrix();

	///MANER STANGA 
	glPushMatrix();
	if (check == 0)
		glColor3f(0.1, 0.1, 0.1);
	else glColor3f(0, 0, 0);
	glTranslatef(1, 0.5, 1.55);
	glRotatef(45, 0, 1, 0);
	gluCylinder(gluNewQuadric(), 0.05, 0.05, 1.2, 100, 100);
	glPopMatrix();
	

		///CORPUL TROTINETEI
		///PARALELIPIPEDUL DE JOS
		
	glPushMatrix();
	glTranslatef(2.1, -0.2, 1.4);
	if (check == 0)
		glColor3f(0.4, 0.4, 0.4);
	else glColor3f(0, 0, 0);
	glRotatef(45, 0, 1, 0);
	glScalef(0.7, 0.33, 0.4);
	auxSolidBox(3, 0.5, 1);
	//glutSolidCube(1);
	glPopMatrix();

	
	///cauciucuri

		///FATA
	glPushMatrix();
	glTranslatef(1.5, -0.4, 2.02);
	glRotatef(45, 0, 1, 0);
	if (check == 0)
		glColor3f(1, 1, 1);
	else glColor3f(1, 1, 1);
	glutSolidTorus(0.09, 0.27, 20, 20);
	glPopMatrix();

	///SPATE
	glPushMatrix();
	glTranslatef(2.8, -0.4, 0.67);
	glRotatef(45, 0, 1, 0);
	if (check == 0)
		glColor3f(0, 0, 0);
	else glColor3f(0, 0, 0);
	glutSolidTorus(0.09, 0.27, 20, 20);
	glPopMatrix();


	///PARALELIPIPED SPATE
	glPushMatrix();
	glTranslatef(2.63, -0.03, 0.85);
	if (check == 0)
		glColor3f(r, g, b);
	else glColor3f(0, 0, 0);
	glRotatef(45, 0, 1, 0);
	glRotatef(30, 0, 0, 1);
	glScalef(0.3, 0.27, 0.15);
	glutSolidCube(1);
	glPopMatrix();


	///SFERA ROATA SPATE
	glPushMatrix();
	if (check == 0)
		glColor3f(r, g, b);
	else glColor3f(0, 0, 0);
	glTranslatef(2.8, 0.0, 0.7);
	glRotatef(-45, 0, 1, 0);
	glRotatef(-30, 1, 0, 0);
	glScalef(0.12, 0.12, 0.4);
	auxSolidSphere(1);
	glPopMatrix();

	///janta

		///JANTA FATA

		
	glPushMatrix();
	glTranslatef(1.48, -0.4, 2.0);
	if (check == 0)
		glColor3f(0.458824, 0.458824, 0.458824);
	else glColor3f(0, 0, 0);
	glRotatef(45, 0, 1, 0);
	glScalef(0.006, 0.006, 0.006);
	gluDisk(gluNewQuadric(), 0, 30, 30, 10);
	glPopMatrix();


	///JANTA SPATE

	
	glPushMatrix();
	glTranslatef(2.77, -0.4, 0.65);
	if (check == 0)
		glColor3f(0.458824, 0.458824, 0.458824);
	else glColor3f(0, 0, 0);
	glRotatef(45, 0, 1, 0);
	glScalef(0.006, 0.006, 0.006);
	gluDisk(gluNewQuadric(), 0, 30, 30, 10);
	glPopMatrix();
	
	

	///LUMINA SPATE
	glPushMatrix();
	if (check == 0)
		glColor3f(1.0, 0.0, 0.0);
	else glColor3f(0, 0, 0);
	glTranslatef(3.0, 0.02, 0.5);
	glRotatef(-45, 0, 1, 0);
	glScalef(0.01, 0.01, 0.01);
	gluDisk(gluNewQuadric(), 0, 8, 10, 10);
	glPopMatrix();
		
}


///FUNCTIE DRUM
void drum()
{
	glBegin(GL_QUAD_STRIP);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1200, -1200);
	glTexCoord2f(0.0, 300.0);
	glVertex3f(-1200, 2000, -1);
	glTexCoord2f(300.0, 300.0);
	glVertex2f(1200, -1200);
	glTexCoord2f(300.0, 0.0);
	glVertex3f(1200, 2000, -1);
	glEnd();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///DISPLAY/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Display()
{
	///PARTEA CU UMBRELE IN DISPLAY
	GLfloat matUmbra[4][4];
	GLfloat puncte[3][3] = { { -100.0f, -1.0f, 50.0f },{ 100.0f, -1.0f, -20.0f },{ 50.0f, -1.0f, -20.0f } };


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///FORMAREA UMBREI
	MatriceUmbra(puncte, pozSursa, matUmbra);


	glPushMatrix();

	///CONDITIA PENTRU MISCAREA MOUSE-ULUI
	glTranslatef(walkX, -1, walkY);
	glRotatef(lookY, 0, 1, 0);
	glRotatef(lookX, 1, 0, 0);



	glTranslatef(-1, 0, -6); ///AM SETAT VIEWPORT-UL ASTFEL CA SA POT DEPLASA CAMERA SI SA POT VEDEA OBIECTELE DE LA DISTANTELE DEFINITE (NEAR, FAR)
	///DE ACEEA FOLOSESC VALORI FOARTE MICI PENTRU TRANSLATII, DIMENSIONARII


//////////UMBRE////////////
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glMultMatrixf((GLfloat*)matUmbra);
	check = 1;//setarea variabilei pt creearea umbrelor pt obiecte


	///TROTINETA
	glPushMatrix();
	glTranslatef(xr, -0.3, zr);
	trotineta();
	glPopMatrix();


	glPopMatrix();

	glPopAttrib();
	glPopMatrix();

	glEnable(GL_LIGHTING);


	//////////OBIECTE//////////

	check = 0;

	///TROTINETA
	glPushMatrix();
	glTranslatef(xr, -0.3, zr);
	trotineta();
	glPopMatrix();




	///DRUM
	glPushMatrix();
	glColor3f(0.3, 0.3, 0.3);
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	sir = ".\\road.bmp";
	incarca_textura(sir);
	ID1 = IDtextura;
	glEnable(GL_TEXTURE_2D);
	glTranslatef(0, -1.43, 0);
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, ID1);
	drum();
	glPopMatrix();

	glEnable(GL_LIGHTING); ///AM REACTIVAT ILUMINAREA PENTRU RESTUL OBIECTELOR, MAI PUTIN DRUMUL
	glDisable(GL_TEXTURE_2D);


	glPopMatrix();
	glutSwapBuffers();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///SETARE ADANCIME, MATERIAL SI ILUMINARE
void SetupRend()
{
	glClearColor(1, 0.25, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);         //Enable depth testing
	glEnable(GL_LIGHTING);             //Enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);//Set up and enable light zero
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);                   //Enable color tracking
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);//Set material to follow
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecRef);//Set specular reflectivity and shine
	glMateriali(GL_FRONT, GL_SHININESS, Shine);

}

///MUTAREA DIN SAGETI
void walk(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)    walkY += 1;
	if (key == GLUT_KEY_DOWN)  walkY -= 1;
	if (key == GLUT_KEY_RIGHT) walkX -= 1;
	if (key == GLUT_KEY_LEFT)  walkX += 1;
}

///FLUIDITATE MOUSE
void gaze(int x, int y)
{
	if ((oldX < 0) || (oldY < 0))
	{
		oldX = x;
		oldY = y;
	}
	lookX += y - oldY; lookY += x - oldX; oldX = x; oldY = y;
}

///RESHAPE
void myReshape(int w, int h)
{
	GLfloat Ratio;
	glViewport(0, 0, w, h);
	Ratio = 1.0 * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, Ratio, VIEW_NEAR, VIEW_FAR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(INIT_VIEW_X, INIT_VIEW_Y, INIT_VIEW_Z);
	glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);
}

///TEXTURI
void incarca_textura(const char* s)
{
	AUX_RGBImageRec* pImagineTextura = auxDIBImageLoad(s);

	if (pImagineTextura != NULL)
	{
		glGenTextures(1, &IDtextura);

		glBindTexture(GL_TEXTURE_2D, IDtextura);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImagineTextura->sizeX, pImagineTextura->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, pImagineTextura->data);
	}

	if (pImagineTextura)
	{
		if (pImagineTextura->data)
			free(pImagineTextura->data);

		free(pImagineTextura);
	}

}

///FUNCTIE MENIU CULORI
void myMenu(int id)
{
	if (id == 1)
	{
		r = g = 0;
		b = 1;
		glutPostRedisplay();

	}
	if (id == 2)
	{
		r = 1;
		b = g = 0;
		glutPostRedisplay();
	}
	if (id == 3)
	{
		g = 1;
		r = b = 0;
		glutPostRedisplay();
	}
	if (id == 4)
	{
		r = b = 1;
		g = 0;
		glutPostRedisplay();
	}
	if (id == 5)
	{
		b = 0;
		r = g = 1;
		glutPostRedisplay();
	}
	if (id == 6)
	{
		b = r = g = .7;
		glutPostRedisplay();
	}

}

///FUNCTIE PENTRU TASTA ESC 
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;


	case 'a':
		xr = xr - 1;
		zr = zr + 1;
		trotineta();
		glutPostRedisplay();
		break;

	case 'd':
		xr = xr + 1;
		zr = zr - 1;
		trotineta();
		glutPostRedisplay();
		break;

	case 'A':
		xr = xr - 1;
		zr = zr + 1;
		trotineta();
		glutPostRedisplay();
		break;

	case 'D':
		xr = xr + 1;
		zr = zr - 1;
		trotineta();
		glutPostRedisplay();
		break;
	}

}


///MAIN-UL
void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Trotineta");
	PlaySound("tzanca.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP); //sunteul
	glutCreateMenu(myMenu);
	glutAddMenuEntry("blue", 1);
	glutAddMenuEntry("red", 2);
	glutAddMenuEntry("green", 3);
	glutAddMenuEntry("pink", 4);
	glutAddMenuEntry("yellow", 5);
	glutAddMenuEntry("grey", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(Display);
	glutSpecialFunc(walk);
	glutPassiveMotionFunc(gaze);
	SetupRend();
	glEnable(GL_NORMALIZE);
	glutMainLoop();
}
