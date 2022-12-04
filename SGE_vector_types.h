#ifndef INCLUDE_VECTOR_TYPES
#define INCLUDE_VECTOR_TYPES

/* SGE_vector_types.h - part of the SDL2-based Game Engine (SGE) v.20221204
 * ========================================================================
 *
 * The SGE was developed by Joseph21 and is heavily inspired bij the Pixel Game Engine (PGE) by Javidx9
 * (see: https://github.com/OneLoneCoder/olcPixelGameEngine). It's interface is deliberately kept very
 * close to that of the PGE, so that programs can be ported from the one to the other quite easily.
 *
 * License
 * -------
 * This code is completely free to use, change, rewrite or get inspiration from. At the same time, there's
 * no warranty that this code is free of bugs. If you use (any part of) this code, you accept each and any
 * risk or consequence thereof.
 *
 * Although there is no obligation to mention or shout out to the creator, I wouldn't mind if you did :)
 *
 * Have fun with it!
 *
 * Joseph21
 * december 4, 2022
 */

//                          +--------------------+                           //
// -------------------------+ MODULE DESCRIPTION +-------------------------- //
//                          +--------------------+                           //

/*
 * In this module a number of vector types is implemented as a template. Each template can be instantiated
 * with a primitive type like int, float, double etc. This could be done for other primitive types as well.
 * For int, float, double and long long a typedef is implemented per vector type.
 *
 * The vector types are
 *   1. v2d_generic     - generic 2d vector, contains 2 components x and y;
 *   2. v3d_generic     - generic 2d vector, contains 3 components x, y and z;
 *   3. v2d_hom_generic - homogeneous 2d vector, contains components x, y and w;
 *   4. v3d_hom_generic - homogeneous 3d vector, contains components x, y, z and w;
 *   5. v2d_hom_texture - homogeneous 2d vector for texturing, contains components u, v and w ;
 *
 * For each of these types the following functionality is implemented:
 *   * default, initializer and copy constructor;
 *   * length, normalization, perpendicular vector, dot and cross product, reciprocal
 *   * round, floor, ceil and trunc (component wise)
 *   * min, max, clamp (component wise)
 *   * operators +, -, * and / both vector vector and vector scalar
 *   * operators +=, -=, *= and /= the same
 *   * operators [] so that indexing is possible
 *   * operators <, == and > so that vectors can be combined with stl routines depend on these
 *   * casting to other primitive types
 */

#include <cmath>

//                               +---------------------------------------+                           //
// ------------------------------+  Generic 2 d vector type definitions  +-------------------------- //
//                               +---------------------------------------+                           //

// Using the "template" keyword, define a placeholder T for our "input" type for the template
template <class T>
struct v2d_generic {

    // variables section ====================================================
    T x = (T)0;
    T y = (T)0;

    // constructor section ====================================================
    v2d_generic()                       : x( (T)0 ), y( (T)0 ) {}    // default constructor
    v2d_generic( T _x, T _y )           : x(   _x ), y(   _y ) {}    // initializer constructor
    v2d_generic( const v2d_generic &v ) : x(  v.x ), y(  v.y ) {}    // copy constructor

    // utility functions ======================================================
    T mag()                           { return sqrt( x * x + y * y );                           }    // returns magnitude (length)
    T length()                        { return sqrt( x * x + y * y );                           }    // returns length (magnitude)
    T mag2()                          { return x * x + y * y;                                   }    // returns squared magnitude
    T length2()                       { return x * x + y * y;                                   }    // returns squared length
    T dot(   const v2d_generic &rhs ) { return this->x * rhs.x + this->y * rhs.y;               }    // returns the dot product
    T cross( const v2d_generic &rhs ) { return this->x * rhs.y - this->y * rhs.x;               }    // returns the cross product
    v2d_generic norm()                { T r = (T)1 / mag(); return v2d_generic( x * r, y * r ); }    // returns normalized version of the vector
    v2d_generic perp()                { return v2d_generic( -y,  x );                           }    // returns a perpendicular vector
    v2d_generic perp_alt()            { return v2d_generic(  y, -x );                           }    // returns an alternative perpendicular vector
    v2d_generic reciprocal()          { return v2d_generic( (T)1 / this->x, (T)1 / this->y );   }    // returns element wise reciprocal

    // in namespace std, round(), floor(), ceil() and trunc() are defined for type float and double, but not for type int
    v2d_generic round()               { return v2d_generic( std::round( x ), std::round( y ));  }    // returns the vector with elements rounded
    v2d_generic floor()               { return v2d_generic( std::floor( x ), std::floor( y ));  }    // returns the vector with elements floor'ed
    v2d_generic ceil()                { return v2d_generic( std::ceil(  x ), std::ceil(  y ));  }    // returns the vector with elements ceil'ed
    v2d_generic trunc()               { return v2d_generic( std::trunc( x ), std::trunc( y ));  }    // returns the vector with elements trunc'ed

    // std::min() and std::max() are defined for all types where the "<" operator is defined
    v2d_generic min( const v2d_generic &rhs ) { return v2d_generic( std::min( x, rhs.x ), std::min( y, rhs.y ));  }    // element-wise min
    v2d_generic max( const v2d_generic &rhs ) { return v2d_generic( std::max( x, rhs.x ), std::max( y, rhs.y ));  }    // element-wise max

    v2d_generic clamp( const v2d_generic &low, const v2d_generic &hgh ) { return this->max( low ).min( hgh ); }  // element-wise clamp

    // basic operator overloading =============================================
    v2d_generic operator + ( const v2d_generic &rhs ) const { return v2d_generic( this->x + rhs.x, this->y + rhs.y ); }    // element wise add
    v2d_generic operator - ( const v2d_generic &rhs ) const { return v2d_generic( this->x - rhs.x, this->y - rhs.y ); }    // element wise subtract
    v2d_generic operator * ( const v2d_generic &rhs ) const { return v2d_generic( this->x * rhs.x, this->y * rhs.y ); }    // element wise multiply
    v2d_generic operator / ( const v2d_generic &rhs ) const { return v2d_generic( this->x / rhs.x, this->y / rhs.y ); }    // element wise divide
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v2d_generic operator + ( const T            rhs ) const { return v2d_generic( this->x + rhs  , this->y + rhs   ); }    // scalar add
    v2d_generic operator - ( const T            rhs ) const { return v2d_generic( this->x - rhs  , this->y - rhs   ); }    // scalar subtract
    v2d_generic operator * ( const T            rhs ) const { return v2d_generic( this->x * rhs  , this->y * rhs   ); }    // scalar multiply
    v2d_generic operator / ( const T            rhs ) const { return v2d_generic( this->x / rhs  , this->y / rhs   ); }    // scalar divide

    // more sophisticated operator overloading ================================
    // work with references to don't move things around if not necessary
    v2d_generic &operator += (const v2d_generic &rhs ) { this->x += rhs.x; this->y += rhs.y; return *this; }
    v2d_generic &operator -= (const v2d_generic &rhs ) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
    v2d_generic &operator *= (const v2d_generic &rhs ) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
    v2d_generic &operator /= (const v2d_generic &rhs ) { this->x /= rhs.x; this->y /= rhs.y; return *this; }
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v2d_generic &operator += (const T            rhs ) { this->x += rhs  ; this->y += rhs  ; return *this; }
    v2d_generic &operator -= (const T            rhs ) { this->x -= rhs  ; this->y -= rhs  ; return *this; }
    v2d_generic &operator *= (const T            rhs ) { this->x *= rhs  ; this->y *= rhs  ; return *this; }
    v2d_generic &operator /= (const T            rhs ) { this->x /= rhs  ; this->y /= rhs  ; return *this; }

    // overloading of array reference operator ================================
    // must be a reference, otherwise you can't assign to it
    // NOTE: programmer must guarantee that index i is in bounds!! (no error checking)
    T &operator [] ( std::size_t i ) { return *((T *)this + i); }

    // comparison operators - needed for STL container manipulation ================================
    // (for instance std::unique(), std::sort(), ...)
    bool operator == (const v2d_generic &rhs ) { return (this->x == rhs.x && this->y == rhs.y); }
    bool operator != (const v2d_generic &rhs ) { return (this->x != rhs.x || this->y != rhs.y); }
    bool operator < (const v2d_generic &rhs ) {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && (this->y < rhs.y));
    }
    bool operator > (const v2d_generic &rhs ) {
        return ( this->x >  rhs.x) ||
               ((this->x == rhs.x) && (this->y > rhs.y));
    }

    // casting - to enable mingling of different native templated types ================================
    // NOTE: inspired by JavidX9 PGE code (thanks :))
    operator v2d_generic<int      >() const { return { static_cast<int      >(this->x), static_cast<int      >(this->y) }; }
    operator v2d_generic<float    >() const { return { static_cast<float    >(this->x), static_cast<float    >(this->y) }; }
    operator v2d_generic<double   >() const { return { static_cast<double   >(this->x), static_cast<double   >(this->y) }; }
    operator v2d_generic<long long>() const { return { static_cast<long long>(this->x), static_cast<long long>(this->y) }; }
};

// create convenient shorthand types, using typedef

// be aware that not all utility functions or operators have the same meaning (or meaning at all) for all types...
// ceil(), floor(), round() and trunc() are not defined for int or have no meaning
// division has another meaning for int than for float or double

typedef v2d_generic<int      > vi2d;
typedef v2d_generic<float    > vf2d;
typedef v2d_generic<double   > vd2d;
typedef v2d_generic<long long> vllong2d;

//                               +---------------------------------------+                           //
// ------------------------------+  Generic 3 d vector type definitions  +-------------------------- //
//                               +---------------------------------------+                           //

template <class T>
struct v3d_generic {

    // variables section ====================================================
    T x = (T)0;
    T y = (T)0;
    T z = (T)0;

    // constructor section ====================================================
    v3d_generic()                       : x( (T)0 ), y( (T)0 ), z( (T)0 ) {}    // default constructor
    v3d_generic( T _x, T _y, T _z )     : x(   _x ), y(   _y ), z(   _z ) {}    // initializer constructor
    v3d_generic( const v3d_generic &v ) : x(  v.x ), y(  v.y ), z(  v.z ) {}    // copy constructor

    // utility functions ======================================================
    T mag()            { return sqrt( x * x + y * y + z * z );                          }    // returns magnitude (length)
    T length()         { return sqrt( x * x + y * y + z * z );                          }    // returns length (magnitude)
    T mag2()           { return       x * x + y * y + z * z  ;                          }    // returns squared magnitude
    T length2()        { return       x * x + y * y + z * z  ;                          }    // returns squared length
    T dot(             const v3d_generic &rhs ) { return this->x * rhs.x +
                                                         this->y * rhs.y +
                                                         this->z * rhs.z;               }    // returns the dot product
    v3d_generic norm() { T r = (T)1 / mag(); return v3d_generic( x * r, y * r, z * r ); }    // returns normalized version of the vector
    // the 3d version of perp() and perp_alt() needs two vectors (the 2d version only needs one).
    // the two vectors are interpreted as line segments from origin (0, 0, 0). The cross product is returned (not normalized!)
    v3d_generic perp(     const v3d_generic &v ) { return this->cross( v ); }
    v3d_generic perp_alt( const v3d_generic &v ) { return v.cross( *this ); }

    v3d_generic cross( const v3d_generic &rhs ) { return v3d_generic( this->y * rhs.z - this->z * rhs.y,
                                                                      this->z * rhs.x - this->x * rhs.z,
                                                                      this->x * rhs.y - this->y * rhs.x );      }    // returns the cross product
    v3d_generic reciprocal() { return v3d_generic( (T)1 / this->x, (T)1 / this->y, (T)1 / this->z );   }    // returns element wise reciprocal

    // in namespace std, round(), floor(), ceil() and trunc() are defined for type float and double, but not for type int
    v3d_generic round() { return v3d_generic( std::round( x ), std::round( y ), std::round( z )); }    // returns the vector with elements rounded
    v3d_generic floor() { return v3d_generic( std::floor( x ), std::floor( y ), std::floor( z )); }    // returns the vector with elements floor'ed
    v3d_generic ceil()  { return v3d_generic( std::ceil(  x ), std::ceil(  y ), std::ceil(  z )); }    // returns the vector with elements ceil'ed
    v3d_generic trunc() { return v3d_generic( std::trunc( x ), std::trunc( y ), std::trunc( z )); }    // returns the vector with elements trunc'ed

    // std::min() and std::max() are defined for all types where the "<" operator is defined
    v3d_generic max(   const v3d_generic &rhs ) { return v3d_generic( std::max( x, rhs.x ), std::max( y, rhs.y ), std::max( z, rhs.z ));  }    // element-wise max
    v3d_generic min(   const v3d_generic &rhs ) { return v3d_generic( std::min( x, rhs.x ), std::min( y, rhs.y ), std::min( z, rhs.z ));  }    // element-wise min

    v3d_generic clamp( const v3d_generic &low, const v3d_generic &hgh ) { return this->max( low ).min( hgh ); }                                // element-wise clamp

    // basic operator overloading =============================================
    v3d_generic operator + ( const v3d_generic &rhs ) { return v3d_generic( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z ); }    // element wise add
    v3d_generic operator - ( const v3d_generic &rhs ) { return v3d_generic( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z ); }    // element wise subtract
    v3d_generic operator * ( const v3d_generic &rhs ) { return v3d_generic( this->x * rhs.x, this->y * rhs.y, this->z * rhs.z ); }    // element wise multiply
    v3d_generic operator / ( const v3d_generic &rhs ) { return v3d_generic( this->x / rhs.x, this->y / rhs.y, this->z / rhs.z ); }    // element wise divide
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v3d_generic operator + ( const T            rhs ) { return v3d_generic( this->x + rhs  , this->y + rhs  , this->z + rhs   ); }    // scalar add
    v3d_generic operator - ( const T            rhs ) { return v3d_generic( this->x - rhs  , this->y - rhs  , this->z - rhs   ); }    // scalar subtract
    v3d_generic operator * ( const T            rhs ) { return v3d_generic( this->x * rhs  , this->y * rhs  , this->z * rhs   ); }    // scalar multiply
    v3d_generic operator / ( const T            rhs ) { return v3d_generic( this->x / rhs  , this->y / rhs  , this->z / rhs   ); }    // scalar divide

    // more sophisticated operator overloading ================================
    // work with references to don't move things around if not necessary
    v3d_generic &operator += (const v3d_generic &rhs ) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
    v3d_generic &operator -= (const v3d_generic &rhs ) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
    v3d_generic &operator *= (const v3d_generic &rhs ) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.z; return *this; }
    v3d_generic &operator /= (const v3d_generic &rhs ) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.z; return *this; }
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v3d_generic &operator += (const T            rhs ) { this->x += rhs  ; this->y += rhs  ; this->z += rhs  ; return *this; }
    v3d_generic &operator -= (const T            rhs ) { this->x -= rhs  ; this->y -= rhs  ; this->z -= rhs  ; return *this; }
    v3d_generic &operator *= (const T            rhs ) { this->x *= rhs  ; this->y *= rhs  ; this->z *= rhs  ; return *this; }
    v3d_generic &operator /= (const T            rhs ) { this->x /= rhs  ; this->y /= rhs  ; this->z /= rhs  ; return *this; }

    // overloading of array reference operator ================================
    // must be a reference, otherwise you can't assign to it
    // NOTE: programmer must guarantee that index i is in bounds!! (no error checking)
    T &operator [] ( std::size_t i ) { return *((T *)this + i); }

    // comparison operators - needed for STL container manipulation ================================
    // (for instance std::unique(), std::sort(), ...)
    bool operator == (const v3d_generic &rhs ) const { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z); }
    bool operator != (const v3d_generic &rhs ) const { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z); }
    bool operator <  (const v3d_generic &rhs ) const {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && ((this->y <  rhs.y) ||
                                      ((this->y == rhs.y) && (this->z < rhs.z))));
    }
    bool operator > (const v3d_generic &rhs ) const {
        return ( this->x >  rhs.x) ||
               ((this->x == rhs.x) && ((this->y >  rhs.y) ||
                                      ((this->y == rhs.y) && (this->z > rhs.z))));
    }

    // casting - to enable mingling of different native templated types ================================
    // NOTE: inspired by JavidX9 PGE code (thanks :))
    operator v3d_generic<int      >() const { return { static_cast<int      >(this->x), static_cast<int      >(this->y), static_cast<int      >(this->z) }; }
    operator v3d_generic<float    >() const { return { static_cast<float    >(this->x), static_cast<float    >(this->y), static_cast<float    >(this->z) }; }
    operator v3d_generic<double   >() const { return { static_cast<double   >(this->x), static_cast<double   >(this->y), static_cast<double   >(this->z) }; }
    operator v3d_generic<long long>() const { return { static_cast<long long>(this->x), static_cast<long long>(this->y), static_cast<long long>(this->z) }; }
};

// create convenient shorthand types, using typedef
// be aware that not all utility functions or operators have meaning for all types...
typedef v3d_generic<int      > vi3d;
typedef v3d_generic<float    > vf3d;
typedef v3d_generic<double   > vd3d;
typedef v3d_generic<long long> vllong3d;

//                               +---------------------------------------------------+                           //
// ------------------------------+  Generic 2 d homogeneous vector type definitions  +-------------------------- //
//                               +---------------------------------------------------+                           //

// ASSUMPTION: the w component is specifically for projective space. Most operators are working in Euclidian space. The w component is
//             not used / ignored in most methods, except:
//   * constructors,
//   * ==, !=, <, > operators
//   * casting
//   * min, max, Clamp

template <class T>
struct v2d_hom_generic {

    // variables section ====================================================
    T x = (T)0;
    T y = (T)0;
    T w = (T)1;   // for most of the functions, the value of w is either passed unaltered, or ignored
                  // it is mainly of importance in vector-matrix multiplications

    // constructor section ====================================================
    v2d_hom_generic()                           : x( (T)0 ), y( (T)0 ), w( (T)1 ) {}    // default constructor
    v2d_hom_generic( T _x, T _y, T _w = (T)1 )  : x(   _x ), y(   _y ), w(   _w ) {}    // initializer constructor
    v2d_hom_generic( const v2d_hom_generic &v ) : x(  v.x ), y(  v.y ), w(  v.w ) {}    // copy constructor

    // utility functions ======================================================
    T mag()                { return sqrt( x * x + y * y );                                                }    // returns magnitude (length)
    T length()             { return sqrt( x * x + y * y );                                                }    // returns length (magnitude)
    T mag2()               { return       x * x + y * y  ;                                                }    // returns squared magnitude
    T length2()            { return       x * x + y * y  ;                                                }    // returns squared length
    T dot(   const v2d_hom_generic &rhs ) { return                  this->x * rhs.x + this->y * rhs.y;    }    // returns the dot product
    T cross( const v2d_hom_generic &rhs ) { return v2d_hom_generic( this->x * rhs.y - this->y * rhs.x );  }    // returns the cross product
    v2d_hom_generic norm() { T r = (T)1 / mag(); return v2d_hom_generic( x * r, y * r, this->w );         }    // returns normalized version of the vector
    v2d_hom_generic reciprocal() { return v2d_hom_generic( (T)1 / this->x, (T)1 / this->y, this->w );     }    // returns element wise reciprocal

    // in namespace std, round(), floor(), ceil() and trunc() are defined for type float and double, but not for type int
    v2d_hom_generic round() { return v2d_hom_generic( std::round( x ), std::round( y ), this->w ); }    // returns the vector with elements rounded
    v2d_hom_generic floor() { return v2d_hom_generic( std::floor( x ), std::floor( y ), this->w ); }    // returns the vector with elements floor'ed
    v2d_hom_generic ceil()  { return v2d_hom_generic( std::ceil(  x ), std::ceil(  y ), this->w ); }    // returns the vector with elements ceil'ed
    v2d_hom_generic trunc() { return v2d_hom_generic( std::trunc( x ), std::trunc( y ), this->w ); }    // returns the vector with elements trunc'ed

    // std::min() and std::max() are defined for all types where the "<" operator is defined
    v2d_hom_generic max( const v2d_hom_generic &rhs ) { return v2d_hom_generic( std::max( x, rhs.x ), std::max( y, rhs.y ), std::max( w, rhs.w ));  }    // element-wise max
    v2d_hom_generic min( const v2d_hom_generic &rhs ) { return v2d_hom_generic( std::min( x, rhs.x ), std::min( y, rhs.y ), std::min( w, rhs.w ));  }    // element-wise min

    v2d_hom_generic clamp( const v2d_hom_generic &low, const v2d_hom_generic &hgh ) { return this->max( low ).min( hgh ); }          // element-wise clamp

    // basic operator overloading =============================================
    v2d_hom_generic operator + ( const v2d_hom_generic &rhs ) { return v2d_hom_generic( this->x + rhs.x, this->y + rhs.y ); }    // element wise add
    v2d_hom_generic operator - ( const v2d_hom_generic &rhs ) { return v2d_hom_generic( this->x - rhs.x, this->y - rhs.y ); }    // element wise subtract
    v2d_hom_generic operator * ( const v2d_hom_generic &rhs ) { return v2d_hom_generic( this->x * rhs.x, this->y * rhs.y ); }    // element wise multiply
    v2d_hom_generic operator / ( const v2d_hom_generic &rhs ) { return v2d_hom_generic( this->x / rhs.x, this->y / rhs.y ); }    // element wise divide
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v2d_hom_generic operator + ( const T                rhs ) { return v2d_hom_generic( this->x + rhs  , this->y + rhs   ); }    // scalar add
    v2d_hom_generic operator - ( const T                rhs ) { return v2d_hom_generic( this->x - rhs  , this->y - rhs   ); }    // scalar subtract
    v2d_hom_generic operator * ( const T                rhs ) { return v2d_hom_generic( this->x * rhs  , this->y * rhs   ); }    // scalar multiply
    v2d_hom_generic operator / ( const T                rhs ) { return v2d_hom_generic( this->x / rhs  , this->y / rhs   ); }    // scalar divide

    // more sophisticated operator overloading ================================
    // work with references to don't move things around if not necessary
    v2d_hom_generic &operator += (const v2d_hom_generic &rhs ) { this->x += rhs.x; this->y += rhs.y; return *this; }
    v2d_hom_generic &operator -= (const v2d_hom_generic &rhs ) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
    v2d_hom_generic &operator *= (const v2d_hom_generic &rhs ) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
    v2d_hom_generic &operator /= (const v2d_hom_generic &rhs ) { this->x /= rhs.x; this->y /= rhs.y; return *this; }
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v2d_hom_generic &operator += (const T                rhs ) { this->x += rhs  ; this->y += rhs  ; return *this; }
    v2d_hom_generic &operator -= (const T                rhs ) { this->x -= rhs  ; this->y -= rhs  ; return *this; }
    v2d_hom_generic &operator *= (const T                rhs ) { this->x *= rhs  ; this->y *= rhs  ; return *this; }
    v2d_hom_generic &operator /= (const T                rhs ) { this->x /= rhs  ; this->y /= rhs  ; return *this; }

    // overloading of array reference operator ================================
    // must be a reference, otherwise you can't assign to it
    // NOTE: programmer must guarantee that index i is in bounds!! (no error checking)
    T &operator [] ( std::size_t i ) { return *((T *)this + i); }

    // comparison operators - needed for STL container manipulation ================================
    // (for instance std::unique(), std::sort(), ...)
    bool operator == (const v2d_hom_generic &rhs ) const { return (this->x == rhs.x && this->y == rhs.y && this->w == rhs.w); }
    bool operator != (const v2d_hom_generic &rhs ) const { return (this->x != rhs.x || this->y != rhs.y || this->w != rhs.w); }
    bool operator <  (const v2d_hom_generic &rhs ) const {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && ((this->y <  rhs.y) ||
                                      ((this->y == rhs.y) && (this->w < rhs.w))));
    }
    bool operator >  (const v2d_hom_generic &rhs ) const {
        return ( this->x >  rhs.x) ||
               ((this->x == rhs.x) && ((this->y >  rhs.y) ||
                                      ((this->y == rhs.y) && (this->w > rhs.w))));
    }

    // casting - to enable mingling of different native templated types ================================
    // NOTE: inspired by JavidX9 PGE code (thanks :))
    operator v2d_hom_generic<int      >() const { return { static_cast<int      >(this->x), static_cast<int      >(this->y), static_cast<int      >(this->w) }; }
    operator v2d_hom_generic<float    >() const { return { static_cast<float    >(this->x), static_cast<float    >(this->y), static_cast<float    >(this->w) }; }
    operator v2d_hom_generic<double   >() const { return { static_cast<double   >(this->x), static_cast<double   >(this->y), static_cast<double   >(this->w) }; }
    operator v2d_hom_generic<long long>() const { return { static_cast<long long>(this->x), static_cast<long long>(this->y), static_cast<long long>(this->w) }; }
};

// create convenient shorthand types, using typedef
// be aware that not all utility functions or operators have meaning for all types...
typedef v2d_hom_generic<int      > vi2dh;
typedef v2d_hom_generic<float    > vf2dh;
typedef v2d_hom_generic<double   > vd2dh;
typedef v2d_hom_generic<long long> vllong2dh;

//                               +---------------------------------------------------+                           //
// ------------------------------+  Generic 3 d homogeneous vector type definitions  +-------------------------- //
//                               +---------------------------------------------------+                           //

template <class T>
struct v3d_hom_generic {

    // variables section ====================================================
    T x = (T)0;
    T y = (T)0;
    T z = (T)0;
    T w = (T)1;   // for most of the functions, the value of w is either passed unaltered, or ignored
                  // it is mainly of importance in vector-matrix multiplications

    // constructor section ====================================================
    v3d_hom_generic()                                : x( (T)0 ), y( (T)0 ), z( (T)0 ), w( (T)1 ) {}    // default constructor
    v3d_hom_generic( T _x, T _y, T _z, T _w = (T)1 ) : x(   _x ), y(   _y ), z(   _z ), w(   _w ) {}    // initializer constructor
    v3d_hom_generic( const v3d_hom_generic &v )      : x(  v.x ), y(  v.y ), z(  v.z ), w(  v.w ) {}    // copy constructor

    // utility functions ======================================================
    T mag()                { return sqrt( x * x + y * y + z * z );                                       }    // returns magnitude (length)
    T length()             { return sqrt( x * x + y * y + z * z );                                       }    // returns length (magnitude)
    T mag2()               { return       x * x + y * y + z * z  ;                                       }    // returns squared magnitude
    T length2()            { return       x * x + y * y + z * z  ;                                       }    // returns squared length
    T dot( const v3d_hom_generic &rhs ) { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;    }    // returns the dot product
    v3d_hom_generic norm() { T r = (T)1 / mag(); return v3d_hom_generic( x * r, y * r, z * r, this->w ); }    // returns normalized version of the vector
    v3d_hom_generic cross( const v3d_hom_generic &rhs ) { return v3d_hom_generic( this->y * rhs.z - this->z * rhs.y,
                                                                                  this->z * rhs.x - this->x * rhs.z,
                                                                                  this->x * rhs.y - this->y * rhs.x );  }    // returns the cross product
    v3d_hom_generic reciprocal() { return v3d_hom_generic( (T)1 / this->x, (T)1 / this->y, (T)1 / this->z, this->w );            }    // returns element wise reciprocal

    // in namespace std, round(), floor(), ceil() and trunc() are defined for type float and double, but not for type int
    v3d_hom_generic round() { return v3d_hom_generic( std::round( x ), std::round( y ), std::round( z ), this->w ); }    // returns the vector with elements rounded
    v3d_hom_generic floor() { return v3d_hom_generic( std::floor( x ), std::floor( y ), std::floor( z ), this->w ); }    // returns the vector with elements floor'ed
    v3d_hom_generic ceil()  { return v3d_hom_generic( std::ceil(  x ), std::ceil(  y ), std::ceil(  z ), this->w ); }    // returns the vector with elements ceil'ed
    v3d_hom_generic trunc() { return v3d_hom_generic( std::trunc( x ), std::trunc( y ), std::trunc( z ), this->w ); }    // returns the vector with elements trunc'ed

    // std::min() and std::max() are defined for all types where the "<" operator is defined
    v3d_hom_generic max(   const v3d_hom_generic &rhs ) { return v3d_hom_generic( std::max( x, rhs.x ), std::max( y, rhs.y ), std::max( z, rhs.z ), std::max( w, rhs.w ));  }  // element-wise max
    v3d_hom_generic min(   const v3d_hom_generic &rhs ) { return v3d_hom_generic( std::min( x, rhs.x ), std::min( y, rhs.y ), std::min( z, rhs.z ), std::min( w, rhs.w ));  }  // element-wise min
    v3d_hom_generic clamp( const v3d_hom_generic &low, const v3d_hom_generic &hgh ) { return this->max( low ).min( hgh ); }                              // element-wise clamp

    // basic operator overloading =============================================
    v3d_hom_generic operator + ( const v3d_hom_generic &rhs ) { return v3d_hom_generic( this->x + rhs.x, this->y + rhs.y, this->z + rhs.z ); }    // element wise add
    v3d_hom_generic operator - ( const v3d_hom_generic &rhs ) { return v3d_hom_generic( this->x - rhs.x, this->y - rhs.y, this->z - rhs.z ); }    // element wise subtract
    v3d_hom_generic operator * ( const v3d_hom_generic &rhs ) { return v3d_hom_generic( this->x * rhs.x, this->y * rhs.y, this->z * rhs.z ); }    // element wise multiply
    v3d_hom_generic operator / ( const v3d_hom_generic &rhs ) { return v3d_hom_generic( this->x / rhs.x, this->y / rhs.y, this->z / rhs.z ); }    // element wise divide
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v3d_hom_generic operator + ( const T                rhs ) { return v3d_hom_generic( this->x + rhs  , this->y + rhs  , this->z + rhs   ); }    // scalar add
    v3d_hom_generic operator - ( const T                rhs ) { return v3d_hom_generic( this->x - rhs  , this->y - rhs  , this->z - rhs   ); }    // scalar subtract
    v3d_hom_generic operator * ( const T                rhs ) { return v3d_hom_generic( this->x * rhs  , this->y * rhs  , this->z * rhs   ); }    // scalar multiply
    v3d_hom_generic operator / ( const T                rhs ) { return v3d_hom_generic( this->x / rhs  , this->y / rhs  , this->z / rhs   ); }    // scalar divide

    // more sophisticated operator overloading ================================
    // work with references to don't move things around if not necessary
    v3d_hom_generic &operator += (const v3d_hom_generic &rhs ) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
    v3d_hom_generic &operator -= (const v3d_hom_generic &rhs ) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
    v3d_hom_generic &operator *= (const v3d_hom_generic &rhs ) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.z; return *this; }
    v3d_hom_generic &operator /= (const v3d_hom_generic &rhs ) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.z; return *this; }
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v3d_hom_generic &operator += (const T                rhs ) { this->x += rhs  ; this->y += rhs  ; this->z += rhs  ; return *this; }
    v3d_hom_generic &operator -= (const T                rhs ) { this->x -= rhs  ; this->y -= rhs  ; this->z -= rhs  ; return *this; }
    v3d_hom_generic &operator *= (const T                rhs ) { this->x *= rhs  ; this->y *= rhs  ; this->z *= rhs  ; return *this; }
    v3d_hom_generic &operator /= (const T                rhs ) { this->x /= rhs  ; this->y /= rhs  ; this->z /= rhs  ; return *this; }

    // overloading of array reference operator ================================
    // must be a reference, otherwise you can't assign to it
    // NOTE: programmer must guarantee that index i is in bounds!! (no error checking)
    T &operator [] ( std::size_t i ) { return *((T *)this + i); }

    // comparison operators - needed for STL container manipulation ================================
    // (for instance std::unique(), std::sort(), ...)
    bool operator == (const v3d_hom_generic &rhs ) const { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w); }
    bool operator != (const v3d_hom_generic &rhs ) const { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z || this->w != rhs.w); }
    bool operator <  (const v3d_hom_generic &rhs ) const {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && (( this->y <  rhs.y) ||
                                       ((this->y == rhs.y) && (( this->z <  rhs.z) ||
                                                               ((this->z == rhs.z) && (this->w < rhs.w))))));
    }
    bool operator >  (const v3d_hom_generic &rhs ) const {
        return ( this->x >  rhs.x) ||
               ((this->x == rhs.x) && ((this->y >  rhs.y) ||
                                      ((this->y == rhs.y) && (( this->z >  rhs.z) ||
                                                              ((this->z == rhs.z) && (this->w > rhs.w))))));
    }

    // casting - to enable mingling of different native templated types ================================
    // NOTE: inspired by JavidX9 PGE code (thanks :))
    operator v3d_hom_generic<int      >() const { return { static_cast<int      >(this->x), static_cast<int      >(this->y), static_cast<int      >(this->z), static_cast<int      >(this->w) }; }
    operator v3d_hom_generic<float    >() const { return { static_cast<float    >(this->x), static_cast<float    >(this->y), static_cast<float    >(this->z), static_cast<float    >(this->w) }; }
    operator v3d_hom_generic<double   >() const { return { static_cast<double   >(this->x), static_cast<double   >(this->y), static_cast<double   >(this->z), static_cast<double   >(this->w) }; }
    operator v3d_hom_generic<long long>() const { return { static_cast<long long>(this->x), static_cast<long long>(this->y), static_cast<long long>(this->z), static_cast<long long>(this->w) }; }
};

// create convenient shorthand types, using typedef
// be aware that not all utility functions or operators have meaning for all types...
typedef v3d_hom_generic<int      > vi3dh;
typedef v3d_hom_generic<float    > vf3dh;
typedef v3d_hom_generic<double   > vd3dh;
typedef v3d_hom_generic<long long> vllong3dh;

//                               +-----------------------------------------------------------+                           //
// ------------------------------+  2 d homogeneous vector type definitions - for textures!  +-------------------------- //
//                               +-----------------------------------------------------------+                           //

template <class T>
struct v2d_hom_textures {

    // variables section - this is naming convention for texture coordinates ====================================================
    T u = (T)0;
    T v = (T)0;
    T w = (T)1;   // for most of the functions, the value of w is eiter passed unaltered, or ignored
                  // it is mainly of importance in vector-matrix multiplications

    // constructor section ====================================================
    v2d_hom_textures()                            : u( (T)0 ), v( (T)0 ), w( (T)1 ) {}    // default constructor
    v2d_hom_textures( T _u, T _v, T _w = (T)1 )   : u(   _u ), v(   _v ), w(   _w ) {}    // initializer constructor
    v2d_hom_textures( const v2d_hom_textures &v ) : u(  v.u ), v(  v.v ), w(  v.w ) {}    // copy constructor

    // utility functions ======================================================
    T mag()                 { return sqrt( u * u + v * v );                                                 }    // returns magnitude (length)
    T length()              { return sqrt( u * u + v * v );                                                 }    // returns length (magnitude)
    T mag2()                { return       u * u + v * v  ;                                                 }    // returns squared magnitude
    T length2()             { return       u * u + v * v  ;                                                 }    // returns squared length
    T dot(   const v2d_hom_textures &rhs ) { return                   this->u * rhs.u + this->v * rhs.v;    }    // returns the dot product
    T cross( const v2d_hom_textures &rhs ) { return v2d_hom_textures( this->u * rhs.v - this->v * rhs.u );  }    // returns the cross product
    v2d_hom_textures norm() { T r = (T)1 / mag(); return v2d_hom_textures( u * r, v * r, this->w );         }    // returns normalized version of the vector
    v2d_hom_textures reciprocal() { return v2d_hom_textures( (T)1 / this->u, (T)1 / this->v, this->w );     }    // returns element wise reciprocal

    // in namespace std, round(), floor(), ceil() and trunc() are defined for type float and double, but not for type int
    v2d_hom_textures round() { return v2d_hom_textures( std::round( u ), std::round( v ), this->w); }    // returns the vector with elements rounded
    v2d_hom_textures floor() { return v2d_hom_textures( std::floor( u ), std::floor( v ), this->w); }    // returns the vector with elements floor'ed
    v2d_hom_textures ceil()  { return v2d_hom_textures( std::ceil(  u ), std::ceil(  v ), this->w); }    // returns the vector with elements ceil'ed
    v2d_hom_textures trunc() { return v2d_hom_textures( std::trunc( u ), std::trunc( v ), this->w); }    // returns the vector with elements trunc'ed
    // std::min() and std::max() are defined for all types where the "<" operator is defined
    v2d_hom_textures max(   const v2d_hom_textures &rhs ) { return v2d_hom_textures( std::max( u, rhs.u ), std::max( v, rhs.v ), std::max( w, rhs.w )); }    // element-wise max
    v2d_hom_textures min(   const v2d_hom_textures &rhs ) { return v2d_hom_textures( std::min( u, rhs.u ), std::min( v, rhs.v ), std::min( w, rhs.w )); }    // element-wise min
    v2d_hom_textures clamp( const v2d_hom_textures &low, const v2d_hom_textures &hgh ) { return this->max( low ).min( hgh ); }          // element-wise clamp

    // basic operator overloading =============================================
    v2d_hom_textures operator + ( const v2d_hom_textures &rhs ) { return v2d_hom_textures( this->u + rhs.u, this->v + rhs.v ); }    // element wise add
    v2d_hom_textures operator - ( const v2d_hom_textures &rhs ) { return v2d_hom_textures( this->u - rhs.u, this->v - rhs.v ); }    // element wise subtract
    v2d_hom_textures operator * ( const v2d_hom_textures &rhs ) { return v2d_hom_textures( this->u * rhs.u, this->v * rhs.v ); }    // element wise multiply
    v2d_hom_textures operator / ( const v2d_hom_textures &rhs ) { return v2d_hom_textures( this->u / rhs.u, this->v / rhs.v ); }    // element wise divide
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v2d_hom_textures operator + ( const T                 rhs ) { return v2d_hom_textures( this->u + rhs  , this->v + rhs   ); }    // scalar add
    v2d_hom_textures operator - ( const T                 rhs ) { return v2d_hom_textures( this->u - rhs  , this->v - rhs   ); }    // scalar subtract
    v2d_hom_textures operator * ( const T                 rhs ) { return v2d_hom_textures( this->u * rhs  , this->v * rhs   ); }    // scalar multiply
    v2d_hom_textures operator / ( const T                 rhs ) { return v2d_hom_textures( this->u / rhs  , this->v / rhs   ); }    // scalar divide

    // more sophisticated operator overloading ================================
    // work with references to don't move things around if not necessary
    v2d_hom_textures &operator += (const v2d_hom_textures &rhs ) { this->u += rhs.u; this->v += rhs.v; return *this; }
    v2d_hom_textures &operator -= (const v2d_hom_textures &rhs ) { this->u -= rhs.u; this->v -= rhs.v; return *this; }
    v2d_hom_textures &operator *= (const v2d_hom_textures &rhs ) { this->u *= rhs.u; this->v *= rhs.v; return *this; }
    v2d_hom_textures &operator /= (const v2d_hom_textures &rhs ) { this->u /= rhs.u; this->v /= rhs.v; return *this; }
    // NOTE: you can add (subtract, multiply, divide) a vector and a scalar, but it must be in that order!!
    v2d_hom_textures &operator += (const T                 rhs ) { this->u += rhs  ; this->v += rhs  ; return *this; }
    v2d_hom_textures &operator -= (const T                 rhs ) { this->u -= rhs  ; this->v -= rhs  ; return *this; }
    v2d_hom_textures &operator *= (const T                 rhs ) { this->u *= rhs  ; this->v *= rhs  ; return *this; }
    v2d_hom_textures &operator /= (const T                 rhs ) { this->u /= rhs  ; this->v /= rhs  ; return *this; }

    // overloading of array reference operator ================================
    // must be a reference, otherwise you can't assign to it
    // NOTE: programmer must guarantee that index i is in bounds!! (no error checking)
    T &operator [] ( std::size_t i ) { return *((T *)this + i); }

    // comparison operators - needed for STL container manipulation ================================
    // (for instance std::unique(), std::sort(), ...)
    bool operator == (const v2d_hom_textures &rhs ) const { return (this->u == rhs.u && this->v == rhs.v && this->w == rhs.w); }
    bool operator != (const v2d_hom_textures &rhs ) const { return (this->u != rhs.u || this->v != rhs.v || this->w != rhs.w); }
    bool operator <  (const v2d_hom_textures &rhs ) const {
        return ( this->u <  rhs.u) ||
               ((this->u == rhs.u) && (( this->v <  rhs.v) ||
                                       ((this->v == rhs.v) && (this->w < rhs.w))));
    }
    bool operator >  (const v2d_hom_textures &rhs ) const {
        return ( this->u >  rhs.u) ||
               ((this->u == rhs.u) && (( this->v >  rhs.v) ||
                                       ((this->v == rhs.v) && (this->w > rhs.w))));
    }

    // casting - to enable mingling of different native templated types ================================
    // NOTE: inspired by JavidX9 PGE code (thanks :))
    operator v2d_hom_textures<int      >() const { return { static_cast<int      >(this->u), static_cast<int      >(this->v), static_cast<int      >(this->w) }; }
    operator v2d_hom_textures<float    >() const { return { static_cast<float    >(this->u), static_cast<float    >(this->v), static_cast<float    >(this->w) }; }
    operator v2d_hom_textures<double   >() const { return { static_cast<double   >(this->u), static_cast<double   >(this->v), static_cast<double   >(this->w) }; }
    operator v2d_hom_textures<long long>() const { return { static_cast<long long>(this->u), static_cast<long long>(this->v), static_cast<long long>(this->w) }; }
};

// create convenient shorthand types, using typedef
// be aware that not all utility functions or operators have meaning for all types...
typedef v2d_hom_textures<int      > vi2dt;    // will this type ever be needed?
typedef v2d_hom_textures<float    > vf2dt;
typedef v2d_hom_textures<double   > vd2dt;
typedef v2d_hom_textures<long long> vllong2dt;

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // INCLUDE_VECTOR_TYPES
