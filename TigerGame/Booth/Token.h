/*
 * Token.h
 *
 *  Created on: Dec 13, 2017
 *      Author: booth
 */

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <iostream>
using namespace std;

#include "Color.h"
#include "Point.h"
#include "constants.h"



class Token{
    private:
	    Point location;  //(x,y) on a 13x9 grid, will never use pixel to represent location
	    Point oldLocation;
	    Color color;
	    bool  attached, moving;
	    bool  active;
	    int   size;

    public:

	    Token(Point p = Point(), int s = TOKEN_SIZE, Color c = BLACK){
	    		location = p;
	    		oldLocation = location;
	    		color = c;
	    		attached = false;
	    		moving = false;
	    		active = true;
	    		size = s;
	    }

	    void setActive(bool flag){
	    	    active = flag;
	    }

	    bool isActive(){
	    	    return active;
	    }

	    void revertLocation(){
	    	   location = oldLocation;
	    }

	    void setLocation(Point p ){
	    	    	    location = p;
	    }

	    Point getLocation(){
	    	    //cout << "(" << location.x << ", " << location.y << ")" << endl;
	    	   // assert(location.x <= 9 && location.x >= 0);
	    	    return location;
	    }

	    Point getOldLocation(){
	    	    return oldLocation;
	    }

	   void setColor(Color c){
		   color = c;
	   }

	   Color getColor(){
		   return color;
	   }


	   void setAttached(bool flag){
		   attached = flag;
	   }

	   bool isAttached(){
		   return attached;
	   }

	   bool isOn(Point p){
           return p.x == location.x && p.y == location.y;
	   }

	   void dumpState(){
		   if(color.R == 220)
			   cout << "RED:  ";
		   else
			   cout << "BLUE: ";
		   cout << "(" << oldLocation.y << ", " << oldLocation.x << ") ---> ";
		   cout << "(" << location.y    << ", " << location.x << ")";
		   cout << endl;
	   }


       void setOldLocation(Point p){
    	      oldLocation = p;
       }

       void setMoving(bool flag){
    	        moving = flag;
       }

       void setSize(int s){
    	       size = s;
       }

};



#endif /* _TOKEN_H_ */
