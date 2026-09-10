#include <nodepp/nodepp.h>
#include <nodepp/crypto.h>

using namespace nodepp;

string_t base58_encode( string_t message ){
string_t charset = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    
    if( message.empty() ){ return nullptr; } 
    
    queue_t<char>  out, zrs; ulong zeros= 0;
    array_t<uchar> nmb( message.size() );
    memcpy( nmb.get(), (uchar*) message.get(), message.size() );

    while( zeros < message.size() && 
           message[zeros] == 0x00 
    ) { 
        nmb.ptr().slice(1,nmb.size()); 
        zrs.push('1'); zeros++; 
    }
    
    while( !nmb.empty() ){ uchar_32 borrow = 0;

        for( ulong i=0; i<nmb.size(); i++ ){
             uchar_32 current = ( borrow<<8 ) | nmb[i];
             nmb[i] = current / 58;
             borrow = current % 58;
        }

        while( !nmb.empty() && nmb[0]==0x00 ){ nmb.shift(); }
        out.unshift( charset[borrow] );

    }
    
    out.insert( out.first(), zrs.data() ); 
    out.push( '\0' ); return out.data();

}

string_t base58_decode( string_t message ){
string_t charset = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    
    if( message.empty() ){ return nullptr; }
    
    // 1. Construir tabla inversa (carácter → valor)
    int reverse_table[256];
    for( int i = 0; i < 256; i++ ){ reverse_table[i] = -1; }
    for( size_t i = 0; i < charset.size(); i++ ){
         reverse_table[(uint8_t)charset[i]] = i;
    }
    
    // 2. Contar cuántos '1' hay al inicio (ceros a la izquierda)
    ulong zeros = 0;
    while( zeros < message.size() && message[zeros] == charset[0] ){
        zeros++;
    }
    
    // 3. Convertir Base58 a número (multiplicación sucesiva)
    array_t<uchar> number;
    
    for( ulong i = zeros; i < message.size(); i++ ){
        int value = reverse_table[(uint8_t)message[i]];
        if( value < 0 ){
            // Carácter inválido (ignorar)
            continue;
        }
        
        // Multiplicar el número actual por 58 y sumar el valor
        uchar_32 carry = value;
        for( ulong j = 0; j < number.size(); j++ ){
            uchar_32 current = (number[j] * 58) + carry;
            number[j] = current & 0xFF;
            carry = current >> 8;
        }
        while( carry > 0 ){
            number.unshift( carry & 0xFF );
            carry >>= 8;
        }
    }
    
    // 4. Construir el resultado (bytes en orden correcto)
    string_t result;
    
    // Añadir ceros a la izquierda (bytes 0x00)
    for( ulong i = 0; i < zeros; i++ ){
        result.unshift( '\0' );
    }
    
    // Añadir el número convertido (en orden inverso)
    for( int i = (int)number.size() - 1; i >= 0; i-- ){
        result.unshift( (char)number[i] );
    }
    
    return result;
}

void onMain(){

    auto enc = base58_encode( "Hello world!" );
    auto dec = crypto::decoder::BASE58();
         dec.update( enc );

    console::log( enc );
    console::log( dec.get() ); // base58_decode( enc ) );

}