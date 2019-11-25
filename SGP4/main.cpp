/* Satellite Orbit Propagator
 * Adapted from the work of Grady Hillhouse - ISS Tracking Pointer
 * and Vallado's Textbook on the Fundamentals of Astrodynamics and Applications
*/

#include "sgp4ext.h"
#include "sgp4unit.h"
#include "sgp4io.h"
#include "sgp4coord.h"
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

//Function prototypes
vector<string> Parse_TLE();

int main()
{
   
    //SET UP SOME VARIABLES
    double ro[3];
    double vo[3];
    double recef[3];
    double vecef[3];
    char typerun, typeinput, opsmode;
    gravconsttype  whichconst;
    
    double sec, secC, jd, jdC, startmfe, stopmfe, deltamin;
    double tsince;
    double tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2;
    double latlongh[3]; //lat, long in rad, h in km above ellipsoid
    double siteLat, siteLon;
    double siteAlt, siteLatRad, siteLonRad;
    double razel[3];
    double razelrates[3];
    int  year; int mon; int day; int hr; int min;
    int yearC; int monC; int dayC; int hrC; int minC;
    typedef char str3[4];
    str3 monstr[13];
    elsetrec satrec;
    
    float elevation;
    float azimuth = 0.0; //-180 to 0 to 180
    float cAzimuth = 0.0; //From 0 to 359.99

    
    //SET VARIABLES
    opsmode = 'i';
    typerun = 'c';
    typeinput = 'e';
    whichconst = wgs72;
    getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
    strcpy(monstr[1], "Jan");
    strcpy(monstr[2], "Feb");
    strcpy(monstr[3], "Mar");
    strcpy(monstr[4], "Apr");
    strcpy(monstr[5], "May");
    strcpy(monstr[6], "Jun");
    strcpy(monstr[7], "Jul");
    strcpy(monstr[8], "Aug");
    strcpy(monstr[9], "Sep");
    strcpy(monstr[10], "Oct");
    strcpy(monstr[11], "Nov");
    strcpy(monstr[12], "Dec");

    // Retrieve updated TLE's of ISS from TLE.txt
    vector<string> TLE = Parse_TLE();
    
    // char array's should not need more than 80 characters
    char longstr1[80];
    char longstr2[80];
    strcpy(longstr1, TLE[1].c_str());
    strcpy(longstr2, TLE[2].c_str());
    

    //ENTER SITE DETAILS HERE
    siteLat = 30.62;    //+North (College Station)
    siteLon = -96.3487; //+East (College Station)
    siteAlt = 0.12;     //km (College Station)
    siteLatRad = siteLat * pi / 180.0;
    siteLonRad = siteLon * pi / 180.0;
    
    
    //INITIALIZE SATELLITE TRACKING    
    printf("Initializing satellite orbit...\n");
    
    // Called from sgp4io.h
    twoline2rv(longstr1, longstr2, typerun, typeinput, opsmode, whichconst, startmfe, stopmfe, deltamin, satrec );

    printf("twoline2rv function complete...\n");

    //Call propogator to get initial state vector value
    sgp4(whichconst, satrec, 0.0, ro, vo);

    printf("SGP4 at t = 0 to get initial state vector complete...\n"); 
    printf("ro = %f, %f, %f\n", ro[0], ro[1], ro[2]);
    printf("Altitude = %f\n", pow(ro[0]*ro[0]+ro[1]*ro[1]+ro[2]*ro[2],.5));
    jd = satrec.jdsatepoch;    
    
    invjday(jd, year, mon, day, hr, min, sec);

    printf("Scenario Epoch   %3i %3s%5i%3i:%2i:%12.9f \n", day, monstr[mon], year, hr, min, sec);

    jdC = getJulianFromUnix(time(NULL));
    invjday( jdC, yearC, monC, dayC, hrC, minC, secC);
    printf("Current Time    %3i %3s%5i%3i:%2i:%12.9f \n", dayC, monstr[monC], yearC, hrC, minC, secC);
    //printf("            Time Since Epoch (min)            PosX            PosY            PosZ              Vx              Vy              Vz\n");
    printf("            Time             Lat            Long          Height           Range         Azimuth       Elevation\n");
    
    
    //BEGIN SATELLITE TRACKING
    while(1)
    {
        
        //RUN SGP4 AND COORDINATE TRANSFORMATION COMPUTATIONS
        jdC = getJulianFromUnix(time(NULL));
        tsince = (jdC - jd) * 24.0 * 60.0;
        sgp4(whichconst, satrec, tsince, ro, vo);
        teme2ecef(ro, vo, jdC, recef, vecef);
        ijk2ll(recef, latlongh);
        rv2azel(ro, vo, siteLatRad, siteLonRad, siteAlt, jdC, razel, razelrates);
        
        //CHECK FOR ERRORS
        if (satrec.error > 0)
        {
            printf("# *** error: t:= %f *** code = %3d\n", satrec.t, satrec.error);
        }
        else
        {
            azimuth = razel[1]*180/pi;
            if (azimuth < 0) 
            {
                cAzimuth = 360.0 + azimuth;
            }
            else 
            {
                cAzimuth = azimuth;
            }      
            
            elevation = razel[2]*180/pi;
            
            //printf("%28.8f%24.8f%16.8f%16.8f%16.8f%16.8f%16.8f\n", satrec.t, recef[0], recef[1], recef[2], vecef[0], vecef[1], vecef[2]);
            printf("%16.8f%16.8f%16.8f%16.8f%16.8f%16.8f%16.8f\n", satrec.t, latlongh[0]*180/pi, latlongh[1]*180/pi, latlongh[2], razel[0], razel[1]*180/pi, razel[2]*180/pi);
                        
        } 
        
        usleep(1000000);
        
    } //indefinite loop
    
}

vector<string> Parse_TLE()
{
    //file pointer
    fstream f_in;

    // open an existing file
    f_in.open("TLE.txt");

    // read data from the file
    vector<string> lines;
    string temp;

    if (f_in.is_open())
    {
        for (int i = 0; i < 3; i++)
        {
            getline(f_in, temp);
            lines.push_back(temp);
        }
        printf("TLE Line 1: %s\n", lines[1].c_str());
        printf("TLE Line 2: %s\n", lines[2].c_str());
    }
    else
    {
        printf("Unable to open file...\n");
    }

    f_in.close();

    return lines;

}
