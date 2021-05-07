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
dirtyXY(false),
dirtyDR(false),
_x(0.0),
_y(0.0),
_speed(0.0),
_isValid(false)
{
}

/*
Vector::Vector(const double& x, const double& y)
{
	_angle = 0.0;
	_speed = 0.0;
	dirtyXY = false;
	dirtyDR = false;
	setX( x );
	setY( y );
	dirtyXY = false;
	dirtyDR = true;
	_isValid = true;
}
*/

Vector::Vector(const int angle, const double& speed)
{
	// ESP_LOGI(FNAME, "New Vector ang:%d speed %f", angle, speed );
	_x = 0.0;
	_y = 0.0;
	dirtyXY = false;
	dirtyDR = false;
	_speed = speed;
	setAngle( angle );
	dirtyDR = false;
	dirtyXY = true;
	_isValid = true;
}

Vector::Vector(const double angle, const double speed )
{
	// ESP_LOGI(FNAME, "New Vector ang:%f speed %f", angle, speed );
	_x = 0.0;
	_y = 0.0;
	dirtyXY = false;
	dirtyDR = false;
	_speed = speed;
	setAngle( angle );
	dirtyDR = false;
	dirtyXY = true;
	_isValid = true;
}



double Vector::normalize(double angle)
{
	//perhaps use something similar here?
	if (angle < 0)
		return normalize (angle + PI2);

	if (angle >= PI2)
		return normalize (angle - PI2);

	return angle;
}


double Vector::polar(double y, double x)
{
	double angle = 0.0;
	if(x >= -0.001 && x <= 0.001) {
		if(y < 0.0)
			return ( 1.5 * M_PI );
		else
			return ( PI2 );
	}
	// Punkt liegt auf der neg. X-Achse
	if(x < 0.0)
		angle = atan( y / x ) + M_PI;
	else
		angle = atan( y / x );

	// Neg. value not allowed.
	if(angle < 0.0)
		angle = PI2 + angle;

	if(angle > (PI2))
		angle = angle - (PI2);

	return angle;
}




double Vector::angleDiffDeg(double ang1, double ang2)
{
	return R2D( angleDiff( D2R(ang1), D2R(ang2) ) );
};

double Vector::angleDiff(double ang1, double ang2)
{
	double a1 = normalize (ang1);
	double a2 = normalize (ang2);
	double a = a2 - a1;

	if (a > M_PI)
		return (a - PI2);

	if (a < -M_PI)
		return (a + PI2);

	return a;
};

Vector::~Vector()
{}


double Vector::getAngleDeg()
{
	if( dirtyDR )
	{
		recalcDR();
	}

	return (_angle * 180.0/M_PI);
}

/** Get angle in radian. */
double Vector::getAngleRad()
{
	if( dirtyDR )
	{
		recalcDR();
	}

	return _angle;
}

/** Set property of integer angle in degrees. */
void Vector::setAngle(const int angle)
{
	// ESP_LOGI(FNAME, "setAngle I ang:%d", angle );
	if( dirtyDR )
	{
		recalcDR();
	}

	_angle = normalize( angle );
	dirtyXY = true;
	_isValid = true;

}

void Vector::setAngle(const double angle)
{
	// ESP_LOGI(FNAME, "setAngle D ang:%f", angle );
	if( dirtyDR )
	{
		recalcDR();
	}

	_angle = normalize( angle*M_PI/180.0 );
	dirtyXY = true;
	_isValid = true;
	// ESP_LOGI(FNAME, "New angle ang:%f", _angle );
}


/**
 * set the angle in degrees and the speed.
 */
void Vector::setAngleAndSpeed(const int angle, const double & spd)
{
	if( dirtyDR )
	{
		recalcDR();
	}

	setAngle( angle );
	_speed = spd;
	dirtyDR = false;
	dirtyXY = true;
	_isValid = true;
}

/** Set property of double angle as radian. */
void Vector::setAngleRad(const double& angle)
{
	if( dirtyDR )
	{
		recalcDR();
	}

	_angle = normalize( angle );
	dirtyXY = true;
	dirtyDR = false;
	_isValid = true;
}

/**
 * Set the speed
 */
void Vector::setSpeedKmh(const double speed)
{
	if( dirtyDR )
	{
		recalcDR();
	}

	_speed = speed;
	_isValid = true;
}

/**
 * Set the speed. Expected unit is meter per second.
 */
void Vector::setSpeedMps(const double mps)
{
	if( dirtyDR )
	{
		recalcDR();
	}

	_speed = mps*3.6;
	_isValid = true;
}

/**
 * @return The speed
 */
double Vector::getSpeed()
{
	if( dirtyDR )
	{
		recalcDR();
	}

	return double( _speed );
}

double Vector::getSpeedMps()
{
	if( dirtyDR )
	{
		recalcDR();
	}

	return double( Units::kmh2ms( _speed ) );
}


/** Recalculates the the angle and the speed from the known x and y values. */
void Vector::recalcDR()
{
	_angle = normalize( polar( _y, _x ) );
	_speed = hypot( _y, _x );
	dirtyDR = false;
}


/** Recalculates the X and Y values from the known angle and speed. */
void Vector::recalcXY()
{
	_y = _speed * sin( _angle );
	_x = _speed * cos( _angle );
	dirtyXY = false;
}


/** returns the speed in X (latitude) direction (north is positive, south is negative) */
double Vector::getX()
{
	if( dirtyXY )
	{
		recalcXY();
	}

	return double( _x );
}


/** Returns the speed in Y (longitude) direction (east is positive, west is negative) */
double Vector::getY()
{
	if( dirtyXY )
	{
		recalcXY();
	}

	return double( _y );
}


/** returns the speed in X (latitude) direction (north is positive, south is negative) */
double Vector::getXMps()
{
	if( dirtyXY )
	{
		recalcXY();
	}

	return _x;
}

/** Returns the speed in Y (longitude) direction (east is positive, west is negative) */
double Vector::getYMps()
{
	if( dirtyXY )
	{
		recalcXY();
	}

	return _y;
}


/** Sets the Y (longitudinal) speed in meters per second. */
void Vector::setY(const double& y)
{
	if( dirtyXY )
	{
		recalcXY();
	}

	_y = y;
	dirtyDR = true;
	_isValid = true;
}

/** Sets the X (latitudinal) speed in meters per second. */
void Vector::setX(const double& x)
{
	if( dirtyXY )
	{
		recalcXY();
	}

	_x = x;
	dirtyDR = true;
	_isValid = true;
}

/** = operator for Vector. */
Vector& Vector::operator = (const Vector& x)
{
	_isValid = x._isValid;
	setX( x._x );
	setY( x._y );
	_speed = x._speed;
	_angle = x._angle;
	dirtyXY = x.dirtyXY;
	dirtyDR = x.dirtyDR;

	return *this;
}


/** + operator for Vector. */
Vector Vector::operator + (Vector& x)
{
	if( dirtyXY )
	{
		recalcXY();
	}

	if( x.dirtyXY )
	{
		x.recalcXY();
	}

	return Vector( x._x + _x, x._y + _y );
}


/** - operator for Vector. */
Vector Vector::operator - (Vector& x)
{
	if( dirtyXY )
	{
		recalcXY();
	}

	if( x.dirtyXY )
	{
		x.recalcXY();
	}

	return Vector( _x - x._x, _y - x._y );
}

/** * operator for Vector. */
Vector Vector::operator * (double left)
		{
	if( dirtyDR )
	{
		recalcDR();
	}

	return Vector( _angle, double( left * _speed ) );
		}


Vector Vector::operator * (int left)
		{
	if( !dirtyDR )
	{
		return Vector( _angle, double( left * _speed ) );
	}
	else if( !dirtyXY )
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
double Vector::operator / ( Vector& x)
		{
	if (dirtyDR)
		recalcDR();
	if (x.dirtyDR)
		x.recalcDR();
	return _speed / x._speed;
		}


/** * operator for Vector. */
double Vector::operator * ( Vector& x)
		{
	if (dirtyDR)
		recalcDR();
	if (x.dirtyDR)
		x.recalcDR();
	return _speed * x._speed;
		}


/** == operator for Vector */
bool Vector::operator == ( Vector& x)
		{
	Vector t( x );
	Vector u( *this );

	if( u.dirtyDR )
	{
		u.recalcDR();
	}

	if( t.dirtyDR )
	{
		t.recalcDR();
	}

	return ((t._speed == u._speed) && (t._angle == u._angle));
		}


/** != operator for Vector */
bool Vector::operator != ( Vector& x)
		{
	if( dirtyDR )
	{
		recalcDR();
	}

	return ((x._speed != _speed) || (x._angle != _angle));
		}


/** - prefix operator for Vector */
Vector Vector::operator - ()
{
	//there are two options for this. We use the one that involves the least conversions.
	if( !dirtyDR )
	{
		return Vector( _angle + M_PI, double( _speed ) );
	}
	else if( !dirtyXY )
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
Vector operator * (Vector& left, double right)
		{
	return Vector( left.getAngleRad(), double( right * left.getSpeed() ) );
		}


/** * operator for vector. */
Vector operator * (double left, Vector& right)
		{
	return Vector( right.getAngleRad(), double( left * right.getSpeed() ) );
		}


/** / operator for vector. */
Vector operator /( Vector& left, double right )
{
	return Vector( left.getAngleRad(), double( left.getSpeed() / right ) );
}

/** / operator for vector. */
Vector operator /( Vector& left, int right )
{
	return Vector( left.getAngleRad(), double( left.getSpeed() / right ) );
}


/** Poor man's solution for not getting the + operator to work properly. */
void Vector::add(Vector arg)
{
	if( arg.dirtyXY )
	{
		arg.recalcXY();
	}

	if( dirtyXY )
	{
		recalcXY();
	}

	_x += arg.getXMps();
	_y += arg.getYMps();

	dirtyDR = true;
}


/** Returns a copy of the object */
Vector Vector::clone()
{
	Vector result;

	result._isValid = _isValid;
	result._speed = _speed;
	result.setAngleRad( this->getAngleRad() );
	result.dirtyDR = false;
	return result;
}
