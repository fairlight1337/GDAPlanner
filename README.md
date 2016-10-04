Goal Directed Action Planner
===

Compile:
```bash
mkdir build
cd build
cmake ..
make
make test
```

Run (must be run from within `bin` at the moment):
```bash
cd bin
./GDAPlanner-bin
```


Included Examples
---

 * `data/openstacks-example.pddl`: OpenStacks example written in PDDL


Regenerating the Doxygen Documentation
---

To regenerate the Doxygen documentation, you will need Doxygen:
```bash
sudo apt-get install doxygen
```

From the project's root directory, rrun Doxygen like so:
```bash
doxygen doc/Doxyfile
```

This will generate a `doc/html` directory, containing all source code
documentation. View it starting from the `index.html` contained therein:
```bash
google-chrome doc/html/index.html
```
