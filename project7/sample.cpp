#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <omp.h>
#include <math.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include <CL/cl.h>
#include <CL/cl_gl.h>


struct xyzw
{
	float x, y, z, w;
};

struct rgba
{
	float r, g, b, a;
};

GLuint			hPobj;
GLuint			hCobj;
cl_mem			dPobj;
cl_mem			dCobj;
struct xyzw *	hVel;
cl_mem			dVel;
cl_command_queue	CmdQueue;
cl_device_id		Device;
cl_kernel		Kernel;
cl_platform_id		Platform;
cl_program		Program;
cl_platform_id		PlatformID;

const GLfloat AXES_COLOR[] = { 1., .5, 0. };
const float XMIN = { -100.0 };
const float XMAX = { 100.0 };
const float YMIN = { -100.0 };
const float YMAX = { 100.0 };
const float ZMIN = { -100.0 };
const float ZMAX = { 100.0 };
const float VMIN = { -100. };
const float VMAX = { 100. };

const int NUM_PARTICLES = 1024 * 1024;
const int LOCAL_SIZE = 32;
const char *CL_FILE_NAME = { "qud.cl" };
const char *CL_BINARY_NAME = { "particles.nv" };

double	ElapsedTime;
int		ShowPerformance;

size_t GlobalWorkSize[3] = { NUM_PARTICLES, 1, 1 };
size_t LocalWorkSize[3] = { LOCAL_SIZE,    1, 1 };



//	interface from a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	To reset the animation, press "r"
//	To quit, press ESC or "q"
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Sample -- Joe Graphics" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };



// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

float	TransXYZ[3];		// set by glui translation widgets
GLfloat	RotMatrix[4][4];	// set by glui rotation widget
float	Scale2;				// scaling factors
GLuint	SphereList;
GLuint	SphereList2;

int	Paused;
cl_uint status;

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );
void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	InitCL();
bool	IsCLExtensionSupported(const char *);
void	PrintCLError(cl_int, char * = "", FILE * = stderr);
void	Quit();
float	Ranf(float, float);
void	ResetParticles();


// main program:

int
main( int argc, char *argv[ ] )
{
	glutInit(&argc, argv);
	InitGraphics();
	InitLists();
	InitCL();
	Reset();
	InitMenus();
	glutSetWindow(MainWindow);
	glutMainLoop();

	// this is here to make the compiler happy:

	return 0;
}


void
ResetParticles()
{
	glBindBuffer(GL_ARRAY_BUFFER, hPobj);
	struct xyzw *points = (struct xyzw *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		points[i].x = Ranf(XMIN, XMAX);
		points[i].y = Ranf(YMIN, YMAX);
		points[i].z = Ranf(ZMIN, ZMAX);
		points[i].w = 1.;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);


	glBindBuffer(GL_ARRAY_BUFFER, hCobj);
	struct rgba *colors = (struct rgba *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		colors[i].r = Ranf(.1f, 1.);
		colors[i].g = Ranf(.3f, 1.);
		colors[i].b = Ranf(.3f, 1.);
		colors[i].a = 1.;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);


	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		hVel[i].x = Ranf(VMIN, VMAX);
		hVel[i].y = Ranf(0., VMAX);
		hVel[i].z = Ranf(VMIN, VMAX);
	}
}




// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate()
{
	cl_int  status;
	double time0, time1;

	// acquire the vertex buffers from opengl:

	glutSetWindow(MainWindow);
	glFinish();

	status = clEnqueueAcquireGLObjects(CmdQueue, 1, &dPobj, 0, NULL, NULL);
	PrintCLError(status, "clEnqueueAcquireGLObjects (1): ");
	status = clEnqueueAcquireGLObjects(CmdQueue, 1, &dCobj, 0, NULL, NULL);
	PrintCLError(status, "clEnqueueAcquireGLObjects (2): ");

	if (ShowPerformance)
		time0 = omp_get_wtime();

	// 11. enqueue the Kernel object for execution:

	cl_event wait;
	status = clEnqueueNDRangeKernel(CmdQueue, Kernel, 1, NULL, GlobalWorkSize, LocalWorkSize, 0, NULL, &wait);
	PrintCLError(status, "clEnqueueNDRangeKernel: ");

	if (ShowPerformance)
	{
		status = clWaitForEvents(1, &wait);
		PrintCLError(status, "clWaitForEvents: ");
		time1 = omp_get_wtime();
		ElapsedTime = time1 - time0;
	}

	clFinish(CmdQueue);
	status = clEnqueueReleaseGLObjects(CmdQueue, 1, &dCobj, 0, NULL, NULL);
	PrintCLError(status, "clEnqueueReleaseGLObjects (2): ");
	status = clEnqueueReleaseGLObjects(CmdQueue, 1, &dPobj, 0, NULL, NULL);
	PrintCLError(status, "clEnqueueReleaseGLObjects (2): ");

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{
	glutSetWindow(MainWindow);
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-300., 300., -300., 300., 0.1, 4000.);
	else
		gluPerspective(50., 1., 0.1, 4000.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., -100., 4000., 0., -200., 0., 0., 1., 0.);
	//glTranslatef((GLfloat)TransXYZ[0], (GLfloat)TransXYZ[1], -(GLfloat)TransXYZ[2]);
	glTranslatef(TransXYZ[0], TransXYZ[1], -TransXYZ[2]);

	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);
	glMultMatrixf((const GLfloat *)RotMatrix);
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	float scale2 = 1. + Scale2;		// because glui translation starts at 0.
	if (scale2 < MINSCALE)
		scale2 = MINSCALE;
	glScalef((GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2);

	glDisable(GL_FOG);

	if (AxesOn != GLUIFALSE)
		glCallList(AxesList);

	// ****************************************
	// Here is where you draw the current state of the particles:
	// ****************************************

	glBindBuffer(GL_ARRAY_BUFFER, hPobj);
	glVertexPointer(4, GL_FLOAT, 0, (void *)0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, hCobj);
	glColorPointer(4, GL_FLOAT, 0, (void *)0);
	glEnableClientState(GL_COLOR_ARRAY);

	glPointSize(3.);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glPointSize(1.);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glCallList(SphereList);

	if (ShowPerformance)
	{
		char str[128];
		sprintf(str, "%6.1f GigaParticles/Sec", (float)NUM_PARTICLES / ElapsedTime / 1000000000.);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0., 100., 0., 100.);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3f(1., 1., 1.);
		DoRasterString(5., 95., 0., str);
	}

	glutSwapBuffers();
	glFlush();
}



void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);
	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);


	// setup the callback routines:

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);
	glutIdleFunc(Animate);

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
#endif
}




void
InitLists()
{
	SphereList = glGenLists(1);
	glNewList(SphereList, GL_COMPILE);
	glColor3f(.9f, .9f, 0.);
	glPushMatrix();
	glTranslatef(-600., -800., 0.);
	glutWireSphere(600., 100., 100.);
	glPopMatrix();

	glColor3f(0., .9f, .9f);
	glPushMatrix();
	glTranslatef(600., -800., 0.);
	glutWireSphere(600., 100., 100.);
	glPopMatrix();

	glEndList();

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glColor3fv(AXES_COLOR);
	glLineWidth(AXES_WIDTH);
	Axes(150.);
	glLineWidth(1.);
	glEndList();
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		case 'r':
		case 'R':
			Reset();
			ResetParticles();
			status = clEnqueueWriteBuffer(CmdQueue, dVel, CL_FALSE, 0, 4 * sizeof(float)*NUM_PARTICLES, hVel, 0, NULL, NULL);
			PrintCLError(status, "clEneueueWriteBuffer: ");
			glutIdleFunc(Animate);
			glutSetWindow(MainWindow);
			glutPostRedisplay();
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}



void
Reset()
{
	ActiveButton = 0;
	AxesOn = GLUIFALSE;
	Paused = GLUIFALSE;
	Scale = 1.0;
	Scale2 = 0.0;		// because add 1. to it in Display( )
	ShowPerformance = GLUITRUE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;

	RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0] = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1] = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3] = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}



void
InitCL()
{
	// see if we can even open the opencl Kernel Program
	// (no point going on if we can't):

	FILE *fp = fopen(CL_FILE_NAME, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open OpenCL source file '%s'\n", CL_FILE_NAME);
		return;
	}

	// 2. allocate the host memory buffers:

	cl_int status;		// returned status from opencl calls
						// test against CL_SUCCESS

						// get the platform id:

	status = clGetPlatformIDs(1, &Platform, NULL);
	PrintCLError(status, "clGetPlatformIDs: ");

	// get the device id:

	status = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 1, &Device, NULL);
	PrintCLError(status, "clGetDeviceIDs: ");


	// since this is an opengl interoperability program,
	// check if the opengl sharing extension is supported,
	// (no point going on if it isn't):
	// (we need the Device in order to ask, so can't do it any sooner than here)
	
	if (IsCLExtensionSupported("cl_khr_gl_sharing"))
	{
		fprintf(stderr, "cl_khr_gl_sharing is supported.\n");
	}
	else
	{
		fprintf(stderr, "cl_khr_gl_sharing is not supported -- sorry.\n");
		return;
	}
	


	// 3. create an opencl context based on the opengl context:

	cl_context_properties props[] =
	{
		CL_GL_CONTEXT_KHR,		(cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR,			(cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM,		(cl_context_properties)Platform,
		0
	};

	cl_context Context = clCreateContext(props, 1, &Device, NULL, NULL, &status);
	PrintCLError(status, "clCreateContext: ");

	// 4. create an opencl command queue:

	CmdQueue = clCreateCommandQueue(Context, Device, 0, &status);
	if (status != CL_SUCCESS)
		fprintf(stderr, "clCreateCommandQueue failed\n");

	// create the velocity array and the opengl vertex array buffer and color array buffer:

	delete[] hVel;
	hVel = new struct xyzw[NUM_PARTICLES];

	glGenBuffers(1, &hPobj);
	glBindBuffer(GL_ARRAY_BUFFER, hPobj);
	glBufferData(GL_ARRAY_BUFFER, 4 * NUM_PARTICLES * sizeof(float), NULL, GL_STATIC_DRAW);

	glGenBuffers(1, &hCobj);
	glBindBuffer(GL_ARRAY_BUFFER, hCobj);
	glBufferData(GL_ARRAY_BUFFER, 4 * NUM_PARTICLES * sizeof(float), NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// unbind the buffer

										// fill those arrays and buffers:

	ResetParticles();

	// 5. create the opencl version of the opengl buffers:

	dPobj = clCreateFromGLBuffer(Context, 0, hPobj, &status);
	PrintCLError(status, "clCreateFromGLBuffer (1)");

	dCobj = clCreateFromGLBuffer(Context, 0, hCobj, &status);
	PrintCLError(status, "clCreateFromGLBuffer (2)");

	// 5. create the opencl version of the velocity array:

	dVel = clCreateBuffer(Context, CL_MEM_READ_WRITE, 4 * sizeof(float)*NUM_PARTICLES, NULL, &status);
	PrintCLError(status, "clCreateBuffer: ");

	// 6. enqueue the command to write the data from the host buffers to the Device buffers:

	status = clEnqueueWriteBuffer(CmdQueue, dVel, CL_FALSE, 0, 4 * sizeof(float)*NUM_PARTICLES, hVel, 0, NULL, NULL);
	PrintCLError(status, "clEneueueWriteBuffer: ");

	// 7. read the Kernel code from a file:

	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *clProgramText = new char[fileSize + 1];		// leave room for '\0'
	size_t n = fread(clProgramText, 1, fileSize, fp);
	clProgramText[fileSize] = '\0';
	fclose(fp);

	// create the text for the Kernel Program:

	char *strings[1];
	strings[0] = clProgramText;
	Program = clCreateProgramWithSource(Context, 1, (const char **)strings, NULL, &status);
	if (status != CL_SUCCESS)
		fprintf(stderr, "clCreateProgramWithSource failed\n");
	delete[] clProgramText;

	// 8. compile and link the Kernel code:

	char *options = { "" };
	status = clBuildProgram(Program, 1, &Device, options, NULL, NULL);
	if (status != CL_SUCCESS)
	{
		size_t size;
		clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
		cl_char *log = new cl_char[size];
		clGetProgramBuildInfo(Program, Device, CL_PROGRAM_BUILD_LOG, size, log, NULL);
		fprintf(stderr, "clBuildProgram failed:\n%s\n", log);
		delete[] log;
	}

#ifdef  EXPORT_BINARY
	size_t binary_sizes;
	status = clGetProgramInfo(Program, CL_PROGRAM_BINARY_SIZES, 0, NULL, &binary_sizes);
	PrintCLError(status, "clGetProgramInfo (1):");
	//fprintf( stderr, "binary_sizes = %d\n", binary_sizes );
	size_t size;
	status = clGetProgramInfo(Program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &size, NULL);
	PrintCLError(status, "clGetProgramInfo (2):");
	//fprintf( stderr, "size = %d\n", size );
	unsigned char *binary = new unsigned char[size];
	status = clGetProgramInfo(Program, CL_PROGRAM_BINARIES, size, &binary, NULL);
	PrintCLError(status, "clGetProgramInfo (3):");
	FILE *fpbin = fopen(CL_BINARY_NAME, "wb");
	if (fpbin == NULL)
	{
		fprintf(stderr, "Cannot create '%s'\n", CL_BINARY_NAME);
	}
	else
	{
		fwrite(binary, 1, size, fpbin);
		fclose(fpbin);
		fprintf(stderr, "Binary written to '%s'\n", CL_BINARY_NAME);
	}
	delete[] binary;
#endif

	// 9. create the Kernel object:

	Kernel = clCreateKernel(Program, "Particle", &status);
	PrintCLError(status, "clCreateKernel failed: ");


	// 10. setup the arguments to the Kernel object:

	status = clSetKernelArg(Kernel, 0, sizeof(cl_mem), &dPobj);
	PrintCLError(status, "clSetKernelArg (1): ");

	status = clSetKernelArg(Kernel, 1, sizeof(cl_mem), &dVel);
	PrintCLError(status, "clSetKernelArg (2): ");

	status = clSetKernelArg(Kernel, 2, sizeof(cl_mem), &dCobj);
	PrintCLError(status, "clSetKernelArg (3): ");
	
}


#define TOP	2147483647.		// 2^31 - 1	

float
Ranf(float low, float high)
{
	long random();		// returns integer 0 - TOP

	float r = (float)rand();
	return(low + r * (high - low) / (float)RAND_MAX);
}


bool
IsCLExtensionSupported(const char *extension)
{
	// see if the extension is bogus:

	if (extension == NULL || extension[0] == '\0')
		return false;

	char * where = (char *)strchr(extension, ' ');
	if (where != NULL)
		return false;

	// get the full list of extensions:

	size_t extensionSize;
	clGetDeviceInfo(Device, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
	char *extensions = new char[extensionSize];
	clGetDeviceInfo(Device, CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);

	for (char * start = extensions; ; )
	{
		where = (char *)strstr((const char *)start, extension);
		if (where == 0)
		{
			delete[] extensions;
			return false;
		}

		char * terminator = where + strlen(extension);	// points to what should be the separator

		if (*terminator == ' ' || *terminator == '\0' || *terminator == '\r' || *terminator == '\n')
		{
			delete[] extensions;
			return true;
		}
		start = terminator;
	}

	delete[] extensions;
	return false;
}


struct errorcode
{
	cl_int		statusCode;
	char *		meaning;
}
ErrorCodes[] =
{
	{ CL_SUCCESS,				"" },
{ CL_DEVICE_NOT_FOUND,			"Device Not Found" },
{ CL_DEVICE_NOT_AVAILABLE,		"Device Not Available" },
{ CL_COMPILER_NOT_AVAILABLE,		"Compiler Not Available" },
{ CL_MEM_OBJECT_ALLOCATION_FAILURE,	"Memory Object Allocation Failure" },
{ CL_OUT_OF_RESOURCES,			"Out of resources" },
{ CL_OUT_OF_HOST_MEMORY,		"Out of Host Memory" },
{ CL_PROFILING_INFO_NOT_AVAILABLE,	"Profiling Information Not Available" },
{ CL_MEM_COPY_OVERLAP,			"Memory Copy Overlap" },
{ CL_IMAGE_FORMAT_MISMATCH,		"Image Format Mismatch" },
{ CL_IMAGE_FORMAT_NOT_SUPPORTED,	"Image Format Not Supported" },
{ CL_BUILD_PROGRAM_FAILURE,		"Build Program Failure" },
{ CL_MAP_FAILURE,			"Map Failure" },
{ CL_INVALID_VALUE,			"Invalid Value" },
{ CL_INVALID_DEVICE_TYPE,		"Invalid Device Type" },
{ CL_INVALID_PLATFORM,			"Invalid Platform" },
{ CL_INVALID_DEVICE,			"Invalid Device" },
{ CL_INVALID_CONTEXT,			"Invalid Context" },
{ CL_INVALID_QUEUE_PROPERTIES,		"Invalid Queue Properties" },
{ CL_INVALID_COMMAND_QUEUE,		"Invalid Command Queue" },
{ CL_INVALID_HOST_PTR,			"Invalid Host Pointer" },
{ CL_INVALID_MEM_OBJECT,		"Invalid Memory Object" },
{ CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,	"Invalid Image Format Descriptor" },
{ CL_INVALID_IMAGE_SIZE,		"Invalid Image Size" },
{ CL_INVALID_SAMPLER,			"Invalid Sampler" },
{ CL_INVALID_BINARY,			"Invalid Binary" },
{ CL_INVALID_BUILD_OPTIONS,		"Invalid Build Options" },
{ CL_INVALID_PROGRAM,			"Invalid Program" },
{ CL_INVALID_PROGRAM_EXECUTABLE,	"Invalid Program Executable" },
{ CL_INVALID_KERNEL_NAME,		"Invalid Kernel Name" },
{ CL_INVALID_KERNEL_DEFINITION,		"Invalid Kernel Definition" },
{ CL_INVALID_KERNEL,			"Invalid Kernel" },
{ CL_INVALID_ARG_INDEX,			"Invalid Argument Index" },
{ CL_INVALID_ARG_VALUE,			"Invalid Argument Value" },
{ CL_INVALID_ARG_SIZE,			"Invalid Argument Size" },
{ CL_INVALID_KERNEL_ARGS,		"Invalid Kernel Arguments" },
{ CL_INVALID_WORK_DIMENSION,		"Invalid Work Dimension" },
{ CL_INVALID_WORK_GROUP_SIZE,		"Invalid Work Group Size" },
{ CL_INVALID_WORK_ITEM_SIZE,		"Invalid Work Item Size" },
{ CL_INVALID_GLOBAL_OFFSET,		"Invalid Global Offset" },
{ CL_INVALID_EVENT_WAIT_LIST,		"Invalid Event Wait List" },
{ CL_INVALID_EVENT,			"Invalid Event" },
{ CL_INVALID_OPERATION,			"Invalid Operation" },
{ CL_INVALID_GL_OBJECT,			"Invalid GL Object" },
{ CL_INVALID_BUFFER_SIZE,		"Invalid Buffer Size" },
{ CL_INVALID_MIP_LEVEL,			"Invalid MIP Level" },
{ CL_INVALID_GLOBAL_WORK_SIZE,		"Invalid Global Work Size" },
};

void
PrintCLError(cl_int errorCode, char * prefix, FILE *fp)
{
	if (errorCode == CL_SUCCESS)
		return;

	const int numErrorCodes = sizeof(ErrorCodes) / sizeof(struct errorcode);
	char * meaning = "";
	for (int i = 0; i < numErrorCodes; i++)
	{
		if (errorCode == ErrorCodes[i].statusCode)
		{
			meaning = ErrorCodes[i].meaning;
			break;
		}
	}

	fprintf(fp, "%s %s\n", prefix, meaning);
}


void
Quit()
{
	glutSetWindow(MainWindow);
	glFinish();
	glutDestroyWindow(MainWindow);


	// 13. clean everything up:

	clReleaseKernel(Kernel);
	clReleaseProgram(Program);
	clReleaseCommandQueue(CmdQueue);
	clReleaseMemObject(dPobj);
	clReleaseMemObject(dCobj);

	exit(0);
}
