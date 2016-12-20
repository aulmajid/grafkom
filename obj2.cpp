#include "Angel.h"
#include <stdlib.h>
#include <vector>
#include <fstream>	// FILE
//----------------------------------------------------------------------------
std::vector<vec3> vertices;
std::vector<vec3> normals; // for shading
std::vector<vec2> UVs; //for texture

GLuint v_model,v_view,v_proj;
GLuint start_1, count_1, start_2, count_2, start_3, count_3;
GLint transform1;

mat4 projection = Frustum(-0.2, 0.2, -0.2, 0.2, 0.2, 2.0);
mat4 view = Translate(0.0, 0.0, -0.35);
mat4 model = Translate(0.0, 0.0, 0.0);

mat4 model_1, model_2, model_3;
mat4 model_1T; //translate
float model_1R_y;
mat4 model_1S = Translate(0.0, 0.0, 0.0);

void initModel_1(const char*);
void drawModel_1();
void initModel_2(const char*);
void drawModel_2();
void initModel_3(const char*);
void drawModel_3();


bool loadObj(const char* filename);


void init( void )
{
    initModel_1("obj/mr_krab.obj");
    initModel_2("obj/mr_krab.obj");
    initModel_3("obj/mr_krab.obj");
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER,
                  8* vertices.size()*sizeof(GLfloat)
                  ,NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER,
                     0,
                     (3* vertices.size()*sizeof(GLfloat)),
                     vertices.data() );

    glBufferSubData( GL_ARRAY_BUFFER,
                     (3* vertices.size()*sizeof(GLfloat)),
                     (3* normals.size()*sizeof(GLfloat)),
                     normals.data() );

    glBufferSubData( GL_ARRAY_BUFFER,
                     6*vertices.size()*sizeof(GLfloat),
                     (2* UVs.size()*sizeof(GLfloat)),
                     UVs.data() );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshaderobj.glsl", "fshaderobj.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint Position = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( Position );
    glVertexAttribPointer( Position, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint Normal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( Normal );
    glVertexAttribPointer( Normal, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(3*vertices.size()*sizeof(GLfloat)) );

    GLuint UV = glGetAttribLocation( program, "vUV" );
    glEnableVertexAttribArray( UV );
    glVertexAttribPointer( UV, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(6*vertices.size()*sizeof(GLfloat)) );

    //transform
    v_model = glGetUniformLocation( program, "vModel" );
    v_view= glGetUniformLocation( program, "vView" );
    v_proj= glGetUniformLocation( program, "vProj" );



    //material - static ==========================================
    vec4 light_position( 0.0, 0.0, 2.0, 0.0 );
    vec4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    vec4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    vec4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    vec4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    vec4 material_specular( 1.0, 0.0, 1.0, 1.0 );
    float  material_shininess = 5.0;

    vec4 ambient_product = light_ambient * material_ambient;
    vec4 diffuse_product = light_diffuse * material_diffuse;
    vec4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
                  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
                  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
                  1, specular_product );
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
                  1, light_position );
    glUniform1f( glGetUniformLocation(program, "Shininess"),
                 material_shininess );
    //=============================================================

    glEnable( GL_DEPTH_TEST );

    glClearColor( 0, 0, 0, 1.0 ); /* white background */
}
//----------------------------------------------------------------------------
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //model = Translate(0,-.25,0) *  Scale(.005,.005,.005);

    drawModel_1();
    drawModel_2();
    drawModel_3();

    //glUniformMatrix4fv( v_model, 1, GL_TRUE, model );
    glUniformMatrix4fv( v_view, 1, GL_TRUE, view );
    glUniformMatrix4fv( v_proj, 1, GL_TRUE,projection );

    GLfloat angle = 0.001 * glutGet(GLUT_ELAPSED_TIME);
    mat3 mTransform2 = mat3(cos(angle),   sin(angle),        0.0,
                        -sin(angle), cos(angle), 0.0,
	                       0.0, 0.0,  1);
    glUniformMatrix3fv( transform1, 1, false, mTransform2 );

    //glDrawArrays( GL_TRIANGLES, 0,vertices.size());
    glutSwapBuffers();
}


//----------------------------------------------------------------------------
void initModel_1(const char*  filename)
{
    start_1 = vertices.size();
    loadObj(filename);
    count_1 = vertices.size() - start_1;
    model_1T = Translate(0,0,0);
    model_1 = model_1T * Scale(.08,.08,.08);

}

void drawModel_1()
{
    model_1 = model_1T * RotateY(model_1R_y) * Scale(.08,.08,.08);
    glUniformMatrix4fv( v_model, 1, GL_TRUE, model_1 );
    glDrawArrays(GL_TRIANGLES, start_1, count_1);
}

void initModel_2(const char*  filename)
{
    start_2 = vertices.size();
    loadObj(filename);
    count_2 = vertices.size() - start_2;
    model_2 = Translate(-.25,0,0) * Scale(.0008,.0008,.0008);
    //model_1S = Scale(.25,.25,.25);
}
void drawModel_2()
{
    glUniformMatrix4fv( v_model, 1, GL_TRUE, model_2 );
    glDrawArrays(GL_TRIANGLES, start_2, count_2);
}

void initModel_3(const char* filename)
{

    start_3 = vertices.size();
    loadObj(filename);
    count_3 = vertices.size() - start_3;
    model_3 = Translate(.25,0,0) * Scale(.0005,.0005,.0005);
}

void drawModel_3()
{
    glUniformMatrix4fv( v_model, 1, GL_TRUE, model_3 );
    glDrawArrays(GL_TRIANGLES, start_3, count_3);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//vec2 xy_model1;
void keyboard( unsigned char key, int x, int y )
{
    switch( key )
    {
    case 033: // Escape Key
    case 'q':
    case 'Q':
        exit( EXIT_SUCCESS );
        break;

    case 'w': //maju
        model_1T[0][3] -= sin(model_1R_y * DegreesToRadians) * -.01;
        model_1T[2][3] += cos(model_1R_y * DegreesToRadians) * .01;
        break;
    case 's': //mundur
        model_1T[0][3] -= sin(model_1R_y * DegreesToRadians) * .01;
        model_1T[2][3] += cos(model_1R_y * DegreesToRadians) * -.01;
        break;
    case 'a':
        model_1R_y += 3;
        model_1T[0][3] -= sin(model_1R_y * DegreesToRadians) * -.01;
        model_1T[2][3] += cos(model_1R_y * DegreesToRadians) * .01;
        break;
    case 'd':
        model_1R_y += -3;
        model_1T[0][3] -= sin(model_1R_y * DegreesToRadians) * -.01;
        model_1T[2][3] += cos(model_1R_y * DegreesToRadians) * .01;
        break;

    }
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{

}
//----------------------------------------------------------------------------
void
idle( void )
{
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize(600, 600 );

    glutCreateWindow( "RESPONSI" );

    glewInit();

    init();

    glutIdleFunc(idle);
    glutDisplayFunc( display );
    glutMouseFunc(mouse);
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}


bool loadObj(const char* filename)
{
    FILE * file = fopen(filename, "r");
    if( file == NULL )
    {
        printf("failed to open the file !\n");
        return false;
    }
    std::vector<vec3> temp_vertices;
    std::vector<vec3> temp_normals;
    std::vector<vec2> temp_UVs;
    while( true )
    {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            vec3 v;
            fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z );
            temp_vertices.push_back(v);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
        {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_UVs.push_back(uv);
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
        {
            vec3 n;
            fscanf(file, "%f %f %f\n", &n.x, &n.y, &n.z );
            temp_normals.push_back(n);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            unsigned int vIDX[3], uvIDX[3], nIDX[3];
            /*int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                         &vIDX[0], &uvIDX[0], &nIDX[0],
                         &vIDX[1], &uvIDX[1], &nIDX[1],
                         &vIDX[2], &uvIDX[2], &nIDX[2]); */

            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n",
                         &vIDX[0], &nIDX[0],
                         &vIDX[1], &nIDX[1],
                         &vIDX[2], &nIDX[2]);

            if(matches!=6)
            {
                printf("failed to read command !\n");
                return false;
            }
            //obj file index start at 1
            for(int a = 0; a<3 ; a++ )
            {
                vertices.push_back(temp_vertices[vIDX[a]-1]);
                normals.push_back(temp_normals[nIDX[a]-1]);
                UVs.push_back(vec2(0,0));
            }
        }
    }
    //don't forget to close the file, clean your own mess!
    fclose(file);
    return true;
}

