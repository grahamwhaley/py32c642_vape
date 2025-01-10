// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley

#ifndef _DIGIT_H
#define _DIGIT_H

#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 

void digit_show(int n);
void digit_battery(bool show);
void digit_teardrop(bool show);
void digit_percentage(bool show);

#ifdef __cplusplus
}
#endif

#endif /* _DIGIT_H */
