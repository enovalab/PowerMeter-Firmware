#include <iostream>
#include <vector>
#include <math.h>

#define M_PI 3.1515926

template<typename ContainerType>
float compensatePhaseShift(const ContainerType& container, size_t index, float calPhase)
{
    auto at = [&container](int32_t index){
        size_t size = container.size();
        index %= static_cast<int32_t>(size);
        if (index < 0)
            index += size;
        return container.at(index);
    };
    
    index += static_cast<int32_t>(calPhase);
    size_t indexNext = index + 1 ;
    float weightA = calPhase - static_cast<int32_t>(calPhase);
    if(weightA == 0)
        return at(index);
    float weightB = 1 - weightA;
    float valueA = at(index);
    float valueB = at(indexNext);
    return valueA * weightB + valueB * weightA;
}

double degreesToRadians(double degrees) 
{
    return degrees * M_PI / 180.0;
}

int main()
{
    std::vector<float> values;
    for(size_t i = 0; i < 360; i++)
    {
        float newValue = sin(degreesToRadians(i));
        values.push_back(newValue);
        // std::cout << newValue << std::endl;
    }
    std::cout << std::endl;

    for(size_t i = 0; i < values.size(); i++)
    {
        std::cout << compensatePhaseShift<std::vector<float>>(values, i, 45.2) << std::endl;    }
}