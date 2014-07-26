#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <memory>
#include <map>
#include "functions.h"

namespace Compiler
{
    struct Pos
    {   
        Pos(): Pos ( 1, 0 ) { }
        Pos( const int &l, const int &c ): line( l ), column( l ) { }
        ~Pos () = default;

        int line;
        int column;
    };
    
    enum class Type
    {
        IDENTIFIER,
        KEYWORDS,
        CONSTANT,
        
        B_EOF,
        
        INT,
        DOUBLE,
        STRING,
        
        IF, FOR, DO, WHILE, 
        
        ASSIGN, PLUS, MINUS, DIV, MULT, LESS, GREAT, GEQU, LEQU, EQU, NEQU, NOT,
        
        LPAREN, RPAREN, LBRACE, RBRACE, SCOLON,
        NONE
    };
    
    struct Symbol
    {
        Symbol () = default;
        Symbol( std::string && lex, Type _type ): lexeme ( std::move ( lex ) ), type( _type ) { }
        Symbol( Type _type, std::string && lex ): lexeme( std::move( lex ) ), type( _type ) { }
        std::string str () const { return this->lexeme; }

        std::string lexeme { };
        Type type;
    };
    struct Token
    {
        Token() = default;
        Token ( const Pos & pos, const Symbol &symbol, const Type &_type ): position( pos ),
                                                                           sym( symbol ), type( _type )
        {
            
        }
        friend std::ostream &operator<<(std::ostream &os, const Token &tk ){
            return os << "Position -> Line: " << tk.position.line << " column :" << tk.position.column << " "
                << "Symbol -> " << tk.sym.lexeme << " : " << (int) tk.sym.type << " "
                << "Type: " << (int)tk.type;
        }
        
        Pos position;
        Symbol sym;
        Type type;
        static inline Token eof ( const Pos & pos);
        static inline Token keywords( const Pos &pos, const Symbol &symbol, const Type &type );
        static inline Token validID( const Pos &pos, const Symbol &symbol, const Type &type );
        static inline Token digit( const Pos &pos, const int &value );
        
    };
    
    struct Lexer
    {
        Lexer( const std::string & filename );
        explicit Lexer( std::ifstream &file );
        ~Lexer ();
        Token getNextToken ( );
        bool eof () { return stream.eof(); }
        
    //private methods
    private:
        void updatePos( char ch );
        int peek( ) const;
        inline void getNextChar( );
        void singleLineCommentHandler();
        void doubleLineCommentHandler();
        static void initKeywordTable();
    private:
        std::unique_ptr< std::ifstream > file_ptr;
        std::ifstream &stream;
        int currChar;
        static std::map< std::string, Symbol > keywordTable;
        Pos position, old_position;
    };
    
}

#endif
