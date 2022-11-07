[![arduino-library-badge](https://www.ardu-badge.com/badge/StreamAverage.svg?)](https://www.ardu-badge.com/StreamAverage)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/samuel-lehnen/library/StreamAverage.svg)](https://registry.platformio.org/libraries/samuel-lehnen/StreamAverage)

# StreamAverage

Single Header C++ / Arduino Stream-Average-Library

**Disclaimer:** This is not a moving average, instead it accumulates the average of the
values you add

## Installation
#### PlatformIO
Go to PlatformIO Home, click on Libraries and search for `StreamAverage`


#### Arduino Library Manager
Open the library manager by clicking on `Tools > Manage Libraries` and search for `StreamAverage`. Then click install.

#### From Github
In this Repository go to [Releases](https://github.com/plsTrustMeImAnEngineer/StreamAverage/releases). There choose the Version you like an download the library as `.zip` just the `.h` file.

## Documentation

First create a `StreamAverage` object of your desired type by passing as template argument.
```cpp
StreamAverage<float> average;
```


You cann get values into the average by using the `add()` method
```cpp
average.add(newValue);
```
or the shift operator
```cpp
average << newValue;
```

Both methods return the new value of the average, including the `newValue`
```cpp
float newAverage;
newAverage = average.add(2);
newAverage = average << 4;
// new Average now contains 3
```


You can get the current average using `get()`
```cpp
float currentAverage = average.get();
```
or get it implicitly
```cpp
float currentAverage = average;
```


You can get the maximum and minimum value the average holds, by calling `getMax()` and `getMin()`
```cpp
average << 5.5;
average << 43.27;
average << -19.3;
average << 0.02;
float maximum = average.getMax(); // 43.27
float minimum = average.getMin(); // -19.3 
```


To find out how many values have been added to the average, use `getNumValues()`
```cpp
size_t num = average.getNumValues();
```


After calling the `reset()` method, `get()`, `getMax()`, `getMin()`, `getNumValues()` will evalueate to 0
```cpp
average.reset();
```
