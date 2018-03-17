#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
//#include <GL/glx.h>

/* dimensions de la fenetre */
int width = 600;
int height = 400;


/*************************************************************************/
/* Bezier */
/*************************************************************************/

#define MAX_POINTS 100
typedef struct
{
	float x, y;
} Point;
int nbPoints = 0;
typedef Point Vecteur[MAX_POINTS];
Vecteur poly;

// Vecteur utilisé pour l'algorithme de DE CASTELJAU
Vecteur copie;

// déplacement de point de contrôle
int deplacement_pc = 0;
int indice_pc = -1;

// Fonction		: interpolationLineaire 
// Argument(s)	: - p1 : premier Point
//				  - p1 : deuxième Point
//				  - t : paramètre d'interpolation [0;1]
// Résultat		: un nouveau Point
Point interpolationLineaire(Point p1, Point p2, float t)
{
	Point res;
	res.x = (1.f - t) * p1.x + t * p2.x;
	res.y = (1.f - t) * p1.y + t * p2.y;
	return res;
}

// Fonction	 	: deCasteljau (Algorithme de DE CASTELJAU)
// Argument(s)	:	- t : paramètre d'interpolation [0;1]
// Résultat 	:	- p : le point sur la courbe de Bézier au paramètre t
Point deCasteljau(float t)
{
	// copie des points de contrôle
	for (int i = 0; i < nbPoints; i++)
	{
		copie[i].x = poly[i].x;
		copie[i].y = poly[i].y;
	}

	// on itère tant qu'il reste plus d'un point
	int nb = nbPoints;
	while (nb > 1)
	{
		// on interpole chaque point avec le paramètre t
		for (int i = 0; i < nb - 1; i++)
		{
			copie[i] = interpolationLineaire(copie[i], copie[i+1], t);
		}
		nb--;
	}

	// le dernier point restant est le point sur la courbe
	return copie[0]; 
} 

/*************************************************************************/
/* Fonctions de dessin */
/*************************************************************************/

/* rouge vert bleu entre 0 et 1 */
void chooseColor(double r, double g, double b)
{
	glColor3d(r,g,b);
}

void drawPoint(double x, double  y)
{
	glBegin(GL_POINTS);
	glVertex2d(x,y);
	glEnd();
}

void drawLine(double x1, double  y1, double x2, double y2)
{
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();
}


/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT);

	// tracé du polygone de controle
	chooseColor(1,1,1);
	if (nbPoints == 1) 
		drawPoint(poly[0].x, poly[0].y);
	else if (nbPoints > 1) 
	{
		for (i=0;i<nbPoints-1;i++)
			drawLine(poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
	}

	// ** Dessiner ici ! **
	float t_incr = 0.001f;
	chooseColor(1, 0, 0);

	if (nbPoints > 1)
	{
		for (float t = 0.f; t <= 1.f - t_incr; t += t_incr)
		{
			Point p1 = deCasteljau(t);
			Point p2 = deCasteljau(t + t_incr);
			drawLine(p1.x, p1.y, p2.x, p2.y);
		}
	}

	glutSwapBuffers();
}

void keyboard(unsigned char keycode, int x, int y)
{
	/* touche ECHAP */
	if (keycode==27)
		exit(0);
	/* touche ECHAP */
	if (keycode=='z')
		printf("La touche z a ete enfoncee\n");

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	width=w;
	height=h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D( 0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
}

/* Evenement : clic souris */
void mouse(int button, int state, int x, int y)
{
	int i;

	// ajout d'un point de contrôle
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Clic at %d %d\n",x,y);
		poly[nbPoints].x = x;
		poly[nbPoints].y = y;
		nbPoints++;
		glutPostRedisplay();
	}

	// déplacement d'un point de contrôle
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		// recherche du plus proche point
		float dist2_min = 10000.f;
		for (int i = 0; i < nbPoints; i++)
		{
			float dist2 = (poly[i].x - x) * (poly[i].x - x) + 
			(poly[i].y - y) * (poly[i].y - y);

			if (dist2 < dist2_min)
			{
				dist2_min = dist2;
				indice_pc = i;
			}
		}
		deplacement_pc = 1;
	}

	// fin de déplacement d'un point de contrôle
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		deplacement_pc = 0;
		indice_pc = -1;
	}
}

/* Evenement : souris bouge */
void mousemove(int x, int y)
{
	if (deplacement_pc == 1)
	{
		poly[indice_pc].x = x;
		poly[indice_pc].y = y;
		display();
	}
}

/*************************************************************************/
/* Fonction principale */
/*************************************************************************/

int main(int argc, char *argv[]) 
{
	/* Initialisations globales */
	glutInit(&argc, argv);

	/* Définition des attributs de la fenetre OpenGL */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	/* Placement de la fenetre */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	
	/* Création de la fenetre */
    glutCreateWindow("Courbe de Bézier");

	/* Choix de la fonction d'affichage */
	glutDisplayFunc(display);

	/* Choix de la fonction de redimensionnement de la fenetre */
	glutReshapeFunc(reshape);
	
	/* Choix des fonctions de gestion du clavier */
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(special);
	
	/* Choix de la fonction de gestion de la souris */
	glutMouseFunc(mouse);
	glutMotionFunc(mousemove);

	/* Boucle principale */
    glutMainLoop();

	/* Même si glutMainLoop ne rends JAMAIS la main, il faut définir le return, sinon
	le compilateur risque de crier */
    return 0;
}
