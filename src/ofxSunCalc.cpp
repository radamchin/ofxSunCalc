
#include "ofxSunCalc.h"

//--------------------------------------------------------------
ofxSunCalc::ofxSunCalc() {
	
}

//--------------------------------------------------------------
double ofxSunCalc::dateToJulianDate( const Poco::DateTime & date ) {
    return date.julianDay(); // - 1.0; // -1 is correction for DateTime being the next day // - 0.5 + J1970;
}

//--------------------------------------------------------------
Poco::DateTime ofxSunCalc::julianDateToDate( double j ) {
    if(!std::isnan(j)){
        return Poco::DateTime(j); // + 0.5 - J1970);
    }else{
        return Poco::DateTime(0,1,1,0,0,0);    // this is error
    }
}

//--------------------------------------------------------------
int ofxSunCalc::getJulianCycle( double J, double lw ) {
    return round(J - J2000 - J0 - lw/(2 * PI));
}

//--------------------------------------------------------------
double ofxSunCalc::getApproxSolarTransit( double Ht, double lw, double n ) {
    return J2000 + J0 + (Ht + lw)/(2 * PI) + n;
}

//--------------------------------------------------------------
double ofxSunCalc::getSolarMeanAnomaly( double Js ) {
    return M0 + M1 * (Js - J2000);
}

//--------------------------------------------------------------
double ofxSunCalc::getEquationOfCenter( double M ) {
    return C1 * sin(M) + C2 * sin(2 * M) + C3 * sin(3 * M);
}

//--------------------------------------------------------------
double ofxSunCalc::getEclipticLongitude( double M, double C ) {
    return M + P + C + PI;
}

//--------------------------------------------------------------
double ofxSunCalc::getSolarTransit( double Js, double M, double Lsun ) {
    return Js + (J1 * sin(M)) + (J2 * sin(2 * Lsun));
}

//--------------------------------------------------------------
double ofxSunCalc::getSunDeclination( double Lsun ) {
    return asin(sin(Lsun) * sin(e));
}

//--------------------------------------------------------------
double ofxSunCalc::getRightAscension( double Lsun ) {
    return atan2(sin(Lsun) * cos(e), cos(Lsun));
}

//--------------------------------------------------------------
double ofxSunCalc::getSiderealTime( double J, double lw ) {
    return th0 + th1 * (J - J2000) - lw;
}

//--------------------------------------------------------------
double ofxSunCalc::getAzimuth( double th, double a, double phi, double d ) {
    double H = th - a;
    return atan2(sin(H), cos(H) * sin(phi) - tan(d) * cos(phi));
}

//--------------------------------------------------------------
double ofxSunCalc::getAltitude( double th, double a, double phi, double d ) {
    double H = th - a;
    return asin(sin(phi) * sin(d) + cos(phi) * cos(d) * cos(H));
}

//--------------------------------------------------------------
double ofxSunCalc::getHourAngle( double h, double phi, double d ) {
    return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d)));
}

//--------------------------------------------------------------
double ofxSunCalc::getSunsetJulianDate( double w0, double M, double Lsun, double lw, double n ) {
    return getSolarTransit( getApproxSolarTransit(w0, lw, n), M, Lsun );
}

//--------------------------------------------------------------
double ofxSunCalc::getSunriseJulianDate( double Jtransit, double Jset ) {
    return Jtransit - (Jset - Jtransit);
}

//--------------------------------------------------------------
SunCalcPosition ofxSunCalc::getSunPosition( const Poco::DateTime & date, double lat, double lon ) {
    return getSunPosition( dateToJulianDate(date), -lon * deg2rad, lat * deg2rad );
}

//--------------------------------------------------------------
SunCalcPosition ofxSunCalc::getSunPosition( double J, double lw, double phi ) {
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

//--------------------------------------------------------------
double ofxSunCalc::rightAscension(double l, double b) { return atan2(sin(l) * cos(e) - tan(b) * sin(e), cos(l)); }
double ofxSunCalc::declination(double l, double b){ return asin(sin(b) * cos(e) + cos(b) * sin(e) * sin(l)); }
double ofxSunCalc::azimuth(double H, double phi, double dec)  { return atan2(sin(H), cos(H) * sin(phi) - tan(dec) * cos(phi)); }
double ofxSunCalc::altitude(double H, double phi, double dec) { return asin(sin(phi) * sin(dec) + cos(phi) * cos(dec) * cos(H)); }
double ofxSunCalc::siderealTime(double d, double lw) { return deg2rad * (280.16 + 360.9856235 * d) - lw; }
double ofxSunCalc::astroRefraction(double h) {
    if (h < 0) // the following formula works for positive altitudes only.
        h = 0; // if h = -0.08901179 a div/0 would occur.
    
    // formula 16.4 of "Astronomical Algorithms" 2nd edition by Jean Meeus (Willmann-Bell, Richmond) 1998.
    // 1.02 / tan(h + 10.26 / (h + 5.10)) h in degrees, result in arc minutes -> converted to rad:
    return 0.0002967 / tan(h + 0.00312536 / (h + 0.08901179));
}

//--------------------------------------------------------------
MoonCalcPosition ofxSunCalc::getMoonPosition( const Poco::DateTime & date, double lat, double lng){
    
    double rad = deg2rad;
    double lw  = rad * -lng;
    double phi = rad * lat;
    double d = dateToJulianDate(date) - J2000;
    
        // function moonCoord(d)
        double L = rad * (218.316 + 13.176396 * d); // ecliptic longitude
        double M = rad * (134.963 + 13.064993 * d); // mean anomaly
        double F = rad * (93.272 + 13.229350 * d);  // mean distance
        
        double l = L + rad * 6.289 * sin(M);    // longitude
        double b = rad * 5.128 * sin(F);        // latitude
        double dt = 385001 - 20905 * cos(M);    // distance to the moon in km
        
        double ra = rightAscension(l, b);
        double dec = declination(l,b);
        double dist = dt;
    
    double H = siderealTime(d, lw) - ra;
    double h = altitude(H, phi, dec);
    
    // formula 14.1 of "Astronomical Algorithms" 2nd edition by Jean Meeus (Willmann-Bell, Richmond) 1998.
    double pa = atan2(sin(H), tan(phi) * cos(dec) - sin(dec) * cos(H));
    h = h + astroRefraction(h); // altitude correction for refraction

    MoonCalcPosition mp;
    mp.azimuth =  azimuth(H, phi, dec);
    mp.altitude =  h;
    mp.distance = dist;
    mp.parallacticAngle = pa;
    return mp;
}

//--------------------------------------------------------------
SunCalcDayInfo ofxSunCalc::getDayInfo( const Poco::DateTime & date, double lat, double lon, bool detailed, int tz_offset ) {
	
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
    
	info.dawn = offsetDate( julianDateToDate(Jciv2), tz_offset );
    info.sunrise.start = offsetDate( julianDateToDate(Jrise), tz_offset );
    info.sunrise.end = offsetDate( julianDateToDate(Jriseend), tz_offset );
    info.transit = offsetDate( julianDateToDate(Jtransit), tz_offset );
    info.sunset.start = offsetDate( julianDateToDate(Jsetstart), tz_offset );
    info.sunset.end = offsetDate( julianDateToDate(Jset), tz_offset );
    info.dusk = offsetDate( julianDateToDate(Jnau), tz_offset );

    if(detailed){
		
        double w3 = getHourAngle(h2, phi, d);
        double w4 = getHourAngle(h3, phi, d);
        double Jastro = getSunsetJulianDate(w3, M, Lsun, lw, n);
        double Jdark = getSunsetJulianDate(w4, M, Lsun, lw, n);
        double Jnau2 = getSunriseJulianDate(Jtransit, Jastro);
        double Jastro2 = getSunriseJulianDate(Jtransit, Jdark);
        
        info.extended.isSet = true;
        
        info.extended.morningTwilightAstronomical.start = offsetDate( julianDateToDate(Jastro2), tz_offset );
        info.extended.morningTwilightAstronomical.end = offsetDate( julianDateToDate(Jnau2), tz_offset );

        info.extended.morningTwilightNautical.start = offsetDate( julianDateToDate(Jnau2), tz_offset );
        info.extended.morningTwilightNautical.end = offsetDate( julianDateToDate(Jciv2), tz_offset );
        
        info.extended.morningTwilightCivil.start = offsetDate( julianDateToDate(Jciv2), tz_offset );
        info.extended.morningTwilightCivil.end = offsetDate( julianDateToDate(Jrise), tz_offset );
      
        info.extended.nightTwilightCivil.start = offsetDate( julianDateToDate(Jset), tz_offset );
        info.extended.nightTwilightCivil.end = offsetDate( julianDateToDate(Jnau), tz_offset );
        
        info.extended.nightTwilightNautical.start = offsetDate( julianDateToDate(Jnau), tz_offset );
        info.extended.nightTwilightNautical.end = offsetDate( julianDateToDate(Jastro), tz_offset );
       
        info.extended.nightTwilightAstronomical.start = offsetDate( julianDateToDate(Jastro), tz_offset );
        info.extended.nightTwilightAstronomical.end = offsetDate( julianDateToDate(Jdark), tz_offset );
		
    }
    
    return info;
}

//--------------------------------------------------------------
string ofxSunCalc::infoToString(const SunCalcDayInfo & info, bool min, int tz_offset) {
    std::ostringstream out;
    
    if(min) { // copy the suncalc.net legend min details widget
        
        out << dateToTimeString(info.dawn, tz_offset) << " - dawn";
        out << endl << dateToTimeString(info.sunrise.start, tz_offset) << " - sunrise";
        out << endl << dateToTimeString(info.transit, tz_offset) << " - solar noon";
        out << endl << dateToTimeString(info.sunset.end, tz_offset) << " - sunset";
        out << endl << dateToTimeString(info.dusk, tz_offset) << " - dusk";
        
    }else{
        
        out << "00:00-" << dateToTimeString(info.extended.morningTwilightAstronomical.start, tz_offset) << " - night" << endl;
        out << dateToTimeString(info.extended.morningTwilightAstronomical.start, tz_offset) << "-" << dateToTimeString(info.extended.morningTwilightAstronomical.end, tz_offset) << " - astronomical twilight" << endl;
        out << dateToTimeString(info.extended.morningTwilightNautical.start, tz_offset) << "-" << dateToTimeString(info.extended.morningTwilightNautical.end, tz_offset) << " - nautical twilight" << endl;
        out << dateToTimeString(info.extended.morningTwilightCivil.start, tz_offset) << "-" << dateToTimeString(info.extended.morningTwilightCivil.end, tz_offset) << " - civil twilight" << endl;
        out << dateToTimeString(info.sunrise.start, tz_offset) << "-" << dateToTimeString(info.sunrise.end, tz_offset) << " - sunrise" << endl;
        out << dateToTimeString(info.sunrise.end, tz_offset) << "-" << dateToTimeString(info.sunset.start) << " - daylight" << endl;
        out << dateToTimeString(info.sunset.start, tz_offset) << "-" << dateToTimeString(info.sunset.end) << " - sunset" << endl;
        out << dateToTimeString(info.extended.nightTwilightCivil.start, tz_offset) << "-" << dateToTimeString(info.extended.nightTwilightCivil.end, tz_offset) << " - civil twilight" << endl;
        out << dateToTimeString(info.extended.nightTwilightNautical.start, tz_offset) << "-" << dateToTimeString(info.extended.nightTwilightNautical.end, tz_offset) << " - nautical twilight" << endl;
        out << dateToTimeString(info.extended.nightTwilightAstronomical.start, tz_offset) << "-" << dateToTimeString(info.extended.nightTwilightAstronomical.end, tz_offset) << " - astronomical twilight" << endl;
        out << dateToTimeString(info.extended.nightTwilightAstronomical.end, tz_offset) << "-00:00:00 - night" << endl;
        
        out << dateToDateString(info.extended.morningTwilightAstronomical.start, tz_offset) << " - " << dateToDateString(info.extended.nightTwilightAstronomical.end, tz_offset) << " - date range";
    }
    return out.str();
}

//--------------------------------------------------------------
string ofxSunCalc::dateToString(const Poco::DateTime & date, int tz_offset) {
    return formatDate(date, "%Y-%m-%d %H:%M:%S", tz_offset);
}

//--------------------------------------------------------------
string ofxSunCalc::dateToDateString(const Poco::DateTime & date, int tz_offset) {
    return formatDate(date, "%Y-%m-%d", tz_offset);
}

//--------------------------------------------------------------
string ofxSunCalc::dateToTimeString(const Poco::DateTime & date, int tz_offset) {
    return formatDate(date, "%H:%M:%S", tz_offset);
}

//--------------------------------------------------------------
string ofxSunCalc::formatDate(const Poco::DateTime & date, const string pattern, int tz_offset) {
	if(date.year() == 0){
		return "n.a.";
	}else{
		return Poco::DateTimeFormatter::format( offsetDate(date, tz_offset), pattern );
	}
}

//--------------------------------------------------------------
Poco::DateTime ofxSunCalc::offsetDate(const Poco::DateTime & date, int tz_offset) {
	Poco::DateTime dt(date); // copy it
	dt.makeLocal(tz_offset);
	return dt;
}

//--------------------------------------------------------------
float ofxSunCalc::getSunBrightness(SunCalcDayInfo & info, const Poco::DateTime time, int tz_offset) {

    // NOTE: this method not scientific, just a linear approximating hack when sun is setting.
    // TODO: do based on position and full twilights,
    
	Poco::DateTime t = offsetDate(time, tz_offset);
	
    int cur_mins = t.hour() * 60 + t.minute();
    
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

//--------------------------------------------------------------
void ofxSunCalc::drawSimpleDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info) {
    
    ofPushStyle();
    ofPushMatrix();
    float hour_w = target.getWidth() / 24;
    
    float fill_top = 22;
    float fill_h = 10;
    
    ofColor nightCol(127, 142, 180);
    ofColor twilightCol(180, 212, 239);
    ofColor sunriseCol(254, 237, 127);
    ofColor dayCol(254, 215, 127);
    ofColor sunsetCol(254, 215, 127);
    
    
    static ofTrueTypeFont ofx_suncalc_font;
    if(!ofx_suncalc_font.isLoaded()) {
        ofx_suncalc_font.load(OF_TTF_MONO, 8, false);
    }
    
    target.begin();
    
   // ofEnableAlphaBlending();
    
    ofClear(0,0,0);
    ofSetColor(255);
    ofDrawRectangle(0,fill_top,target.getWidth(), fill_h);
    
    ofNoFill();
    ofSetLineWidth(1.0);
    
    for(int hr = 0; hr<24; hr++) {
        float cx = (hr*hour_w)+1;
        
        ofSetColor(255);
        ofDrawLine(cx,0,cx,target.getHeight()); // vertical hour marker
        
        // shadowed time label
        ofSetColor(0);
        ofx_suncalc_font.drawString(ofToString(hr) + ":00", cx + 4, 15);
        ofSetColor(255);
        ofx_suncalc_font.drawString(ofToString(hr) + ":00", cx + 3, 14);
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
    ofDrawRectangle(tx, fill_top, tw, fill_h);
    tx += tw;
    
    // Sunrise (twilight) : info.dawn -> info.sunrise.end;
	tw = (pixels_per_min * (info.sunrise.end.hour() * 60 + info.sunrise.end.minute())) - tx;
    
    ofSetColor(twilightCol);
    ofDrawRectangle(tx, fill_top, tw, fill_h);
    tx += tw;
    
    // Day : info.sunrise.end -> info.sunset.start
    tw = (pixels_per_min * (info.sunset.start.hour() * 60 + info.sunset.start.minute())) - tx;
    
    ofSetColor(dayCol);
    ofDrawRectangle(tx, fill_top, tw, fill_h);
    tx += tw;
    
    // Sunset (twilight) : info.sunset.start -> info.dusk
    tw = (pixels_per_min * (info.dusk.hour() * 60 + info.dusk.minute())) - tx;
    
    ofSetColor(twilightCol);
    ofDrawRectangle(tx, fill_top, tw, fill_h);
    tx += tw;
    
    // Evening night : info.dusk -> 24:00
    tw = target.getWidth() - tx;
    
    ofSetColor(nightCol);
    ofDrawRectangle(tx, fill_top, tw, fill_h);
    
    target.end();
    
   // ofDisableAlphaBlending();
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofxSunCalc::drawExtendedDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info) {
	
    // TODO: draw all extended twilights and details
    
    
}

//--------------------------------------------------------------
