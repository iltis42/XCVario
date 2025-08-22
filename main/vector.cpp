/***********************************************************************
 **
 **   vector.cpp
 **
 **   This file is part of Cumulus
 **
 ************************************************************************
 **
 **   Copyright (c):  2002      by André Somers
 **                   2009-2015 by Axel Pauli
 **
 **   This file is distributed under the terms of the General Public
 **   License. See the file COPYING for more information.
 **
 ***********************************************************************/

#include <cmath>
#include "Units.h"
#include "vector.h"
#include "logdef.h"

Vector::Vector() :
_angle(0.0),
_x(0.0),
_y(0.0),
_speed(0.0)
{
	flags.dirtyXY = false;
	flags.dirtyDR = true;
	flags._isValid = true;
}

Vector::Vector(const float angle, const float speed )
{
	// ESP_LOGI(FNAME, "New Vector ang:%f speed %f", angle, speed );
	_x = 0.0;
	_y = 0.0;
	flags.dirtyXY = false;
	flags.dirtyDR = false;
	_speed = speed;
	setAngle( angle );
	flags.dirtyDR = false;
	flags.dirtyXY = true;
	flags._isValid = true;
}

//  in: -oo < angle < oo
// out: 0 .. 2*pi
float Vector::normalizePI2(float angle)
{
    return angle - PI2f * fast_floorf(angle / PI2f);
}

//  in: -oo < angle < oo
// out: 0 .. 2*pi
float Vector::normalizePI(float angle)
{
    return angle - PI2f * fast_floorf((angle + My_PIf) / PI2f);
}

//  in: -oo < angle < oo
// out: 0 .. 360
float Vector::normalizeDeg(float angle)
{
    return angle - 360.0f * fast_floorf(angle / 360.0f);
}

//  in: -oo < angle < oo
// out: -180 .. 180
float Vector::normalizeDeg180(float angle)
{
    return angle - 360.0f * fast_floorf((angle+180.0f) / 360.0f);
}

//  in: -oo < angle < oo
// out: -180 .. 180
float Vector::reverseBearing(float angle)
{
	return normalizeDeg180(angle+180.0f);
}

//  in: -oo < x,y < oo
// out: 0 .. 2*pi
float Vector::polar(float y, float x)
{
    float angle = atan2f(y, x);  // range: (–π, π]
    if (angle < 0.0f) {
        angle += PI2f;    // convert to [0, 2π)
	}
    return angle;
}

//  in: -oo < ang1, ang2 < oo
// out: -180 .. 180
float Vector::angleDiffDeg(float ang1, float ang2)
{
	return normalizeDeg180(ang1 - ang2);
}

//  in: -oo < ang1, ang2 < oo
// out: -pi .. pi
float Vector::angleDiff(float ang1, float ang2)
{
	return normalizePI(ang1 - ang2);
}

Vector::~Vector()
{}

float Vector::getAngleDeg()
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	return (_angle * 180.0/My_PIf);
}

/** Get angle in radian. */
float Vector::getAngleRad()
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	return _angle;
}

void Vector::setAngle(const float angle)
{
	// ESP_LOGI(FNAME, "setAngle D ang:%f", angle );
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	_angle = normalizePI2( angle*My_PIf/180.0 );
	flags.dirtyXY = true;
	flags._isValid = true;
	// ESP_LOGI(FNAME, "New angle ang:%f", _angle );
}


/**
 * set the angle in degrees and the speed.
 */
void Vector::setAngleAndSpeed(const int angle, const float & spd)
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	setAngle( angle );
	_speed = spd;
	flags.dirtyDR = false;
	flags.dirtyXY = true;
	flags._isValid = true;
}

/** Set property of float angle as radian. */
void Vector::setAngleRad(const float& angle)
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	_angle = normalizePI2( angle );
	flags.dirtyXY = true;
	flags.dirtyDR = false;
	flags._isValid = true;
}

/**
 * Set the speed
 */
void Vector::setSpeedKmh(const float speed)
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	_speed = speed;
	flags._isValid = true;
}

/**
 * Set the speed. Expected unit is meter per second.
 */
void Vector::setSpeedMps(const float mps)
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	_speed = mps*3.6;
	flags._isValid = true;
}

/**
 * @return The speed
 */
float Vector::getSpeed()
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	return float( _speed );
}

float Vector::getSpeedMps()
{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	return float( Units::kmh2ms( _speed ) );
}


/** Recalculates the the angle and the speed from the known x and y values. */
void Vector::recalcDR()
{
	_angle = normalizePI2( polar( _y, _x ) );
	_speed = hypot( _y, _x );
	flags.dirtyDR = false;
}


/** Recalculates the X and Y values from the known angle and speed. */
void Vector::recalcXY()
{
	_y = _speed * sin( _angle );
	_x = _speed * cos( _angle );
	flags.dirtyXY = false;
}


/** returns the speed in X (latitude) direction (north is positive, south is negative) */
float Vector::getX()
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	return float( _x );
}


/** Returns the speed in Y (longitude) direction (east is positive, west is negative) */
float Vector::getY()
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	return float( _y );
}


/** returns the speed in X (latitude) direction (north is positive, south is negative) */
float Vector::getXMps()
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	return _x;
}

/** Returns the speed in Y (longitude) direction (east is positive, west is negative) */
float Vector::getYMps()
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	return _y;
}


/** Sets the Y (longitudinal) speed in meters per second. */
void Vector::setY(const float& y)
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	_y = y;
	flags.dirtyDR = true;
	flags._isValid = true;
}

/** Sets the X (latitudinal) speed in meters per second. */
void Vector::setX(const float& x)
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	_x = x;
	flags.dirtyDR = true;
	flags._isValid = true;
}

/** = operator for Vector. */
Vector& Vector::operator = (const Vector& x)
{
	flags._isValid = x.flags._isValid;
	setX( x._x );
	setY( x._y );
	_speed = x._speed;
	_angle = x._angle;
	flags.dirtyXY = x.flags.dirtyXY;
	flags.dirtyDR = x.flags.dirtyDR;

	return *this;
}


/** + operator for Vector. */
Vector Vector::operator + (Vector& x)
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	if( x.flags.dirtyXY )
	{
		x.recalcXY();
	}

	return Vector( x._x + _x, x._y + _y );
}


/** - operator for Vector. */
Vector Vector::operator - (Vector& x)
{
	if( flags.dirtyXY )
	{
		recalcXY();
	}

	if( x.flags.dirtyXY )
	{
		x.recalcXY();
	}

	return Vector( _x - x._x, _y - x._y );
}

/** * operator for Vector. */
Vector Vector::operator * (float left)
		{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	return Vector( _angle, float( left * _speed ) );
		}


Vector Vector::operator * (int left)
		{
	if( !flags.dirtyDR )
	{
		return Vector( _angle, float( left * _speed ) );
	}
	else if( !flags.dirtyXY )
	{
		return Vector( left * _x, left * _y );
	}
	else
	{
		recalcXY();
		return Vector( left * _x, left * _y );
	}
		}


/** / operator for Vector. */
float Vector::operator / ( Vector& x)
		{
	if (flags.dirtyDR)
		recalcDR();
	if (x.flags.dirtyDR)
		x.recalcDR();
	return _speed / x._speed;
		}


/** * operator for Vector. */
float Vector::operator * ( Vector& x)
		{
	if (flags.dirtyDR)
		recalcDR();
	if (x.flags.dirtyDR)
		x.recalcDR();
	return _speed * x._speed;
		}


/** == operator for Vector */
bool Vector::operator == ( Vector& x)
		{
	Vector t( x );
	Vector u( *this );

	if( u.flags.dirtyDR )
	{
		u.recalcDR();
	}

	if( t.flags.dirtyDR )
	{
		t.recalcDR();
	}

	return ((t._speed == u._speed) && (t._angle == u._angle));
		}


/** != operator for Vector */
bool Vector::operator != ( Vector& x)
		{
	if( flags.dirtyDR )
	{
		recalcDR();
	}

	return ((x._speed != _speed) || (x._angle != _angle));
		}


/** - prefix operator for Vector */
Vector Vector::operator - ()
{
	//there are two options for this. We use the one that involves the least conversions.
	if( !flags.dirtyDR )
	{
		return Vector( _angle + My_PIf, float( _speed ) );
	}
	else if( !flags.dirtyXY )
	{
		return Vector( -_x, -_y );
	}
	else
	{ //should not happen! Either XY or DR should be clean, or both.
		recalcXY();
		return Vector( -_x, -_y );
	}
}


/** * operator for vector. */
Vector operator * (Vector& left, float right)
{
	return Vector( left.getAngleRad(), float( right * left.getSpeed() ) );
}


/** * operator for vector. */
Vector operator * (float left, Vector& right)
{
	return Vector( right.getAngleRad(), float( left * right.getSpeed() ) );
}


/** / operator for vector. */
Vector operator /( Vector& left, float right )
{
	return Vector( left.getAngleRad(), float( left.getSpeed() / right ) );
}

/** / operator for vector. */
Vector operator /( Vector& left, int right )
{
	return Vector( left.getAngleRad(), float( left.getSpeed() / right ) );
}


/** Poor man's solution for not getting the + operator to work properly. */
void Vector::add(Vector arg)
{
	if( arg.flags.dirtyXY )
	{
		arg.recalcXY();
	}

	if( flags.dirtyXY )
	{
		recalcXY();
	}

	_x += arg.getXMps();
	_y += arg.getYMps();

	flags.dirtyDR = true;
}


/** Returns a copy of the object */
Vector Vector::clone()
{
	Vector result;

	result.flags._isValid = flags._isValid;
	result._speed = _speed;
	result.setAngleRad( this->getAngleRad() );
	result.flags.dirtyDR = false;
	return result;
}
