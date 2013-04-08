#ifndef ER_HEADER_SORT_H
#define ER_HEADER_SORT_H

#include "misc.h"

template <class T>
    int QuickSort_Partition( T *a, int l, int r, int p ) {
        T v = a[p];
        Swap( a[p], a[r] );
        int s = l;
        for (int i=l; i<=r; i++) {
            if ( a[i] < v ) {
                Swap( a[i], a[s] );
                s = s + 1;
            }
        }
        Swap( a[s], a[r] );
        return s;
    }
template <class T>
   int QuickSort_Partition2( T*a, int left, int right, int p )
   {
       int first=left;
       int pivot=right--;
       while(left<=right) {
           while(a[left]<a[pivot]) {
               left++;
           }
           while((right>=first)&&(a[pivot]<a[right])) {
               right--;
           }
           if(left<right) {
               Swap(a[left],a[right]);
               left++;
           }
       }
       if(left!=pivot) {
           Swap(a[left],a[pivot]);
       }

       return left;
   }

template <class T>
   void QuickSort_r( T *a, int l, int r ) {
       if ( r > l ) {
           int p = (l+r) / 2;
           int np = QuickSort_Partition2( a, l, r, p );
           QuickSort_r( a, l, np-1 );
           QuickSort_r( a, np+1, r );
       }
   }

#if 0
   // fix equals case
template <class T>
   void QuickSort( T *a, int n ) {
       QuickSort_r( a, 0, n-1 );
   }
#endif

template <class T, class S >
   void BubbleSort( T *a, int n, S const &compare ) {
       bool swapped;
       do {
           swapped = false;
           for (int i=1; i<n; i++) {
               if ( compare.Less( a[i], a[i-1] ) ) {
                   Swap( a[i-1], a[i] );
                   swapped = true;
               }
           }
       } while ( swapped );
   }

template <class T >
   void BubbleSort( T *a, int n ) {
       bool swapped;
       do {
           swapped = false;
           for (int i=1; i<n; i++) {
               if ( a[i] < a[i-1] ) {
                   Swap( a[i-1], a[i] );
                   swapped = true;
               }
           }
       } while ( swapped );
   }

template <class T>
   void ShellSort( T *A, int size) {
       int i, j, increment, temp;
       increment = size / 2;
       while (increment > 0) {
           for (i = increment; i < size; i++) {
               j = i;
               temp = A[i];
               while ((j >= increment) && (A[j-increment] > temp)) {
                   A[j] = A[j - increment];
                   j = j - increment;
               }
               A[j] = temp;
           }

           if (increment == 2) {
              increment = 1;
           } else {
              increment = (int) (increment / 2.2);
           }
       }
   }

#endif

