#include "Cube.hpp"
#include "Camera.hpp"
#include "Light.hpp"

extern Camera myCamera;
extern Light myLight;
extern bool isShading;
extern bool isTexture;

Cube::Cube()
{
    vertex[0][0] = -1;vertex[0][1] = -1;vertex[0][2] = -1;
    vertex[1][0] = -1;vertex[1][1] = 1; vertex[1][2] = -1;
    vertex[2][0] = 1;vertex[2][1] = -1; vertex[2][2] = -1;
    vertex[3][0] = 1;vertex[3][1] = 1;vertex[3][2] = -1;
    vertex[4][0] = -1;vertex[4][1] = -1;vertex[4][2] = 1;
    vertex[5][0] = -1;vertex[5][1] = 1; vertex[5][2] = 1;
    vertex[6][0] = 1;vertex[6][1] = -1;vertex[6][2] = 1;
    vertex[7][0] = 1;vertex[7][1] = 1;vertex[7][2] = 1;

    face[0][0] = 0;face[0][1] = 1;face[0][2] = 3; face[0][3] = 2;
    face[1][0] = 3;face[1][1] = 7;face[1][2] = 6;face[1][3] = 2;
    face[2][0] = 7;face[2][1] = 5;face[2][2] = 4;face[2][3] = 6;
    face[3][0] = 4;face[3][1] = 5;face[3][2] = 1;face[3][3] = 0;
    face[4][0] = 5;face[4][1] = 7;face[4][2] = 3;face[4][3] = 1;
    face[5][0] = 6;face[5][1] = 4;face[5][2] = 0;face[5][3] = 2;

	faceColor[0][0] = 1.0, faceColor[0][1] = 0.0; faceColor[0][2] = 0.0;
	faceColor[1][0] = 0.0, faceColor[1][1] = 1.0; faceColor[1][2] = 0.0;
	faceColor[2][0] = 0.0, faceColor[2][1] = 0.0; faceColor[2][2] = 1.0;
	faceColor[3][0] = 1.0, faceColor[3][1] = 1.0; faceColor[3][2] = 0.0;
	faceColor[4][0] = 1.0, faceColor[4][1] = 0.0; faceColor[4][2] = 1.0;
	faceColor[5][0] = 0.0, faceColor[5][1] = 1.0; faceColor[5][2] = 1.0;

	cube_face_norm_mc[0][0] = 0.0,cube_face_norm_mc[0][1] = 0.0,cube_face_norm_mc[0][2] = -1.0,
	cube_face_norm_mc[1][0] = 1.0, cube_face_norm_mc[1][1] = 0.0, cube_face_norm_mc[1][2] = 0.0;
	cube_face_norm_mc[2][0] = 0.0, cube_face_norm_mc[2][1] = 0.0, cube_face_norm_mc[2][2] = 1.0;
	cube_face_norm_mc[3][0] = -1.0, cube_face_norm_mc[3][1] = 0.0, cube_face_norm_mc[3][2] = 0.0;
	cube_face_norm_mc[4][0] = 0.0, cube_face_norm_mc[4][1] = 1.0, cube_face_norm_mc[4][2] = 0.0;
	cube_face_norm_mc[5][0] = 0.0, cube_face_norm_mc[5][1] = -1.0, cube_face_norm_mc[5][2] = 0.0;

}

void Cube::draw_face(int i)
{
	if (isTexture) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, textureID[i]);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0,0.0);
			glVertex3fv(&vertex[face[i][0]][0]);
			glTexCoord2d(1.0,0.0);
			glVertex3fv(&vertex[face[i][1]][0]);
			glTexCoord2d(1.0,1.0);
			glVertex3fv(&vertex[face[i][2]][0]);
			glTexCoord2d(0.0,1.0);
			glVertex3fv(&vertex[face[i][3]][0]);\
		glEnd();
        // set mapping for

	} else {
		glBegin(GL_POLYGON);
			glVertex3fv(&vertex[face[i][0]][0]);
			glVertex3fv(&vertex[face[i][1]][0]);
			glVertex3fv(&vertex[face[i][2]][0]);
			glVertex3fv(&vertex[face[i][3]][0]);
		glEnd();
	}
}

void Cube::draw()
{
	glPushMatrix();
    this->ctm_multiply();
	glScalef(s, s, s);

    for (int i = 0; i < 6; i++) {
    	if (!isBackface(i)) {
    		if (!isTexture) {
				if (isShading ) {
					GLfloat shade = getFaceShade(i);
					glColor3f(faceColor[i][0]*shade, faceColor[i][1]*shade, faceColor[i][2]*shade);
				} else {
					glColor3f(faceColor[i][0], faceColor[i][1], faceColor[i][2]);
				}
    		}

    		draw_face(i);
    	}
    }
    glPopMatrix();
}

bool Cube::isBackface(int faceindex) {
	GLfloat v[4];
	v[0] = cube_face_norm_mc[faceindex][0]; //Gets the XYZ normal of the face
	v[1] = cube_face_norm_mc[faceindex][1];
	v[2] = cube_face_norm_mc[faceindex][2];
	v[3] = 0.0;
	this->MC.multiply_vector(v); //Multiplies the normals by the current matrix
	return (myCamera.ref.x - myCamera.eye.x) * v[0]
			+ (myCamera.ref.y - myCamera.eye.y) * v[1]
			+ (myCamera.ref.z - myCamera.eye.z) * v[2] > 0;
}

GLfloat Cube::getFaceShade(int faceindex) {
	GLfloat v[4], s[3], temp;

	s[0] = myLight.getMC().mat[0][3] - vertex[face[faceindex][0]][0]; //Gets the difference between the lights XYZ and the face
	s[1] = myLight.getMC().mat[1][3] - vertex[face[faceindex][0]][1];
	s[2] = myLight.getMC().mat[2][3] - vertex[face[faceindex][0]][2];

	temp = sqrt((s[0] * s[0]) + (s[1] * s[1]) + (s[2] * s[2])); //Calculates the distance
	s[0] = s[0] / temp;
	s[1] = s[1] / temp;
	s[2] = s[2] / temp;

	v[0] = cube_face_norm_mc[faceindex][0];
	v[1] = cube_face_norm_mc[faceindex][1];
	v[2] = cube_face_norm_mc[faceindex][2];
	v[3] = 1.0;
	this->MC.multiply_vector(v); //Used to account for where the face is on the cube

	temp = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	v[0] = v[0] / temp;
	v[1] = v[1] / temp;
	v[2] = v[2] / temp;

	temp = ((v[0] * s[0]) + (v[1] * s[1]) + (v[2] * s[2]));

	GLfloat shade = myLight.I * myLight.Rd * temp + myLight.Ia * myLight.Ra;

	//Shade must be in the range of 0-1
	if (shade < 0) {
		shade = 0;
	}
	if (shade > 1) {
		shade = 1;
	}
	return shade;
}


void Cube::setTextureID(int index, int textureid) {
	textureID[index] = textureid;
}

