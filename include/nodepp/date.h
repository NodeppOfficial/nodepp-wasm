/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_DATE
#define NODEPP_WASM_DATE

/*────────────────────────────────────────────────────────────────────────────*/

#include <emscripten.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class date_t {
protected:

    struct NODE {
        bool utc;
        uint day;
        uint year;
        uint hour;
        uint month;
        uint minute;
        uint second;
    };  ptr_t<NODE> obj;

    /*─······································································─*/

    void set_time( ulong time, bool utc ) const noexcept {
    auto tmp = EM_EVAL({ 
        
        let date = new Date(${1});
        let out  = new Object(  );

        if(${0}){ 
            out["year"]   = date.getUTCFullYear(); 
            out["second"] = date.getUTCSeconds ();
            out["minute"] = date.getUTCMinutes ();
            out["month"]  = date.getUTCMonth   ();
            out["hour"]   = date.getUTCHours   ();
            out["day"]    = date.getUTCDay     ();
        } else { 
            out["year"]   = date.getFullYear   (); 
            out["second"] = date.getSeconds    ();
            out["minute"] = date.getMinutes    ();
            out["month"]  = date.getMonth      ();
            out["hour"]   = date.getHours      ();
            out["day"]    = date.getDay        ();
        }

    return out; }, utc, time ); obj->utc = utc;

        obj->day    = out["day"]   .as<uint>();
        obj->year   = out["year"]  .as<uint>();
        obj->second = out["second"].as<uint>();
        obj->hour   = out["hour"]  .as<uint>();
        obj->month  = out["month"] .as<uint>();
        obj->minute = out["minute"].as<uint>();

    }

    void get_time( bool utc ) const noexcept {
    auto tmp = EM_EVAL({ 
        
        let date = new Date  ();
        let out  = new Object();

        if(${0}){ 
            out["year"]   = date.getUTCFullYear(); 
            out["second"] = date.getUTCSeconds ();
            out["minute"] = date.getUTCMinutes ();
            out["month"]  = date.getUTCMonth   ();
            out["hour"]   = date.getUTCHours   ();
            out["day"]    = date.getUTCDay     ();
        } else { 
            out["year"]   = date.getFullYear   (); 
            out["second"] = date.getSeconds    ();
            out["minute"] = date.getMinutes    ();
            out["month"]  = date.getMonth      ();
            out["hour"]   = date.getHours      ();
            out["day"]    = date.getDay        ();
        }

    return out; }, utc ); obj->utc = utc;

        obj->day    = out["day"]   .as<uint>();
        obj->year   = out["year"]  .as<uint>();
        obj->second = out["second"].as<uint>();
        obj->hour   = out["hour"]  .as<uint>();
        obj->month  = out["month"] .as<uint>();
        obj->minute = out["minute"].as<uint>();

    }
    
public:

    template< class... V >
    date_t( const V&... args ) noexcept : obj( 0UL, NODE() ) { set_date( args... ); }

    date_t() noexcept : obj( 0UL, NODE() ) { set_date( false ); }
    
    /*─······································································─*/

    bool operator==( const date_t& other ) const noexcept { return get_stamp()==other.get_stamp(); }
    bool operator<=( const date_t& other ) const noexcept { return get_stamp()<=other.get_stamp(); }
    bool operator>=( const date_t& other ) const noexcept { return get_stamp()>=other.get_stamp(); }
    bool operator< ( const date_t& other ) const noexcept { return get_stamp()< other.get_stamp(); }
    bool operator> ( const date_t& other ) const noexcept { return get_stamp()> other.get_stamp(); }

    /*─······································································─*/

    void operator+=( const date_t& other ) const noexcept {
         set_stamp( get_stamp() + other.get_stamp(), obj->utc );
    }

    void operator-=( const date_t& other ) const noexcept {
         set_stamp( get_stamp() - other.get_stamp(), obj->utc );
    }

    void operator*=( const date_t& other ) const noexcept {
         set_stamp( get_stamp() * other.get_stamp(), obj->utc );
    }

    void operator/=( const date_t& other ) const noexcept {
         set_stamp( get_stamp() / other.get_stamp(), obj->utc );
    }

    /*─······································································─*/

    void set_stamp( const time_t& time, bool utc ) const noexcept {
         set_time ( time, utc );
   }
    
    /*─······································································─*/

    void set_date( bool utc ) const noexcept {
         set_utc(utc); get_time( utc );
    }

    void set_date( uint year, bool utc ) const noexcept {
         set_utc(utc); set_year(year);
    }

    void set_date( uint year, uint month, bool utc ) const noexcept {
         set_utc(utc); set_year(year); set_month(month);
    }

    void set_date( uint year, uint month, uint day, bool utc ) const noexcept {
         set_utc(utc); set_year(year); set_month(month); set_day(day);
    }

    void set_date( uint year, uint month, uint day, uint hour, bool utc ) const noexcept {
         set_utc(utc); set_year(year); set_month(month); set_day(day); set_hour(hour);
    }

    void set_date( uint year, uint month, uint day, uint hour, uint min, bool utc ) const noexcept {
         set_utc(utc); set_year(year); set_month(month); set_day(day); set_hour(hour); set_minute(min);
    }

    void set_date( uint year, uint month, uint day, uint hour, uint min, uint second, bool utc ) const noexcept {
         set_utc(utc); set_year(year); set_month(month); set_day(day); set_hour(hour); set_minute(min); set_second(second);
    }
    
    /*─······································································─*/
    
    void set_year  ( uint year  ) const noexcept { obj->year   = year;  }
    void set_month ( uint month ) const noexcept { obj->month  = month; }
    void set_second( uint sec   ) const noexcept { obj->second = sec;   }
    void set_minute( uint min   ) const noexcept { obj->minute = min;   }
    void set_hour  ( uint hour  ) const noexcept { obj->hour   = hour;  }
    void set_day   ( uint day   ) const noexcept { obj->day    = day;   }
    void set_utc   ( bool utc   ) const noexcept { obj->utc    = utc;   }

    /*─······································································─*/

    string_t get_fulltime() const noexcept { 

        return string_t( EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
            }   return data.toString();
            
        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<EM_STRING>() );

    }
    
    uint get_year() const noexcept { 

        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCFullYear(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getFullYear(); 
            }

        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<uint>();
        
    }
    
    uint get_month() const noexcept {

        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCMonth(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getMonth(); 
            }

        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<uint>();

    }

    uint get_hour() const noexcept { 
        
        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCHours(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getHours(); 
            }

        ),
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<uint>();

    }
    
    uint get_day() const noexcept {
        
        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCDay(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getUTCDay(); 
            }

        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<uint>();

    }

    uint get_minute() const noexcept {
        
        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCMinutes(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getMinutes(); 
            }

        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<uint>();

    }
    
    uint get_second() const noexcept {

        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCSeconds(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getSeconds(); 
            }

        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<uint>();

    }

    size_t get_stamp() const noexcept { 

        return EM_EVAL( NODEPP_STRINGIFY(
            let data = new Date();

            if( ${0} ){
                data.setUTCDate   (${1});
                data.setUTCYear   (${2});
                data.setUTCHour   (${3});
                data.setUTCMonth  (${4});
                data.setUTCMinutes(${5});
                data.setUTCSeconds(${6});
                return data.getUTCSeconds(); 
            } else {
                data.setDate      (${1});
                data.setYear      (${2});
                data.setHour      (${3});
                data.setMonth     (${4});
                data.setMinutes   (${5});
                data.setSeconds   (${6});
                return data.getSeconds(); 
            }

        ), 
            obj->utc , obj->day  , obj->year  , 
            obj->hour, obj->month, obj->minute, 
            obj->second 
        ).as<size_t>();

    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp {

    inline date_t operator+( const date_t& A, const date_t& B ){
        date_t C; C.set_stamp( A.get_stamp() + B.get_stamp(), false );
        return C;
    }

    inline date_t operator-( const date_t& A, const date_t& B ){
        date_t C; C.set_stamp( A.get_stamp() - B.get_stamp(), false );
        return C;
    }

    inline date_t operator*( const date_t& A, const date_t& B ){
        date_t C; C.set_stamp( A.get_stamp() * B.get_stamp(), false );
        return C;
    }

    inline date_t operator/( const date_t& A, const date_t& B ){
        date_t C; C.set_stamp( A.get_stamp() / B.get_stamp(), false );
        return C;
    }

}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace date {

    inline size_t now(){ return date_t().get_stamp(); }

    inline string_t fulltime(){ return date_t().get_fulltime(); }

    inline uint day( bool utc ){ return date_t(utc).get_day(); }

    inline uint year( bool utc ){ return date_t(utc).get_year(); }

    inline uint hour( bool utc ){ return date_t(utc).get_hour(); }

    inline uint month( bool utc ){ return date_t(utc).get_month(); }

    inline uint minute( bool utc ){ return date_t(utc).get_minute(); }

    inline uint second( bool utc ){ return date_t(utc).get_second(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/