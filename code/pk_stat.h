
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

   int istate;
};

// a forward declaration so that the agent object can have a parent
class pks_Dataset;

//
// Object that is a subject in the study
//
class pks_Agent {
 public:
   pks_Agent( pks_Dataset *dset_ );
   virtual ~pks_Agent();

   int id;
   pks_Dataset *dset;
   std::vector< pks_Entry * > entries;
   std::vector< double > mu,sigma;
   std::vector< int > icnt;

   void calcStats();
   void showStats() const;

 protected:

 private:

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
   int ncond;
   double low_thres,high_thres;
   int nlow,nhigh;
   std::vector< pks_Agent > agents;

   int read( void );
   int write( char *out_fname ) const;
   int resetEntries( void );
   int filterEntries( void );
   int makeAgents( void );

 private:

};





}
#endif

#endif
