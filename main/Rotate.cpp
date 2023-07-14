#include <math.h>
#include "Rotate.h"

Point Point::rotate(const Point& center, float radians) {

    // Verschieben des Punktes zum Koordinatenursprung
    float translatedX = x - center.x;
    float translatedY = y - center.y;

    // Berechnung der rotierten Koordinaten
    float newX = translatedX * cosf(radians) - translatedY * sinf(radians);
    float newY = translatedX * sinf(radians) + translatedY * cosf(radians);

    // Verschieben des rotierten Punktes zurück zum ursprünglichen Koordinatensystem
    newX += center.x;
    newY += center.y;

    // Erstellen und Zurückgeben des rotierten Punktes
    Point rotatedPoint( rint(newX), rint(newY) );
    return rotatedPoint;
}

void Point::moveVertical(int pixel){
	y += pixel;
}
