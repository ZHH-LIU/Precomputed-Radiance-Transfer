#include "SH.h"
#include <math.h>

const float PI = 3.14159265359;

float P(int l, int m, float x)
{
    //l=m
    float Pmm = 1.0;

    if (m > 0)
    {
        float item = sqrt(1.0 - x * x);
        for (int i = 1; i <= m; i++)
        {
            Pmm *= item * (2.0 * i - 1.0) * (-1.0);
        }
    }

    if (l == m)
        return Pmm;

    //l=m+1
    float Pmp1m = (2.0 * m + 1.0) * x * Pmm;
    if (l == m + 1)
        return Pmp1m;

    //l>m+1
    float Plm = 1.0;
    for (int i = m + 2; i <= l; i++)
    {
        Plm = x * (2.0 * i - 1.0) * Pmp1m - (i + m - 1.0) * Pmm;
        Plm /= (i - m);
        Pmm = Pmp1m;
        Pmp1m = Plm;
    }
    return Plm;
}

float K(int l, int m)
{
    float item = (2.0 * l + 1.0) / (4.0 * PI) * Factorial(l - m) / Factorial(l + m);
    return sqrt(item);
}

float SH(int l, int m, float theta, float phi)
{
    if (m == 0)
        return K(l, m) * P(l, m, cos(theta));

    if (m > 0)
        return K(l, m) * P(l, m, cos(theta)) * sqrt(2.0) * cos(m * phi);

    return K(l, -m) * P(l, -m, cos(theta)) * sqrt(2.0) * sin(-m * phi);
}

int Factorial(int n)
{
    if (n <= 1)
        return 1;

    int k = n;
    int fact = n;
    while (--k > 1)
    {
        fact *= k;
    }
    return fact;
}
