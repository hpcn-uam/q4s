#include "Q4SMathUtils.h"

#include <algorithm>

float EMathUtils_median( std::vector < float > &vector)
{
    float median = 0.f;

    std::vector < float > vectorToSort (vector);

    std::sort( vectorToSort.begin(), vectorToSort.end());

    int size = vectorToSort.size();

    if (size > 0 )
    {
        // size is even
        if ( size%2 == 0 )
        {
            median = ( vectorToSort[size/2] + vectorToSort[(size/2) - 1] ) / 2.0f;
        }
        else
        {
            median = (float)vectorToSort[size/2];
        }
    }
            
    return median;
}

float EMathUtils_median( std::vector < unsigned long > &vector)
{
    float median = 0.f;

    std::vector < unsigned long > vectorToSort (vector);

    std::sort( vectorToSort.begin(), vectorToSort.end());

    int size = vectorToSort.size();

    // size is even
    if ( size%2 == 0 )
    {
        median = ( vectorToSort[size/2] + vectorToSort[(size/2) - 1] ) / 2.0f;
    }
    else
    {
        median = (float)vectorToSort[size/2];
    }
            
    return median;
}

float EMathUtils_mean( std::vector < float > &vector)
{
    float mean = 0;

    float total = 0;

    std::for_each(vector.begin(), vector.end(), [&](float element) {
        total += element;
    });

    mean = total/(float)vector.size();
            
    return mean;
}

float EMathUtils_mean( std::vector < unsigned long > &vector)
{
    float mean = 0;

    float total = 0;

    std::for_each(vector.begin(), vector.end(), [&](float element) {
        total += element;
        //printf("ELEMENTO: %f\n", element);
    });

    mean = total/(float)vector.size();
            
    return mean;
}