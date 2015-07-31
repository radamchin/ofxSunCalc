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
    
    public:
    
        ofxSunCalc() {
            J1970 = 2440588;
            J2000 = 2451545;
            deg2rad = PI / 180.0;
            M0 = 357.5291 * deg2rad;
            M1 = 0.98560028 * deg2rad;
            J0 = 0.0009;
            J1 = 0.0053;
            J2 = -0.0069;
            C1 = 1.9148 * deg2rad;
            C2 = 0.0200 * deg2rad;
            C3 = 0.0003 * deg2rad;
            P = 102.9372 * deg2rad;
            e = 23.45 * deg2rad;
            th0 = 280.1600 * deg2rad;
            th1 = 360.9856235 * deg2rad;
            h0 = -0.83 * deg2rad; //sunset angle
            d0 = 0.53 * deg2rad; //sun diameter
            h1 = -6 * deg2rad; //nautical twilight angle
            h2 = -12 * deg2rad; //astronomical twilight angle
            h3 = -18 * deg2rad; //darkness angle
        }
    
        double dateToJulianDate( const Poco::LocalDateTime & date ) {
            return date.julianDay() - 1.0;// -1 is correction for LocalDateTime being the next day // - 0.5 + J1970;
        }
        
        Poco::LocalDateTime julianDateToDate( double j ) {
            Poco::LocalDateTime d(j);// + 0.5 - J1970);
            return d;
        }
        
        int getJulianCycle( double J, double lw ) {
            return round(J - J2000 - J0 - lw/(2 * PI));
        }
        
        double getApproxSolarTransit( double Ht, double lw, double n ) {
            return J2000 + J0 + (Ht + lw)/(2 * PI) + n;
        }
        
        double getSolarMeanAnomaly( double Js ) {
            return M0 + M1 * (Js - J2000);
        }
        
        double getEquationOfCenter( double M ) {
            return C1 * sin(M) + C2 * sin(2 * M) + C3 * sin(3 * M);
        }
        
        double getEclipticLongitude( double M, double C ) {
            return M + P + C + PI;
        }
        
        double getSolarTransit( double Js, double M, double Lsun ) {
            return Js + (J1 * sin(M)) + (J2 * sin(2 * Lsun));
        }
        
        double getSunDeclination( double Lsun ) {
            return asin(sin(Lsun) * sin(e));
        }
        
        double getRightAscension( double Lsun ) {
            return atan2(sin(Lsun) * cos(e), cos(Lsun));
        }
        
        double getSiderealTime( double J, double lw ) {
            return th0 + th1 * (J - J2000) - lw;
        }
        
        double getAzimuth( double th, double a, double phi, double d ) {
            double H = th - a;
            return atan2(sin(H), cos(H) * sin(phi) - tan(d) * cos(phi));
        }
        
        double getAltitude( double th, double a, double phi, double d ) {
            double H = th - a;
            return asin(sin(phi) * sin(d) + cos(phi) * cos(d) * cos(H));
        }
        
        double getHourAngle( double h, double phi, double d ) {
            return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d)));
        }
        
        double getSunsetJulianDate( double w0, double M, double Lsun, double lw, double n ) {
            return getSolarTransit( getApproxSolarTransit(w0, lw, n), M, Lsun );
        }
        
        double getSunriseJulianDate( double Jtransit, double Jset ) {
            return Jtransit - (Jset - Jtransit);
        }
    
        SunCalcPosition getSunPosition( const Poco::LocalDateTime & date, double lat, double lon ) {
            return getSunPosition( dateToJulianDate(date), -lon * deg2rad, lat * deg2rad );
        }
    
        SunCalcPosition getSunPosition( double J, double lw, double phi ) {
            double M = getSolarMeanAnomaly(J);
            double C = getEquationOfCenter(M);
            double Lsun = getEclipticLongitude(M, C);
            double d = getSunDeclination(Lsun);
            double a = getRightAscension(Lsun);
            double th = getSiderealTime(J, lw);
            
            SunCalcPosition pos;
            pos.azimuth = getAzimuth( th, a, phi, d );
            pos.altitude = getAltitude( th, a, phi, d );
            return pos;
        }
    
        SunCalcDayInfo getDayInfo( const Poco::LocalDateTime & date, double lat, double lon, bool detailed = false ) {
            double lw = -lon * deg2rad;
            double phi = lat * deg2rad;
            double J = dateToJulianDate(date);
        
            double n = getJulianCycle(J, lw);
            double Js = getApproxSolarTransit(0, lw, n);
            double M = getSolarMeanAnomaly(Js);
            double C = getEquationOfCenter(M);
            double Lsun = getEclipticLongitude(M, C);
            double d = getSunDeclination(Lsun);
            double Jtransit = getSolarTransit(Js, M, Lsun);
            double w0 = getHourAngle(h0, phi, d);
            double w1 = getHourAngle(h0 + d0, phi, d);
            double Jset = getSunsetJulianDate(w0, M, Lsun, lw, n);
            double Jsetstart = getSunsetJulianDate(w1, M, Lsun, lw, n);
            double Jrise = getSunriseJulianDate(Jtransit, Jset);
            double Jriseend = getSunriseJulianDate(Jtransit, Jsetstart);
            double w2 = getHourAngle(h1, phi, d);
            double Jnau = getSunsetJulianDate(w2, M, Lsun, lw, n);
            double Jciv2 = getSunriseJulianDate(Jtransit, Jnau);
        
            SunCalcDayInfo info;
            
            info.lat = lat;
            info.lon = lon;
            
            info.dawn = julianDateToDate(Jciv2);
            info.sunrise.start = julianDateToDate(Jrise);
            info.sunrise.end = julianDateToDate(Jriseend);
            info.transit = julianDateToDate(Jtransit);
            info.sunset.start = julianDateToDate(Jsetstart);
            info.sunset.end = julianDateToDate(Jset);
            info.dusk = julianDateToDate(Jnau);
        
            if(detailed){
                double w3 = getHourAngle(h2, phi, d);
                double w4 = getHourAngle(h3, phi, d);
                double Jastro = getSunsetJulianDate(w3, M, Lsun, lw, n);
                double Jdark = getSunsetJulianDate(w4, M, Lsun, lw, n);
                double Jnau2 = getSunriseJulianDate(Jtransit, Jastro);
                double Jastro2 = getSunriseJulianDate(Jtransit, Jdark);
                
                info.extended.isSet = true;
                
                info.extended.morningTwilightAstronomical.start = julianDateToDate(Jastro2);
                info.extended.morningTwilightAstronomical.end = julianDateToDate(Jnau2);

                info.extended.morningTwilightNautical.start = julianDateToDate(Jnau2);
                info.extended.morningTwilightNautical.end = julianDateToDate(Jciv2);
                
                info.extended.morningTwilightCivil.start = julianDateToDate(Jciv2);
                info.extended.morningTwilightCivil.end = julianDateToDate(Jrise);
              
                info.extended.nightTwilightCivil.start = julianDateToDate(Jset);
                info.extended.nightTwilightCivil.end = julianDateToDate(Jnau);
                
                info.extended.nightTwilightNautical.start = julianDateToDate(Jnau);
                info.extended.nightTwilightNautical.end = julianDateToDate(Jastro);
               
                info.extended.nightTwilightAstronomical.start = julianDateToDate(Jastro);
                info.extended.nightTwilightAstronomical.end = julianDateToDate(Jdark);
            }
            
            return info;
        }
    
    
        string infoToString(const SunCalcDayInfo & info, bool min = true) {
            ostringstream out;
            
            if(min) { // copy the suncalc.net legend min details widget
                
                out << dateToTimeString(info.dawn) << " - dawn";
                out << endl << dateToTimeString(info.sunrise.start) << " - sunrise";
                out << endl << dateToTimeString(info.transit) << " - solar noon";
                out << endl << dateToTimeString(info.sunset.end) << " - sunset";
                out << endl << dateToTimeString(info.dusk) << " - dusk";
                
            }else{
              /*  out << "dawn = " << dateToString(info.dawn) << endl;
                out << "sunrise: " << dateToString(info.sunrise.start) << " -> " << dateToString(info.sunrise.end) << endl;
                out << "transit = " << dateToString(info.transit) << endl;
                out << "sunset: " << dateToString(info.sunset.start) << " -> " << dateToString(info.sunset.end) << endl;
                out << "dusk = " << dateToString(info.dusk);*/
                
                out << "00:00-" << dateToTimeString(info.extended.morningTwilightAstronomical.start) << " - night" << endl;
                out << dateToTimeString(info.extended.morningTwilightAstronomical.start) << "-" << dateToTimeString(info.extended.morningTwilightAstronomical.end) << " - astronomical twilight" << endl;
                out << dateToTimeString(info.extended.morningTwilightNautical.start) << "-" << dateToTimeString(info.extended.morningTwilightNautical.end) << " - nautical twilight" << endl;
                out << dateToTimeString(info.extended.morningTwilightCivil.start) << "-" << dateToTimeString(info.extended.morningTwilightCivil.end) << " - civil twilight" << endl;
                out << dateToTimeString(info.sunrise.start) << "-" << dateToTimeString(info.sunrise.end) << " - sunrise" << endl;
                out << dateToTimeString(info.sunrise.end) << "-" << dateToTimeString(info.sunset.start) << " - daylight" << endl;
                out << dateToTimeString(info.sunset.start) << "-" << dateToTimeString(info.sunset.end) << " - sunset" << endl;
                out << dateToTimeString(info.extended.nightTwilightCivil.start) << "-" << dateToTimeString(info.extended.nightTwilightCivil.end) << " - civil twilight" << endl;
                out << dateToTimeString(info.extended.nightTwilightNautical.start) << "-" << dateToTimeString(info.extended.nightTwilightNautical.end) << " - nautical twilight" << endl;
                out << dateToTimeString(info.extended.nightTwilightAstronomical.start) << "-" << dateToTimeString(info.extended.nightTwilightAstronomical.end) << " - astronomical twilight" << endl;
                out << dateToTimeString(info.extended.nightTwilightAstronomical.end) << "-00:00 - night" << endl;
                
                out << dateToDateString(info.extended.morningTwilightAstronomical.start) << " - " << dateToDateString(info.extended.nightTwilightAstronomical.end) << " - date range";
            }
            return out.str();
        }
    
        string static dateToString(const Poco::LocalDateTime & date) {
            return Poco::DateTimeFormatter::format(date, "%Y-%m-%d %H:%M:%S");
        }
    
        string static dateToDateString(const Poco::LocalDateTime & date) {
            return Poco::DateTimeFormatter::format(date, "%Y-%m-%d");
        }
    
        string static dateToTimeString(const Poco::LocalDateTime & date) {
            return Poco::DateTimeFormatter::format(date, "%H:%M");
        }
    
        float static getSunBrightness(SunCalcDayInfo & info, const Poco::LocalDateTime time) {
        
            // NOTE: this method not scientific, just a linear approximating hack when sun is setting.
            // TODO: do based on position and full twilights,
            
            int cur_mins = time.hour() * 60 + time.minute();
            
            int rise_start_mins = info.dawn.hour() * 60 + info.dawn.minute();
            int rise_end_mins = info.sunrise.end.hour() * 60 + info.sunrise.end.minute();
            
            int set_start_mins = info.sunset.start.hour() * 60 + info.sunset.start.minute();
            int set_end_mins = info.dusk.hour() * 60 + info.dusk.minute();
            
            // Work out if they are in night or day or a twilight
            
            if(cur_mins >= rise_start_mins && cur_mins <= rise_end_mins) {
                return (cur_mins-rise_start_mins)/(float)(rise_end_mins-rise_start_mins); // sunrising
            }
            
            if(cur_mins >= set_start_mins && cur_mins <= set_end_mins){
                return 1 - (cur_mins-set_start_mins)/(float)(set_end_mins-set_start_mins); // sunsetting
            }
            
            if(cur_mins < rise_start_mins || cur_mins > set_end_mins) {
                return 0.0; // night
            }
            
            return 1.0; // day
        }
  
        void static drawSimpleDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info, ofTrueTypeFont & font) {
            
            float hour_w = target.getWidth() / 24;
            
            float fill_top = 22;
            float fill_h = 10;
            
            ofColor nightCol(127, 142, 180);
            ofColor twilightCol(180, 212, 239);
            ofColor sunriseCol(254, 237, 127);
            ofColor dayCol(254, 215, 127);
            ofColor sunsetCol(254, 215, 127);
            
            target.begin();
            
            ofClear(0,0,0);
            ofSetColor(255);
            ofRect(0,fill_top,target.getWidth(), fill_h);
            
            ofNoFill();
            ofSetLineWidth(1.0);
            
            for(int hr = 0; hr<24; hr++) {
                float cx = (hr*hour_w)+1;
                ofSetColor(0);
                ofLine(cx,0,cx,target.getHeight());
                
                font.drawString(ofToString(hr) + ":00", cx + 4, 15);
                ofSetColor(255);
                font.drawString(ofToString(hr) + ":00", cx + 3, 14);
            }
            
            // Draw the ranges.
            
            float tx = 0;
            float tw;
            float pixels_per_min = hour_w / 60.0;
            
            ofFill();
            
            // Morning Night : 0:00 -> info.dawn;
            int dawn_mins = info.dawn.hour() * 60 + info.dawn.minute();
            tw = pixels_per_min*dawn_mins;
            
            ofSetColor(nightCol);
            ofRect(tx, fill_top, tw, fill_h);
            tx += tw;
            
            // Sunrise (twilight) : info.dawn -> info.sunrise.end;
            tw = (pixels_per_min * (info.sunrise.end.hour() * 60 + info.sunrise.end.minute())) - tx;
            
            ofSetColor(twilightCol);
            ofRect(tx, fill_top, tw, fill_h);
            tx += tw;
            
            // Day : info.sunrise.end -> info.sunset.start
            tw = (pixels_per_min * (info.sunset.start.hour() * 60 + info.sunset.start.minute())) - tx;
            
            ofSetColor(dayCol);
            ofRect(tx, fill_top, tw, fill_h);
            tx += tw;
            
            // Sunset (twilight) : info.sunset.start -> info.dusk
            tw = (pixels_per_min * (info.dusk.hour() * 60 + info.dusk.minute())) - tx;
            
            ofSetColor(twilightCol);
            ofRect(tx, fill_top, tw, fill_h);
            tx += tw;
            
            // Evening night : info.dusk -> 24:00
            tw = target.getWidth() - tx;
            
            ofSetColor(nightCol);
            ofRect(tx, fill_top, tw, fill_h);
            
            target.end();
        }
        
        void static drawExtendedDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info) {
            
            // TODO: draw all extended twilights and details
            
            
        }
    
};

#endif /* defined(__ofxSunCalc__) */
