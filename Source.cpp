

/*
    Simple Calculator

    Revision History:

        Revised by Bjarne Stroustrup November 2013
        Revised by Bjarne Stroustrup May 2007
        Revised by Bjarne Stroustrup August 2006
        Revised by Bjarne Stroustrup August 2004
        Originally written by Bjarne Stroustrup 
         (bs@cs.tamu.edu) Spring 2004

    This program implements a basic expression calculator,
    input from cin; output to cout.
    The grammar for input is:

    Statement:
        Expression
        Print
        Quit

        Print:
            =

        Quit:
            q

        Expression:
            Term
            Expression + Term
            Expression - Term
        Term:
                    Primary        // Stroustrup code  Deprecated
                    Term * Primary // Stroustrup code  Deprecated
                    Term / Primary // Stroustrup code  Deprecated
                    Term % Primary // Stroustrup code  Deprecated

            Factorial          // Geoff Lords contribution as apart of the exercises With a little google help
            Term * Factorial   // Geoff Lords contribution as apart of the exercises With a little google help
            Term / Factorial   // Geoff Lords contribution as apart of the exercises With a little google help
            Term % Factorial   // Geoff Lords contribution as apart of the exercises With a little google help
            Term ! Factorial   // Geoff Lords contribution as apart of the exercises With a little google help

        Primary:
            Number
            { Expression } // Geoff Lords contribution as apart of the exercises
            ( Expression )
            - Primary
            + Primary
        Number:
            floating-point literal

        Input comes from cin through the Token_Stream called ts
 */

#include "std_lib_facilities.h"



///constants///
const char number = '7'; //tk.kind == number means that tk is a number Token
const char quit = 'q';
const char print = '=';
const string prompt = "> ";
const string result = "= ";
const char name = 'a';  // name token
const char let = 'L';       // declaration token
const string declkey = "let";   // declaration keyword

/// Functions

void calculate();       //
void clean_up_mess();   //Clean up errors
double get_value(string s);
void set_value(string s, double d);
double statement();
bool is_declared(string var);
double define_name(string var, double val);
double decleration();

//Classes
class Token {
public:
    char kind;        // what kind of token
    double value{};     // for numbers: a value
    string name{};
    Token(char ch)  : kind(ch) { }  // make a Token from a char
    Token(char ch, double val) : kind(ch), value(val) { } // initialise kind and value

    Token(char ch, string n) : kind{ ch }, name{ n } { } // initialise kind and name
};

class Token_stream {
public:
    Token_stream();   // make a Token_stream that reads from cin
   
    Token get();      // get a Token (get() is defined elsewhere)
    void putback(Token tk);    // put a Token back
    void ignore(char c);
private:
    bool full {false};  // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

class Variable {
public:
    string name;
    double value;
    Variable(string n, double v) :name(n), value(v)
    {
    }
};
vector<Variable>var_table;

//Constructors
Token_stream::Token_stream()
    :full(false), buffer(0)    // no Token in buffer
{
}

//putback into stream
void Token_stream::putback(Token tk)
{
    // The putback() member function puts its argument back into the Token_stream's buffer:
    if (full) error("putback() buffer is already full");
    buffer = tk;       // copy tk to buffer
    full = true;      // buffer is now full
}

double define_name(string var, double val)
{
    if (is_declared(var)) error(var, " declared twice!");
    var_table.push_back(Variable(var, val)); 
    return val;
}

Token Token_stream::get()
{
    if (full) {       // do we already have a Token ready?
        // remove token from buffer
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)

    switch (ch) {
    case print:
    case quit: // not working, something went wron in the editing.
    case 'L':
        //in order of priority for clarity
    case '!':       // factorial
    case '{':
    case '}':
    case '(':
    case ')':
    case '*':
    case '/':
    case '%':       //Modulo (remainder of a divided number - int is the usual but for floating points use fmod() )
    case '+':
    case '-':
        return Token(ch);        // let each character represent itself
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '9':
    {
        cin.putback(ch);         // put digit back into the input stream
        double val;
        cin >> val;              // read a floating-point number
        return Token(number, val);
    }
    default:
        if (isalpha(ch)) {
           // cin.putback(ch);
            string s;
            s += ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
            cin.putback(ch);
            if (s == declkey) return Token(let); // declaration keyword
            return Token{ name,s };
        }
        //error("Default: Bad token");
       //throw (main());
    }
}

void Token_stream::ignore(char c) // ch represent the kind of token
{
    //first look in the buffer
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;
    //now search input
    char ch = 0;
    while (cin >> ch)
        if (ch == c)return;
}

/*struct Variable {
    string name;
    double value;
    Variable(string n, double v) :name(n), value(v)
    {
    }
};*/

    vector<Variable> names;

    double get_value(string s)
    {
        for (const Variable& v : var_table)
            if (v.name == s) return v.value;
        error("get: undefined variable ", s);
    }

    void set_value(string s, double d) // set the variable name s to d
    {
        for (Variable& v : var_table)
            if (v.name == s) {
                v.value = d;
                return;
            }
        error("set: undefined variable", s);
    }

    bool is_declared(string var) // is var already in the var_table?
    {
        for (const Variable& v : var_table)
            if (v.name == var)return true;
        return false;
    }

    Token_stream ts;        // provides get() and putback()

    double expression();    // declaration so that primary() can call expression()

    // Handle numbers and parentheses and curly brackets as 
    // primary focus (Starting point of calculations)
    double primary()
    {
        Token tk = ts.get();
        switch (tk.kind) {
        case '{':    // handle '{' expression '}'
        {
            double d = expression();
            tk = ts.get();
            if (tk.kind != '}') error("'}' was expected.");
            return d;
        }
        case '(':    // handle '(' expression ')'
        {
            double d = expression();
            tk = ts.get();
            if (tk.kind != ')') error("')' was expected.");
            return d;
        }
        case number:            // const char number token
            return tk.value;  // return the number's value

        case '-':
            return -primary();
        case '+':
            return primary();
        default:
            //error("primary token expected."); //TODO Uncomment this when error is found the loop is broken somewhere.
            cout << "In place of error command /'Primary Token Expected/'\n";
            calculate();
            
        }
        return 0;
    }

    // handle '!' - factorial but do the conversion to int first,
    // fractorials only work as an integer.
    double factorial()
    {
        double left = primary();
        Token tk = ts.get();        // get the next token from token stream

        while (true) {
            switch (tk.kind) {
            case '!':
            {
                int n = abs(left); // convert to int abs() has six overload functions really cool!
                long int fac = 1;  // 0!=1
                while (n > 1) {
                    fac *= n--;  // 1*n*(n-1)*.. *3*2
                }
                if (left < 0) {
                    left = -fac;  // we want to handle the case -5!
                }
                else {
                    left = fac;
                }
                tk = ts.get();
                break;
            }
            default:
                ts.putback(tk);     // put tk back into the token stream
                return left;
            }
        }
    }
    // Handle * & /
    double term()
    {
        //double left = primary();
        double left = factorial();
        Token tk = ts.get();        // get the next token from token stream

        while (true) {
            switch (tk.kind) {
            case '*':
                //left *= primary();
                left *= factorial();
                tk = ts.get();
                break;
            case '/':
            {
                //double d = primary();
                double d = factorial();
                if (d == 0) error("divide by zero?");
                left /= d;
                tk = ts.get();
                break;
            }
            case '%':
            {
                double d = primary();
                if (d == 0)error("Divide by zero?");
                left = fmod(left, d);
                tk = ts.get();
                break;
            }
            case 'x':
                error("x");
                break;
            default:
                ts.putback(tk);     // put tk back into the token stream
                return left;
            }
        }
    }

    // Handle + and -
    double expression()
    {
        double left = term();      // read and evaluate a Term
        Token tk = ts.get();        // get the next token from token stream

        while (true) {
            switch (tk.kind) {
            case '+':
                left += term();    // evaluate Term and add
                tk = ts.get();
                break;
            case '-':
                left -= term();    // evaluate Term and subtract
                tk = ts.get();
                break;
            default:
                ts.putback(tk);     // put tk back into the token stream
                return left;       // finally: no more + or -: return the answer
            }
        }
    }

    double decleration()
        // assume we have seen "let"
        // handle: name = expression
        // declare a variable called "name" with the initial value "expression"
    {
        Token t = ts.get();
        if (t.kind != name) error("name expected in declaration");
        string var_name = t.name;

        Token t2 = ts.get();
        if (t2.kind != '=') error(" =  missing in declaration of ", var_name);

        double d = expression();
        define_name(var_name, d);
        return d;
    }

    double statement()
    {
        Token tk = ts.get();
        switch (tk.kind)
        {
        case let:
            return decleration();
        default:
            ts.putback(tk);
            return expression();
        }
    }

    //ignore typo errors
    void clean_up_mess()
    {
        while (true)
        {
            ts.ignore(print);
        }
    }

    // Calculation loop
    void calculate()
    {
        cout << "Welcome to the Calculator based on Stroustrup' Tutorial.\n";
        cout << "Please enter a mathematical formular, the supported symbols are:\n";
        cout << "Minus: '-', \nAddition: '+', \nMultiplication: '*', \nDivision: '/'.\n\n";
        cout << "In order of top-most priority the calculator is also supporting:\n";
        cout << "Factorial: '!' i.e(\" > 5!= \")\n";
        cout << "Curly braces:'{' and '}' \nParenthesis: '(' and ')' \n\n";
        cout << "At the end of each eqation please type the '=' followed \nby enter key for the answer.\n\n";
        cout << "To exit the application please type 'q' followed \nby the enter key.\n";
        while (cin)
            try {
            cout << prompt;
            Token tk = ts.get(); // eat '=' or ';' for old version
            while (tk.kind == print)
            {
                tk = ts.get(); //first discard all "prints"
            }
            if (tk.kind == quit) return;
            

            ts.putback(tk);
            cout << result << statement() << '\n'; //removed expression() to replace with statement()
        }
        catch (exception& e) {
            cerr << e.what() << '\n';
            clean_up_mess();
        }
    }

    int main()
        try
    {
        define_name("pi", 3.1415926535);
        define_name("e", 2.7182818284);

        calculate(); //Calculation Loop
        keep_window_open();
        return 0;
    }
    catch (runtime_error& e) {
        cerr << e.what() << '\n';
        keep_window_open("~~");
        return 1;
    }
    catch (...) {
        cerr << "Default catch: Unknown exception!\n";
        keep_window_open("~~");
        return 2;
    }
    /*catch (runtime_error& e) {
        cerr << e.what() << '\n';
        cout << "Please enter the character ~ to close the window.\n";
        for (char ch; cin >> ch;)
            if (ch == '~')return 1;
        return 1;
    }*/

    /*catch (exception& e) {
        if (*e.what() == 'x') {
            cout << "\nExiting the program...\n";
            cout << "Program exited: Come back soon.";
            return 0;
        }
        cerr << "error: " << e.what() << '\n';
        keep_window_open("~~");
        return 1;
    }*/
