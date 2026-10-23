#pragma once
// Substituto minimo do <D3dx9math.h> (DirectX SDK de 2010, descontinuado).
//
// O projeto usa quatro funcoes e quatro tipos, mais os operadores que o D3DX
// fornecia (+, -, +=, -=, ==, != entre vetores, e ! em matriz para testar zero).
// Os campos sao os mesmos do D3DX (_11.._44, x/y/z/w), entao declarar os tipos
// com o mesmo layout mantem todo o codigo existente intacto.
//
// Semantica conferida contra a documentacao do D3DX:
//   Transpose     - troca linhas por colunas
//   Normalize     - divide pelo comprimento; se for ~0, devolve (0,0,0)
//   Vec3Transform - ponto (w=1): aplica a matriz, a ultima LINHA translada
//                   x' = x*_11 + y*_21 + z*_31 + _41
// A saida de Vec3Transform e D3DXVECTOR4 porque e isso que o codigo espera
// (GameClasses.hpp:355) - o w resultante nao e usado.
#include <cmath>

struct D3DXVECTOR4;

struct D3DXVECTOR2 {
    float x, y;
    D3DXVECTOR2( ) : x( 0 ) , y( 0 ) { }
    D3DXVECTOR2( float _x , float _y ) : x( _x ) , y( _y ) { }
    // O codigo faz if (!WorldToScreen(...)) para detectar alvo fora da tela,
    // que a funcao sinaliza com (0,0) (Esp.cpp:667, SDK.hpp:108).
    explicit operator bool( ) const { return x != 0.f || y != 0.f; }
    bool operator==( const D3DXVECTOR2& o ) const { return x == o.x && y == o.y; }
    bool operator!=( const D3DXVECTOR2& o ) const { return !( *this == o ); }
};

struct D3DXVECTOR3 {
    float x, y, z;
    D3DXVECTOR3( ) : x( 0 ) , y( 0 ) , z( 0 ) { }
    D3DXVECTOR3( float _x , float _y , float _z ) : x( _x ) , y( _y ) , z( _z ) { }
    D3DXVECTOR3 operator+( const D3DXVECTOR3& o ) const { return D3DXVECTOR3( x + o.x , y + o.y , z + o.z ); }
    D3DXVECTOR3 operator-( const D3DXVECTOR3& o ) const { return D3DXVECTOR3( x - o.x , y - o.y , z - o.z ); }
    D3DXVECTOR3& operator+=( const D3DXVECTOR3& o ) { x += o.x; y += o.y; z += o.z; return *this; }
    D3DXVECTOR3& operator-=( const D3DXVECTOR3& o ) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    bool operator==( const D3DXVECTOR3& o ) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=( const D3DXVECTOR3& o ) const { return !( *this == o ); }
};

struct D3DXVECTOR4 {
    float x, y, z, w;
    D3DXVECTOR4( ) : x( 0 ) , y( 0 ) , z( 0 ) , w( 0 ) { }
    D3DXVECTOR4( float _x , float _y , float _z , float _w ) : x( _x ) , y( _y ) , z( _z ) , w( _w ) { }
};

// Mesmo layout do D3DXMATRIX: 16 floats, _11.._44 na ordem linha/coluna.
struct D3DXMATRIX {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;

    // O codigo original faz if (!matriz) apos ler memoria, para checar leitura
    // vazia (SDK.hpp:173,176). Sem isso o compilador recusa o operador.
    explicit operator bool( ) const {
        return _11 != 0.f || _12 != 0.f || _13 != 0.f || _14 != 0.f ||
               _21 != 0.f || _22 != 0.f || _23 != 0.f || _24 != 0.f ||
               _31 != 0.f || _32 != 0.f || _33 != 0.f || _34 != 0.f ||
               _41 != 0.f || _42 != 0.f || _43 != 0.f || _44 != 0.f;
    }
    bool operator==( const D3DXMATRIX& o ) const {
        for ( int i = 0; i < 16; i++ )
            if ( ( &_11 )[ i ] != ( &o._11 )[ i ] ) return false;
        return true;
    }
    bool operator!=( const D3DXMATRIX& o ) const { return !( *this == o ); }
};

inline void D3DXMatrixTranspose( D3DXMATRIX* out , const D3DXMATRIX* in ) {
    D3DXMATRIX t = *in;
    t._11 = in->_11; t._12 = in->_21; t._13 = in->_31; t._14 = in->_41;
    t._21 = in->_12; t._22 = in->_22; t._23 = in->_32; t._24 = in->_42;
    t._31 = in->_13; t._32 = in->_23; t._33 = in->_33; t._34 = in->_43;
    t._41 = in->_14; t._42 = in->_24; t._43 = in->_34; t._44 = in->_44;
    *out = t;
}

inline void D3DXVec3Normalize( D3DXVECTOR3* out , const D3DXVECTOR3* v ) {
    float len = std::sqrt( v->x * v->x + v->y * v->y + v->z * v->z );
    if ( len < 1e-6f ) { *out = D3DXVECTOR3( 0.f , 0.f , 0.f ); return; }
    out->x = v->x / len; out->y = v->y / len; out->z = v->z / len;
}

// Duas sobrecargas: o projeto chama com saida D3DXVECTOR3 (Aimbot) e D3DXVECTOR4
// (GameClasses.hpp:355). O D3DX original aceitava D3DXVECTOR4, mas aqui as duas
// convivem sem mudar nenhum chamador.
inline void D3DXVec3Transform( D3DXVECTOR3* out , const D3DXVECTOR3* v , const D3DXMATRIX* m ) {
    float x = v->x * m->_11 + v->y * m->_21 + v->z * m->_31 + m->_41;
    float y = v->x * m->_12 + v->y * m->_22 + v->z * m->_32 + m->_42;
    float z = v->x * m->_13 + v->y * m->_23 + v->z * m->_33 + m->_43;
    out->x = x; out->y = y; out->z = z;
}

inline void D3DXVec3Transform( D3DXVECTOR4* out , const D3DXVECTOR3* v , const D3DXMATRIX* m ) {
    out->x = v->x * m->_11 + v->y * m->_21 + v->z * m->_31 + m->_41;
    out->y = v->x * m->_12 + v->y * m->_22 + v->z * m->_32 + m->_42;
    out->z = v->x * m->_13 + v->y * m->_23 + v->z * m->_33 + m->_43;
    out->w = v->x * m->_14 + v->y * m->_24 + v->z * m->_34 + m->_44;
}
