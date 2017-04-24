#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pk_stat.h"


#ifdef __cplusplus
extern "C" {


pks_Entry::pks_Entry( )
{
#ifdef _DEBUG2_
   printf("Entry instantiated\n");
#endif
}

pks_Entry::~pks_Entry( )
{
#ifdef _DEBUG2_
   printf("Entry deconstructed\n");
#endif
}

pks_Dataset::pks_Dataset( int id_, char *fname_ )
{
   id = id_;

   memset( fname, '\0', FILENAME_SIZE );
   if( fname_ != NULL ) sprintf( fname, "%s", fname_ );

}

pks_Dataset::~pks_Dataset()
{

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
         printf("Line %d: %s", n, cdata );
         ++n;

         pks_Entry e;
         sscanf( cdata, "%d,%d,%lf,%d",
                &( e.isubject ),
                &( e.icondition ),
                &( e.dt ),
                &( e.item ) );
         e.id = n;

         entries.push_back( e );

      }
   }
   printf("Number of lines with data: %d \n", n );

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




int driver()
{
   int ierr=0;
   pks_Dataset d( 0, NULL );

   sprintf( d.fname, "sample.csv" );
   ierr = d.read();
   ierr = d.write( (char *) "crap.csv" );

int icrap;
scanf("%d", &icrap );

   return(0);
}


}
#endif

