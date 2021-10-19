# netlist_converter
Program for visualization of spice netlists and conversion to visual format (.asc understandable by LTSpice).

## Building and running UTs

    mkdir build
    cd build
    cmake .. (or cmake .. -DCMAKE_BUILD_TYPE=Debug for verbose logging turned on)
    make run_uts

## Building and running app
Current output of app is svg file containing drawn layout of parsed netlist.  
Directory *example* contains example netlists that can be successfully parsed.

    mkdir build
    cd build
    cmake .. (or cmake .. -DCMAKE_BUILD_TYPE=Debug for verbose logging turned on)
    make app
    ./app <file containing netlist>

