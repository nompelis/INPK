
#ifndef _PK_STAT_H_
#define _PK_STAT_H_

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <list>

#define FILENAME_SIZE   256
#define LINE_SIZE       256


#ifdef __cplusplus
extern "C" {


//
// Object that is a datapoint in the loaded data
//
class pks_Entry {
 public:
    pks_Entry();
    ~pks_Entry();

    int id;

    int isubject;
    int icondition;
    double dt;
    int item;

};


//
// Object that is a dataset in the study
//
class pks_Dataset {
 public:
    pks_Dataset( int id_, char *fname );
    ~pks_Dataset();

    int id;
    char fname[FILENAME_SIZE];
    char header[LINE_SIZE];
    std::vector< pks_Entry > entries;
    double low_thres,high_thres;

    int read( void );
    int write( char *out_fname ) const;

 private:

};



//
// Object that is a subject in the study
//
class pks_Agent {
 public:
    pks_Agent();
    ~pks_Agent();

    int id;
    std::list< double > idata;

 protected:

 private:

};




}
#endif

#endif
