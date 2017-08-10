//
//  main.c
//  variable-align
//
//  Created by Nat! on 25.09.12.
//  Copyright (c) 2012 Mulle kybernetiK. All rights reserved.
//
//  http://github.com/mulle-nat/mulle-c-code-align
//  http://www.mulle-kybernetik.com
//  http://www.mulle-kybernetik.com/weblog
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST   0
#define SORT   1

#define VERSION   ((1 << 20) | (1 << 8) | 1)


int   iswhite( int c)
{
   return( c > 0 && c != '\n' && c <=  ' ');
}


int   isident( int c)
{
   if( c >= 'A' && c <= 'Z')
      return( 1);
   if( c >= 'a' && c <= 'z')
      return( 1);
   if( c >= '0' && c <= '9')
      return( 1);
   return( c == '_' || c == '$');
}


typedef struct _line
{
   struct _line   *next;

   char   *buf;
   char   *curr;
   char   *sentinel;

   char   *white;
   int    white_len;

   // variety of const char const volatile const fucking const extern
   char   *left;
   int    left_len;

   char   *right;

   char   storage[ 120];
} line;


static line   *alloc_line()
{
   line  *p;

   p = malloc( sizeof( line));
   memset( p, 0, sizeof( *p));

   p->buf      = p->storage;
   p->curr     = p->buf;
   p->sentinel = &p->buf[ sizeof( p->storage)];

   return( p->buf ? p : NULL);
}


static void   grow_line( line *p)
{
   size_t   size;
   size_t   nsize;

   size  = p->sentinel - p->buf;
   nsize = size * 2;

   if( p->buf == p->storage)
   {
      p->buf = malloc( nsize);
      memcpy( p->buf, p->storage, size);
   }
   else
   {
      p->buf = realloc( p->buf, nsize);
   }

   p->curr     = &p->buf[ size];
   p->sentinel = &p->buf[ nsize];
}


static inline void   add_c_to_line( line *p, int c)
{
   if( p->curr == p->sentinel)
      grow_line( p);
   *p->curr++ = c;
}


typedef struct
{
   line   *line;
   char   *s;
   char   *start;
} parse_context;


static char   *reverse_trim_white( char *s, char *start)
{
   for(; s > start;)
   {
      --s;
      if( ! iswhite( *s))
      {
         ++s;
         break;
      }
   }
   return( s);
}


static void   skip_white( parse_context *ctxt)
{
   while( ctxt->s < ctxt->line->sentinel)
   {
      if( ! iswhite( *ctxt->s))
         break;
      ++ctxt->s;
   }
}


static int   grab_white( parse_context *ctxt)
{
   int   n;

   ctxt->start = NULL;

   n = 0;
   while( ctxt->s < ctxt->line->sentinel)
   {
      if( ! iswhite( *ctxt->s))
         break;

      ++n;
      ++ctxt->s;
   }
   return( n);
}


static int   grab_identifier( parse_context *ctxt)
{
   int   n;

   skip_white( ctxt);

   ctxt->start = ctxt->s;
   n           = 0;

   while( ctxt->s < ctxt->line->sentinel)
   {
      if( ! isident( *ctxt->s))
         break;

      ++n;
      ++ctxt->s;
   }
   return( n);
}


static int   is_member_of_charset( char *charset, int c)
{
   while( *charset)
   {
      if( c == *charset)
         return( 1);
      ++charset;
   }
   return( 0);
}


static int   grab_non_charset( parse_context *ctxt, char *charset)
{
   int   n;

   skip_white( ctxt);

   ctxt->start = ctxt->s;
   n            = 0;

   while( ctxt->s < ctxt->line->sentinel)
   {
      if( is_member_of_charset( charset, *ctxt->s))
         break;

      ++n;
      ++ctxt->s;
   }
   return( n);
}


static int   grab_until_char( parse_context *ctxt, int c)
{
   int   n;

   skip_white( ctxt);

   ctxt->start = ctxt->s;
   n           = 0;

   while( ctxt->s < ctxt->line->sentinel)
   {
      if( *ctxt->s == c)
         return( n);

      ++n;
      ++ctxt->s;
   }
   return( 0);
}


// we can only do
// <type modifier><type>   *<name>;
//
static void   parse_variable_line( line *line)
{
   parse_context  ctxt;
   int            len;
   char           *last_identifier[ 2];
   int            last_left_len[ 2];

   memset( last_identifier, 0, sizeof( last_identifier));
   memset( last_left_len, 0, sizeof( last_left_len));

   ctxt.s    = line->buf;
   ctxt.line = line;

   // first grab off leading white and preserve
   line->white     = ctxt.s;
   line->white_len = grab_white( &ctxt);

   line->left     = ctxt.s;
   line->left_len = grab_identifier( &ctxt);

   if( ! line->left_len)
      return;

   for(;;)
   {
      last_identifier[ 1] = last_identifier[ 0];
      last_left_len[ 1]   = last_left_len[ 0];

      last_identifier[ 0] = ctxt.start;
      last_left_len[ 0]   = line->left_len;

      len = grab_identifier( &ctxt);
      if( ! len)
         break;

      line->left_len = (int) (ctxt.s - line->left);
   }

   switch( *ctxt.s)
   {
   case '*'  :
         line->right    = ctxt.s;
         line->left_len = last_left_len[ 0];
         return;

         // last identifier must have been the name
   default   :
         line->right    = last_identifier[ 0];
         line->left_len = last_left_len[ 1];
         return;
   }
}


static void   parse_assignment_line( line *line)
{
   parse_context  ctxt;

   ctxt.s    = line->buf;
   ctxt.line = line;

   // first grab off leading white and preserve
   line->white     = ctxt.s;
   line->white_len = grab_white( &ctxt);

   line->left     = ctxt.s;
   line->left_len = grab_until_char( &ctxt, '=');

   if( ! line->left_len)
      return;

   line->left_len = (int) (reverse_trim_white( &line->left[ line->left_len], line->left) - line->left);

   // ignore ==
   if( *++ctxt.s == '=')
   {
      line->left_len = 0;
      return;
   }

   skip_white( &ctxt);

   line->right = ctxt.s;
}


static int  mode( char *name)
{
   char  *s;

   if( ! name)
      return( 0);

   s = strrchr( name, '/');
   if( ! s)
      s = name;
   else
      s++;

   if( ! strncmp( s, "mulle-", 6))
     s += 6;

   return( *s == 'v');
}


int main( int argc, const char * argv[])
{
   char   *op;
   FILE   *fp;
   int    c;
   int    maxlen;
   line   *head;
   line   *p;
   line   *q;
   void   (*f)( line *p);

   if( mode( (char *)  argv[ 0]))
   {
      op   = "   ";
      f    = parse_variable_line;
   }
   else
   {
      op   = " = ";
      f    = parse_assignment_line;
   }

   maxlen = 0;

   p    = NULL;
   q    = p;
   head = NULL;

   fp = stdin;
#if TEST
   fp = fopen( "/tmp/test.txt", "r");
#endif

   while( (c = getc( fp)) != EOF)
   {
      if( q == p)
      {
         p = alloc_line();
         if( ! q)
            head = p;
         else
            q->next = p;
      }

      add_c_to_line( p, c);

      if( c == '\n')
      {
         add_c_to_line( p, 0);
         q = p;
      }
   }

   if( p)
      add_c_to_line( p, 0);

   // got the lines now
   for( p = head; p; p = p->next)
   {
      (*f)( p);

      if( p->left_len > maxlen)
         maxlen = p->left_len;
   }

   for( p = head; p; p = p->next)
   {
      if( ! p->left_len)
      {
         printf( "%s", p->buf);
         continue;
      }

      // keep indentation of first line
      printf( "%.*s", head->white_len, p->white);

      printf( "%-*.*s", maxlen, p->left_len, p->left);
      printf( "%s", op);
      printf( "%s", p->right);
   }
}

