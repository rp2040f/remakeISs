# Measure.cpp

### `measures.get_mode_regular_measure()`
this function read params for a regular measure and init the regular measure , error code:
- -1 : period never read
- -2 : sensorID never read
- -3 : period don't satisfy the conditions
- -4 : sensorID is not valid
- 1 : success

### `measure.init_regular_measure()`
initialize a regular measure by setting up a timer, error code:
- -1 : max number of measures
- -2 : found a free timer but couldn't set the timer ( freq or timer problem)
