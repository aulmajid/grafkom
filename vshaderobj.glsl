#version 150
in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;

//transform
uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProj;

//shading
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform mat4 Projection;
void main()
{  
	fN = vNormal;
    fE = (ModelView*vec4(vPosition,1.0)).xyz;
    fL = LightPosition.xyz;
    
    if( LightPosition.w != 0.0 ) {
		fL = LightPosition.xyz - vPosition.xyz;
    }
    gl_Position = vProj * vView * vModel * vec4(vPosition,1.0);
}
