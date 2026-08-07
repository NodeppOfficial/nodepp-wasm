#include <nodepp/nodepp.h>
#include <simd/simd.h>

using namespace nodepp;

void onMain(){

    simd::simd_t var1 ({ 10, 20, 30, 40 });
    simd::simd_t var2 ({ 40, 30, 20, 10 });
    simd::simd_t var3 ({ 10, 10, 10, 10 });

    auto out = simd::add( var1, var2 );

    console::log( ">>", sizeof(uchar_32), sizeof(float) );

    console::log( out.x, out.y, out.z, out.w );

}