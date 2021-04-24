#include "ratio.h"

int Ratio::width = 0; // no padding by default
std::atomic<unsigned> Ratio::counter_less( 0 );

// when first operand is not Ratio, conversion does not kick in, so swap
Ratio operator- ( int op1, Ratio const & op2 ) { return op2-op1; }
Ratio operator* ( int op1, Ratio const & op2 ) { return op2*op1; }
Ratio operator+ ( int op1, Ratio const & op2 ) { return op2+op1; }
Ratio operator/ ( int op1, Ratio const & op2 ) { return op2/op1; }
bool  operator< ( int op1, Ratio const & op2 ) { return Ratio(op1)<op2; }

