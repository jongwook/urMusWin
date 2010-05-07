#ifndef _OF_TTF_FONT_H_
#define _OF_TTF_FONT_H_


//#include "ofConstants.h"

// from ofConstants.h

//-------------------------------
#define OF_VERSION	6
//------------------------------


#if defined( __WIN32__ ) || defined( _WIN32 )
	#define TARGET_WIN32
#elif defined( __APPLE_CC__)
	#include <TargetConditionals.h>

	#if (TARGET_OF_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
		#define TARGET_OF_IPHONE
		#define TARGET_OPENGLES
	#else
		#define TARGET_OSX
	#endif
#else
	#define TARGET_LINUX
#endif
//-------------------------------


// then the the platform specific includes:
#ifdef TARGET_WIN32
	//this is for TryEnterCriticalSection
	//http://www.zeroc.com/forums/help-center/351-ice-1-2-tryentercriticalsection-problem.html
	#ifndef _WIN32_WINNT
		#   define _WIN32_WINNT 0x400
	#endif
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	//#include "GLee.h"
   	#include <gl/glu.h>
	#if (_MSC_VER)       // microsoft visual studio
		#pragma warning(disable : 4996)     // disable all deprecation warnings
		#pragma warning(disable : 4068)     // unknown pragmas
		#pragma warning(disable : 4101)     // unreferenced local variable
		#pragma	warning(disable : 4312)		// type cast conversion (in qt vp)
		#pragma warning(disable : 4311)		// type cast pointer truncation (qt vp)
		#pragma warning(disable : 4018)		// signed/unsigned mismatch (since vector.size() is a size_t)
		#pragma warning(disable : 4267)		// conversion from size_t to Size warning... possible loss of data
		#pragma warning(disable : 4800)		// 'Boolean' : forcing value to bool 'true' or 'false'
		#pragma warning(disable : 4099)		// for debug, PDB 'vc80.pdb' was not found with...
	#endif

	#define TARGET_LITTLE_ENDIAN			// intel cpu

	// some gl.h files, like dev-c++, are old - this is pretty universal
	#ifndef GL_BGR_EXT
	#define GL_BGR_EXT 0x80E0
	#endif
#endif

#ifdef TARGET_OSX
	#ifndef __MACOSX_CORE__
		#define __MACOSX_CORE__
	#endif
	#include <unistd.h>
	#include "GLee.h"
	#include <OpenGL/glu.h>
	#include <ApplicationServices/ApplicationServices.h>

	#if defined(__LITTLE_ENDIAN__)
		#define TARGET_LITTLE_ENDIAN		// intel cpu
	#endif
#endif

#ifdef TARGET_LINUX
        #include <unistd.h>
        #include "GLee.h"
        #include <GL/glu.h>
		#define TARGET_LITTLE_ENDIAN		// intel cpu
        #define B14400	14400
        #define B28800	28800
#endif


#ifdef TARGET_OF_IPHONE
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
#endif


// core: ---------------------------
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision



using namespace std;



// from ofTypes.h


//----------------------------------------------------------
// urPoint
//----------------------------------------------------------
class urPoint {
  public:
	virtual ~urPoint(){}

    urPoint( float _x=0.0f, float _y=0.0f, float _z=0.0f ) {
        x = _x;
        y = _y;
        z = _z;
    }

    urPoint( const urPoint & pnt) {
        x = pnt.x;
        y = pnt.y;
        z = pnt.z;
    }

    void set(float _x, float _y, float _z = 0){
        x = _x;
        y = _y;
        z = _z;
    }

    urPoint operator+( const urPoint& pnt ) const {
        return urPoint( x+pnt.x, y+pnt.y, z+pnt.z );
    }

	urPoint & operator+=( const urPoint& pnt ) {
        x+=pnt.x;
        y+=pnt.y;
        z+=pnt.z;
        return *this;
    }

    urPoint operator-(const urPoint& pnt) const {
        return urPoint( x-pnt.x, y-pnt.y, z-pnt.z );
    }

    urPoint operator*(const float& val) const {
        return urPoint( x*val, y*val, z*val);
    }

    urPoint & operator*=( const float & val ) {
        x*=val;
        y*=val;
        z*=val;
        return *this;
    }

    urPoint operator/( const float &val ) const {
		if( val != 0){
			return urPoint( x/val, y/val, z/val );
		}
        return urPoint(x, y, z );
    }

    urPoint& operator/=( const float &val ) {
		if( val != 0 ){
			x /= val;
			y /= val;
			z /= val;
		}
		return *this;
    }

	// union allows us to access the coordinates through
    // both an array 'v' and 'x', 'y', 'z' member varibles
    union  {
        struct {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
};


// from ofUtils.h
int 	ofNextPow2(int input);
string 	ofToDataPath(string path, bool absolute=false);
void	ofSetDataPathRoot( string root );


//--------------------------------------------------
typedef struct {
	int value;
	int height;
	int width;
	int setWidth;
	int topExtent;
	int leftExtent;
	float tTex;
	float vTex;		//0-1 pct of bitmap...
	float xOff;
	float yOff;
} charProps;

class ofTTFContour{
	public:
		vector <urPoint>pts;
};

class ofTTFCharacter{
	public:
		vector <ofTTFContour> contours;
};

//--------------------------------------------------
#define NUM_CHARACTER_TO_START		33		// 0 - 32 are control characters, no graphics needed.

class urFont{
public:
	urFont();
	virtual ~urFont();
		
	// 			-- default, non-full char set, anti aliased:
	void 		loadFont(string filename, int fontsize);
	void 		loadFont(string filename, int fontsize, bool _bAntiAliased, bool _bFullCharacterSet, bool makeContours = false);

	bool		bLoadedOk;
	bool 		bAntiAlised;
	bool 		bFullCharacterSet;

	float 		getLineHeight() {return lineHeight;}
	void 		setLineHeight(float height) { lineHeight = height; }
	
	void 		drawString(string s, float x, float y);
	int 		nCharacters;
	
	ofTTFCharacter getCharacterAsPoints(int character);

protected:
	vector <ofTTFCharacter> charOutlines;

	float 			lineHeight;
	charProps 		* 	cps;			// properties for each character
	GLuint			*	texNames;		// textures for each character
	int				fontSize;
	bool			bMakeContours;

	void 			drawChar(int c, float x, float y);
	
	int 			ofNextPow2(int a);
	int				border, visibleBorder;
};


#endif

