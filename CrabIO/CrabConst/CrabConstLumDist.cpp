/* 
 calculate lumdist
 same as the IDL code lumdist
 
 originally in SpireLinesListOfSample
 void SpireLinesListOfSample::calcLuminDistances()
 {
     // Read redshift values from a common SUPPLEMENTARY REDSHIFT data file e.g. "_NEDRedshift.CSV"
     // Then calculate the luminosity distance with cosmic parameters given below.
     // The calculation follows    http://www.astro.ucla.edu/~wright/CosmoCalc.html
     ...
 
 
 double SpireLinesListOfSample::lumdist(double redshift)
 {
     ...
 
 
 Last update: 
     2014-10-29 dzliu
 
 
 */



#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setw
#include <sstream>
#include <fstream>      // std::ifstream, std::ofstream

using namespace std;

double lumdist(double redshift, double HubbleConstant, double OmegaMatter, double OmegaLambda, int verbose = 0);

double lumdist(double redshift, double HubbleConstant, double OmegaMatter, double OmegaLambda, int verbose)
{
    /* Calculate luminosity distance according to redshift value */
    if(redshift<0.0)
        return 0.0;
    // double HubbleConstant = 71.0;  // h kms-1 Mpc-1
    // double double OmegaMatter is the matter density // http://www.astro.cornell.edu/academics/courses/astro201/cosmoparms.htm
    // double double OmegaLambda is the dark energy density // http://www.astro.cornell.edu/academics/courses/astro201/cosmoparms.htm
    // double double OmegaMatter is the matter density // http://www.astro.cornell.edu/academics/courses/astro201/cosmoparms.htm
    double SpeedOfLight = 2.99792458e5;  // kms-1
    double HubbleFlowDistance = SpeedOfLight/HubbleConstant;  // h-1 Mpc
    double ComovingDistance = 0.0;
    double LuminosityDistance = 0.0;
    double CosmicScaleFactor_1 = 1.0/(1.0+redshift); // in unit of R(0)(@current)
    //double CosmicScaleFactor_0 = 1.0;              // in unit of R(0)(@current)
    double CosmicAge_1 = 0.0;    // 1/H0
    double CosmicAge_0 = 0.0;    // 1/H0 current age of universe should be 1.0
    double LookBackTime = 0.0;   //1/H0
    double CosmicScaleFactor_t = 0.0; // at cosmic time t
    double CosmicScaleFactor_z = 0.0; // at cosmic redshift z
    // Integrate from CosmicScaleFactor_1->CosmicScaleFactor_0:
    // Considering: Wcurvaturve = 0 (k=0)
    //              Wradiation = 4.165E-5/(h*h) // includes 3 massless neutrino species, T0 = 2.72528
    //              Wmatter = 0.27
    //              Wvaccum = 0.73
    if(verbose>0) {
        std::cout << "lumdist" << " z=" << redshift << " H0=" << HubbleConstant << " Omega_M=" << OmegaMatter << " Lambda0=" << OmegaLambda << " Curvaturve_k=0 " << "Radiation=4.165E-5/(h*h)" << std::endl;
    }
    int coloop = 1000;
    double dot_CosmicScaleFactor_t=0.0;
    double dot_CosmicScaleFactor_z=0.0;
    for(int costep=0; costep<coloop; costep++) {
        // from t=0->t=t1(@z1) integrate time of universe @z.
        CosmicScaleFactor_t = CosmicScaleFactor_1*(double(costep)+0.5)/double(coloop);
        dot_CosmicScaleFactor_t = sqrt( 0.0 \
                                       + 0.4165/HubbleConstant/HubbleConstant/CosmicScaleFactor_t/CosmicScaleFactor_t \
                                       + OmegaMatter/CosmicScaleFactor_t \
                                       + OmegaLambda*CosmicScaleFactor_t*CosmicScaleFactor_t );
        CosmicAge_1 = CosmicAge_1 + 1.0/dot_CosmicScaleFactor_t;
        // from z=z1->z=0 integrate lookbacktime and radial comoving distance.
        // which is R=R(1)->R=R(0)=1
        CosmicScaleFactor_z = CosmicScaleFactor_1 + \
        (1.0-CosmicScaleFactor_1)*(double(costep)+0.5)/double(coloop);
        dot_CosmicScaleFactor_z = sqrt( 0.0 \
                                       + 0.4165/HubbleConstant/HubbleConstant/CosmicScaleFactor_z/CosmicScaleFactor_z \
                                       + OmegaMatter/CosmicScaleFactor_z \
                                       + OmegaLambda*CosmicScaleFactor_z*CosmicScaleFactor_z );
        LookBackTime = LookBackTime + 1.0/dot_CosmicScaleFactor_z;
        ComovingDistance = ComovingDistance + 1.0/(CosmicScaleFactor_z*dot_CosmicScaleFactor_z);
    }
    CosmicAge_1 = CosmicScaleFactor_1*CosmicAge_1/coloop;
    LookBackTime = (1.0-CosmicScaleFactor_1)*LookBackTime/coloop;
    CosmicAge_0 = CosmicAge_1 + LookBackTime;
    ComovingDistance = (1.0-CosmicScaleFactor_1)*ComovingDistance/coloop;
    LuminosityDistance = ComovingDistance*(1.0+redshift);
    LuminosityDistance = LuminosityDistance*HubbleFlowDistance;
    if(verbose>0) {
        double CosmicAge_Now = log((1.0+sqrt(OmegaLambda))/(sqrt(OmegaLambda)))/sqrt(OmegaMatter)*6.52*100/HubbleConstant; // http://ned.ipac.caltech.edu/level5/March03/Lineweaver/Lineweaver7_8.html <Note> this website is wrong!!! see instead: http://cds.cern.ch/record/519900/files/0109232.ps.gz?version=1
        std::cout << "lumdist" << " d_L=" << LuminosityDistance << " Mpc, d_c=" << LuminosityDistance/(1.0+redshift) << " Mpc, d_A=" << LuminosityDistance/(1.0+redshift)/(1.0+redshift) << " Mpc" << std::endl;
        std::cout << "lumdist" << " t_age=" << CosmicAge_1*CosmicAge_Now << " Gyr, t_lookback=" << LookBackTime*CosmicAge_Now << " Gyr, t_now=" << CosmicAge_0*CosmicAge_Now << " Gyr" << std::endl;
    }
    return LuminosityDistance;
}
