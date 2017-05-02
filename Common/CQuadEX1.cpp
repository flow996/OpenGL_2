#include "CQuadEX1.h"

CQuadEX1::CQuadEX1()
{
	
	ObjLoad();
	// Create and initialize a buffer object 
	CreateBufferObject();
	m_bUpdateProj = false;
}


void CQuadEX1::CreateBufferObject()
{
    glGenVertexArrays( 1, &m_uiVao );
    glBindVertexArray( m_uiVao );

    // Create and initialize a buffer object

    glGenBuffers( 1, &m_uiBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(m_Points) + sizeof(m_Colors), NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(m_Points), m_Points ); 
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_Points), sizeof(m_Colors), m_Colors );
}

void CQuadEX1::SetShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle)
{
    // Load shaders and use the resulting shader program
	if( uiShaderHandle == MAX_UNSIGNED_INT) m_uiProgram = InitShader("vsVtxColor.glsl", "fsVtxColor.glsl");
	else m_uiProgram = uiShaderHandle;

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( m_uiProgram, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( m_uiProgram, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(m_Points)) );

	m_uiModelView = glGetUniformLocation( m_uiProgram, "ModelView" );
	m_mxView = mxModelView;
	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxView );

    m_uiProjection = glGetUniformLocation( m_uiProgram, "Projection" );
	m_mxProjection = mxProjection;
	glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void CQuadEX1::SetViewMatrix(mat4 &mat)
{
	m_mxView = mat;
	m_bUpdateMV = true;
}

void CQuadEX1::SetProjectionMatrix(mat4 &mat)
{
	m_mxProjection = mat;
	m_bUpdateProj = true;
}

void CQuadEX1::SetTRSMatrix(mat4 &mat)
{
	m_mxTRS = mat;
	m_bUpdateMV = true;
}

void CQuadEX1::SetColor(GLfloat vColor[4])
{
	for( int i = 0 ; i < 6 ; i++ ) {
		m_Colors[i].x = vColor[0];
		m_Colors[i].y = vColor[1];
		m_Colors[i].z = vColor[2];
		m_Colors[i].w = vColor[3];
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_Points), sizeof(m_Colors), m_Colors );
}

void CQuadEX1::SetVtxColors(GLfloat vLFColor[], GLfloat vLRColor[], GLfloat vTRColor[], GLfloat vTLColor[])
{
	m_Colors[0].x = vLFColor[0];
	m_Colors[0].y = vLFColor[1];
	m_Colors[0].z = vLFColor[2];
	m_Colors[0].w = vLFColor[3];
	m_Colors[3] = m_Colors[0];

	m_Colors[1].x = vLRColor[0];
	m_Colors[1].y = vLRColor[1];
	m_Colors[1].z = vLRColor[2];
	m_Colors[1].w = vLRColor[3];

	m_Colors[2].x = vTRColor[0];
	m_Colors[2].y = vTRColor[1];
	m_Colors[2].z = vTRColor[2];
	m_Colors[2].w = vTRColor[3];
	m_Colors[4] = m_Colors[2];

	m_Colors[5].x = vTLColor[0];
	m_Colors[5].y = vTLColor[1];
	m_Colors[5].z = vTLColor[2];
	m_Colors[5].w = vTLColor[3];

	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_Points), sizeof(m_Colors), m_Colors );
}

void CQuadEX1::Draw()
{
	glUseProgram( m_uiProgram );
	glBindVertexArray( m_uiVao );
	if( m_bUpdateMV ) {
		m_mxMVFinal = m_mxView*m_mxTRS;
		m_bUpdateMV = false;
	}
	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );

	if( m_bUpdateProj ) {
		glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
		m_bUpdateProj = false;
	}
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
}



void CQuadEX1::DrawW()
{
	glBindVertexArray( m_uiVao );

	if( m_bUpdateMV ) {
		m_mxMVFinal = m_mxView*m_mxTRS;
		m_bUpdateMV = false;
	}

	glUniformMatrix4fv( m_uiModelView, 1, GL_TRUE, m_mxMVFinal );
	if( m_bUpdateProj ) {
		glUniformMatrix4fv( m_uiProjection, 1, GL_TRUE, m_mxProjection );
		m_bUpdateProj = false;
	}
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
}

void CQuadEX1::ObjLoad() {
	int vindex = 0;//儲存temp_vertices目前引數
	int mindex = 0;//儲存m_Points目前引數
	objfile = fopen("test.obj", "r");//開啟檔案(以example1.cpp為基準)
	if (objfile == NULL) {
		printf("fail to open the file !\n");
	}
	//讀檔，直至結尾前，持續讀入"一行"資料，存於lineHeader
	while (fscanf(objfile, "%s", lineHeader) != EOF) {
		if (strcmp(lineHeader, "v") == 0) {//點(V)資料
			fscanf(objfile, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			temp_vertices[vindex] = vec4(vertex[0], vertex[1], vertex[2], 1.0f);
			vindex++;
		}
		else if (strcmp(lineHeader, "f") == 0) {//面(F)資料紀錄(三角)面的生成順序
			fscanf(objfile, "%d/%d/%d %d/%d/%d %d/%d/%d", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5], &indices[6], &indices[7], &indices[8]);
			m_Points[mindex] = temp_vertices[indices[0] - 1];
			m_Colors[mindex] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Points[mindex + 1] = temp_vertices[indices[3] - 1];
			m_Colors[mindex + 1] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Points[mindex + 2] = temp_vertices[indices[6] - 1];
			m_Colors[mindex + 2] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			mindex += 3;
		}
	}
	fclose(objfile);
}