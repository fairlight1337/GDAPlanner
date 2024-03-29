Goal Directed Action Planner
===

To compile the software package, run this:
```bash
mkdir build
cd build
cmake ..
make
```

Run (must be run from within `bin` at the moment):
```bash
cd bin
./GDAPlanner-bin
```


Testing the code
---

To see whether a current implementation suffices all automated tests,
after compiling, run this from the `build/` directory:
```bash
make test
```


Included Examples
---

 * `data/tablesetting-domain.pddl`: Table setting example domain file written in PDDL
 * `data/tablesetting-problem.pddl`: Table setting example problem file written in PDDL (fits the accompanying example domain)
 * `data/test/*`: Numerous PDDL domain and problem definitions for testing the PDDL parser


Regenerating the Doxygen Documentation
---

To regenerate the Doxygen documentation, you will need Doxygen:
```bash
sudo apt-get install doxygen
```

From the project's root directory, run Doxygen like so:
```bash
doxygen doc/Doxyfile
```

This will generate a `doc/html` directory, containing all source code
documentation. View it starting from the `index.html` contained therein:
```bash
google-chrome doc/html/index.html
```


Online Documentation
===

Be sure to check the
[Wiki](https://github.com/fairlight1337/GDAPlanner/wiki)! There are
tutorials and further explanations of the concepts and code pieces
there.