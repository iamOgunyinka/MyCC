#include "lexer.h"
#include <iostream>

using namespace Compiler;

Lexer::Lexer( const std::string & filename ):  file_ptr ( new std::ifstream { filename } ),
                                               stream( * file_ptr ), currChar { }, position { }
{
    stream.is_open();
    initKeywordTable();
    getNextChar();
}
                                               
Lexer::Lexer( std::ifstream &file ): file_ptr ( nullptr ), stream ( file ), currChar { }, position { }
{
    stream.is_open();
    initKeywordTable();
}

Lexer::~Lexer() = default;
std::map< std::string, Symbol > Lexer::keywordTable { };

void Lexer::initKeywordTable( )
{
    keywordTable.insert ( { "double", Symbol { Type::KEYWORDS, "double" }} );
    keywordTable.insert ( { "int", Symbol { Type::KEYWORDS, "int" }} );
    keywordTable.insert ( { "string", Symbol { Type::KEYWORDS, "string" }} );
    
    keywordTable.insert ( { "if", Symbol { Type::KEYWORDS, "if" }} );
    keywordTable.insert ( { "for", Symbol { Type::KEYWORDS, "for" }} );
    keywordTable.insert ( { "do", Symbol { Type::KEYWORDS, "do" }} );
    keywordTable.insert ( { "while", Symbol { Type::KEYWORDS, "while" }} );
    
    
}

Token Lexer::getNextToken( )
{
    Pos old_position;
    for(;; ) {
        old_position = position;
        if( currChar == EOF ){
            return Token::eof( position );
        } else {
            if ( isLetter { }( currChar ) ){
                std::string strBuf { };
                do {
                    strBuf.push_back( currChar );
                    getNextChar();
                } while ( currChar != EOF && isLetterOrDigit { }( currChar ));
                
                std::map< std::string, Symbol >::const_iterator isKeyword = keywordTable.find( strBuf );
                if( isKeyword != keywordTable.cend() ) {
                    return Token::keywords( old_position, isKeyword->second, isKeyword->second.type );
                } else {
                    return Token::validID( old_position, Symbol { std::move( strBuf ), Type::IDENTIFIER },
                                            Type::IDENTIFIER );
                }
            } else if ( isDigit { }( currChar ) ) {
                int value { };
                do {
                    value = value * 10 + ToDigit { }( currChar );
                    getNextChar();
                } while ( isDigit {}( currChar ) );
                return Token::digit( old_position, value );
            } else {
                switch ( currChar ) {
                    case ' ': case '\t':
                        getNextChar();
                        continue;
                    case '\n':
                        updatePos( currChar );
                        getNextChar();
                        continue;
                    case '(':
                        getNextChar();
                        return Token { old_position, Symbol { "(", Type::LPAREN }, Type::LPAREN };
                    case ')':
                        getNextChar();
                        return Token { old_position, Symbol { ")", Type::RPAREN }, Type::RPAREN };
                    case '{':
                        getNextChar();
                        return Token { old_position, Symbol { "{", Type::LBRACE }, Type::LBRACE };
                    case '}':
                        getNextChar();
                        return Token { old_position, Symbol { "}", Type::RBRACE }, Type::RBRACE };
                    case '<':
                        getNextChar();
                        if( currChar == '=' ) {
                            getNextChar( );
                            return Token { old_position, Symbol { "<=", Type::LEQU }, Type::LEQU };
                        }
                        return Token { old_position, Symbol { "<", Type::LESS }, Type::LESS };
                    case '>':
                        getNextChar();
                        if ( currChar == '=' ) {
                            getNextChar();
                            return Token { old_position, Symbol { ">=", Type::GEQU }, Type::GEQU };
                        }
                        return Token { old_position, Symbol { "<", Type::GREAT }, Type::GREAT };
                    case '=':
                        getNextChar();
                        if( currChar == '=' ) {
                            getNextChar();
                            return Token { old_position, Symbol { "==", Type::EQU }, Type::EQU };
                        }
                        return Token { old_position, Symbol { "=", Type::ASSIGN }, Type::ASSIGN };
                    case '*':
                        getNextChar();
                        return Token { old_position, Symbol { "*", Type::MULT }, Type::MULT };
                    case '/':
                        getNextChar();
                        if( currChar == '/' ){ //Single line comment
                            singleLineCommentHandler();
                        } else if ( currChar == '*' ) {
                            doubleLineCommentHandler();
                        }
                        return Token { old_position, Symbol { "/", Type::DIV }, Type::DIV };
                    case '+':
                        getNextChar();
                        return Token { old_position, Symbol { "+", Type::PLUS }, Type::PLUS };
                    case '-':
                        getNextChar();
                        return Token { old_position, Symbol { "-", Type::MINUS }, Type::MINUS };
                    case '!':
                        getNextChar();
                        if( currChar == '=' ){
                            getNextChar();
                            return Token { old_position, Symbol { "!=", Type::NEQU }, Type::NEQU };
                        }
                        return Token { old_position, Symbol { "!", Type::NOT }, Type::NOT };
                    case ';':
                        getNextChar();
                        return Token { old_position, Symbol { ";", Type::SCOLON }, Type::SCOLON };
                    default:
                        return Token { old_position, Symbol { std::string { 1, (char)currChar }, Type::NONE },
                                        Type::NONE };
                }
            }
        }
    }
}

Token Token::eof( const Pos &pos )
{
    return Token { pos, Symbol { "EOF", Type::B_EOF }, Type::B_EOF };
}

inline Token Token::keywords( const Pos &pos, const Symbol &symbol, const Type &t)
{
    return Token { pos, symbol, t };
}

inline Token Token::validID( const Pos &pos, const Symbol &symbol, const Type &t )
{
    return Token { pos, symbol, t };
}

inline Token Token::digit( const Pos &pos, const int &value )
{
    return Token { pos, Symbol { "CONSTANT", Type::CONSTANT }, Type::CONSTANT };
}

inline void Lexer::singleLineCommentHandler()
{
    for( ; ; getNextChar() ){
        if( currChar == '\n' ) break;
    }
    getNextChar();
}

inline void Lexer::doubleLineCommentHandler()
{
    while( currChar != '*' && stream.peek() != '/' ){
        getNextChar();
    }
}
inline void Lexer::getNextChar( )
{
    if ( stream.eof() ){
        currChar = EOF;
        return;
    }
    currChar = stream.get();
    updatePos ( currChar );
    
    //Handle line continuation
    if( currChar == '\\' && stream.peek() == '\n'){
        currChar = stream.get(); //getNextChar == '\n', discard it and get next 
        updatePos( currChar );
        currChar = stream.get(); //now get next char
        updatePos( currChar );
    }
}

void Lexer::updatePos( char c )
{
    if( c == '\n' ){
        ++position.line;
        position.column = 1;
    } else if ( c == ' ' || c == '\t' ) {
        ++position.column;
    } else {
        ++position.column;
    }
}

int main()
{
    Lexer l ( "foo.txt" );
    while ( !l.eof() ){
        Token a = l.getNextToken();
        std::cout << a << std::endl;
    }
    return 0;
}
