// axes.cpp


#include "headers.h"
#include "axes.h"


Axes::Axes()

{
  // VAO

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // Positions

  vec3 verts[] = {
    vec3(0,0,0), vec3(1,0,0),
    vec3(0,0,0), vec3(0,1,0),
    vec3(0,0,0), vec3(0,0,1)
  };

  GLuint vertexbuffer;
  glGenBuffers( 1, &vertexbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
  glBufferData( GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // Colours

  vec3 colours[] = {
    vec3(1,0,0), vec3(1,0,0),
    vec3(0,1,0), vec3(0,1,0),
    vec3(0.3,0.3,1), vec3(0.3,0.3,1)
  };

  GLuint colourbuffer;
  glGenBuffers( 1, &colourbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, colourbuffer );
  glBufferData( GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), colours, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  program.initFromFile( "axes.vert", "axes.frag" );
}



void Axes::draw( mat4 &MVP, vec3 lightDir )

{
  glDisable( GL_DEPTH_TEST );

  program.activate();

  glUniformMatrix4fv( glGetUniformLocation( program.id(), "MVP"), 1, GL_TRUE, &MVP[0][0] );

  glLineWidth( 3.0 );

  glBindVertexArray( VAO );
  glDrawArrays( GL_LINES, 0, 6 );

  glLineWidth( 1.0 );

  // Hack to draw lightDir with old openGL

  glEnable( GL_DEPTH_TEST );

  vec3 dir = 10 * lightDir;
  glColor3f(1,1,1);
  glBegin( GL_LINES );
  glVertex3f(0,0,0);
  glVertex3fv( &dir.x );
  glEnd();

  program.deactivate();
}

