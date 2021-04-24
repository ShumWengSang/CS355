template< typename T>
void move( T data[5], int i, int j ) 
{ 
    T p = data[i];
    while( i<j ) { 
        data[i] = data[i+1]; 
        ++i; 
    } 
    while( i>j ) { 
        data[i] = data[i-1]; 
        --i; 
    } 
    data[i] = p; 
} 
template< typename T>
void move_pointers( T* data[5], int i, int j ) 
{ 
    T p = *data[i];
    while( i<j ) { 
        *data[i] = *data[i+1]; 
        ++i; 
    } 
    while( i>j ) { 
        *data[i] = *data[i-1]; 
        --i; 
    } 
    *data[i] = p; 
} 

template< typename T>
void quicksort_base_2( T *data ) 
{
    if( data[1] < data[0] ) { 
        std::swap(data[0], data[1]); 
    } 
    // 0<1
}

template< typename T>
void quicksort_base_3( T *data ) 
{
    if( data[1] < data[0] ) { std::swap(data[0], data[1]); } // 0<1 
    if( data[2] < data[0] ) { std::swap(data[0], data[2]); } // 0<1, 0<2
    if( data[2] < data[1] ) { std::swap(data[1], data[2]); } // 0<1<2
}

template< typename T>
void quicksort_base_4( T *data ) 
{
    if( data[1] < data[0] ) { 
        std::swap(data[0], data[1]); 
    } 
    // 0<1 

    if( data[3] < data[2] ) { 
        std::swap(data[2], data[3]); 
    } 
    // 0<1, 2<3 

    if( data[2] < data[0] ) { 
        std::swap(data[0], data[2]); 
        std::swap(data[1], data[3]); 
    } 
    // 0<1, 0<2<3 

    if( data[3] < data[1] ) { 
        move(data, 1, 3); 
        // 0<1<2<3
    } else { 
        // 0<1<3, 0<2<3
        if( data[2] < data[1] ) { 
            std::swap(data[1], data[2]); 
        } 
        // 0<1<2<3
    } 
    // 0<1<2<3
}
template< typename T>
void quicksort_base_4_pointers( T *data[] ) 
{
    if( *data[1] < *data[0] ) { 
        std::swap(*data[0], *data[1]); 
    } 
    // 0<1 

    if( *data[3] < *data[2] ) { 
        std::swap(*data[2], *data[3]); 
    } 
    // 0<1, 2<3 

    if( *data[2] < *data[0] ) { 
        std::swap(*data[0], *data[2]); 
        std::swap(*data[1], *data[3]); 
    } 
    // 0<1, 0<2<3 

    if( *data[3] < *data[1] ) {
        move_pointers(data, 1, 3);
        // 0<1<2<3
    } else { 
        // 0<1<3, 0<2<3
        if( *data[2] < *data[1] ) { 
            std::swap(*data[1], *data[2]); 
        } 
        // 0<1<2<3
    } 
    // 0<1<2<3
}

template< typename T>
void quicksort_base_5_pointers( T *data ) 
{
    if( *data[1] < *data[0] ) { 
        std::swap(*data[0], *data[1]); 
    } 
    // 0<1 

    if( *data[3] < *data[2] ) { 
        std::swap(*data[2], *data[3]); 
    } 
    // 0<1, 2<3 

    if( *data[2] < *data[0] ) { 
        std::swap(*data[0], *data[2]); 
        std::swap(*data[1], *data[3]); 
    } 
    // 0<1, 0<2<3 

    if( *data[4] < *data[2] ) {
        // 0<1, 0<2<3, 4<2 
        move_pointers(data, 4, 2); 
        // 0<1, 0<3<4, 2<3<4 
        if( *data[2] < *data[0] ) { 
            std::swap(*data[0], *data[2]); 
        } 
        // 0<1, 0<2<3<4 
    } else { 
        // 0<1, 0<2<3, 2<4 
        if( *data[4] < *data[3] ) { 
            std::swap(*data[3], *data[4]); 
        } 
        // 0<1, 0<2<3<4 
    } 
    // 0<1, 0<2<3<4 

    if( *data[3] < *data[1] ) { 
        move_pointers(data, 1, 3); 
        // 0<1<2<3, 2<4 
        if( *data[4] < *data[3] ) { 
            std::swap(*data[3], *data[4]); 
        } 
        // 0<1<2<3<4 
    } else { 
        // 0<1<3, 0<2<3<4 
        if( *data[2] < *data[1] ) { 
            std::swap(*data[1], *data[2]); 
        } 
        // 0<1<2<3<4 
    } 
    // 0<1<2<3<4 
}
template< typename T>
void quicksort_base_5( T *data ) 
{
    if( data[1] < data[0] ) { 
        std::swap(data[0], data[1]); 
    } 
    // 0<1 

    if( data[3] < data[2] ) { 
        std::swap(data[2], data[3]); 
    } 
    // 0<1, 2<3 

    if( data[2] < data[0] ) { 
        std::swap(data[0], data[2]); 
        std::swap(data[1], data[3]); 
    } 
    // 0<1, 0<2<3 

    if( data[4] < data[2] ) {
        // 0<1, 0<2<3, 4<2 
        move(data, 4, 2); 
        // 0<1, 0<3<4, 2<3<4 
        if( data[2] < data[0] ) { 
            std::swap(data[0], data[2]); 
        } 
        // 0<1, 0<2<3<4 
    } else { 
        // 0<1, 0<2<3, 2<4 
        if( data[4] < data[3] ) { 
            std::swap(data[3], data[4]); 
        } 
        // 0<1, 0<2<3<4 
    } 
    // 0<1, 0<2<3<4 

    if( data[3] < data[1] ) { 
        move(data, 1, 3); 
        // 0<1<2<3, 2<4 
        if( data[4] < data[3] ) { 
            std::swap(data[3], data[4]); 
        } 
        // 0<1<2<3<4 
    } else { 
        // 0<1<3, 0<2<3<4 
        if( data[2] < data[1] ) { 
            std::swap(data[1], data[2]); 
        } 
        // 0<1<2<3<4 
    } 
    // 0<1<2<3<4 
}

