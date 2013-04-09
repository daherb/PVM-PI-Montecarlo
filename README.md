PVM-PI-Montecarlo
=================

A small PVM3 demo computing pi with the Montecarlo method (http://en.wikipedia.org/wiki/Monte_Carlo_method)
in multiple threads. Requires the PVM3 framework (http://www.netlib.org/pvm3/index.html) and can spawn
the threads across the network on configured hosts
 - master: spawns the slaves and collects the results
 - slave: does the computation part
