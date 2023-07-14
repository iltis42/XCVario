
class Point {
public:
    int x;
    int y;
    Point( int ax, int ay ){
    	x = ax;
    	y = ay;
    };
    Point(){
        	x = 0;
        	y = 0;
    };
    Point rotate(const Point& center, float angle);
    void moveVertical(int pixel);
};



