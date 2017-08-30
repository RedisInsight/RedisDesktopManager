// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

/* This executable is used for testing parser/writer using real JSON files.
 */


#include <json/json.h>
#include <algorithm> // sort
#include <stdio.h>

#if defined(_MSC_VER)  &&  _MSC_VER >= 1310
# pragma warning( disable: 4996 )     // disable fopen deprecation warning
#endif

static std::string 
normalizeFloatingPointStr( double value )
{
    char buffer[32];
    sprintf( buffer, "%.16g", value );
    buffer[sizeof(buffer)-1] = 0;
    std::string s( buffer );
    std::string::size_type index = s.find_last_of( "eE" );
    if ( index != std::string::npos )
    {
        std::string::size_type hasSign = (s[index+1] == '+' || s[index+1] == '-') ? 1 : 0;
        std::string::size_type exponentStartIndex = index + 1 + hasSign;
        std::string normalized = s.substr( 0, exponentStartIndex );
        std::string::size_type indexDigit = s.find_first_not_of( '0', exponentStartIndex );
        std::string exponent = "0";
        if ( indexDigit != std::string::npos ) // There is an exponent different from 0
        {
            exponent = s.substr( indexDigit );
        }
        return normalized + exponent;
    }
    return s;
}


static std::string
readInputTestFile( const char *path )
{
   FILE *file = fopen( path, "rb" );
   if ( !file )
      return std::string("");
   fseek( file, 0, SEEK_END );
   long size = ftell( file );
   fseek( file, 0, SEEK_SET );
   std::string text;
   char *buffer = new char[size+1];
   buffer[size] = 0;
   if ( fread( buffer, 1, size, file ) == (unsigned long)size )
      text = buffer;
   fclose( file );
   delete[] buffer;
   return text;
}

static void
printValueTree( FILE *fout, Json::Value &value, const std::string &path = "." )
{
   switch ( value.type() )
   {
   case Json::nullValue:
      fprintf( fout, "%s=null\n", path.c_str() );
      break;
   case Json::intValue:
      fprintf( fout, "%s=%s\n", path.c_str(), Json::valueToString( value.asLargestInt() ).c_str() );
      break;
   case Json::uintValue:
      fprintf( fout, "%s=%s\n", path.c_str(), Json::valueToString( value.asLargestUInt() ).c_str() );
      break;
   case Json::realValue:
       fprintf( fout, "%s=%s\n", path.c_str(), normalizeFloatingPointStr(value.asDouble()).c_str() );
      break;
   case Json::stringValue:
      fprintf( fout, "%s=\"%s\"\n", path.c_str(), value.asString().c_str() );
      break;
   case Json::booleanValue:
      fprintf( fout, "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false" );
      break;
   case Json::arrayValue:
      {
         fprintf( fout, "%s=[]\n", path.c_str() );
         int size = value.size();
         for ( int index =0; index < size; ++index )
         {
            static char buffer[16];
            sprintf( buffer, "[%d]", index );
            printValueTree( fout, value[index], path + buffer );
         }
      }
      break;
   case Json::objectValue:
      {
         fprintf( fout, "%s={}\n", path.c_str() );
         Json::Value::Members members( value.getMemberNames() );
         std::sort( members.begin(), members.end() );
         std::string suffix = *(path.end()-1) == '.' ? "" : ".";
         for ( Json::Value::Members::iterator it = members.begin(); 
               it != members.end(); 
               ++it )
         {
            const std::string &name = *it;
            printValueTree( fout, value[name], path + suffix + name );
         }
      }
      break;
   default:
      break;
   }
}


static int
parseAndSaveValueTree( const std::string &input, 
                       const std::string &actual,
                       const std::string &kind,
                       Json::Value &root,
                       const Json::Features &features,
                       bool parseOnly )
{
   Json::Reader reader( features );
   bool parsingSuccessful = reader.parse( input, root );
   if ( !parsingSuccessful )
   {
      printf( "Failed to parse %s file: \n%s\n", 
              kind.c_str(),
              reader.getFormattedErrorMessages().c_str() );
      return 1;
   }

   if ( !parseOnly )
   {
      FILE *factual = fopen( actual.c_str(), "wt" );
      if ( !factual )
      {
         printf( "Failed to create %s actual file.\n", kind.c_str() );
         return 2;
      }
      printValueTree( factual, root );
      fclose( factual );
   }
   return 0;
}


static int
rewriteValueTree( const std::string &rewritePath, 
                  const Json::Value &root, 
                  std::string &rewrite )
{
   //Json::FastWriter writer;
   //writer.enableYAMLCompatibility();
   Json::StyledWriter writer;
   rewrite = writer.write( root );
   FILE *fout = fopen( rewritePath.c_str(), "wt" );
   if ( !fout )
   {
      printf( "Failed to create rewrite file: %s\n", rewritePath.c_str() );
      return 2;
   }
   fprintf( fout, "%s\n", rewrite.c_str() );
   fclose( fout );
   return 0;
}


static std::string
removeSuffix( const std::string &path, 
              const std::string &extension )
{
   if ( extension.length() >= path.length() )
      return std::string("");
   std::string suffix = path.substr( path.length() - extension.length() );
   if ( suffix != extension )
      return std::string("");
   return path.substr( 0, path.length() - extension.length() );
}


static void
printConfig()
{
   // Print the configuration used to compile JsonCpp
#if defined(JSON_NO_INT64)
   printf( "JSON_NO_INT64=1\n" );
#else
   printf( "JSON_NO_INT64=0\n" );
#endif
}


static int 
printUsage( const char *argv[] )
{
   printf( "Usage: %s [--strict] input-json-file", argv[0] );
   return 3;
}


int
parseCommandLine( int argc, const char *argv[], 
                  Json::Features &features, std::string &path,
                  bool &parseOnly )
{
   parseOnly = false;
   if ( argc < 2 )
   {
      return printUsage( argv );
   }

   int index = 1;
   if ( std::string(argv[1]) == "--json-checker" )
   {
      features = Json::Features::strictMode();
      parseOnly = true;
      ++index;
   }

   if ( std::string(argv[1]) == "--json-config" )
   {
      printConfig();
      return 3;
   }

   if ( index == argc  ||  index + 1 < argc )
   {
      return printUsage( argv );
   }

   path = argv[index];
   return 0;
}


int main( int argc, const char *argv[] )
{
   std::string path;
   Json::Features features;
   bool parseOnly;
   int exitCode = parseCommandLine( argc, argv, features, path, parseOnly );
   if ( exitCode != 0 )
   {
      return exitCode;
   }

   try
   {
      std::string input = readInputTestFile( path.c_str() );
      if ( input.empty() )
      {
         printf( "Failed to read input or empty input: %s\n", path.c_str() );
         return 3;
      }

      std::string basePath = removeSuffix( argv[1], ".json" );
      if ( !parseOnly  &&  basePath.empty() )
      {
         printf( "Bad input path. Path does not end with '.expected':\n%s\n", path.c_str() );
         return 3;
      }

      std::string actualPath = basePath + ".actual";
      std::string rewritePath = basePath + ".rewrite";
      std::string rewriteActualPath = basePath + ".actual-rewrite";

      Json::Value root;
      exitCode = parseAndSaveValueTree( input, actualPath, "input", root, features, parseOnly );
      if ( exitCode == 0  &&  !parseOnly )
      {
         std::string rewrite;
         exitCode = rewriteValueTree( rewritePath, root, rewrite );
         if ( exitCode == 0 )
         {
            Json::Value rewriteRoot;
            exitCode = parseAndSaveValueTree( rewrite, rewriteActualPath, 
               "rewrite", rewriteRoot, features, parseOnly );
         }
      }
   }
   catch ( const std::exception &e )
   {
      printf( "Unhandled exception:\n%s\n", e.what() );
      exitCode = 1;
   }

   return exitCode;
}

