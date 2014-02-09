#include <string>

namespace blend {

    template< typename T >
    T mix( const T &a, const T &b, float dt01 ) {
        if( dt01 <= 0.f )
            return a;
        if( dt01 >= 1.f )
            return b;
        return a * (1 - dt01) + b * dt01;
    }

    template<>
    std::string mix( const std::string &a, const std::string &b, float dt01 ) {
        return dt01 < 0.5f ? a : b;
    }
    template<>
    bool mix( const bool &a, const bool &b, float dt01 ) {
        return dt01 < 0.5f ? a : b;
    }

    // containers
    template< typename C >
    typename C::value_type at( const C &t, float posf ) {
        if( t.empty() )
            return C::value_type();
        if( t.size() == 1 )
            return t.at(0);
        if( posf <= 0 )
            return t.at(0);
        if( posf >= t.size() - 1 )
            return *t.rbegin();

        int pos = int(posf);

        const C::value_type &a = t.at( pos );
        const C::value_type &b = t.at( pos + 1 );

        return mix( a, b, posf - pos );
    }

    template<typename C>
    C resample( const C &t, unsigned resize ) {
        C o(resize);
        // in-betweeners
        for( unsigned it = 0, end = resize; it < end; ++it ) {
            o[ it ] = at(t, ( t.size() - 1 ) * it/float(end - 1) );
        }
        // overwrite keyframes with original values
        for( unsigned it = 0, end = t.size(); it < end; ++it ) {
            o[ it * (o.size()-1) / float(t.size()-1) ] = t[ it ];
        }
        return o;
    }

    /*
    template<typename C>
    C weighted( const C &t, const std::vector<float> &weights ) {
    }
    */
}

#include <iostream>
#include <vector>

template<typename T>
void print( const T &t ) {
    std::cout << t << std::endl;
}

template<>
void print( const std::vector<int> &t ) {
    for( auto &it : t )
        std::cout << it << ',';
    if( !t.empty() ) {
        std::cout << std::endl;
    }
}

#include <string>

namespace strings {
    std::string a = "hello", b = "world";
}

namespace sizes {
    size_t a = 0, b = 75;
}

namespace doubles {
    double a = -100, b = 0;
}

namespace booleans {
    bool a = false, b = true;
}

int main() {
    using namespace blend;

    print( mix( strings::a, strings::b, 0.5f ) );
    print( mix( doubles::a, doubles::b, 0.5f ) );
    print( mix( sizes::a, sizes::b, 0.5f ) );
    print( "---" );
    print( mix( booleans::a, booleans::b, 0.f ) );
    print( mix( booleans::a, booleans::b, 0.20f ) );
    print( mix( booleans::a, booleans::b, 0.25f ) );
    print( mix( booleans::a, booleans::b, 0.33f ) );
    print( mix( booleans::a, booleans::b, 0.5f ) );
    print( mix( booleans::a, booleans::b, 0.66f ) );
    print( mix( booleans::a, booleans::b, 0.75f ) );
    print( mix( booleans::a, booleans::b, 0.80f ) );
    print( mix( booleans::a, booleans::b, 1.f ) );
    print( "---" );
    std::vector<int> t( {10,20,40,20,10} );
    print( at(t,0) );
    print( at(t,1) );
    print( at(t,0.25) );
    print( at(t,0.5) );
    print( at(t,0.75) );
    print( "---" );
    print( at(t,2.5) );
    print( "---" );
    print( resample(t,16) );
}
