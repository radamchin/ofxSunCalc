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

#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormatter.h"

typedef struct {
    double azimuth;
    double altitude;
} SunCalcPosition;

typedef struct {
    Poco::LocalDateTime start;
    Poco::LocalDateTime end;
} SunCalcDateRange;

typedef struct {
    bool isSet = false;
    
    SunCalcDateRange morningTwilightAstronomical;
    SunCalcDateRange morningTwilightNautical;
    SunCalcDateRange morningTwilightCivil;
    
    SunCalcDateRange nightTwilightAstronomical;
    SunCalcDateRange nightTwilightNautical;
    SunCalcDateRange nightTwilightCivil;
    
} SunCalcDayInfoExtended;

typedef struct {
    
    Poco::LocalDateTime dawn;
    SunCalcDateRange sunrise;
    Poco::LocalDateTime transit;
    SunCalcDateRange sunset;
    Poco::LocalDateTime dusk;
    
    SunCalcDayInfoExtended extended;
    
    double lat;
    double lon;
    
} SunCalcDayInfo;

class ofxSunCalc {
    
    public:
    
        ofxSunCalc();
        
        double dateToJulianDate( const Poco::LocalDateTime & date );
        Poco::LocalDateTime julianDateToDate( double j );
        
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
        
        SunCalcPosition getSunPosition( const Poco::LocalDateTime & date, double lat, double lon );
        SunCalcPosition getSunPosition( double J, double lw, double phi );
        
        SunCalcDayInfo getDayInfo( const Poco::LocalDateTime & date, double lat, double lon, bool detailed = false );
        string infoToString(const SunCalcDayInfo & info, bool min = true);
        
        string static dateToString(const Poco::LocalDateTime & date);
        
        string static dateToDateString(const Poco::LocalDateTime & date);
        
        string static dateToTimeString(const Poco::LocalDateTime & date);
        
        float static getSunBrightness(SunCalcDayInfo & info, const Poco::LocalDateTime time);
        
        void static drawSimpleDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info);
        void static drawExtendedDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info);
    
    private:
        
        double J1970;
        double J2000;
        double deg2rad;
        double M0;
        double M1;
        double J0;
        double J1;
        double J2;
        double C1;
        double C2;
        double C3;
        double P;
        double e;
        double th0;
        double th1;
        double h0;
        double d0;
        double h1;
        double h2;
        double h3;
        
};

#endif /* defined(__ofxSunCalc__) */
