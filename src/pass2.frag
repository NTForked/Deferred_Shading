// Pass 2 fragment shader
//
// Apply diffuse lighting to fragment.  Later do Phong lighting.
//
// Determine whether fragment is in shadow.  If so, reduce intensity to 50%.

#version 300 es
precision mediump float;


uniform vec3      lightDir;	    // direction to light in WCS
uniform mat4      WCS_to_lightCCS;  // transform from WCS to light's CCS
uniform sampler2D shadowBuffer;     // texture [0,1]x[0,1] of depth from light.  Values in [0,1].
uniform sampler2D objTexture;       // object's texture (might not be provided)
uniform bool      texturing;        // =1 if object's texture is provided

in vec3 colour;        // fragment colour
in vec3 normal;        // fragment normal in WCS
in vec3 wcsPosition;   // fragemnt position in WCS
in vec2 texCoords;     // fragment texture coordinates (if provided) 


out vec4 fragColour;   // fragment's final colour

uniform vec3 eyePosition;
uniform vec3 Ia;
uniform vec3 ks;
uniform vec3 kd;
uniform float shininess;

void main()

{
  vec3 Iin = vec3(1.0, 1.0, 1.0);
  // Calculate the position of this fragment in the light's CCS.

  vec4 ccsLightPos = WCS_to_lightCCS * vec4(wcsPosition, 1.0); 

  // Calculate the depth of this fragment in the light's CCS in the range [0,1]
  
  float fragDepth = (ccsLightPos.z + 1.0)/2.0; 

  // Determine the (x,y) coordinates of this fragment in the light's
  // CCS in the range [0,1]x[0,1].

  vec2 shadowTexCoords = ccsLightPos.xy * 0.5 + 0.5; 

  // Look up the depth from the light in the shadowBuffer texture.

  float shadowDepth = texture(shadowBuffer, shadowTexCoords).r; 

  


  // Determine whether the fragment is in shadow.
  //
  // If results look bad, add a bit to the shadow texture depth to
  // prevent z-fighting.
 
  
  
  vec3 normaler = normalize(normal);
  vec3 lightColour = vec3(1.0);
  vec3 texColour = texture(objTexture, texCoords).rgb;

  // Cheating ambient lighting
  vec3 ambient = 0.15 * texColour;

  // Doing diffuse lighting
  float diff = max(dot(lightDir, normaler), 0.0);
  vec3 diffuse = diff * lightColour;

  // Specular lighting
  vec3 eyeDir = normalize(eyePosition - wcsPosition);
  
  vec3 halfwayDir = normalize(lightDir + eyeDir);
  float spec = pow(max(dot(normaler, halfwayDir), 0.0), 64.0);
  vec3 specular = spec * lightColour;
  
  // Lighting Total
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  float shadow = fragDepth - bias > shadowDepth ? 1.0 : 0.0;
  vec3 lighting = vec3(1.0, 1.0, 1.0);

  // Compute Illumination via Phong
  // Choose the colour either from the object's texture (if
  // 'texturing' == 1) or from the input colour.
  if (texturing){
    lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColour;
  } else {
    lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * colour;
  }

  // Output the fragment colour, modified by the illumination model
  // and shadowing.
  fragColour = vec4(lighting, 1.0f);  

  

  




 


  

}
