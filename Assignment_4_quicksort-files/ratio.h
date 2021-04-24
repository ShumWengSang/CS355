#ifndef RATIO_H
#define RATIO_H
#include <fstream>  // fstream
#include <cstdlib>  // abs
#include <atomic>   // std::atomic
#include <iomanip>  // setw
#include <thread>   // sleep_for

class Ratio {
    public:
        Ratio( int i = 0 )    : n(i),  d(1)   {}
        Ratio(int _n, int _d) : n(_n), d(_d)  { CheckSignAndGCD(); }

        // set width
        static void Width( int w ) { width = w; }
        // negate
        Ratio operator- () const { return Ratio(-n,d); }
        ////////////////////////////////////////////////////////////////////////
        // multiplication
        Ratio operator* ( Ratio const & op2 ) const {
            Ratio res( *this );
            return res *= op2;
        }
        Ratio& operator*= ( Ratio const & op2 ) {
            n *= op2.n;
            d *= op2.d;
            CheckSignAndGCD();
            return *this;
        }
        ////////////////////////////////////////////////////////////////////////
        // division
        Ratio operator/ ( Ratio const & op2 ) const {
            Ratio res( *this );
            return res /= op2;
        }
        Ratio& operator/= ( Ratio const & op2 ) {
            n *= op2.d;
            d *= op2.n;
            CheckSignAndGCD();
            return *this;
        }
        ////////////////////////////////////////////////////////////////////////
        // subtraction
        Ratio operator- ( Ratio const & op2 ) const {
            return *this + -op2;
        }
        Ratio& operator-= ( Ratio const & op2 ) {
            return *this += -op2;
        }
        ////////////////////////////////////////////////////////////////////////
        // addition
        Ratio operator+ ( Ratio const & op2 ) const {
            Ratio res( *this );
            return res += op2;
        }
        Ratio& operator+= ( Ratio const & op2 ) {
            //n1/d1 + n2/d2 = n1*d2 + n2*d1 / d1*d2;
            n = n*op2.d + op2.n*d;
            d = d*op2.d;
            CheckSignAndGCD();
            return *this;
        }
        ////////////////////////////////////////////////////////////////////////
        // comparisons
        bool operator< ( Ratio const & op2 ) const {
            ++counter_less;
            std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
            return (n*op2.d < d*op2.n);
        }
        bool operator== ( Ratio const & op2 ) const {
            std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
            return (n==op2.n && d==op2.d);
        }
        bool operator!= ( Ratio const & op2 ) const {
            std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
            return !(*this==op2);
        }
        bool operator> ( Ratio const & op2 ) const {
            std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
            return (n*op2.d > d*op2.n);
        }
        bool operator>= ( Ratio const & op2 ) const {
            return !(*this<op2);
        }
        bool operator<= ( Ratio const & op2 ) const {
            return !(*this>op2);
        }
        static int GetLessCounter() {
            return counter_less;
        }
        static void ZeroLessCounter() {
            counter_less = 0;
        }

        ////////////////////////////////////////////////////////////////////////
        friend std::ostream& operator<<( std::ostream& os, Ratio const& r ) {
            os << std::setw( r.width ) << r.n;
            if ( r.d != 1 ) { os << "/" << std::setw( r.width-1 ) << r.d; }
            else            { os <<        std::setw( r.width ) << " "; }
            return os;
        }

    private:
        int n,d;
        static int width;
        ////////////////////////////////////////////////////////////////////////
        void CheckSignAndGCD() {
            if ( d<0 ) { n *= -1; d*= -1; }
            int gcd = GCD();
            n = n/gcd;
            d = d/gcd;
        }
        ////////////////////////////////////////////////////////////////////////
        int GCD() const {
            int a=std::abs(n), b=std::abs(d);
            while ( a && b ) {
                if ( a < b ) { b = b%a; }
                else         { a = a%b; }
            }
            return ( a==0 )?b:a;
        }
        static std::atomic<unsigned> counter_less;
};

// when first operand is not Ratio, conversion does not kick in, so swap (see implementation)
Ratio operator- ( int op1, Ratio const & op2 );
Ratio operator* ( int op1, Ratio const & op2 );
Ratio operator+ ( int op1, Ratio const & op2 );
Ratio operator/ ( int op1, Ratio const & op2 );
bool  operator< ( int op1, Ratio const & op2 );

#endif
