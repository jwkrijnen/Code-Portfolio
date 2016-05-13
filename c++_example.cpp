//  ========================================================================
//  COSC363: Computer Graphics (2016);  University of Canterbury.
//
//  FILE NAME: Yard.cpp
//  See Lab05.pdf for details.
//  ========================================================================
 
#include <iostream>
#include <cmath> 
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "loadTGA.h"
using namespace std;

GLUquadric *q;    //Required for creating cylindrical objects
GLuint txId[7];   //Texture ids

//Colours
float white[4]  = {1.0, 1.0, 1.0, 1.0};
float sun_white[4]  = {2, 2, 1.75, 1.0};
float grey[4]  = {0.2, 0.2, 0.2, 1.0};
float blue[4]  = {0.8, 1.3, 2, 1.0};
float barely_blue[4]  = {0.006, 0.062, 0.08, 1.0};
float black[4] = {0.0, 0.0, 0.0, 1.0};

//Camera Movements
float  eye_x = -10,  eye_y = 2,  eye_z = 10;      //Initial camera position
float look_x = 0, look_y = 0, look_z = 0;   	 //"Look-at" point, values are set in display() function
float  theta = 4.712;                           //Look angle, 90 degrees = 1.571 radians, 270 degrees = 4.712 radians
int step = 0;								   //camera motion

//Animation Variables
float timer_delay = 41.667;

///For Liftoff
int liftoff = 0; //Should be 0 by default, 1 for testing.
int spaceship_hover = 0;
float spaceship_height = 0;
float spaceship_height_inc = 0.05;
float spaceship_hover_inc = 0.005;

///For Takeoff
int takeoff = 0;
float spaceship_z_move = 0;
float spaceship_x_move = 0;
float velocity = 4;
float spaceship_angle = 0;
float spaceship_angle_inc = 0.5;

///Spaceship Lasers
float laser_number = 5.0;

///Robot Button Light Flicker
float robot_button = 2.0;

///Alien Foot Tapping
float alien_foot_angle = 0;
float alien_foot_inc = 2;

///Hangar Door Opening
int hangardoor_toggle = 0; //Should be 0 by default, 1 for testing.
float hangardoor_move = 0;

///Robots Moving Boxes
int box_move = 0;
int direction = 1;
int okay_to_move_x = 0;
int okay_to_move_z = 1;
float robot_move_x = 0.0;
float robot_move_z = 0.0;
int corner = 0;
int near_box_pos = 0;
int arms_up = 0;
int arms_down = 0;
float arm_angle = 0;
int robot_turn_left = 0;
int robot_turn_right = 0;
float robot_turn_angle = 0;
float whole_turn_angle = 0;
int num_turns = 0;

///Box Movement
int box_direction = 1;
float box_angle = 0;
float box_move_x = 0.0;
float box_move_z = 0.0;

//Texture Names

char skybox[] = "textures/skybox.tga";
char hangarwall[] = "textures/hangarwall.tga";
char hangarfloor[] = "textures/hangarfloor.tga";
char hangarroof[] = "textures/hangarroof.tga";
char hangarsupport[] = "textures/hangarsupport.tga";
char hangardoor[] = "textures/hangardoor.tga";
char cargobox[] = "textures/cargobox.tga";

//--------------------------------------------------------------------------------
void loadTexture()				
{
	glGenTextures(7, txId); 	// Create 4 texture ids

	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture
    loadTGA(skybox);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
    loadTGA(hangarwall);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture
    loadTGA(hangarfloor);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[3]);  //Use this texture
    loadTGA(hangarroof);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	
	glBindTexture(GL_TEXTURE_2D, txId[4]);  //Use this texture
    loadTGA(hangarsupport);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	
	glBindTexture(GL_TEXTURE_2D, txId[5]);  //Use this texture
    loadTGA(hangardoor);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	
	glBindTexture(GL_TEXTURE_2D, txId[6]);  //Use this texture
    loadTGA(cargobox);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);			
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

//--------------------------------------------------------------------------------
void special(int key, int x, int y)
{
	step = 0;
	if(key == GLUT_KEY_LEFT) theta += 0.0873;   //in radians
	else if(key == GLUT_KEY_RIGHT) theta -= 0.0873;
	else if(key == GLUT_KEY_DOWN) step = -1;
	else if(key == GLUT_KEY_UP) step = 1;
	else if(key == GLUT_KEY_F1) hangardoor_toggle = !hangardoor_toggle;
	else if(key == GLUT_KEY_F2 && !takeoff) liftoff = !liftoff;
	else if(key == GLUT_KEY_F3){
		if (hangardoor_toggle && hangardoor_move > 3 && liftoff && spaceship_height > 3) takeoff = 1;
		else if (hangardoor_toggle) cout << "The spaceship must be hovering (F2)\n";
		else if (liftoff) cout << "The hangar door must be open (F1)\n";
		else cout << "The hangar door must be open (F1) and the spaceship must be hovering (F2)\n";
	}		

	glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void timer(int value)
{
	step = 0;
	
	//Hangar Doors
	if (hangardoor_toggle){
		hangardoor_move += 0.05;
		glEnable(GL_LIGHT0);
	}
	else {
		hangardoor_move -= 0.05;
	}
		
	if (hangardoor_move > 4.5) hangardoor_move = 4.5;
	
	else if (hangardoor_move < 0) {
		hangardoor_move = 0;
		glDisable(GL_LIGHT0);
	}
	
	//Alien Pilot tapping foot
	
	alien_foot_angle += alien_foot_inc;
	if ((alien_foot_angle >= 40) || (alien_foot_angle <= 0)) {
		alien_foot_inc = -alien_foot_inc;
	}
	
	//Spaceship laser colour changing
	
	laser_number -= 0.10;
	if (laser_number <= 0.00) laser_number = 5.00;

	//Robot Button colour changing
	
	robot_button -= 0.10;
	if (robot_button <= 0.00) robot_button = 2.00;
	
	//Spaceship Hovering
	
	if  (liftoff){
		if (spaceship_hover){
			spaceship_height += spaceship_hover_inc;
			if (spaceship_height < 4.95 || spaceship_height > 5.05) spaceship_hover_inc = -spaceship_hover_inc;
		}
		else {
			if (spaceship_height < 5) spaceship_height += spaceship_height_inc;
			else {
				spaceship_hover = !spaceship_hover;
			}
		}		
	}
	else {
		spaceship_hover = 0;
		if (spaceship_height > 0) spaceship_height -= spaceship_height_inc;
	}

	//Spaceship Takeoff
	
	if (takeoff){
		spaceship_z_move += velocity;
		if (spaceship_z_move > 60) {
			spaceship_angle += spaceship_angle_inc;
			spaceship_x_move -= velocity*0.0025*spaceship_angle ;
		}
		if (spaceship_angle > 50) spaceship_angle = 50;
	}

	//Robot Moving
	
	if (num_turns == 4) {
		whole_turn_angle = 0;
		num_turns = 0;
	}	
	if (okay_to_move_z) {
		robot_move_z += 0.05*direction;
		if (box_move) box_move_z += 0.05*direction*box_direction;
	}
	if (okay_to_move_z && (robot_move_z > 1.99 && robot_move_z < 2.01)) {
		okay_to_move_z = !okay_to_move_z;
		near_box_pos = !near_box_pos;
	}
	if (near_box_pos) {
		near_box_pos = !near_box_pos;
		if (box_move) robot_turn_left = !robot_turn_left;
		else arms_up = !arms_up;
	}
	if (arms_up) arm_angle += 2;
	if (arms_up && arm_angle >= 90) {
		arms_up = !arms_up;
		if (box_move) {
			box_move = !box_move;
			robot_turn_right = !robot_turn_right;
		}
		else robot_turn_left = !robot_turn_left;
	}
	if (robot_turn_left) {
		robot_turn_angle += 2;
		if (box_move) box_angle += 2;
	}
	if (robot_turn_left && robot_turn_angle >= 90) {
		robot_turn_left = !robot_turn_left;
		if (box_move) arms_up = !arms_up;
		else {
			arms_down = !arms_down;
			box_move = !box_move;
		}
	}
	if (arms_down) arm_angle -= 2;
	if (arms_down && arm_angle <= 0) {
		arms_down = !arms_down;
		if (box_move) robot_turn_right = !robot_turn_right;
		else okay_to_move_z = !okay_to_move_z;

	}
	if (robot_turn_right) {
		robot_turn_angle -= 2;
		if (box_move) box_angle -= 2;
	}
	if (robot_turn_right && robot_turn_angle <= 0) {
		robot_turn_right = !robot_turn_right;
		if (box_move) okay_to_move_z = !okay_to_move_z;
		else arms_down = !arms_down;
	}
	if (okay_to_move_z && ((robot_move_z > 3.99 && robot_move_z < 4.01) || (robot_move_z > -0.01 && robot_move_z < 0.01))) {
		okay_to_move_z = !okay_to_move_z;
		corner = !corner;
	}
	if (corner) {
		whole_turn_angle -= 2;
		if (box_move) box_angle -= 2;
	}
	if (corner && whole_turn_angle <= -90+(-90*num_turns)) {
		corner = !corner;
		num_turns += 1;
		if(num_turns == 3) box_direction = -box_direction;
		if (num_turns % 2 == 1) okay_to_move_x = !okay_to_move_x;	
		else {
			direction = -direction;
			okay_to_move_z = !okay_to_move_z;
		}
	}
	if (okay_to_move_x) {
		robot_move_x += 0.1*direction;
		if (box_move) box_move_x += 0.1*direction*box_direction;
	}
	if (okay_to_move_x && ((robot_move_x >17.99  && robot_move_x < 18.01)|| (robot_move_x > -0.01 && robot_move_x < 0.01))) {
		okay_to_move_x = !okay_to_move_x;
		corner = !corner;
	}
		
	glutTimerFunc(timer_delay, timer, value);
	glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void normal(float x1, float y1, float z1, 
            float x2, float y2, float z2,
		      float x3, float y3, float z3 )
{
	  float nx, ny, nz;
	  nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
	  ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
	  nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(nx, ny, nz);
}

void inv_normal(float x1, float y1, float z1, 
            float x2, float y2, float z2,
		      float x3, float y3, float z3 )
{
	  float nx, ny, nz;
	  nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
	  ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
	  nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(-nx, -ny, -nz);
}

//--------------------------------------------------------------------------------

void initialise()
{ 	
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
    
	loadTexture();
	
	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_white);
    glDisable(GL_LIGHT0);		
    
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, barely_blue);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, blue);
    glLightfv(GL_LIGHT1, GL_SPECULAR, blue);    
    
    glClearColor(1., 1., 0., 1.);  //Background colour  = yellow
	glEnable(GL_TEXTURE_2D);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();
	gluPerspective(67., 1.77778, 1., 6000.);
}

//--------------------------------------------------------------------------------

void space()
{
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	
	float max_x = 3000;
	float min_x = -3000;
	float max_y = 2000;
	float min_y = -2000;
	float max_z = 3000;
	float min_z = -3000;
	
	
	glBegin(GL_QUADS);
	
	////////////////////// FRONT WALL ///////////////////////		
		glTexCoord2f(0.33, 0.33); glVertex3f(min_x, min_y, max_z);
		glTexCoord2f(0.67, 0.33); glVertex3f(max_x, min_y, max_z);
		glTexCoord2f(0.67, 0.67); glVertex3f(max_x, max_y, max_z);
		glTexCoord2f(0.33, 0.67); glVertex3f(min_x, max_y, max_z);

	////////////////////// LEFT WALL ///////////////////////
		glTexCoord2f(0.0, 0.33); glVertex3f(min_x*2, min_y, min_z);
		glTexCoord2f(0.33, 0.33); glVertex3f(min_x, min_y, max_z);
		glTexCoord2f(0.33, 0.67); glVertex3f(min_x, max_y, max_z);
		glTexCoord2f(0.0, 0.67); glVertex3f(min_x*2, max_y, min_z);
	
	////////////////////// RIGHT WALL ///////////////////////		
		glTexCoord2f(1.0, 0.33); glVertex3f(max_x*2, min_y, min_z);
		glTexCoord2f(0.67, 0.33); glVertex3f(max_x, min_y, max_z);
		glTexCoord2f(0.67, 0.67); glVertex3f(max_x, max_y, max_z);
		glTexCoord2f(1.0, 0.67); glVertex3f(max_x*2, max_y, min_z);	
		
	////////////////////// BOTTOM WALL ///////////////////////	
		glTexCoord2f(0.33, 0.0); glVertex3f(min_x, min_y, min_z);
		glTexCoord2f(0.67, 0.0); glVertex3f(max_x, min_y, min_z);
		glTexCoord2f(0.67, 0.33); glVertex3f(max_x*2, min_y, max_z);
		glTexCoord2f(0.33, 0.33); glVertex3f(min_x*2, min_y, max_z);
		
	////////////////////// TOP WALL ///////////////////////	
		glTexCoord2f(0.2, 0.67); glVertex3f(min_x, max_y, min_z);
		glTexCoord2f(0.44, 0.67); glVertex3f(max_x, max_y, min_z);
		glTexCoord2f(0.44, 1.0); glVertex3f(max_x*2, max_y, max_z);
		glTexCoord2f(0.2, 1.0); glVertex3f(min_x*2, max_y, max_z);	
		
	glEnd();
	
	glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------------

void walls()
{
	glColor3f(0.5, 0.5, 0.5);

	//The walls are made up of several tiny squares on a 30x10 grid. Each square has a unit size.
	glBegin(GL_QUADS);
		
	////////////////////// BACK WALL ///////////////////////
	glNormal3f(0.0, 0.0, 1.0);
	for(float i = -15; i < 15; i++)
	{
		for(float j = 0;  j < 10; j++)
		{
			//Each texture coord is 
			glTexCoord2f((float)(i+15)/30, (float)(j)/10); glVertex3f(i, j, -15);
			glTexCoord2f((float)(i+15)/30, (float)(j+1)/10); glVertex3f(i, j+1, -15);
			glTexCoord2f((float)(i+16)/30, (float)(j+1)/10); glVertex3f(i+1, j+1, -15);
			glTexCoord2f((float)(i+16)/30, (float)(j)/10); glVertex3f(i+1, j, -15);
		}
	}

	////////////////////// LEFT WALL ///////////////////////
	glNormal3f(1.0, 0.0, 0.0);
	for(float i = -15; i < 15; i++)
	{
		for(float j = 0;  j < 10; j++)
		{
			//Each texture coord is 
			glTexCoord2f((float)(i+15)/30, (float)(j)/10); glVertex3f(-15, j, i);
			glTexCoord2f((float)(i+15)/30, (float)(j+1)/10); glVertex3f(-15, j+1, i);
			glTexCoord2f((float)(i+16)/30, (float)(j+1)/10); glVertex3f(-15, j+1, i+1);
			glTexCoord2f((float)(i+16)/30, (float)(j)/10); glVertex3f(-15, j, i+1);
		}
	}
	
	////////////////////// RIGHT WALL ///////////////////////
	
	glNormal3f(-1.0, 0.0, 0.0);
	for(float i = -15; i < 15; i++)
	{
		for(float j = 0;  j < 10; j++)
		{
			//Each texture coord is 
			glTexCoord2f((float)(i+15)/30, (float)(j)/10); glVertex3f(15, j, i);
			glTexCoord2f((float)(i+15)/30, (float)(j+1)/10); glVertex3f(15, j+1, i);
			glTexCoord2f((float)(i+16)/30, (float)(j+1)/10); glVertex3f(15, j+1, i+1);
			glTexCoord2f((float)(i+16)/30, (float)(j)/10); glVertex3f(15, j, i+1);
		}
	}
	
	glEnd();

}

//--------------------------------------------------------------------------------

void floor()
{
	glColor3f(0.9, 0.9, 0.9);
	glNormal3f(0.0, 1.0, 0.0);

	//The floor is made up of several tiny squares on a 30x30 grid. Each square has a unit size.
	glBegin(GL_QUADS);
	for(float i = -15; i < 15; i++)
	{
		for(float j = -15;  j < 15; j++)
		{
			//Each texture coord is 
			glTexCoord2f((float)(i+15)/30*5, (float)(j+15)/30*5); glVertex3f(i, 0.0, j);
			glTexCoord2f((float)(i+15)/30*5, (float)(j+16)/30*5); glVertex3f(i, 0.0, j+1);
			glTexCoord2f((float)(i+16)/30*5, (float)(j+16)/30*5); glVertex3f(i+1, 0.0, j+1);
			glTexCoord2f((float)(i+16)/30*5, (float)(j+15)/30*5); glVertex3f(i+1, 0.0, j);
		}
	}
	glEnd();

}

//--------------------------------------------------------------------------------

void roof()
{
	glColor3f(0.9, 0.9, 0.9);
	glNormal3f(0.0, -1.0, 0.0);

	//The roof is made up of several tiny squares on a 30x30 grid. Each square has a unit size.
	glBegin(GL_QUADS);
	for(float i = -15; i < 15; i++)
	{
		for(float j = -15;  j < 15; j++)
		{
			//Each texture coord is 
			glTexCoord2f((float)(i+15)/30, (float)(j+15)/30); glVertex3f(i, 10.0, j);
			glTexCoord2f((float)(i+15)/30, (float)(j+16)/30); glVertex3f(i, 10.0, j+1);
			glTexCoord2f((float)(i+16)/30, (float)(j+16)/30); glVertex3f(i+1, 10.0, j+1);
			glTexCoord2f((float)(i+16)/30, (float)(j+15)/30); glVertex3f(i+1, 10.0, j);
		}
	}
	glEnd();

}

//--------------------------------------------------------------------------------

void createSupport()
{
	glColor3f(0.8, 0.8, 0.8);

	glBegin(GL_QUADS);
	
	////////////////////// BOTTOM FRONT WALL ///////////////////////
	
		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.4, 0); glVertex3f(-1, 0, 0);
		glTexCoord2f(0.6, 0); glVertex3f(1, 0, 0);
		glTexCoord2f(0.6, 1); glVertex3f(1, 5, -2);
		glTexCoord2f(0.4, 1); glVertex3f(-1, 5, -2);

	////////////////////// BOTTOM LEFT WALL ///////////////////////
	
		glNormal3f(-1.0, 0.0, 0.0);
		glTexCoord2f(0, 0); glVertex3f(-1, 0, -4);
		glTexCoord2f(0.4, 0); glVertex3f(-1, 0, 0);
		glTexCoord2f(0.2, 1); glVertex3f(-1, 5, -2);
		glTexCoord2f(0, 1); glVertex3f(-1, 5, -4);

	////////////////////// BOTTOM RIGHT WALL ///////////////////////
	
		glNormal3f(1.0, -1.0, 0.0);
		glTexCoord2f(0.6, 0); glVertex3f(1, 0, 0);
		glTexCoord2f(1, 0); glVertex3f(1, 0, -4);
		glTexCoord2f(1, 1); glVertex3f(1, 5, -4);
		glTexCoord2f(0.8, 1); glVertex3f(1, 5, -2);
		
	////////////////////// TOP FRONT WALL ///////////////////////
	
		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.4, 0); glVertex3f(-1, 5,-2);
		glTexCoord2f(0.6, 0); glVertex3f(1, 5, -2);
		glTexCoord2f(0.6, 1); glVertex3f(1, 10, 0);
		glTexCoord2f(0.4, 1); glVertex3f(-1, 10, 0);

	////////////////////// TOP LEFT WALL ///////////////////////
	
		glNormal3f(-1.0, 0.0, 0.0);
		glTexCoord2f(0.6, 0); glVertex3f(-1, 5, -4);
		glTexCoord2f(1, 0); glVertex3f(-1, 5, -2);
		glTexCoord2f(1, 1); glVertex3f(-1, 10, 0);
		glTexCoord2f(0.8, 1); glVertex3f(-1, 10, -4);


	////////////////////// TOP RIGHT WALL ///////////////////////
		
		glNormal3f(1.0, 0.0, 0.0);
		glTexCoord2f(0, 0); glVertex3f(1, 5, -2);
		glTexCoord2f(0.4, 0); glVertex3f(1, 5, -4);
		glTexCoord2f(0.2, 1); glVertex3f(1, 10, -4);
		glTexCoord2f(0, 1); glVertex3f(1, 10, 0);
		
	glEnd();
}

//--------------------------------------------------------------------------------

void supports()
{
	//Back Left Support (Looking into the spaceport hangar)
	glPushMatrix();
		glTranslatef(-11,10,-5);
		glRotatef(180,1,0,0);
		glRotatef(90,0,1,0);
		createSupport();
	glPopMatrix();
	
	//Front Left Support
	glPushMatrix();
		glTranslatef(-11,10,5);
		glRotatef(180,1,0,0);
		glRotatef(90,0,1,0);
		createSupport();
	glPopMatrix();
	
	//Back Right Support
	glPushMatrix();
		glTranslatef(11,10,-5);
		glRotatef(180,1,0,0);
		glRotatef(-90,0,1,0);
		createSupport();
	glPopMatrix();
	
	//Front Right Support
	glPushMatrix();
		glTranslatef(11,10,5);
		glRotatef(180,1,0,0);
		glRotatef(-90,0,1,0);
		createSupport();
	glPopMatrix();
}

//--------------------------------------------------------------------------------

void door()
{
	
	glColor3f(0.5, 0.5, 0.5);

	////////////////////// FRONT FACE ///////////////////////

	//Bottom Door

	glNormal3f(0.0, 0.0, -1.0);
	
	glBegin(GL_TRIANGLES);
		
		glTexCoord2f(0.333, 0.4); glVertex3f(5, 4 - hangardoor_move, 13);
		glTexCoord2f(0.383, 0.4); glVertex3f(3.5 , 4 - hangardoor_move, 13);
		glTexCoord2f(0.333, 0.5); glVertex3f(5, 5 - hangardoor_move, 13);
		
		glTexCoord2f(0.617, 0.4); glVertex3f(-3.5 , 4 - hangardoor_move, 13);		
		glTexCoord2f(0.667, 0.4); glVertex3f(-5, 4 - hangardoor_move, 13);
		glTexCoord2f(0.667, 0.5); glVertex3f(-5, 5 - hangardoor_move, 13);
		
	glEnd();
	
	glBegin(GL_QUADS);
	
		glTexCoord2f(0.0, 0.0); glVertex3f(15,0 - hangardoor_move,13);
		glTexCoord2f(0.333, 0.0); glVertex3f(5, 0 - hangardoor_move, 13);
		glTexCoord2f(0.333, 0.5); glVertex3f(5, 5 - hangardoor_move, 13);
		glTexCoord2f(0.0, 0.5); glVertex3f(15, 5 - hangardoor_move, 13);
		
		glTexCoord2f(0.333, 0.0); glVertex3f(5,0 - hangardoor_move,13);
		glTexCoord2f(0.667, 0.0); glVertex3f(-5, 0 - hangardoor_move, 13);
		glTexCoord2f(0.667, 0.4); glVertex3f(-5 , 4 - hangardoor_move, 13);
		glTexCoord2f(0.33, 0.4); glVertex3f(5, 4 - hangardoor_move, 13);
		
		glTexCoord2f(0.667, 0.0); glVertex3f(-5,0 - hangardoor_move,13);
		glTexCoord2f(1.0, 0.0); glVertex3f(-15, 0 - hangardoor_move, 13);
		glTexCoord2f(1.0, 0.5); glVertex3f(-15 , 5 - hangardoor_move, 13);
		glTexCoord2f(0.667, 0.5); glVertex3f(-5, 5 - hangardoor_move, 13);
		
	glEnd();
	
	//Top Door	
	
	glBegin(GL_TRIANGLES);
		
		glTexCoord2f(0.383, 0.4); glVertex3f(3.5, 4 + hangardoor_move, 13);
		glTexCoord2f(0.383, 0.5); glVertex3f(3.5 , 5 + hangardoor_move, 13);
		glTexCoord2f(0.333, 0.5); glVertex3f(5, 5 + hangardoor_move, 13);
		
		glTexCoord2f(0.617, 0.4); glVertex3f(-3.5 , 4 + hangardoor_move, 13);		
		glTexCoord2f(0.667, 0.5); glVertex3f(-5, 5 + hangardoor_move, 13);
		glTexCoord2f(0.617, 0.5); glVertex3f(-3.5, 5 + hangardoor_move, 13);
		
	glEnd();	
	
	glBegin(GL_QUADS);
	
		glTexCoord2f(0.0, 0.5); glVertex3f(15,5 + hangardoor_move,13);
		glTexCoord2f(1.0, 0.5); glVertex3f(-15, 5 + hangardoor_move, 13);
		glTexCoord2f(1.0, 1.0); glVertex3f(-15, 10 + hangardoor_move, 13);
		glTexCoord2f(0.0, 1.0); glVertex3f(15, 10 + hangardoor_move, 13);
		
		glTexCoord2f(0.383, 0.4); glVertex3f(3.5,4 + hangardoor_move,13);
		glTexCoord2f(0.617, 0.4); glVertex3f(-3.5, 4 + hangardoor_move, 13);
		glTexCoord2f(0.617, 0.5); glVertex3f(-3.5 , 5 + hangardoor_move, 13);
		glTexCoord2f(0.383, 0.5); glVertex3f(3.5, 5 + hangardoor_move, 13);
		
		
	glEnd();	
	
	////////////////////// TOP DOOR BOTTOM FACE ///////////////////////		
	
	glBegin(GL_QUADS);
		
		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(0, 0.55); glVertex3f(15,5 - hangardoor_move,13);
		glTexCoord2f(0.333, 0.55); glVertex3f(5, 5 - hangardoor_move, 13);
		glTexCoord2f(0.333, 0.75); glVertex3f(5, 5 - hangardoor_move, 15);
		glTexCoord2f(0, 0.75); glVertex3f(15, 5 - hangardoor_move, 15);
		
		glNormal3f(1.0, -1.0, 0.0);
		glTexCoord2f(0.333, 0.55); glVertex3f(5,5 - hangardoor_move,13);
		glTexCoord2f(0.383, 0.55); glVertex3f(3.5, 4 - hangardoor_move, 13);
		glTexCoord2f(0.383, 0.75); glVertex3f(3.5 , 4 - hangardoor_move, 15);
		glTexCoord2f(0.333, 0.75); glVertex3f(5, 5 - hangardoor_move, 15);
		
		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(0.383, 0.55); glVertex3f(3.5,4 - hangardoor_move,13);
		glTexCoord2f(0.617, 0.55); glVertex3f(-3.5, 4 - hangardoor_move, 13);
		glTexCoord2f(0.617, 0.75); glVertex3f(-3.5 , 4 - hangardoor_move, 15);
		glTexCoord2f(0.383, 0.75); glVertex3f(3.5, 4 - hangardoor_move, 15);
		
		glNormal3f(-1.0, -1.0, 0.0);
		glTexCoord2f(0.617, 0.55); glVertex3f(-3.5,4 - hangardoor_move,13);
		glTexCoord2f(0.667, 0.55); glVertex3f(-5, 5 - hangardoor_move, 13);
		glTexCoord2f(0.667, 0.75); glVertex3f(-5 , 5 - hangardoor_move, 15);
		glTexCoord2f(0.617, 0.75); glVertex3f(-3.5, 4 - hangardoor_move, 15);
		
		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(0.667, 0.55); glVertex3f(-5,5 - hangardoor_move,13);
		glTexCoord2f(1.0, 0.55); glVertex3f(-15, 5 - hangardoor_move, 13);
		glTexCoord2f(1.0, 0.75); glVertex3f(-15 , 5 - hangardoor_move, 15);
		glTexCoord2f(0.667, 0.75); glVertex3f(-5, 5 - hangardoor_move, 15);
		
	glEnd();

	////////////////////// BOTTOM DOOR TOP FACE ///////////////////////	

	glBegin(GL_QUADS);
	
		glNormal3f(0.0, 1.0, 0.0);
		glTexCoord2f(0, 0.15); glVertex3f(15,5 + hangardoor_move,13);
		glTexCoord2f(0.333, 0.15); glVertex3f(5, 5 + hangardoor_move, 13);
		glTexCoord2f(0.333, 0.35); glVertex3f(5, 5 + hangardoor_move, 15);
		glTexCoord2f(0, 0.35); glVertex3f(15, 5 + hangardoor_move, 15);
		
		glNormal3f(-1.0, 1.0, 0.0);
		glTexCoord2f(0.333, 0.15); glVertex3f(5,5 + hangardoor_move,13);
		glTexCoord2f(0.383, 0.15); glVertex3f(3.5, 4 + hangardoor_move, 13);
		glTexCoord2f(0.383, 0.35); glVertex3f(3.5 , 4 + hangardoor_move, 15);
		glTexCoord2f(0.333, 0.35); glVertex3f(5, 5 + hangardoor_move, 15);
		
		glNormal3f(0.0, 1.0, 0.0);
		glTexCoord2f(0.383, 0.15); glVertex3f(3.5,4 + hangardoor_move,13);
		glTexCoord2f(0.617, 0.15); glVertex3f(-3.5, 4 + hangardoor_move, 13);
		glTexCoord2f(0.617, 0.35); glVertex3f(-3.5 , 4 + hangardoor_move, 15);
		glTexCoord2f(0.383, 0.35); glVertex3f(3.5, 4 + hangardoor_move, 15);
		
		glNormal3f(1.0, 1.0, 0.0);
		glTexCoord2f(0.617, 0.15); glVertex3f(-3.5,4 + hangardoor_move,13);
		glTexCoord2f(0.667, 0.15); glVertex3f(-5, 5 + hangardoor_move, 13);
		glTexCoord2f(0.617, 0.35); glVertex3f(-5 , 5 + hangardoor_move, 15);
		glTexCoord2f(0.667, 0.35); glVertex3f(-3.5, 4 + hangardoor_move, 15);
		
		glNormal3f(0.0, 1.0, 0.0);
		glTexCoord2f(0.667, 0.15); glVertex3f(-5,5 + hangardoor_move,13);
		glTexCoord2f(1.0, 0.15); glVertex3f(-15, 5 + hangardoor_move, 13);
		glTexCoord2f(1.0, 0.35); glVertex3f(-15 , 5 + hangardoor_move, 15);
		glTexCoord2f(0.667, 0.35); glVertex3f(-5, 5 + hangardoor_move, 15);
		
	glEnd();
		
}

//--------------------------------------------------------------------------------

void spaceship()
{
	glDisable(GL_TEXTURE_2D);
	
    //Transformations to entire Ship
    glPushMatrix(); 
		glTranslatef(0 + spaceship_x_move, 0 + spaceship_height,0 + spaceship_z_move);
		glScalef(1,1.2,1);
		glRotatef(spaceship_angle,0,0,1);
    	glRotatef(1.5,1,0,0);

		//Front
		glColor3f(0.8, 0.518, 0.0);
		glPushMatrix();
			glScalef(2,0.5,1);
			glTranslatef(0,1.4,4);
			glutSolidCone(1.0, 2.0, 50.0, 50.0);
		glPopMatrix();
 
 		//Middle beam
 		glColor3f(0.8, 0.518, 0);
		glPushMatrix();
			glRotatef(1.5,1,0,0);
			glTranslatef(0,1.07,0);
			glScalef(1,0.5,8);
			glutSolidCube(1);
		glPopMatrix();
    
		//Body		
		glColor3f(0.8, 0.8, 0.8);
		glPushMatrix();
			glScalef(2,0.5,1);
			glTranslatef(0,1.4,-4);
			gluCylinder(q, 1.4, 1.0, 8.0, 50.0, 25.0);
			glRotatef(180,1,0,0);
			gluDisk(q, 0, 1.4, 50.0, 5.0);
		glPopMatrix();
		
		//Cockpit
		glColor3f(0.1, 0.9, 0.9);
		glPushMatrix();
			glRotatef(1.5,1,0,0);
			glTranslatef(0,1.3,-2);			
			glScalef(1.3,0.6,2);
			glutSolidSphere(1,50,50);
		glPopMatrix();
		
    	//Left Back Wing
		glColor3f(0.8, 0.518, 0.0);
		glPushMatrix();
			glTranslatef(3.2,0.75,-2);
			glScalef(5,0.45,2.6);
			glRotatef(-30,0,1,0);
			glutSolidTetrahedron();
		glPopMatrix();
		
		 //Right Back Wing
		glColor3f(0.8, 0.518, 0.0);
		glPushMatrix();
			glTranslatef(-3.2,0.75,-2);
			glScalef(5,0.45,2.6);
			glRotatef(210,0,1,0);
			glutSolidTetrahedron();
		glPopMatrix();
		
		//Left Front Wing
		glColor3f(0.8, 0.518, 0.0);
		glPushMatrix();
			glTranslatef(2.25,0.75,2);
			glRotatef(-6.5,0,1,0);
			glRotatef(-90,0,0,1);
			glRotatef(-90,0,1,0);
			glScalef(4.5,0.225,0.5);
			glutSolidTetrahedron();
		glPopMatrix();

		//Left Laser
		glMaterialfv(GL_FRONT, GL_SPECULAR, black);
		glColor3f(0.1, 0.06, 0.1);
		glPushMatrix();
			glTranslatef(1.8,0.75,5.75);
			glRotatef(-6.5,0,1,0);
			glScalef(0.1,0.1,0.05);
			if (laser_number < 1) glColor3f(0.8, 0.4, 0.8);
			glutSolidTorus(0.75,1,50,50);
			glTranslatef(0,0,3);
			if (laser_number < 2) glColor3f(0.8, 0.4, 0.8);
			glutSolidTorus(0.75,1,50,50);
			glTranslatef(0,0,3);
			if (laser_number < 3) glColor3f(0.8, 0.4, 0.8);
			glutSolidTorus(0.75,1,50,50);
			glTranslatef(0,0,6);
			glScalef(1,1,2);
			if (laser_number < 4) glColor3f(0.8, 0.4, 0.8);
			glutSolidSphere(1.25,50,50);
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_SPECULAR, sun_white);
		
		//Right Front Wing
		glColor3f(0.8, 0.518, 0.0);
		glPushMatrix();
			glTranslatef(-2.25,0.75,2);
			glRotatef(6.5,0,1,0);
			glRotatef(90,0,0,1);
			glRotatef(-90,0,1,0);
			glScalef(4.5,0.225,0.5);
			glutSolidTetrahedron();
		glPopMatrix();		

		//Right Laser
		glMaterialfv(GL_FRONT, GL_SPECULAR, black);
		glColor3f(0.1, 0.06, 0.1);
		glPushMatrix();
			glTranslatef(-1.8,0.75,5.75);
			glRotatef(6.5,0,1,0);
			glScalef(0.1,0.1,0.05);
			if (laser_number < 1) glColor3f(0.8, 0.4, 0.8);
			glutSolidTorus(0.75,1,50,50);
			glTranslatef(0,0,3);
			if (laser_number < 2) glColor3f(0.8, 0.4, 0.8);
			glutSolidTorus(0.75,1,50,50);
			glTranslatef(0,0,3);
			if (laser_number < 3) glColor3f(0.8, 0.4, 0.8);
			glutSolidTorus(0.75,1,50,50);
			glTranslatef(0,0,6);
			glScalef(1,1,2);
			if (laser_number < 4) glColor3f(0.8, 0.4, 0.8);
			glutSolidSphere(1.25,50,50);
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_SPECULAR, sun_white);
		
		//Left Exhaust
		glColor3f(0.2,0.2,0.2);	 	
		glPushMatrix();
			glTranslatef(0.75,0.5,-4.25);		
			glScalef(0.5,0.25,0.25);
			gluCylinder(q, 1.4, 1.0, 1.0, 50.0, 20.0);
			glRotatef(180,1,0,0);
			if (takeoff) glColor3f(0.8,0.2,0.2);	
			gluDisk(q, 0, 1.4, 50.0, 5.0);
		glPopMatrix();
		
		//Right Exhaust
		glColor3f(0.2,0.2,0.2);		 	
		glPushMatrix();
			glTranslatef(-0.75,0.5,-4.25);		
			glScalef(0.5,0.25,0.25);
			gluCylinder(q, 1.4, 1.0, 1.0, 50.0, 20.0);
			glRotatef(180,1,0,0);
			if (takeoff) glColor3f(0.8,0.2,0.2);
			gluDisk(q, 0, 1.4, 50.0, 5.0);
		glPopMatrix();		
		
	glPopMatrix();
	
	glEnable(GL_TEXTURE_2D);	
}

//--------------------------------------------------------------------------------

void createCoat()
{
	
	//Coords for Sweep Surface (Pilot Aliens' Coat)
	const int N = 32;  // Total number of vertices on the base curve

	float vx[N] = {7.438, 7.184, 6.943, 6.717, 6.505, 6.307, 6.124,
				   5.954, 5.799, 5.658, 5.531, 5.419, 5.320, 5.236, 
				   5.166, 5.110, 5.069, 5.041, 5.007, 4.930, 4.804, 
				   4.631,  4.409, 4.139, 3.822, 3.456, 3.043, 2.581, 
				   2.5, 2.8, 3.6, 4};
				   
	float vy[N] =  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
					16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 
					29, 29, 28, 27};
			   
	float vz[N] = {0};
	
	float wx[N], wy[N], wz[N]; 
	
	float coat_theta =  0.021816625; /// Amount each point of vase is rotate around the origin
	
	const int H = 248; ///Number of types display is looped (to create open coat shape)
	
	for(int j = 0; j < H; j++) {
		for(int i = 0; i < N; i++) {
			wx[i] = vx[i] * cos(coat_theta) + vz[i] * sin(coat_theta);
			wy[i] = vy[i];
			wz[i] = -vx[i] * sin(coat_theta) + vz[i] * cos(coat_theta);
		}
	
		glBegin(GL_TRIANGLE_STRIP);
			for(int i = 0; i < N; i++) {
				
				if (i > 27) normal( wx[i-1], wy[i-1], wz[i-1],
							       vx[i-1], vy[i-1], vz[i-1],
							       vx[i], vy[i], vz[i] );
				
				else if (i > 0) inv_normal( wx[i-1], wy[i-1], wz[i-1],
							       vx[i-1], vy[i-1], vz[i-1],
							       vx[i], vy[i], vz[i] );
							   
				glVertex3f(vx[i], vy[i], vz[i]);
			
				if (i > 27) normal( wx[i-1], wy[i-1], wz[i-1],
								   vx[i], vy[i], vz[i],
								   wx[i], wy[i], wz[i] );
				
				else if (i > 0) inv_normal( wx[i-1], wy[i-1], wz[i-1],
								   vx[i], vy[i], vz[i],
								   wx[i], wy[i], wz[i] );
							   
				glVertex3f(wx[i], wy[i], wz[i]);
			}
					
		glEnd();

		for(int i = 0; i < N; i++) {
			vx[i] = wx[i];
			vy[i] = wy[i];
			vz[i] = wz[i];
		}
	}
}

//--------------------------------------------------------------------------------

void alien()
{
	glDisable(GL_TEXTURE_2D);
	
    //Transformations to the whole alien
    glPushMatrix();
		glTranslatef(14.625,0.66,10);
		glScalef(0.25,0.25,0.25);
		glRotatef(180,0,1,0); 

		//Coat
		glColor3f(0.225, 0.102, 0.063);
		glPushMatrix();
			glScalef(0.25,0.25,0.25);
			glRotatef(5,0,0,1);
			glRotatef(25.5,0,1,0); 		
			createCoat();
		glPopMatrix();

		//Back of Coat (Because actual back of coat clips through wall)
		glColor3f(0.225, 0.102, 0.063);
		glPushMatrix();
			glTranslatef(0,1.7,0);
			glScalef(0.01,3,1.5);
			glutSolidCube(1);
		glPopMatrix();
		
		//Head
		glColor3f(0.1, 0.301, 0.032);
		glPushMatrix();
			glScalef(0.8,0.9,0.6);
			glTranslatef(-0.3,8.8,0);
			glRotatef(-33,0,0,1); 		
			glutSolidSphere(1,50,50);
		glPopMatrix();

		//Right Eye
		glMaterialfv(GL_FRONT, GL_SPECULAR, sun_white);
		glColor3f(0,0,0);
		glPushMatrix();
			glTranslatef(0.44,7.9,0.24);
			glRotatef(-25,0,1,0); 
			glRotatef(-75,1,0,0); 
			glScalef(0.15,0.4,0.7);
			glutSolidSphere(0.5,50,50);
		glPopMatrix();
		
		//Left Eye
		glColor3f(0, 0, 0);
		glPushMatrix();
			glTranslatef(0.44,7.9,-0.24);
			glRotatef(25,0,1,0); 
			glRotatef(75,1,0,0); 
			glScalef(0.15,0.4,0.7);
			glutSolidSphere(0.5,50,50);
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_SPECULAR, black);

		//Neck
		glColor3f(0.1, 0.301, 0.032);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(-0.4,6.7,0);
			glRotatef(-90,1,0,0); 		
			gluCylinder(q,0.4,0.25,0.5,50,50);
		glPopMatrix();
		
		//Torso
		glColor3f(0.25, 0.2, 0.23);
		glPushMatrix();
			glScalef(0.5,1,1);
			glTranslatef(-0.1,5.7,0);
			glRotatef(-90,1,0,0); 		
			gluCylinder(q,0.9,0.3,1,50,50);
		glPopMatrix();
		
		//Chest
		glColor3f(0.25, 0.2, 0.23);
		glPushMatrix();
			glScalef(0.5,1,1);
			glTranslatef(-0.1,3.7,0);
			glRotatef(-90,1,0,0); 		
			gluCylinder(q,0.8,0.9,2,50,50);
		glPopMatrix();
		
		//Right Shoulder
		glColor3f(0.225, 0.102, 0.063);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(-0.3,6.2,0.8); 		
			glutSolidSphere(0.4,50,50);
		glPopMatrix();
		
		//Right Upper Arm
		glColor3f(0.225, 0.102, 0.063);
		glPushMatrix();
			glScalef(0.7, 0.9,1);
			glTranslatef(-0.3,6.9,0.9);
			glRotatef(70,1,0,0); 
			glRotatef(70,0,1,0); 		
			gluCylinder(q,0.4,0.36,2.3,50,50);
		glPopMatrix();
		
		//Right Elbow
		glColor3f(0.1, 0.301, 0.032);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(1.35, 5.5,1.175); 		
			glutSolidSphere(0.36,50,50);
		glPopMatrix();
		
		//Right Lower Arm
		glColor3f(0.1, 0.301, 0.032);
		glPushMatrix();
			glScalef(0.56, 0.81,0.9);		
			glTranslatef(2.6,6.8,1.25);
			glRotatef(200,0,1,0);
			glRotatef(-10,1,0,0);		
			gluCylinder(q,0.4,0.36,1.8,50,50);
		glPopMatrix();	
		
		//Right Hand
		glColor3f(0.185, 0.16, 0.13);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(1.05, 5.8,-0.475); 		
			glutSolidSphere(0.3,50,50);
		glPopMatrix();			
		
		// Left Shoulder
		glColor3f(0.225, 0.102, 0.063);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(-0.3,6.2,-0.8); 		
			glutSolidSphere(0.4,50,50);
		glPopMatrix();				

		//Left Upper Arm
		glColor3f(0.225, 0.102, 0.063);
		glPushMatrix();
			glScalef(0.7, 0.9,1);
			glTranslatef(-0.3,6.9,-0.9);
			glRotatef(-70,1,0,0); 
			glRotatef(110,0,1,0); 		
			gluCylinder(q,0.4,0.36,2.3,50,50);
		glPopMatrix();

		//Left Elbow
		glColor3f(0.1, 0.301, 0.032);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(1.35, 5.5,-1.175); 		
			glutSolidSphere(0.36,50,50);
		glPopMatrix();
		
		//Left Lower Arm
		glColor3f(0.1, 0.301, 0.032);
		glPushMatrix();
			glScalef(0.56, 0.81,0.9);		
			glTranslatef(2.6,6.8,-1.25);
			glRotatef(-20,0,1,0);	
			glRotatef(10,1,0,0);	
			gluCylinder(q,0.4,0.36,1.8,50,50);
		glPopMatrix();				

		//Left Hand
		glColor3f(0.185, 0.16, 0.13);
		glPushMatrix();
			glScalef(1,1,1);
			glTranslatef(1.05, 5.25,0.525); 		
			glutSolidSphere(0.3,50,50);
		glPopMatrix();

		//Belt
		glColor3f(0.163, 0.071, 0.039);
		glPushMatrix();
			glScalef(0.5,1,1);
			glTranslatef(-0.1,3.3,0);
			glRotatef(-90,1,0,0); 		
			gluCylinder(q,0.8,0.8,0.5,50,50);
		glPopMatrix();

		//Lower Body
		glColor3f(0.06, 0.02, 0.01);
		glPushMatrix();
			glScalef(0.5,1,1);
			glTranslatef(-0.1,2.3,0);
			glRotatef(-90,1,0,0); 		
			gluCylinder(q,0.9,0.8,1,50,50);
		glPopMatrix();

		//Right Upper Leg
		glColor3f(0.06, 0.02, 0.01);
		glPushMatrix();
			glScalef(0.9, 0.81,0.72);		
			glTranslatef(-0.5,2.9,0.75);
			glRotatef(-66,0,0,1);
			glRotatef(90,0,1,0);
			gluCylinder(q,0.7,0.5,3.2,50,50);
		glPopMatrix();
		
		//Right Knee
		glColor3f(0.06, 0.02, 0.01);
		glPushMatrix();
			glTranslatef(0.7,0,0.55); 		
			glutSolidSphere(0.4,50,50);
		glPopMatrix();
		
		//Right Lower Leg
		glColor3f(0.26, 0.12, 0.11);
		glPushMatrix();
			glScalef(0.9, 0.81,0.72);		
			glTranslatef(0.8,-0.15,0.75);
			glRotatef(-10,0,0,1);
			glRotatef(127.5,1,0,0);
			gluCylinder(q,0.5,0.4,2.8,50,50);
		glPopMatrix();
		
		//Right Heel
		glColor3f(0.26, 0.12, 0.11);
		glPushMatrix();
			glTranslatef(0.35,-2,-0.75); 		
			glutSolidSphere(0.275,50,50);
		glPopMatrix();
		
		//Right Foot
		glColor3f(0.26, 0.12, 0.11);
		glPushMatrix();
			glTranslatef(0.4,-2.05,-0.8);
			glRotatef(45,1,0,0); 
			glScalef(1,0.6,1);
			glRotatef(90,0,1,0);		
			gluCylinder(q,0.3,0.3,1.4,50,50);
			glRotatef(180,1,0,0);
			glTranslatef(0,0,-1.4);
			gluDisk(q, 0, 0.3, 50.0, 5.0);
		glPopMatrix();				
		
		//Left Upper Leg
		glColor3f(0.06, 0.02, 0.01);
		glPushMatrix();
			glScalef(0.9, 0.81,0.72);		
			glTranslatef(-0.5,2.9,-0.75);
			glRotatef(-66,0,0,1);
			glRotatef(90,0,1,0);
			gluCylinder(q,0.7,0.5,3.2,50,50);
		glPopMatrix();
		
		//Left Knee
		glColor3f(0.06, 0.02, 0.01);
		glPushMatrix();
			glTranslatef(0.7,0,-0.5); 		
			glutSolidSphere(0.38,50,50);
		glPopMatrix();
		
		//Left Lower Leg
		glColor3f(0.26, 0.12, 0.11);
		glPushMatrix();
			glScalef(0.9, 0.81,0.55);		
			glTranslatef(0.8,-0.15,-0.75);
			glRotatef(10,0,0,1);
			glRotatef(82.5,1,0,0);
			gluCylinder(q,0.5,0.4,2.8,50,50);
		glPopMatrix();	
		
		//Left Heel
		glColor3f(0.26, 0.12, 0.11);
		glPushMatrix();
			glTranslatef(1.15,-2.5,-0.2); 		
			glutSolidSphere(0.275,50,50);
		glPopMatrix();												

		//Left Foot
		glColor3f(0.26, 0.12, 0.11);
		glPushMatrix();
			glTranslatef(1.1,-2.5,-0.2);
			glScalef(1,0.6,1);
			glRotatef(alien_foot_angle,0,0,1);
			glRotatef(90,0,1,0);		
			gluCylinder(q,0.3,0.3,1.4,50,50);
			glRotatef(180,1,0,0);
			glTranslatef(0,0,-1.4);
			gluDisk(q, 0, 0.3, 50.0, 5.0);
		glPopMatrix();	

	glPopMatrix();
	
	glEnable(GL_TEXTURE_2D);	
	
}

//--------------------------------------------------------------------------------

void createRobot()
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	//Transformation to robot body (not chassis or treads)
		glRotatef(robot_turn_angle,0,1,0);
		
		//Head
		glColor3f(0.6, 0.6, 0.6);
		glPushMatrix();
			glTranslatef(0.1,0.5,0);
			glScalef(1,0.67,1);
			glutSolidCube(0.5);
		glPopMatrix();
		
		//Right Eye
		glColor3f(0, 0, 0);
		glPushMatrix();
			glTranslatef(0.35,0.5,0.12);
			glScalef(0.025,1,1);
			glutSolidSphere(0.05,20,20);
		glPopMatrix();
		
		//Left Eye
		glColor3f(0, 0, 0);
		glPushMatrix();
			glTranslatef(0.35,0.5,-0.12);
			glScalef(0.025,1,1);
			glutSolidSphere(0.05,20,20);
		glPopMatrix();		
		
		//Body
		glColor3f(0.6, 0.6, 0.6);
		glPushMatrix();
			glScalef(1,0.67,1);
			glutSolidCube(1);
		glPopMatrix();
		
		//Buttons
		glColor3f(0, 1, 0);
		glPushMatrix();
			glTranslatef(-0.5,0.05,0.2);
			glutSolidSphere(0.03125,20,20);
			glTranslatef(0,0,0.1);
			glColor3f(0.1, 0, 0);
			if (robot_button > 1) glColor3f(1, 0, 0);
			glutSolidSphere(0.03125,20,20);
		glPopMatrix();
		
		//Disk Drive
		glColor3f(0, 0, 0);
		glPushMatrix();
			glTranslatef(-0.5,-0.1,0);
			glScalef(1,0.25,20);
			glutSolidCube(0.03125);
		glPopMatrix();		
		
		//Right Arm
		glColor3f(0.4, 0.4, 0.4);
		glPushMatrix();
			glRotatef(arm_angle, 0,0,1);
			glTranslatef(0.5,0,0.575);
			glScalef(1.5,0.25,0.0625);
			glutSolidCube(1);
			glTranslatef(0.4,0,-1);
			glScalef(0.75,4,16);
			gluCylinder(q,0.25,0.05,0.05,50,50);
			
		glPopMatrix();

		//Right Arm Joint
		glColor3f(0.4, 0.4, 0.4);
		glPushMatrix();
			glTranslatef(0,0,0.5);
			gluCylinder(q,0.1,0.1,0.05,50,50);
		glPopMatrix();
		
		//Left Arm
		glColor3f(0.4, 0.4, 0.4);
		glPushMatrix();
			glRotatef(arm_angle, 0,0,1);
			glTranslatef(0.5,0,-0.575);
			glScalef(1.5,0.25,0.0625);
			glRotatef(180,1,0,0);
			glutSolidCube(1);
			glTranslatef(0.4,0,-1);
			glScalef(0.75,4,16);
			gluCylinder(q,0.25,0.05,0.05,50,50);
		glPopMatrix();
		
		//Left Arm Joint
		glColor3f(0.4, 0.4, 0.4);
		glPushMatrix();
			glTranslatef(0,0,-0.5);
			glRotatef(180,1,0,0);
			gluCylinder(q,0.1,0.1,0.05,50,50);
		glPopMatrix();

		
	glPopMatrix();
		//Chassis
		glColor3f(0.2, 0.2, 0.2);
		glPushMatrix();
		glTranslatef(0,-0.45,0);
			glScalef(0.8,0.2,0.99);
			glutSolidCube(1);
		glPopMatrix();		
		
		//Right Tread
		glColor3f(0.2, 0.2, 0.2);
		glPushMatrix();
			glTranslatef(0,-0.5,0.5);
			glScalef(0.6,0.1,1);
			gluCylinder(q,1,1,0.25,50,10);
			glColor3f(0.2, 0.2, 0.2);
			gluDisk(q,0,1,50,10);
		glPopMatrix();
		
		//Left Tread
		glColor3f(0.2, 0.2, 0.2);
		glPushMatrix();
			glTranslatef(0,-0.5,-0.5);
			glScalef(0.6,0.1,1);
			glRotatef(180,1,0,0);
			gluCylinder(q,1,1,0.25,50,10);
			glColor3f(0.2, 0.2, 0.2);
			gluDisk(q,0,1,50,10);
		glPopMatrix();	
	
	glEnable(GL_TEXTURE_2D);
}

//--------------------------------------------------------------------------------

void robots()
{
	//Robot One
	glPushMatrix();
		glTranslatef(-9 + robot_move_x,0.6,-8 - robot_move_z);
		glRotatef(90+whole_turn_angle,0,1,0);
		createRobot();
	glPopMatrix();
	
	//Robot Two
	glPushMatrix();
		glTranslatef(9 - robot_move_x,0.6,-12 + robot_move_z);
		glRotatef(-90+whole_turn_angle,0,1,0);
		createRobot();
	glPopMatrix();
	
}
//--------------------------------------------------------------------------------

void createBox(){
	
	//The Boxes are 16x16, so scale them down
	
	glBegin(GL_QUADS);
	for(float i = -8; i < 8; i++)
	{
		for(float j = -8;  j < 8; j++)
		{
			
			//Top
			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f((float)(i+8)/16, (float)(j+8)/16); glVertex3f(i, 8, j);
			glTexCoord2f((float)(i+8)/16, (float)(j+9)/16); glVertex3f(i, 8, j+1);
			glTexCoord2f((float)(i+9)/16, (float)(j+9)/16); glVertex3f(i+1, 8, j+1);
			glTexCoord2f((float)(i+9)/16, (float)(j+8)/16); glVertex3f(i+1, 8, j);
			
			//Bottom
			glNormal3f(0.0, -1.0, 0.0);
			glTexCoord2f((float)(i+8)/16, (float)(j+8)/16); glVertex3f(i, -8, j);
			glTexCoord2f((float)(i+8)/16, (float)(j+9)/16); glVertex3f(i, -8, j+1);
			glTexCoord2f((float)(i+9)/16, (float)(j+9)/16); glVertex3f(i+1, -8, j+1);
			glTexCoord2f((float)(i+9)/16, (float)(j+8)/16); glVertex3f(i+1, -8, j);

			//Front
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f((float)(i+8)/16, (float)(j+8)/16); glVertex3f(i, j, 8);
			glTexCoord2f((float)(i+8)/16, (float)(j+9)/16); glVertex3f(i, j+1, 8);
			glTexCoord2f((float)(i+9)/16, (float)(j+9)/16); glVertex3f(i+1, j+1, 8);
			glTexCoord2f((float)(i+9)/16, (float)(j+8)/16); glVertex3f(i+1, j, 8);
			
			//Back
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)(i+8)/16, (float)(j+8)/16); glVertex3f(i, j, -8);
			glTexCoord2f((float)(i+8)/16, (float)(j+9)/16); glVertex3f(i, j+1, -8);
			glTexCoord2f((float)(i+9)/16, (float)(j+9)/16); glVertex3f(i+1, j+1, -8);
			glTexCoord2f((float)(i+9)/16, (float)(j+8)/16); glVertex3f(i+1, j, -8);

			//Left
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f((float)(i+8)/16, (float)(j+8)/16); glVertex3f(-8, j, i);
			glTexCoord2f((float)(i+8)/16, (float)(j+9)/16); glVertex3f(-8, j+1, i);
			glTexCoord2f((float)(i+9)/16, (float)(j+9)/16); glVertex3f(-8, j+1, i+1);
			glTexCoord2f((float)(i+9)/16, (float)(j+8)/16); glVertex3f(-8, j, i+1);
			
			//Right
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f((float)(i+8)/16, (float)(j+8)/16); glVertex3f(8, j, i);
			glTexCoord2f((float)(i+8)/16, (float)(j+9)/16); glVertex3f(8, j+1, i);
			glTexCoord2f((float)(i+9)/16, (float)(j+9)/16); glVertex3f(8, j+1, i+1);
			glTexCoord2f((float)(i+9)/16, (float)(j+8)/16); glVertex3f(8, j, i+1);
		}
	}
			
	glEnd();
}

//--------------------------------------------------------------------------------

void boxes(){
	
	//Box pushed by Robot 1
	
	glColor3f(0.5, 0.5, 1);
	glPushMatrix();
		glTranslatef(-10.25+box_move_x,0.5,-10-box_move_z);
		glTranslatef(1.25,0,0);
		glRotatef(0+box_angle,0,1,0);
		glTranslatef(-1.25,0,0);
		glRotatef(90,0,1,0);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	//Box pushed by Robot 2

	glColor3f(1, 0.5, 0.5);
	glPushMatrix();
		
		glTranslatef(10.25-box_move_x ,0.5,-10+box_move_z);
		glTranslatef(-1.25,0,0);
		glRotatef(0+box_angle,0,1,0);
		glTranslatef(1.25,0,0);
		glRotatef(-90,0,1,0);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	////////////////////// Boxes against Right wall ///////////////////////
	
	//Box by Alien
	glColor3f(0.5, 0.5, 0.5); 
	glPushMatrix();
		glTranslatef(14.5,0.5,9);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(14.5,0.5,-10);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();	
	
	glPushMatrix();
		glTranslatef(14.5,0.5,-14.5);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(14.5,0.5,-10);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(14.5,0.5,-12);
		glScalef(0.0625,0.0625,0.0625);
		glRotatef(22.5,0,1,0);
		createBox();
	glPopMatrix();	
	
	glPushMatrix();
		glTranslatef(14.5,0.5,-6.5);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(12.5,0.25,-6.5);
		glScalef(0.125,0.03125,0.0625);
		createBox();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(13.5,0.5,-14.5);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(14.5,0.5,-13.5);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(14.25,1.5,-14.25);
		glScalef(0.0625,0.0625,0.0625);
		glRotatef(45,0,1,0);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(12.5,0.375,-14.5);
		glScalef(0.0625,0.046875,0.0625);
		createBox();
	glPopMatrix();
	
	////////////////////// Boxes against Back wall ///////////////////////
	
	glPushMatrix();
		glTranslatef(-2,0.5,-14.25);
		glScalef(0.0625,0.0625,0.0625);
		glRotatef(33,0,1,0);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-0.4,0.5,-14.5);
		glScalef(0.125,0.0625,0.03125);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-1.8,1.25,-14.25);
		glScalef(0.03125,0.03125,0.03125);
		glRotatef(-10,0,1,0);
		createBox();
	glPopMatrix();		
	
	////////////////////// Boxes against Left wall ///////////////////////
	
	glPushMatrix();
		glTranslatef(-14.5,0.5,-10);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-14,1,-14);
		glScalef(0.125,0.125,0.125);
		createBox();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-12.75,0.5,-14.75);
		glScalef(0.03125,0.0625,0.03125);
		createBox();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-14.33,2.5,-14.16);
		glScalef(0.0625,0.0625,0.0625);
		glRotatef(33,0,1,0);
		createBox();
	glPopMatrix();
		
	glPushMatrix();
		glTranslatef(-14.5,0.5,-12.5);
		glScalef(0.0625,0.0625,0.0625);
		createBox();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-14,1.5,-6.83);
		glRotatef(-67,1,0,0);
		glScalef(0.125,0.03125,0.1875);
		
		createBox();
	glPopMatrix();				
							
}

//--------------------------------------------------------------------------------

void cameraBounds(float s, float dir_x, float dir_z) 
{
	
	if (eye_x > 13.75) {
		eye_x = 13.75;
	}
	else if (eye_x < -13.75) {
		eye_x = -13.75;
	}
	if (eye_z > 10.75) {
		eye_z = 10.75;
	}
	else if (eye_z < -13.75) {
		eye_z = -13.75;
	}

	bool outOfBounds = false; 

	//Spaceship
	if (liftoff == 0) {
		if ((eye_x > -3.5 && eye_x < 3.5) && (eye_z > -4.5 && eye_z < 5.5)) outOfBounds = true;
		if ((eye_x > -6.5 && eye_x < -3.5) && (eye_z > -4.5 && eye_z < 0)) outOfBounds = true;
		if ((eye_x > 3.5 && eye_x < 6.5) && (eye_z > -4.5 && eye_z < 0)) outOfBounds = true;
	}
	
	//Hangar Supports
	if ((eye_x > 10.5 && eye_x <= 13.75) && (eye_z > 3.5 && eye_z < 6.5)) outOfBounds = true;
	if ((eye_x >= -13.75 && eye_x < -10.5) && (eye_z > 3.5 && eye_z < 6.5)) outOfBounds = true;
	if ((eye_x > 10.5 && eye_x <= 13.75) && (eye_z > -6.5 && eye_z < -3.5)) outOfBounds = true;
	if ((eye_x >= -13.75 && eye_x < -10.5) && (eye_z > -6.5 && eye_z < -3.5)) outOfBounds = true;

	if (outOfBounds) {
		if (dir_x < 0) {	
			eye_x = eye_x + s*(dir_x*dir_x);
		}
		else {
			eye_x = eye_x - s*(dir_x*dir_x);
		}
		if (dir_z < 0) {
			eye_z = eye_z + s*(dir_z*dir_z);
		}
		else {
			eye_z = eye_z - s*(dir_z*dir_z);	
		}		
	}	
}

//--------------------------------------------------------------------------------

void display() 
{
	float lgt0_pos[] = {0.0, 8.0, 20.0, 1.0};  //light0 position outside the hangar door.
	float lgt1_pos[] = {0.0, 9.5f, 0.0f, 1.0};  //light1 position on the ceiling.
	float dir_x = -sin(theta), dir_y = 0,  dir_z = -cos(theta); 	
	float s = 0.2;
	float d = 2;
	
	eye_x = eye_x + (s * dir_x * step);
	eye_z = eye_z + (s * dir_z * step);

	look_x = eye_x + (d * dir_x);
	look_y = eye_y + (d * dir_y);
	look_z = eye_z + (d * dir_z);
	
	cameraBounds(s, dir_x,dir_z); ///Uncomment this to turn collision detection off
	
	// Uncomment to print eye, look and dir coordinates
	//cout << "Current location is (" << eye_x << ", "<< eye_y << ", " << eye_z << ")\n";
	//cout << "Current look location is (" << look_x << ", "<< look_y << ", " << look_z << ")\n";
	//cout << "Current direction is (" << dir_x << ", "<< dir_y << ", " << dir_z << ")\n";	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glMatrixMode(GL_MODELVIEW);								
	glLoadIdentity();
	gluLookAt(eye_x, eye_y, eye_z,  look_x, look_y, look_z,   0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lgt0_pos);   //set light position
	glLightfv(GL_LIGHT1, GL_POSITION, lgt1_pos);   //light1 position
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	alien();
	space();
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, grey);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	walls();
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	floor();
	glBindTexture(GL_TEXTURE_2D, txId[3]);	
	roof();
	glBindTexture(GL_TEXTURE_2D, txId[4]);
	supports();
	glBindTexture(GL_TEXTURE_2D, txId[5]);
	door();
	robots();
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	spaceship();
	glBindTexture(GL_TEXTURE_2D, txId[6]);
	boxes();
	
	glutSwapBuffers();									
}

//--------------------------------------------------------------------------------

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (1280,720); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow ("Alien Spaceport");
   initialise();


   glutTimerFunc(timer_delay, timer, 0);
   glutDisplayFunc(display); 
   glutSpecialFunc(special);
   glutMainLoop();
   return 0;
}
