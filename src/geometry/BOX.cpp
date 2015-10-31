/*
 This file is part of SSFR (Zephyr).
 
 Zephyr is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Zephyr is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Zephyr.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2013 Theodore Kim
*/

#include "BOX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOX::BOX(const VEC3F& center, const VEC3F& lengths, double period) :
  _center(center), _lengths(lengths), _period(period)
{
  set_halfLengths();
  set_angularVelocity();
  initialize_rotationMatrix();
  _theta = 0.0;
  _currentTime = 0.0;
  _rotationAxis = VEC3F(0.0, 0.0, 1.0);
}

BOX::BOX() 
{
}

BOX::~BOX()
{
}

void BOX::initialize_rotationMatrix()
{
  MATRIX3 eye = MATRIX3::I();
  _rotation = eye;
}

void BOX::update_rotationMatrix()
{
  // we must rotate *clockwise*, hence the minus sign
  _rotation = MATRIX3::rotation(_rotationAxis, -1 * _theta);
}

bool BOX::inside(const VEC3F& point) 
{
  // translate so that the center of rotation is the origin
  VEC3F pointTransformed = point - _center;
  // cout << "pointTransformed after translation: " << endl;
  // cout << pointTransformed << endl;
  // rotate *clockwise* back to the original position
  pointTransformed = _rotation * pointTransformed;
  // cout << "pointTransformed after rotation: " << endl;
  // cout << pointTransformed << endl;

  return ( fabs(pointTransformed[0]) <= _halfLengths[0]  &&
           fabs(pointTransformed[1]) <= _halfLengths[1]  &&
           fabs(pointTransformed[2]) <= _halfLengths[2] );

}

void BOX::update_r(const VEC3F& point, VEC3F* r)
{
  VEC3F u = point - _center;
  // we want to project u onto the plane z = z0
  // this plane has unit normal n = (0, 0, 1)
  // so we project u onto this unit normal and then subtract off that component
  VEC3F normalComponent = project_onto(u, _rotationAxis);
  *r = u - normalComponent;
}

void BOX::spin() 
{
  float d_theta = 2 * M_PI * _dt / _period;
  _theta += d_theta;
  // printf("Theta is updated to: %f\n", _theta);
  // work modulo 2pi
  if (_theta >= 2 * M_PI) { _theta = 0.0; };
  this->update_time();
}

void BOX::draw() const
{
  glPushMatrix();
    glTranslatef(_center[0], _center[1], _center[2]);
    // OpenGL uses degrees, so we need to convert
    glRotatef(this->thetaDegrees(), _rotationAxis[0], _rotationAxis[1], _rotationAxis[2]);
    glScalef(_lengths[0], _lengths[1], _lengths[2]);
    glColor4b(30, 10, 5, 40);
    glutSolidCube(1.0);
  glPopMatrix();  
}


