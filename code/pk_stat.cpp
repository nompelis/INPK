#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pk_stat.h"


#ifdef __cplusplus
extern "C" {


pks_Entry::pks_Entry( )
{
#ifdef _DEBUG2_
   printf("Entry instantiated\n");
#endif

   istate = 0;
}

pks_Entry::~pks_Entry( )
{
#ifdef _DEBUG2_
   printf("Entry deconstructed\n");
#endif
}

pks_Agent::pks_Agent( pks_Dataset *dset_ )
{
#ifdef _DEBUG2_
   printf("Agent instantiated \n");
#endif
   dset = dset_;
}

pks_Agent::~pks_Agent( )
{
   entries.clear();
   mu.clear();
   sigma.clear();
   icnt.clear();

#ifdef _DEBUG2_
   printf("Agent deconstructed \n");
#endif
}

void pks_Agent::calcStats( )
{
   int ncond = icnt.size();

   // clean-up agent stats
   for(int k=0;k<ncond;++k) {
      mu[k] = 0.0;
      sigma[k] = 0.0;
      icnt[k] = 0;
   }

   int ne = entries.size();
   // loop over all entries and calculate the mean for all conditions
   for(int n=0;n<ne;++n) {
      int ic = entries[n]->icondition - 1;
      if( entries[n]->istate == 0 ) {
         mu[ic] += entries[n]->dt;
         icnt[ic] += 1;
      }
   }
   for(int k=0;k<ncond;++k) {
      mu[k] = mu[k] / ((double) icnt[k]);
   }
   // loop over all entries and calculate the variance for all conditions
   for(int n=0;n<ne;++n) {
      int ic = entries[n]->icondition - 1;
      if( entries[n]->istate == 0 ) {
         double t = entries[n]->dt - mu[ic];
         sigma[ic] += t*t;
      }
   }
   // calculate the standard deviation for all conditions
   for(int k=0;k<ncond;++k) {
      sigma[k] = sqrt( sigma[k] / ((double) icnt[k]) );
   }
}

void pks_Agent::showStats( ) const
{
   int ncond = icnt.size();

   for(int k=0;k<ncond;++k) {
      printf(" Condition %2d, (cnt %3d), mean %10.2lf, std.dev. %10.2lf \n", 
             k, icnt[k], mu[k], sigma[k] );
   }
}


pks_Dataset::pks_Dataset( int id_, char *fname_ )
{
#ifdef _DEBUG_
   printf("Dataset instantiated\n");
#endif
   id = id_;

   memset( fname, '\0', FILENAME_SIZE );
   if( fname_ != NULL ) sprintf( fname, "%s", fname_ );

   low_thres = -1.0;
   high_thres = -1.0;
   ncond = 0;
   nlow = 0;
   nhigh = 0;
}

pks_Dataset::~pks_Dataset()
{
   agents.clear();
   entries.clear();
#ifdef _DEBUG_
   printf("Dataset deconstructed\n");
#endif
}

int pks_Dataset::read( void )
{
   if( fname[0] == '\0' ) {
      printf("Filename is empty \n");
      return(1);
   }

   FILE *fp;
   fp = fopen( fname, "r" );
   if( fp == NULL ) {
      printf("Could not open filename for reading \n");
      return(2);
   }

   fgets( header, LINE_SIZE, fp );
   int n=0;
   while( n < LINE_SIZE && header[n] != '\n' ) {
      ++n;
   }
   if( n != LINE_SIZE ) header[n] = '\0';
   printf("Header: \"%s\"\n", header );

   char cdata[LINE_SIZE];
   n = 0;
   while( !feof( fp ) ) {
      memset( cdata, '\0', LINE_SIZE );
      fgets( cdata, LINE_SIZE, fp );

      if( cdata[0] != '\0' )  {
#ifdef _DEBUG2_
         printf("Line %d: %s", n, cdata );
#endif
         ++n;

         pks_Entry e;
         sscanf( cdata, "%d,%d,%lf,%d",
                &( e.isubject ),
                &( e.icondition ),
                &( e.dt ),
                &( e.item ) );
         e.id = n;

         entries.push_back( e );

         if( e.icondition > ncond ) ncond = e.icondition;
      }
   }
   printf("Number of lines with data: %d \n", n );
   printf("Number of conditions in data: %d \n", ncond );

   fclose( fp );

   return(0);
}


int pks_Dataset::write( char *fname_out ) const
{
   if( fname[0] == '\0' ) {
      printf("Filename is empty \n");
      return(1);
   }

   FILE *fp;
   fp = fopen( fname_out, "w" );
   if( fp == NULL ) {
      printf("Could not open filename for writing \n");
      return(2);
   }

   fprintf( fp, "%s\n", header );

   int ns = entries.size();
   for(int n=0;n<ns;++n) {
      fprintf( fp, "%d, %d, %d, %d\n",
               entries[n].isubject,
               entries[n].icondition,
               (int) (entries[n].dt),
               entries[n].item );
   }

   fclose( fp );

   return(0);
}

int pks_Dataset::resetEntries( void )
{
   int ne = entries.size();

   for(int n=0;n<ne;++n) entries[n].istate = 0;
   nlow=0;
   nhigh=0;

   int na = agents.size();
   for(int n=0;n<na;++n) agents[n].calcStats();

   return(0);
}


int pks_Dataset::filterEntries( void )
{
   if( low_thres <= 0.0 || high_thres <= 0.0 ) {
      printf("Thresholds are non-sensical: low %lf  high %lf. Operation failed\n", low_thres, high_thres);
      return(1);
   }

   nlow=0;
   nhigh=0;
   int ne = entries.size();
   for(int n=0;n<ne;++n) {

      if( entries[n].dt < low_thres ) {
         entries[n].istate = -1;
         nlow += 1;
      }

      if( entries[n].dt > high_thres ) {
         entries[n].istate = +1;
         nhigh += 1;
      }
   }

   printf("After threshold-filtering %d entries...\n", ne );
   printf("   %d were found below the low threshold \n", nlow );
   printf("   %d were found above the high threshold \n", nhigh );

   printf("Calculating agent/subject stats after data filtering \n");
   int na = agents.size();
   for(int n=0;n<na;++n) {
      agents[n].calcStats();
      agents[n].showStats();
   }

   return(0);
}

int pks_Dataset::makeAgents( void )
{
   if( entries.size() == 0 ) {
      printf("Dataset has no data from which to infer agents/subjects\n");
      return(1);
   }

   if( agents.size() != 0 ) {
      printf("Dataset already agents/subjects; will re-create them.\n");

      printf("Agents vetor size before clear: %ld \n", agents.size() );
      agents.clear();
   }

   printf("Creating agents\n");

   int ne = entries.size();
   for(int n=0;n<ne;++n) {
      int isubject = entries[n].isubject;
      int ifound = -1;

      int na = agents.size();
      int m=0;
      while( m < na ) {
         if( agents[m].id == isubject ) {
            ifound = m;
            m = na;    // terminate the loop over existing agents
         }
         ++m;
      }

      if( ifound == -1 ) {
         pks_Agent a( this );
         a.id = isubject;
         agents.push_back( a );
         agents[ agents.size() - 1 ].entries.push_back( &(entries[n]) );
      } else {
         agents[ ifound ].entries.push_back( &(entries[n]) );
      }
   }

   // create the stats arrays for the agents
   int na = agents.size();
   for(int n=0;n<na;++n) {
      for(int m=0;m<ncond;++m) {
         double dum = 0.0;
         agents[n].mu.push_back( dum );
         agents[n].sigma.push_back( dum );
         int idum = 0;
         agents[n].icnt.push_back( idum );
      }
      agents[n].calcStats();
      agents[n].showStats();
   }

#ifdef _DEBUG_
   for(int n=0;n<na;++n) {
      printf(" Agent %d, subject_no %d, entries %ld \n",
             n, agents[n].id, agents[n].entries.size() );
#ifdef _DEBUG2_
   // IN THE EVENT THAT WE USE A LIST (TO BE REMOVED)
   // pks_Entry :: iterator ei;
   // for( ei = agents[n].entries.begin();
   //      ei != agents[n].entries.end(); ++ei ) {
   //     printf("   Entry pointer %p, condition %d, dt= %lf \n",
   //                ei, (*ei).icondition, (ei).dt );
      for(int i=0;i<agents[n].entries.size();++i) {
          printf("   Entry pointer %p, condition %d, dt= %lf \n",
                     agents[n].entries[i],
                     agents[n].entries[i]->icondition,
                     agents[n].entries[i]->dt );
      }
#endif
   }
#endif

   return(0);
}

//---------------- driver related functions ------------------

//
// Function for the user to enter thresholds
//
int enter_thresholds( pks_Dataset & d )
{
   double lowt,hight;
   int idone = 1;

   while( idone != 0 ) {
      printf("Enter thresholds (low and high) separated by a space:\n");
      scanf("%lf %lf", &lowt, &hight );
//lowt =1000;hight=7000; //HACK
      printf("You have entered: low %lf high %lf \n", lowt,hight );

      if( lowt > 0.0 && hight > lowt ) {
         idone = 0;
      } else {
         printf("The thresholds you provided do not make sense. Retry.\n");
      }
   }

   d.low_thres = lowt;
   d.high_thres = hight;

   return(0);
}



int driver()
{
   int ierr=0;

   // create a dataset object
   pks_Dataset d( 0, NULL );

   // assign a filename and read (and test-write) it
   sprintf( d.fname, "sample.csv" );
   ierr = d.read();
   ierr = d.write( (char *) "crap.csv" );

   // create the agent/subject objects for this dataset
   ierr = d.makeAgents();

   // ask user for thresholds
   enter_thresholds( d );

   // fitler based on thresholds
   ierr = d.filterEntries();

   return(0);
}


}
#endif

