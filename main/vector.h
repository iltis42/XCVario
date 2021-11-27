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
 * It is a subclass from the @ref double class, meaning you can use it as a
 * normal double object. The values returned or set in that way are the
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

class Vector
{
public:

    Vector();
    // Vector(const double& x, const double& y);
    Vector(const int angle, const double& R);
    Vector(const double angle, const double speed);

    ~Vector();

    static double polar(double y, double x);
    static double normalize(double angle);
    static double normalizeDeg(double angle);
    static float normalizeDeg(float angle);
    static double normalizeDeg180(double angle);
    static float normalizeDeg180(float angle);

    static double angleDiff(double ang1, double ang2);    // RAD
    static float angleDiffDeg(float ang1, float ang2);    // DEG
    static double angleDiffDeg(double ang1, double ang2); // DEG


    /**
     * Get angle in degrees.
     */
    double getAngleDeg();

    /**
     * Get angle in radian.
     */
    double getAngleRad();

    /**
     * set the angle in degrees
     */
    void setAngle(const int angle);
    void setAngle(const double angle);

    /**
     * set the angle in degrees  and the speed
     */
    void setAngleAndSpeed(const int angle, const double&);

    /**
     * set the angle in radian
     */
    void setAngleRad(const double& angle);

    /**
     * Set the speed. Expected unit is meter per second.
     */
    void setSpeedKmh(const double kmh);
    void setSpeedMps(const double mps);

    /**
     * @return The speed
     */
    double getSpeed();  // in internal units of Kmh

    double getSpeedMps();

    /**
     * @return The speed in Y (longitude) direction
     * (east is positive, west is negative)
     */
    double getY();

    /**
     * @return The speed in X (latitude) direction
     * (north is positive, south is negative)
     */
    double getX();

    /**
     * @returns The speed in Y (longitude) direction
     * (east is positive, west is negative) in meters per second
     */
    double getYMps();

    /**
     * @return The speed in X (latitude) direction
     * (north is positive, south is negative) in meters per second
     */
    double getXMps();


    /**
     * Sets the X (latitudinal) speed in meters per second.
     */
    void setX(const double& x);

    /**
     * Sets the Y (longitudinal) speed in meters per second.
     */
    void setY(const double& y);

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
    double operator / (Vector& x);

    /**
     * operator for Vector.
     */
    double operator * (Vector& x);

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
    Vector operator * (double left);

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
        _isValid=false;
        dirtyXY=true;
        dirtyDR=true;
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
      return _isValid;
    };

protected: // Protected attributes
    /**
     * Contains the angle of the speed. 0 is north, pi/2 east, pi south, etc.
     */
    double _angle;

    /**
     *  True if the speed and/or direction have been set, and XY need to be recalculated
     */
    bool dirtyXY;

    /**
     * True if X and/or Y have been set, and speed and direction need to be recalculated
     */
    bool dirtyDR;

    /**
     * double in X (latitudinal) direction in meters per second, north being positive.
     */
    double _x;

    /**
     * double in Y (longitudinal) direction in meters per second, east being positive.
     */
    double _y;

    /**
     * double in mps
     */
    double _speed;

    /**
     * value valid?
     */
    bool _isValid;

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
Vector operator * (double left, Vector& right);
Vector operator * (Vector& left, double right);
Vector operator / (Vector& left, double right);
Vector operator / (Vector& left, int right);

#endif
