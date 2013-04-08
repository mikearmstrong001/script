#ifndef ER_HEADER_MISC_H
#define ER_HEADER_MISC_H

template <class T>
    void Swap( T &a, T &b ) {
        T s = a;
        a = b;
        b = s;
    }


#endif