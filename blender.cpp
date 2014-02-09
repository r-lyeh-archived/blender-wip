// blender - An agnostic keyframe interpolation and animation controller. MIT licensed.
// - rlyeh ~~ listening to Jeremy Irons & the Ratgang Malibus / Elefanta

// - play anim forward
// - play anim backward
// - freeze
// - reverse
// - reposition animation

#pragma once

#include <string>
#include <deque>
#include <map>
#include <algorithm>
#include <set>

// animation nodes, sound, video
using dt = int;
using key = unsigned;
using frame = std::string;
using data = std::string;
struct keyframe {
    key first;
    frame second;
};
struct keydata {
    key first;
    data second;
};
using animation = std::deque< keyframe >;
using datatrack = std::deque< keydata >;

// tools
template<typename T>
key key_of( const T &kf ) {
    return kf.first;
}
key min_key_of( const animation &anim ) {
    key k = ~0;
    for( auto &in : anim ) {
        k = std::min( k, key_of(in) );
    }
    return k;
}
key max_key_of( const animation &anim ) {
    key k = 0;
    for( auto &in : anim ) {
        k = std::max( k, key_of(in) );
    }
    return k;
}
frame frame_of( const keyframe &kf ) {
    return kf.second;
}
data data_of( const keydata &kd ) {
    return kd.second;
}

keyframe &invalid_kf() {
    static keyframe kf;
    return kf = keyframe( {0, "#invalid#"} );
}
keydata &invalid_kd() {
    static keydata kd;
    return kd = keydata( {0, "#invalid#"} );
}

keyframe &frame_at( animation &an, int pos ) {
    signed size = signed( an.size() );
    if( size )
        return an[ ( pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ) ];
    static std::map< const animation *, keyframe > map;
    return ( ( map[ &an ] = map[ &an ] ) = invalid_kf() );
}
keydata &track_at( datatrack &tr, int pos ) {
    signed size = signed( tr.size() );
    if( size )
        return tr[ ( pos >= 0 ? pos % size : size - 1 + ((pos+1) % size) ) ];
    static std::map< const datatrack *, keydata > map;
    return ( ( map[ &tr ] = map[ &tr ] ) = invalid_kd() );
}

unsigned distance( const key &k1, const key &k2 ) {
    return k1 < k2 ? k2 - k1 : k1 - k2;
}
unsigned maxdistance( const animation &anim ) {
    if( anim.empty() )
        return 0;
    key min = ~0, max = 0;
    for( auto &kf : anim ) {
        min = std::min( min, kf.first );
        max = std::max( max, kf.first );
    }
    return max - min;
}

frame blend( const frame &f1, const frame &f2 ) {
    return f1 + f2;
}

#include <iostream>

// reverse flow of anim
void reverse( animation &anim ) {
    // 4 5 0 1 2 3 -> 4 3 2 1 0 5
    // 3 4 0 1 2   -> 3 2 1 0 4
    if( anim.size() > 2 ) {
        std::reverse( anim.begin() + 1, anim.end() );
    }
}
bool is_forward( const animation &anim ) {
    // 0 < 1 < 2 -> cforward  L L L 3L
    // 1 < 2 > 0 -> forward  L G G 1L
    // 2 > 0 < 1 -> forward  G L G 1L
    // 0 < 2 > 1 -> backward L G L 2L
    // 2 > 1 > 0 -> backward G G G 0L
    // 1 > 0 < 2 -> backward G L L 2L
    if( anim.size() > 2 ) {
        key k1 = key_of( *(anim.begin()+0) );
        key k2 = key_of( *(anim.begin()+1) );
        key k3 = key_of( *(anim.begin()+2) );
        int Ls = (k1 < k2) + (k2 < k3) + (k1 < k3);
        return Ls & 1;
    }
    return false;
}
bool is_backward( const animation &anim ) {
    return !is_forward( anim );
}
void forward( animation &anim ) {
    if( is_backward(anim) ) {
        reverse( anim );
    }
}
void backward( animation &anim ) {
    if( is_forward(anim) ) {
        reverse( anim );
    }
}
void cycle( animation &anim, unsigned steps = 1 ) {
    if( anim.size() ) {
        for( unsigned step = 0; step < steps; ++step ) {
            anim.push_back( anim.front() );
            anim.pop_front();
        }
    }
}
void reposition( animation &anim, key index ) {
    if( index < anim.size() ) {
        while( anim.begin()->first != index ) {
            cycle( anim );
        }
    }
}
void restart( animation &anim ) {
    reposition( anim, 0 );
}
bool check_continuity( animation &anim ) {
    if( anim.size() > 2 )
        return true;
    key k1 = key_of( anim.at(0) );
    key k2 = key_of( anim.at(1) );
    if( distance(k1,k2) <= 1 || distance(k1,k2) == maxdistance(anim) )
        return true;
    std::cout << "continuity failed!" << std::endl;
    return false;
}

#include <windows.h>
#include <iomanip>
#pragma comment(lib, "user32.lib")

int delay = 160;

void render( const animation &anim ) {
    const keyframe &kf = anim.at(0);
    #define $right(N) \
    (std::cout.width(N), std::right)
    std::cout << "   current frame[ ";
    std::cout << $right(3) << key_of(kf) << "." << frame_of(kf) << " ] << next[ ";
    for( unsigned it = 1; it < anim.size(); ++it ) {
        const keyframe &kf = anim.at(it);
        std::cout << $right(3) << key_of(kf) << "." << $right(3) << frame_of(kf) << ",";
    }
    std::cout << "],";
    std::cout << "is_forward=" << is_forward(anim) << ',';
    std::cout << "is_backward=" << is_backward(anim) << ',';
    std::cout << "delay=" << $right(4) << delay << ',';
    std::cout << "blending_weight=" << $right(3) << key_of(kf) - int(key_of(kf)) << ',';
    std::cout << std::string( 10, ' ' ) << '\r';
}

#include <functional>

#define cmd_pattern()

struct controller {
    animation anim;
    datatrack data;

    std::function< void(animation *) > on_begin, on_end;
    std::map< void *, std::function< void(animation &) > > cmd;

    controller() {
        on_begin =
        []( animation * )
        {};
        on_end =
        []( animation * )
        {};
    }

    void update() {

        for( auto &call : cmd ) {
            call.second(anim);
        }
        cmd.clear();

        if( key_of(anim.front()) == min_key_of(anim) ) {
            on_begin( &anim );
        }
        if( key_of(anim.front()) == max_key_of(anim) ) {
            on_end( &anim );
        }
    }

    void cycle( int steps = 1 ) {
        using std::placeholders::_1;
        cmd[ ::cycle ] = std::bind(::cycle,_1,steps);
    }

    void render() {
        using std::placeholders::_1;
        cmd[ ::render ] = std::bind(::render,_1);
    }
};

#define $yes(...) __VA_ARGS__
#define $no(...)

#define $classic      $no
#define $controller   $yes

// 100 ms -> 3 frames (@30Hz)
// 3 frames -> 100 ms
int ms2frames( int ms ) {
    return ms / 33;
}
int frames2ms( int frames ) {
    return 33.334 * frames;
}

int main() {

    std::cout << "left|right: flow, up|down: speed, space: pause" << std::endl;

    animation an;
    an.push_back( keyframe({ frames2ms(0), "|" }) );
    an.push_back( keyframe({ frames2ms(1), "/" }) );
    an.push_back( keyframe({ frames2ms(2), "-" }) );
    an.push_back( keyframe({ frames2ms(3),"\\" }) );

    controller player;
    player.anim = an;
    player.on_begin = []( animation *anim ) {
        std::cout << "\r:)\r";
    };
    player.on_end = []( animation *anim ) {
        std::cout << "\r:D\r";
    };

    for( ;; ) {

        animation &ani = $classic(an) $controller(player.anim);

        $classic(
            render( ani );
            check_continuity( ani );
        )
        $controller(
            player.render();
        )

        for( int i = 0; i < delay; ++i ) {

            Sleep(1);

            if( GetAsyncKeyState(VK_DOWN) & 0x8000 ) {
                if( --delay < 1 ) delay = 1;
            }
            else
            if( GetAsyncKeyState(VK_UP) & 0x8000 ) {
                if( ++delay > 10000 ) delay = 10000;
            }

            if( GetAsyncKeyState(VK_ESCAPE) & 0x8000 ) {
                restart( ani );
            }
            if( GetAsyncKeyState(VK_LEFT) & 0x8000 ) {
                backward( ani );
            }
            if( GetAsyncKeyState(VK_RIGHT) & 0x8000 ) {
                forward( ani );
            }
        }

        if( GetAsyncKeyState(VK_SPACE) & 0x8000 ) {
            // freeze classic
            $classic( cycle( an, 0 );  )
            $controller( player.cycle( 0 ); )
        } else {
            // cycle classic
            $classic( cycle( an, 1 );  )
            $controller( player.cycle( 1 ); )
        }

        player.update();
    }
}
