#include <GL/glut.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>

/* dimensions de la fenetre */
int width = 600;
int height = 400;

/*************************************************************************/
/* Bezier */
/*************************************************************************/

#define MAX_POINTS 100
typedef struct
{
	float x, y, z;
} Point;

typedef Point Grille[MAX_POINTS][MAX_POINTS];
typedef Point Vecteur[MAX_POINTS];
Grille carreau;

Point pt(float x, float y, float z)
{
	Point p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}

// dimension de la grille
int grille_x = 4;
int grille_y = 4;

// copie d'une colonne de la grille (DE CASTELJAU)
Vecteur colonne;
int taille_colonne;

// contient les points de contrôle obtenus par interpolation linéaire
Vecteur pts_inter;
int taille_pts_inter;

// fonction d'ajout des points de contrôle initiaux
void ajouter_pts_controle()
{
	carreau[0][0] = pt(0.f, 0.f, 0.f);
	carreau[0][1] = pt(0.f, 1.f, 0.f);
	carreau[0][2] = pt(0.f, 2.f, 0.f);
	carreau[0][3] = pt(0.f, 3.f, 0.f);

	carreau[1][0] = pt(1.f, 0.f, 0.f);
	carreau[1][1] = pt(1.f, 1.f, 2.f);
	carreau[1][2] = pt(1.f, 2.f, 1.f);
	carreau[1][3] = pt(1.f, 3.f, 1.f);

	carreau[2][0] = pt(2.f, 0.f, 0.f);
	carreau[2][1] = pt(2.f, 1.f, 2.f);
	carreau[2][2] = pt(2.f, 2.f, 1.f);
	carreau[2][3] = pt(2.f, 3.f, 1.f);

	carreau[3][0] = pt(3.f, 0.f, 0.f);
	carreau[3][1] = pt(3.f, 1.f, 0.f);
	carreau[3][2] = pt(3.f, 2.f, 0.f);
	carreau[3][3] = pt(3.f, 3.f, 0.f);
}

// interpolation linéaire en 3D
Point interpolationLineaire(Point p1, Point p2, float t)
{
	Point res;
	res.x = (1.f - t) * p1.x + t * p2.x;
	res.y = (1.f - t) * p1.y + t * p2.y;
	res.z = (1.f - t) * p1.z + t * p2.z;
	return res;
}

// algorithme de DE CASTELJAU pour une colonne
Point deCasteljauColonne(int col, float t)
{
	// copie des points de la colonne col
	for (int i = 0; i < grille_x; i++)
	{
		colonne[i] = carreau[i][col];
	}
	taille_colonne = grille_y;

	// tant qu'il ne reste pas un seul point
	while (taille_colonne > 1)
	{
		for(int i = 0; i < taille_colonne - 1; i++)
		{
			colonne[i] = interpolationLineaire(colonne[i], colonne[i+1], t);
		}
		taille_colonne--;
	}

	return colonne[0];
}

// algorithme de DE CASTELJAU pour les nouveaux points de controle
Point deCasteljau(float s)
{
	// sans copie on fait directement dans pts_inter !

	while (taille_pts_inter > 1)
	{
		for (int i = 0; i < taille_pts_inter - 1; i++)
		{
			pts_inter[i] = interpolationLineaire(pts_inter[i], pts_inter[i+1], s);
		}
		taille_pts_inter--;
	}

	// le dernier point est sur la surface
	return pts_inter[0];
}

// interpolation bi linéaire
Point interpolationBiLineaire(float s, float t)
{
	// interpolation linéaire pour chaque colonne
	for (int i = 0; i < grille_x; i++)
	{
		pts_inter[i] = deCasteljauColonne(i, t);
	}
	taille_pts_inter = grille_x;

	return deCasteljau(s);
}

/*************************************************************************/
/* Fonctions de dessin */
/*************************************************************************/

/* rouge vert bleu entre 0 et 1 */
void chooseColor(double r, double g, double b)
{
	glColor3d(r,g,b);
}

void drawPoint(double x, double  y, double z)
{
	glBegin(GL_POINTS);
	glVertex3d(x,y,z);
	glEnd();
}

void drawLine(double x1, double  y1, double z1, double x2, double y2, double z2)
{
	glBegin(GL_LINES);
	glVertex3d(x1,y1,z1);
	glVertex3d(x2,y2,z2);
	glEnd();
}

void drawQuad(Point p1, Point p2, Point p3, Point p4)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex3f(p1.x,p1.y,p1.z);
	glVertex3f(p2.x,p2.y,p2.z);
	glVertex3f(p3.x,p3.y,p3.z);
	glVertex3f(p4.x,p4.y,p4.z);
	glEnd();
}

/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	int i,j;
	
	glEnable(GL_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60, (float)width/height, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-4,-2,-5);
	glRotated(25,1,0,0);
	glRotated(90,0,1,0);
	glRotated(-120,1,1,1);
	
	// ** Repere du Monde **
	chooseColor(1.0,0,0); // axe x=rouge
	drawLine(0,0,0, 1,0,0);
	chooseColor(0,1.0,0); // axe y=vert
	drawLine(0,0,0, 0,1,0);
	chooseColor(0,0,1.0); // axe z=bleu
	drawLine(0,0,0, 0,0,1);
	
	// ** Grille sur plan XY **
	chooseColor(0.2,0.2,0.2); 
	glLineWidth(2.0);
	for (i=1; i<10; i++)
	{
		drawLine(i,0,0, i,9,0);
		drawLine(0,i,0, 9,i,0);
	}

	// ** Dessinez ici **

	// grille de contrôle
	chooseColor(1.f, 1.f, 1.f);
	for (int i = 0; i < grille_x - 1; i++)
	{
		for (int j = 0; j < grille_y - 1; j++)
		{
			drawQuad(carreau[i][j], carreau[i+1][j], carreau[i+1][j+1], carreau[i][j+1]);
		}
	}

	// surface
	chooseColor(0.2f, 0.f, 0.f);
	float s_incr = 0.001f;
	float t_incr = 0.001f;

	for (float s = 0.f; s <= 1.f - s_incr; s += s_incr)
	{
		for (float t = 0.f; t <= 1.f - t_incr; t += t_incr)
		{
			Point p1 = interpolationBiLineaire(s, t);
			Point p2 = interpolationBiLineaire(s + s_incr, t);
			Point p3 = interpolationBiLineaire(s + s_incr, t + t_incr);
			Point p4 = interpolationBiLineaire(s, t + t_incr);
			drawQuad(p1, p2, p3, p4);
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

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Clic at %d %d\n",x,y);
		glutPostRedisplay();
	}

}



/*************************************************************************/
/* Fonction principale */
/*************************************************************************/

int main(int argc, char *argv[]) 
{
	/* Initialisations globales */
	glutInit(&argc, argv);

	/* D�finition des attributs de la fenetre OpenGL */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	/* Placement de la fenetre */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	
	/* Cr�ation de la fenetre */
    glutCreateWindow("Carreau de Bezier");

	/* Choix de la fonction d'affichage */
	glutDisplayFunc(display);

	/* Choix de la fonction de redimensionnement de la fenetre */
	glutReshapeFunc(reshape);
	
	/* Choix des fonctions de gestion du clavier */
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(special);
	
	/* Choix de la fonction de gestion de la souris */
	glutMouseFunc(mouse);

	/* ajout des points de contrôle */
	ajouter_pts_controle();

	/* Boucle principale */
    glutMainLoop();

	/* M�me si glutMainLoop ne rends JAMAIS la main, il faut d�finir le return, sinon
	le compilateur risque de crier */
    return 0;
}
