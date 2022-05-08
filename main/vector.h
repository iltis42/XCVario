/***********************************************************************
**
**   vector.h
**
**   This file is part of Cumulus.
**
************************************************************************
**
**   Copyright (c):  2002 by André Somers
**                   2002    Eckhard Völlm
**                   2009-2015 Axel Pauli
**
**   This file is distributed under the terms of the General Public
**   License. See the file COPYING for more information.
**
***********************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

/**
 * \class Vector
 *
 * \author André Somers, Axel Pauli, Eckhard Völlm
 *
 * \brief A vector represents a speed in a certain (2d) direction.
 *
 * A vector represents a speed in a certain (2d) direction.
 * It is a subclass from the @ref float class, meaning you can use it as a
 * normal float object. The values returned or set in that way are the
 * values in the given direction. You can read or set that direction using
 * the getAngle... and setAngle member functions (or their variants).
 * You can also access the components of the speed in the X and Y directions.
 * Note that X counts latitudinal, Y count longitudinal.
 *
 * \date 2002-2015
 */

#define PI2 2*M_PI

#ifndef D2R
#define D2R(x) ((x)/57.2957795131)
#define R2D(x) ((x)*57.2957795131)
#endif

typedef struct bitfield_vector {
	   bool dirtyXY  :1;   // True if X and/or Y have been set, and speed and direction need to be recalculated
       bool dirtyDR  :1;
	   bool _isValid :1;
}bitfield_vector_t;

class Vector
{
public:

    Vector();
    Vector(const float angle, const float speed);

    ~Vector();

    static float polar(float y, float x);
    static float normalize(float angle);
    static float normalizeDeg(float angle);
    static float normalizeDeg180(float angle);

    static float angleDiff(float ang1, float ang2);    // RAD
    static float angleDiffDeg(float ang1, float ang2);    // DEG


    /**
     * Get angle in degrees.
     */
    float getAngleDeg();

    /**
     * Get angle in radian.
     */
    float getAngleRad();

    /**
     * set the angle in degrees
     */
    void setAngle(const float angle);

    /**
     * set the angle in degrees  and the speed
     */
    void setAngleAndSpeed(const int angle, const float&);

    /**
     * set the angle in radian
     */
    void setAngleRad(const float& angle);

    /**
     * Set the speed. Expected unit is meter per second.
     */
    void setSpeedKmh(const float kmh);
    void setSpeedMps(const float mps);

    /**
     * @return The speed
     */
    float getSpeed();  // in internal units of Kmh

    float getSpeedMps();

    /**
     * @return The speed in Y (longitude) direction
     * (east is positive, west is negative)
     */
    float getY();

    /**
     * @return The speed in X (latitude) direction
     * (north is positive, south is negative)
     */
    float getX();

    /**
     * @returns The speed in Y (longitude) direction
     * (east is positive, west is negative) in meters per second
     */
    float getYMps();

    /**
     * @return The speed in X (latitude) direction
     * (north is positive, south is negative) in meters per second
     */
    float getXMps();


    /**
     * Sets the X (latitudinal) speed in meters per second.
     */
    void setX(const float& x);

    /**
     * Sets the Y (longitudinal) speed in meters per second.
     */
    void setY(const float& y);

    /* Operators */
    /**
     * = operator for Vector.
     */
    Vector& operator = (const Vector& x);

    /**
     * + operator for Vector.
     */
    Vector operator + (Vector& x);

    /**
     * - operator for Vector.
     */
    Vector operator - (Vector& x);

    /**
     * / operator for Vector.
     */
    float operator / (Vector& x);

    /**
     * operator for Vector.
     */
    float operator * (Vector& x);

    /**
     * != operator for Vector
     */
    bool operator != ( Vector& x);

    /**
      * == operator for Vector
      */
    bool operator == ( Vector& x);

    /**
     * minus prefix operator for Vector
     */
    Vector operator - ();

    /**
     * * prefix operator for Vector
     */
    Vector operator * (float left);

    Vector operator * (int left);

    /**
     * Poor man's solution for not getting the +
     * operator to work properly.
     */
    void add(Vector arg);

    /**
     * Returns a copy of the object
     */
    Vector clone();

    /**
     * Sets the distance to be invalid
     */
    void setInvalid()
    {
        flags._isValid=false;
        flags.dirtyXY=true;
        flags.dirtyDR=true;
        _angle=0;
        _x=0;
        _y=0;
        _speed=0;
    };

    /**
     * Gets if the distance is valid
     */
    bool isValid() const
    {
      return flags._isValid;
    };

protected: // Protected attributes

    bitfield_vector_t flags;
    /**
     * Contains the angle of the speed. 0 is north, pi/2 east, pi south, etc.
     */
    float _angle;

    /**
     * float in X (latitudinal) direction in meters per second, north being positive.
     */
    float _x;

    /**
     * float in Y (longitudinal) direction in meters per second, east being positive.
     */
    float _y;

    /**
     * float in mps
     */
    float _speed;

private:
    /**
     * Recalculates the X and Y values from the known angle and speed.
     */
    void recalcXY();

    /**
     * Recalculates the the angle and the distance from the known x and y values.
     */
    void recalcDR();
};

/** operators for vector. */
Vector operator * (float left, Vector& right);
Vector operator * (Vector& left, float right);
Vector operator / (Vector& left, float right);
Vector operator / (Vector& left, int right);

#endif
