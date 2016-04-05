// G-buffer renderer


#include "headers.h"
#include "renderer.h"
#include "shader.h"


// Draw a quad over the full screen.  This generates a fragment for
// each pixel on the screen, allowing the fragment shader to run on
// each fragment.


void drawFullscreenQuad()

{
  vec2 verts[4] = { vec2( -1, -1 ), vec2( -1, 1 ), vec2( 1, -1 ), vec2( 1, 1 ) };
    
  GLuint VAO, VBO;

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );

  glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

  glDeleteBuffers( 1, &VBO );
  glDeleteVertexArrays( 1, &VAO );
}


// Render the scene in three passes.


void Renderer::render( seq<wfModel *> &objs, mat4 &WCS_to_VCS, mat4 &WCS_to_CCS, vec3 &lightDir, vec3 &eyePosition )

{
  gbuffer->BindForWriting();

  // Pass 1: Store shadow map

  pass1Prog->activate();

  gbuffer->BindTexture( SHADOW_GBUFFER, 1 ); // location 1

  int activeDrawBuffers1[] = { SHADOW_GBUFFER };
  gbuffer->setDrawBuffers( 1, activeDrawBuffers1 );

  // WCS to light coordinate system (LCS)
  //
  // Rotate VCS so that lightDir lines up with -z.  Then apply
  // orthographic transform to define frustum boundaries.

  mat4 WCS_to_lightCCS
    = ortho( -worldRadius * windowWidth/windowHeight, worldRadius * windowWidth/windowHeight,
	     -worldRadius, worldRadius,
	     1*worldRadius, 3*worldRadius )
    * lookat( worldCentre + 2*worldRadius*lightDir, worldCentre, vec3(0,1,0) );

  // Draw objects

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );

  for (int i=0; i<objs.size(); i++) {
    mat4 OCS_to_lightCCS = WCS_to_lightCCS * objs[i]->objToWorldTransform;
    pass1Prog->setMat4( "OCS_to_lightCCS", OCS_to_lightCCS );
    objs[i]->draw( pass1Prog );
  }

  pass1Prog->deactivate();

  if (debug == 1) {
    gbuffer->DrawGBuffers();
    return;
  }

  // Pass 2: Render with shadows

  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); // fragment shader renders to main framebuffer now

  pass2Prog->activate();

  pass2Prog->setVec3( "lightDir", lightDir );
  pass2Prog->setVec3( "eyePosition", eyePosition );
  pass2Prog->setMat4( "WCS_to_lightCCS", WCS_to_lightCCS );

  gbuffer->BindTexture( SHADOW_GBUFFER, 1 );
  pass2Prog->setInt( "shadowBuffer", 1 );

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );

  for (int i=0; i<objs.size(); i++) {
    pass2Prog->setMat4( "OCS_to_WCS", objs[i]->objToWorldTransform );
    mat4 OCS_to_CCS = WCS_to_CCS * objs[i]->objToWorldTransform;
    pass2Prog->setMat4( "OCS_to_CCS", OCS_to_CCS );
    objs[i]->draw( pass2Prog );
  }

  pass2Prog->deactivate();
}
