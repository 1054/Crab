/*

 a monte carlo program

 we will read a flux.dat file

 and simulate its flux according to its flux error

 following normal distribution

 input 1: *.dat wave flux flux error file
 input 2: number of simulations
 input 3: name of output directory

 Please compile like this:

     clang++ -std=c++11 main.cpp -o CrabPhotMonteCarlo_linux_x86_64
     clang++ -std=c++11 main.cpp -o CrabPhotMonteCarlo_mac

 Initialized:

     2015-10-17

 Last update:

     2015-10-17 initializing
     2015-10-29 fixed bug std:;cout
     2017-05-21 fixed bug cstrInput3; stringstreamSimula.str("");



 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <cmath>
#include <clocale> // under mac
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <libgen.h> // <TODO> additional lib, need to make sure its existence.
#include <sys/types.h> // <TODO> additional lib, need to make sure its existence. // http://linux.die.net/man/3/mkdir
#include <sys/stat.h> // <TODO> additional lib, need to make sure its existence. // http://linux.die.net/man/3/mkdir
#include <random> // <TODO> // see: http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
#include <map>
#include <vector>
#include "CrabTableReadColumn.cpp"

using namespace std;

int main(int argc, char **argv)
{
    /**
     // Note that argv[0] is the path and name of the program itself.
     **/
    char *cstrInput1 = NULL; // input flux data file
    char *cstrInput2 = NULL; // number of simulation to generate
    char *cstrInput3 = NULL; // name of output directory, default value is the input flux file name

    std::string strFileName;
    std::string strBaseName;
    std::string strDirName;

    int debug = 1; // <TODO><DEBUG>

    for(int i=1; i<argc; i++) {
        // read input string
        if(cstrInput1==NULL && i<=argc-1) {
            cstrInput1 = argv[i]; continue;
        }
        if(cstrInput2==NULL && i<=argc-1) {
            cstrInput2 = argv[i]; continue;
        }
        if(cstrInput3==NULL && i<=argc-1) {
            cstrInput3 = argv[i]; continue;
        }
    }
    if(cstrInput1!=NULL && cstrInput2!=NULL)
    {
        int   errStatus = 0;
        long  numSimula = 0; numSimula = atol(cstrInput2); if(debug>0) {std::cout << "# CrabPhotMonteCarlo: We will generate " << numSimula << " simulations!" << std::endl;}
        //
        // extract basename from the input file path
        // method 1:
        // ref: http://bytes.com/topic/c/answers/545168-basename-c
        // cstrInput3 = strrchr(cstrInput1, '/');
        // cstrInput3 = cstrInput3 ? cstrInput3+1 : cstrInput1;
        // method 2:
        // ref: http://bytes.com/topic/c/answers/545168-basename-c
        char *cstrTemp3 = (char *)malloc((strlen(cstrInput1)+1)*sizeof(char)); memset(cstrTemp3,'\0',strlen(cstrInput1)+1); strcpy(cstrTemp3,cstrInput1);
        char *cstrTemp2 = NULL;
        char *cstrTemp1 = (char *)malloc((strlen(cstrInput1)+1)*sizeof(char)); memset(cstrTemp1,'\0',strlen(cstrInput1)+1); strcpy(cstrTemp1,cstrInput1);
        cstrTemp1 = dirname(cstrTemp1); // std::cout << cstrTemp1 << std::endl; // std::cout << strlen(cstrTemp1) << std::endl; // <debug> // dirname will modify the input cstring!
        cstrTemp3 = basename(cstrTemp3); // std::cout << cstrTemp3 << std::endl; // std::cout << strlen(cstrTemp3) << std::endl; // <debug> // basename will modify the input cstring!
        if(cstrTemp1) {
            // if the input file contains a full path, e.g. /home/aaa/flux.dat, then the dir name will be /home/aaa/
            strDirName = std::string(cstrTemp1);
        } else {
            // if the input file is just a file name, then it means the file is under current directory
            strDirName = std::string(".");
        }
        if(cstrTemp3) {
            // search suffix
            cstrTemp2 = strrchr(cstrTemp3,'.');
            // remove suffix
            if(cstrTemp2) {
                // get file base name
                strFileName = std::string(cstrTemp3);
                strBaseName = strFileName.substr(0,strlen(cstrTemp3)-strlen(cstrTemp2));
            } else {
                strFileName = std::string(cstrTemp3);
                strBaseName = strFileName;
            }
        }
        //
        // set default output name to the input file name if user has no input, considering the dir path (cstrTemp1) in the original input file path (cstrInput1)
        if(NULL==cstrInput3) {
            // cstrInput3 = (char *)"";
            if(cstrTemp1) {
                cstrInput3 = (char *)malloc((strlen(cstrTemp1)+1+strlen(cstrTemp3)+1)*sizeof(char));
                memset(cstrInput3,'\0',strlen(cstrTemp1)+1+strlen(cstrTemp3)+1);
                strncpy(cstrInput3,cstrTemp1,strlen(cstrTemp1)); cstrInput3[strlen(cstrTemp1)]='/'; // if the input cstrInput1 contains a full path, then copy this full path to the output dir name.
                if(cstrTemp2) {
                    strncpy(cstrInput3+strlen(cstrTemp1)+1,cstrTemp3,strlen(cstrTemp3)-strlen(cstrTemp2));
                } else {
                    strncpy(cstrInput3+strlen(cstrTemp1)+1,cstrTemp3,strlen(cstrTemp3));
                }
            } else {
                cstrInput3 = (char *)malloc((strlen(cstrTemp3)+1)*sizeof(char));
                memset(cstrInput3,'\0',strlen(cstrTemp3)+1);
                if(cstrTemp2) {
                    strncpy(cstrInput3,cstrTemp3,strlen(cstrTemp3)-strlen(cstrTemp2));
                } else {
                    strncpy(cstrInput3,cstrTemp3,strlen(cstrTemp3));
                }
            }
        }
        //
        // check
        std::cout << "# CrabPhotMonteCarlo: Input file " << cstrInput1 << std::endl;
        std::cout << "# CrabPhotMonteCarlo: Output dir " << cstrInput3 << std::endl;
        //
        // make output directory
        // first check dir exists or not
        // ref: http://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
        struct stat info;
        if(stat(cstrInput3,&info)!=0) {
            // then mkdir // ref: http://linux.die.net/man/3/mkdir
            errStatus = mkdir(cstrInput3,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH); // chmod 755
            if(errStatus!=0) {
                std::cout << "! CrabPhotMonteCarlo: Error! mkdir " << cstrInput3 << " failed!" << std::endl;
                return errStatus;
            }
        }
        //
        // check input file
        if(stat(cstrInput1,&info)!=0) {
            errStatus = -5;
            if(errStatus!=0) {
                std::cout << "! CrabPhotMonteCarlo: Error! input file " << cstrInput1 << " was not found!" << std::endl;
                return errStatus;
            }
        }
        //
        // read input file
        int numWave = 0, numFlux = 0, numFErr = 0;
        double *dblWave = CrabTableReadColumnF(cstrInput1,1,&numWave);
        double *dblFlux = CrabTableReadColumnF(cstrInput1,2,&numFlux);
        double *dblFErr = CrabTableReadColumnF(cstrInput1,3,&numFErr);
        //
        // check the data
        if(numWave<=0) {
            errStatus = -2;
            std::cout << "! CrabPhotMonteCarlo: Error! input file " << cstrInput1 << " does not contain Wave values in its 1st column!" << std::endl;
            return errStatus;
        }
        if(numFlux<=0) {
            errStatus = -2;
            std::cout << "! CrabPhotMonteCarlo: Error! input file " << cstrInput1 << " does not contain Flux values in its 2nd column!" << std::endl;
            return errStatus;
        }
        if(numFErr<=0) {
            errStatus = -2;
            std::cout << "! CrabPhotMonteCarlo: Error! input file " << cstrInput1 << " does not contain Flux Error values in its 3rd column!" << std::endl;
            return errStatus;
        }
        if(numWave!=numFlux||numWave!=numFErr) {
            errStatus = -2;
            std::cout << "! CrabPhotMonteCarlo: Error! input file " << cstrInput1 << " does not contain same rows of Wave and Flux or Flux Error!" << std::endl;
            return errStatus;
        }
        //
        // <debug> output the input data
        if(debug>0) {
            std::cout << "# " << std::endl;
            std::cout << "# " << setw(15-2) << "Wave" << setw(15) << "Flux" << setw(15) << "FErr" << std::endl;
            std::cout << "# " << std::endl;
            for(int i=0; i<numWave; i++) {
                std::cout << setw(15) << dblWave[i] << setw(15) << dblFlux[i] << setw(15) << dblFErr[i] << std::endl << std::flush;
            }
            std::cout << "# " << std::endl;
        }
        //
        // prepare a big data array
        std::vector<double> dblarrWaves; //
        std::vector<double> dblarrFErrs; //
        std::vector<double> dblarrFMeas; //
        std::vector<vector <double>> dblarrFluxes; //
        //
        // loop each Wave
        for(int i=0; i<numWave; i++) {
            // generate random values following normal distribution
            // ref: http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
            std::random_device rd;
            std::mt19937 gen(rd());
            // values near the mean are the most likely
            // standard deviation affects the dispersion of generated values from the mean
            std::normal_distribution<> d(dblFlux[i],dblFErr[i]);
            std::map<double,int> hist;
            std::vector<double> dblarrFlux; // the list of generated flux
            double dblInterval = dblFErr[i]/2.0;
            double dblFractional = dblFlux[i]-double(std::round(dblFlux[i]/dblInterval)*dblInterval);
            long lonInterval = long(double(numSimula)/50.0); lonInterval = (lonInterval==0) ? 1 : lonInterval;
            // std::cout << "# " << std::left << std::setw(10) << dblInterval << std::endl;
            for(int n=0; n<numSimula; ++n) {
                double dblTempVar = d(gen);
                dblarrFlux.push_back(dblTempVar);
                hist[double(std::round(dblTempVar/dblInterval))*dblInterval+dblFractional]++;
            }
            std::cout << "# " << std::endl;
            for(auto p : hist) {
                std::cout << "# " << std::left << std::setw(10) << p.first << " " << std::string(p.second/lonInterval,'*') << std::endl;
            }
            std::cout << "# " << std::endl;
            //
            // now we got a list of generated flux values
            std::cout << "# CrabPhotMonteCarlo: great! Now we have generated " << dblarrFlux.size() << " flux values that follow the " << std::endl <<
                         "# Gaussian distribution with expected mean value " << dblFlux[i] << " and 1-sigma dispersion " << dblFErr[i] << "." << std::endl;
            std::cout << "# " << std::endl;
            //
            // now we store into big data array
            dblarrFluxes.push_back(dblarrFlux);
            dblarrWaves.push_back(dblWave[i]);
            dblarrFErrs.push_back(dblFErr[i]);
            dblarrFMeas.push_back(dblFlux[i]);
        }
        //
        // Now we have finished the loop of each wavelengths,
        // and got a big data array.
        // Then we can start to write into each subdirs.
        if(debug) {
            std::cout << "# CrabPhotMonteCarlo: checking the big data array dblarrFluxes.size() = " << dblarrFluxes.size() << std::endl;
            std::cout << "# CrabPhotMonteCarlo: checking the first sublist of the big data array dblarrFluxes[0].size() = " << dblarrFluxes[0].size() << std::endl;
            std::cout << "# " << std::endl;
        }
        for(int i=0; i<numSimula; i++) {
            std::stringstream stringstreamSimula;
            stringstreamSimula.fill('0');
            stringstreamSimula.width(int(log10(double(numSimula)))+1);
            stringstreamSimula << i+1;
            std::string strSimID = stringstreamSimula.str(); stringstreamSimula.str("");
            std::string strSimDir = std::string(cstrInput3) + "/" + strSimID;
            std::string strSimDat = strSimDir + "/" + strFileName;
            std::cout << "# CrabPhotMonteCarlo: writing to " << strSimDat << std::endl;
            // make subdir
            errStatus = mkdir(strSimDir.c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH); // chmod 755 // <TODO> error check
            // write to flux file in each subdir
            ofstream ofstreamSimDat;
            ofstreamSimDat.open(strSimDat);
            ofstreamSimDat << "# " << setw(20-2) << "w" << setw(20) << "f" << setw(20) << "df" << setw(20) << "f0" << "\n"; // <TODO> keep the same header as input flux file???
            ofstreamSimDat << "# " << setw(20-2) << "um" << setw(20) << "mJy" << setw(20) << "mJy" << setw(20) << "mJy" << "\n"; // <TODO> keep the same header as input flux file???
            for (int w=0; w<numWave; w++) {
                //
                ofstreamSimDat << setw(20) << dblarrWaves[w] << setw(20) << dblarrFluxes[w][i] << setw(20) << dblarrFErrs[w] << setw(20) << dblarrFMeas[w] << "\n";
            }
            ofstreamSimDat << "\n";
            ofstreamSimDat.close();
            // <DEBUG> // break;
        }
        //
        //
    } else {
        // print usage
        std::cout << "Usage: \n    CrabPhotMonteCarlo my_flux.dat 1000 my_output_name" \
                  << "\n    (my_flux.dat should have aligned three columns: wave, flux, and flux error.)" \
                  << "\n" \
                  << std::endl;
    }
    return 0;
}



