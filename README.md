# FastSTI: Agent-based simulator of sexually transmitted infections

FastSTI is a framework for implementing agent-based models of sexually
transmitted infection epidemics. It is designed to handle up to tens of millions
of agents on a mid-range laptop. On a high performance computer it can run
dozens of large simulations or thousands of small ones in parallel.

The output of the simulations is written to CSV files, which can be further
processed in scripting languages like Python or R.

You can do sophisticated simulations without making any modifications to the
code. But if you do wish to extend the framework, it is written in C. The
framework has been designed with extensibility in mind.

FastSTI is developed and tested on GNU/Linux using the gcc compiler (and often
the Clang compiler too). It uses two portable 3rd-party libraries: the GNU
Scientific Library and GLib. The code adheres to the C11 standard and should
compile on any standards-compliant modern C compiler.

The
[documentation is on Readthedocs](https://faststi.readthedocs.io/en/latest/). It
explains how to download, install, test, and run FastSTI, as well as how to
define simulations, and modify the code.

There's a web interface to a toy version of FastSTI on
[Simhub](https://www.simhub.online/faststi/) that you can play around with to
get a feel for how it works. Here's a screenshot from Simhub:

<p align="centre">
    <img src="docs/_static/GraphScreenCaptureFromSimhub.png"
      "alt=Screen capture of FastSTI running on"
      style="max-width:95%;" />
</p>
