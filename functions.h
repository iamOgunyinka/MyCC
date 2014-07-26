#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <functional>
#include <string>

namespace Compiler
{
    namespace Functions{
            
        struct IsDigit
        {
            bool operator ()( const std::string & str ){
                int foo { };
                try {
                    foo = std::stoi( str );
                } catch ( ... ){
                    return false;
                }
                if ( std::to_string( foo ) == str ){
                    return true;
                }
                return false;
            }
            bool operator() ( const int &c ) {
                return ( c >= 48 && c <= 57 ) || ( c == 46 );
            }
        };
        struct IsDecimal
        {
            bool operator ()( const int &c ){
                return c == '.';
            }
        };
        struct IsLetter
        {
            bool operator ()( int c ){
                return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || ( c == 95 );
            }
        };
        
        struct IsLetterOrDigit
        {
            bool operator() ( const int &c ) {
                return IsDigit {}( c ) || IsLetter { }( c );
            }
        };
        
        struct IsWhiteSpace
        {
            bool operator ()( char c ) {
                return c == ' ' || c == '\t';
            }
        };
        struct ToDigit
        {
            int operator () ( int c ) {
                return c - 48;
            }
            int operator ()( const std::string & str ){
                return IsDigit { }( str ) ? std::stoi( str ): 0;
            }
        };
    }
}
#endif
