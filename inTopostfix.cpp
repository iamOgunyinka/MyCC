#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stack>

//You can find the grammar here -> http://pastebin.com/7eCaktLx

//as much as I hate having globals, here have it!
std::string printer { };

struct isDigit
{
    bool operator ()( const std::string & str ){
        int foo { };
        try {
            foo = std::stoi( str );
        } catch ( std::invalid_argument && ){
            return false;
        }
        if ( std::to_string( foo ) == str ){
            return true;
        }
        return false;
    }
    bool operator() ( const char &c ) {
        return c >= 48 && c <= 57;
    }
};
//for the lexer class
struct isAlphabet
{
    bool operator ()( char c ){
        return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) ;
    }
};

struct isWhiteSpace
{
    bool operator ()( char c ) {
        return c == ' ' || c == '\t';
    }
};
struct ToDigit
{
    int operator () ( char c ) {
        return (int)(c) - 48;
    }
};
struct fail: virtual std::length_error {
    fail( std::string && msg ): std::length_error { std::move( msg ) } { }
};

int changeToDigit( const std::string & str )
{
    return std::stoi( str );
}
double applyOpToOperands(double top, double nextTop, char op)
{
    switch ( op ) {
        case '*': return (top * nextTop);
        case '/': return 0 == top ? 0 : ( nextTop / top ); //avoid division by 0
        case '-': return (nextTop - top );
        case '+': default: return (top + nextTop);
    }
}

static double evaluate(std::string &str )
{
    double result { };
    std::string symbol { };
    char opSymbol {};
    std::stack<double> stack_digits { };
    std::istringstream ss( std::move( str ) );
    while ( ss >> symbol ) {
        if(isDigit { }( symbol )) {
            stack_digits.push( changeToDigit(symbol) );
        } else {
            opSymbol = symbol[0];
            double top { }, nextTop { };
            top = stack_digits.top(); stack_digits.pop();
            if( !stack_digits.empty() ) {
                nextTop = stack_digits.top();
                stack_digits.pop();
            }
            result = applyOpToOperands(top, nextTop, opSymbol);
            stack_digits.push( result );
        }
    }
    result = stack_digits.top();
    stack_digits.pop();
    return result;
}

void expr( std::istringstream &ss, char &lookahead );
void rest( std::istringstream &ss, char &lookahead );
void term( std::istringstream &ss, char &lookahead );
void rest_2( std::istringstream &ss, char &lookahead );
void factor( std::istringstream &ss, char &lookahead );
void match( std::istringstream &ss, char &lookahead, const char &currToken ) noexcept ;
void print( const int & value );
void print (const char & c );

void print( const int &value ){
    std::string foo = std::to_string( value );
    printer += ( foo + " " );
}

void print( const char & c ) {
    printer.push_back( c );
    printer.push_back( ' ' );
}

void match ( std::istringstream &ss, char &lookahead, const char &currToken ) noexcept
{
    if ( lookahead == currToken ){
        lookahead = ss.get();
        if( isWhiteSpace {}( lookahead ) ){
            for(;; lookahead = ss.get() ){
                if(isWhiteSpace {}( lookahead ) ) continue;
                else break;
            }
        }
    }
    //otherwise we have an epsilon production
}

void expr( std::istringstream &ss, char &lookahead )
{
    term( ss, lookahead );
    rest( ss, lookahead );
}

void rest( std::istringstream &ss, char &lookahead )
{
    while( !ss.eof() ) {
        char tempToken = lookahead;
        if ( lookahead == '+' ) {
            match( ss, lookahead, '+' );
            term ( ss, lookahead );
            print( tempToken );
            rest( ss, lookahead );
        } else if ( lookahead == '-' ) {
            match( ss, lookahead, '-' );
            term( ss, lookahead );
            print( tempToken );
            rest( ss, lookahead );
        } else if ( isWhiteSpace {} ( lookahead ) ) {
            for(;; lookahead = ss.get() ){
                if( isWhiteSpace {}( lookahead ) ) continue;
                else break;
            }
        } else {
            break; //we have an epsilon production
        }
    }
}

void term( std::istringstream &ss, char &lookahead )
{
    factor( ss, lookahead );
    rest_2( ss, lookahead );
}

void rest_2( std::istringstream &ss, char &lookahead )
{
    while( !ss.eof() ){
        char tempToken = lookahead;
        if( lookahead == '*' ) {
            match( ss, lookahead, '*' );
            factor( ss, lookahead );
            print( tempToken );
        } else if ( lookahead == '/' ) {
            match ( ss, lookahead, '/' );
            factor( ss, lookahead );
            print( '/' );
        } else if ( isWhiteSpace {}( lookahead ) ) {
            for(;; lookahead = ss.get() ){
                if( isWhiteSpace {}( lookahead ) ) continue;
                else break;
            }
        } else {
            break;
        }
    }
}

void factor( std::istringstream &ss, char &lookahead ) {
    if ( isDigit {}( lookahead ) ){
        int value = 0;
        do {
            value = value * 10 + ToDigit {}( lookahead );
            lookahead = ss.get();
        } while ( !ss.eof() && isDigit {} ( lookahead ) );
        print ( value );
    } else if ( lookahead == '(' ) {
        match ( ss, lookahead, '(' );
        expr( ss, lookahead );
        if( lookahead != ')' ){
            throw fail { "Expected a closing parenthesis before end of line." };
        }
        match ( ss, lookahead, ')' );
    } else if ( isWhiteSpace {} ( lookahead ) ) {
        for (;; lookahead = ss.get() ){
            if( isWhiteSpace {}( lookahead ) ) continue;
            else break;
        }
    } else {
        std::cerr << "Syntax Error: expecting a digit, got '" << lookahead << "' instead." << std::endl;
        abort();
    }
}

int main( int argc, char **argv )
{
    std::string str { "9 + (3 + 5) + 6 + 12 + 8 + 12+23435+(12+13+11))" };
    if( argc > 1 ){
        str = std::string { argv [1] };
    }
    std::istringstream ss { str };
    char lookahead = ss.get();
    
    expr( ss, lookahead );
    std::cout << printer << std::endl;
    std::cout << evaluate( printer ) << std::endl;
    
    //~ std::cout << std::endl << "_ ==> " << (int)('.') << std::endl;
    return 0;
}
