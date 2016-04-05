// Phong shading demo
//
// Draw a Phong-shaded object


#include "headers.h"
#include "linalg.h"
#include "wavefront.h"
#include "renderer.h"
#include "shader.h"
#include "font.h"
#include "seq.h"
#include "axes.h"


seq<wfModel *> objs;		// the objects

Renderer *renderer;		// class to do multipass rendering

Axes *axes;

float theta = 0;
bool sleeping = false;
float timeOffset = 0;
bool showAxes = true;

GLuint windowWidth = 1200;
GLuint windowHeight = 900;
float factor = 0;

// Drawing function

float fovy;
vec3 eyePosition;
vec3 worldCentre;
float worldRadius;


void display()

{
  // WCS-to-VCS

  mat4 WCS_to_VCS
    = lookat( eyePosition, worldCentre, vec3(0,1,0) );

  // WCS-to-CCS

  float n = (eyePosition - worldCentre).length() - worldRadius;
  float f = (eyePosition - worldCentre).length() + worldRadius;

  mat4 WCS_to_CCS
    = perspective( fovy, windowWidth / (float) windowHeight, n, f )
    * WCS_to_VCS;

  // Light direction is in the WCS, but rotates

  vec3 lightDir(1,1.5,1);
  lightDir = lightDir.normalize();
  vec4 rotatedLightDir = rotate( theta, vec3(0,1,0) ) * vec4( lightDir, 0 );
  vec3 rotatedLightDir3 = vec3( rotatedLightDir.x, rotatedLightDir.y, rotatedLightDir.z );

  // Draw the objects

  renderer->render( objs, WCS_to_VCS, WCS_to_CCS, rotatedLightDir3, eyePosition );

  // Draw the world axes

  if (showAxes && !renderer->debugOn()) {
    mat4 axesTransform = WCS_to_CCS * scale( 10, 10, 10 );
    axes->draw( axesTransform, rotatedLightDir3 );
  }

  // Output status message

  char buffer[1000];
  renderer->makeStatusMessage( buffer );
  glColor3f(0.3,0.3,1.0);
  printString( buffer, 10, 10, windowWidth, windowHeight );

  // Done

  glutSwapBuffers();
}


// Reshape the window


void reshape( int newWidth, int newHeight )

{
  windowWidth = newWidth;
  windowHeight = newHeight;

  glViewport( 0, 0, newWidth, newHeight );

  renderer->reshape( newWidth, newHeight );
}



// Update the viewpoint angle upon idle


void idle()

{
  // Determine time elapsed from start

  static bool firstTime = true;
  static struct timeb startTime;

  if (firstTime) {
    ftime( &startTime );
    firstTime = false;
  }

  struct timeb thisTime;
  ftime( &thisTime );
  float elapsedTime = (thisTime.time - startTime.time) + (thisTime.millitm - startTime.millitm) / 1000.0 - timeOffset;

  // Set angle based on elapsed time

  if (!sleeping)
    theta = elapsedTime * 0.3;

  // Wait a bit

#ifdef LINUX
  usleep(10000);
#endif

  glutPostRedisplay();
}


// Handle a key press


void keyPress( unsigned char key, int x, int y )

{
  static struct timeb startTime;

  switch (key) {

  case 27: exit(0);

  case 'p':
    sleeping = !sleeping;
    struct timeb thisTime;
    ftime( &thisTime );
    if (sleeping) {
      startTime.time = thisTime.time;
      startTime.millitm = thisTime.millitm;
    } else
      timeOffset += (thisTime.time - startTime.time) + (thisTime.millitm - startTime.millitm) / 1000.0;
    break;

  case 'd':
    renderer->incDebug();
    break;

  case 'a':
    showAxes = !showAxes;
    break;

  case 'F':
    factor += 0.01;
    cout << "factor = " << factor << endl;
    break;

  case 'f':
    factor -= 0.01;
    cout << "factor = " << factor << endl;
    break;
  }
}


void specialKeyPress( int key, int x, int y )

{
  switch (key) {
  case GLUT_KEY_UP:
    eyePosition = eyePosition / 1.1;
    break;
  case GLUT_KEY_DOWN:
    eyePosition = eyePosition * 1.1;
    break;
  case GLUT_KEY_LEFT:
    fovy *= 1.1;
    break;
  case GLUT_KEY_RIGHT:
    fovy /= 1.1;
    break;
  }

  glutPostRedisplay();
}


// Main program


int main( int argc, char **argv )

{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " scene.obj ..." << endl;
    exit(1);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

  int shift = (argc < 4 ? 0 : atoi(argv[3]) * 450);

  glutInitWindowSize( windowWidth, windowHeight );
  glutInitWindowPosition( 50 + shift, 50 );
  glutCreateWindow( "toon shading" );

  GLenum status = glewInit();
  if (status != GLEW_OK) {
    std::cerr << "Error: " << glewGetErrorString(status) << std::endl;
    return 1;
  }

  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutIdleFunc( idle );
  glutKeyboardFunc( keyPress );
  glutSpecialFunc( specialKeyPress );

  // Set up world objects

  for (int i=1; i<argc; i++)
    objs.add( new wfModel( argv[i] ) );

  if (objs.size() == 0) {
    cout << "no objects defined" << endl;
    exit(1);
  }

  // Find world centre and radius

  worldCentre = vec3(0,0,0);
  for (int i=0; i<objs.size(); i++)
    worldCentre = worldCentre + objs[i]->centre;
  worldCentre = (1/(float)objs.size()) * worldCentre;

  worldRadius = 0;
  for (int i=0; i<objs.size(); i++) {
    float radius = (objs[i]->centre - worldCentre).length() + objs[i]->radius;
    if (radius > worldRadius)
      worldRadius = radius;
  }

  // Point camera to the model

  const float initEyeDistance = 5.0;

  eyePosition = (initEyeDistance * worldRadius) * vec3(0.7,0.3,1).normalize() + worldCentre;
  fovy = 2 * atan2( 1, initEyeDistance );

  // Set up renderer

  axes = new Axes();

  renderer = new Renderer( windowWidth, windowHeight );

  // Go

  glutMainLoop();

  return 1;
}



void glErrorReport( char *where )

{
  GLuint errnum;
  const char *errstr;

  while ((errnum = glGetError())) {
    errstr = reinterpret_cast<const char *>(gluErrorString(errnum));
    std::cerr << where << ": " << errstr << std::endl;
  }

  cerr << "passed " << where << endl;
}
