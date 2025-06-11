//
//  ofxSunCalc.h
//
//  Created by Adam Hinshaw on 30/04/2015.
//  Port of js @ http://suncalc.net/
//

#ifndef __ofxSunCalc__
#define __ofxSunCalc__

/*
 (c) 2011-2014, Vladimir Agafonkin
 SunCalc is a JavaScript library for calculating sun/mooon position and light phases.
 https://github.com/mourner/suncalc
 */

#include "ofMain.h"

#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"

typedef struct {
    double azimuth; // The azimuth of the Sun refers to the compass direction from which the sunlight is coming, measured in degrees along the horizon.
    double altitude;
} SunCalcPosition;

typedef struct {
    double azimuth;
    double altitude;
    double distance;
    double parallacticAngle;
} MoonCalcPosition;

typedef struct {
    Poco::DateTime start;
    Poco::DateTime end;
} SunCalcDateRange;

typedef struct SunCalcDayInfoExtended {
    bool isSet = false;
    
    SunCalcDateRange morningTwilightAstronomical;
    SunCalcDateRange morningTwilightNautical;
    SunCalcDateRange morningTwilightCivil;
    
    SunCalcDateRange nightTwilightAstronomical;
    SunCalcDateRange nightTwilightNautical;
    SunCalcDateRange nightTwilightCivil;
    
} SunCalcDayInfoExtended;

typedef struct {
    
    Poco::DateTime dawn;
    SunCalcDateRange sunrise;
    Poco::DateTime transit;
    SunCalcDateRange sunset;
    Poco::DateTime dusk;
    
    SunCalcDayInfoExtended ext;
    
    double lat;
    double lon;
    
} SunCalcDayInfo;

class ofxSunCalc {
    
public:
    
    ofxSunCalc();
    
    double dateToJulianDate( const Poco::DateTime & date );
    Poco::DateTime julianDateToDate( double j );
    
    int getJulianCycle( double J, double lw );
    
    double getApproxSolarTransit( double Ht, double lw, double n );
    double getSolarMeanAnomaly( double Js );
    
    double getEquationOfCenter( double M );
    double getEclipticLongitude( double M, double C );
    
    double getSolarTransit( double Js, double M, double Lsun );
    
    double getSunDeclination( double Lsun );
    
    double getRightAscension( double Lsun );
    double getSiderealTime( double J, double lw );
    
    double getAzimuth( double th, double a, double phi, double d );
    
    double getAltitude( double th, double a, double phi, double d );
    
    double getHourAngle( double h, double phi, double d );
    double getSunsetJulianDate( double w0, double M, double Lsun, double lw, double n );
    
    double getSunriseJulianDate( double Jtransit, double Jset );
    
    SunCalcPosition getSunPosition( const Poco::DateTime & date, double lat, double lon );
    SunCalcPosition getSunPosition( double J, double lw, double phi );
    
    MoonCalcPosition getMoonPosition( const Poco::DateTime & date, double lat, double lon);
    
    SunCalcDayInfo getDayInfo( const Poco::DateTime & date, double lat, double lon, bool detailed = false, int tz_offset = 0 );
    string infoToString(const SunCalcDayInfo & info, bool min = true, int tz_offset = 0);
    
    string static dateToString(const Poco::DateTime & date, int tz_offset = 0);
    string static dateToDateString(const Poco::DateTime & date, int tz_offset = 0);
    string static dateToTimeString(const Poco::DateTime & date, int tz_offset = 0);
	string static formatDate(const Poco::DateTime & date, const string pattern, int tz_offset = 0 );
	Poco::DateTime static offsetDate(const Poco::DateTime & date, int tz_offset);
	
    float static getSunBrightness(SunCalcDayInfo & info, const Poco::DateTime time, int tz_offset = 0);
    
    void drawDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info, bool simple = false);
    // void static drawExtendedDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info);
    
private:
    double rightAscension(double l, double b);
    double declination(double l, double b);
    double azimuth(double H, double phi, double dec);
    double altitude(double H, double phi, double dec);
    double siderealTime(double d, double lw);
    double astroRefraction(double h);

private:
    
    constexpr static double J1970 = 2440588;
    constexpr static double J2000 = 2451545;
    constexpr static double dayMs = 1000 * 60 * 60 * 24;
    constexpr static double deg2rad = PI / 180.0;
    constexpr static double M0 = 357.5291 * deg2rad;
    constexpr static double M1 = 0.98560028 * deg2rad;
    constexpr static double J0 = 0.0009;
    constexpr static double J1 = 0.0053;
    constexpr static double J2 = -0.0069;
    constexpr static double C1 = 1.9148 * deg2rad;
    constexpr static double C2 = 0.0200 * deg2rad;
    constexpr static double C3 = 0.0003 * deg2rad;
    constexpr static double P = 102.9372 * deg2rad;
    constexpr static double e = 23.4397 * deg2rad; // obliquity of the Earth
    constexpr static double th0 = 280.1600 * deg2rad;
    constexpr static double th1 = 360.9856235 * deg2rad;
    constexpr static double h0 = -0.833 * deg2rad; //sunset angle
    constexpr static double d0 = 0.53 * deg2rad; //sun diameter
    constexpr static double h1 = -6 * deg2rad; //nautical twilight angle
    constexpr static double h2 = -12 * deg2rad; //astronomical twilight angle
    constexpr static double h3 = -18 * deg2rad; //darkness angle
    
	ofColor nightCol = ofColor::darkBlue; // 127, 142, 180
	
	ofColor twilightCol = ofColor::mediumPurple;
	ofColor sunriseCol{254, 237, 127};
	ofColor dayCol = ofColor::skyBlue; // 254, 215, 127
	ofColor sunsetCol{254, 215, 127};
	
	ofColor transitCol = ofColor::indigo;
	
	ofColor twilightAstroCol = ofColor::blueViolet;
	ofColor twilightNauticalCol = ofColor::mediumOrchid;
	ofColor twilightCivilCol = ofColor::violet;
};

#endif /* defined(__ofxSunCalc__) */
