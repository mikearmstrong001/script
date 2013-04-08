#ifndef ER_HEADER_SEARCH_H
#define ER_HEADER_SEARCH_H

template <class T, class C, class S >
   int BinarySearch(T *a, int n, const C &target, const S &compare ) {
       int first = 0;
       int last = n-1;
       int mid;
       while(first<=last) {
           mid = (first+last)/2;

           if( compare.Equal( a[mid], target ) ) {
               return mid;
           } else
           if( compare.Less( target, a[mid] ) ) {
               last = mid-1;
           } else {
               first = mid+1;
           }
       }
       return -1;
   }

template <class T, class C>
   int LinearSearch(T *a, int n, const C &target) {
	   for (int i=0; i<n; i++) {
		   if ( a[i] == target ) {
			   return i;
		   }
	   }
	   return -1;
   }

#endif
