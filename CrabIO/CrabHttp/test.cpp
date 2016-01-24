/*
 * try to read part of file via Http
 *
 * compile:
 *     moc-qt5 "CrabHttpReader.h" > "CrabHttpReader.moc"
 *     clang++ CrabHttpReader.cpp test.cpp -I/usr/include/qt5/ -fPIC -lQt5Core -lQt5Gui -lQt5Network
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>   // std
#include "CrabHttpReader.moc"  // moc-qt5 "CrabHttpReader.h" > "CrabHttpReader.moc"
#include "CrabHttpReader.h"

using namespace std;

int main(int argc, char **argv)
{
    CrabHttpReader httpReader("https://github.com/1054/DeepFields.SuperDeblending/blob/master/data/GOODSN_Photo/n_mips_1_s1_v0.37_sci_BS.fits?raw=true");
    printf("%ld\n",httpReader.ReadFileSize());
    printf("%s\n","ok");
    return 0;
}

