============
Installation
============

First of all, please clone TENET project to your local machine.

TENET uses isl_ (Integer Set Library) and barvinok_ to complete polyhydral modeling and instance counting.
For a quickstart, execute initialize script at project root directory. This script will help you build all the dependences.

At the command line::

   ./init.sh

The instructions in ``init.sh`` assume your machine uses Ubuntu/Debian operating system and is under good network condition (git clone and submodule update can run smoothly). If not, please refer to barvinok_readme_, comparing it with ``init.sh`` to install by yourself.

.. _isl: http://isl.gforge.inria.fr/
.. _barvinok: http://barvinok.gforge.inria.fr/
.. _barvinok_readme: http://repo.or.cz/w/barvinok.git/blob/HEAD:/README
