/*
 * @file util.h
 * @author Akagi201
 * @date 2015/02/02
 */

#ifndef UTIL_H_
#define UTIL_H_ (1)

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define BITAT(x, n) ((x & (1 << n)) == (1 << n))

#endif // UTIL_H_
