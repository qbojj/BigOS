Device Tree API
===============

:Author: Kamil Zdancewicz
:Date: 06-03-2026

--------
Overview
--------

The device tree library provides an interface for parsing and accessing flattened device tree (FDT) data.

-------------
API Reference
-------------

~~~~~
Types
~~~~~

.. doxygentypedef:: dt_node_t
.. doxygentypedef:: dt_prop_t

~~~~~~~~~~
Structures
~~~~~~~~~~

.. doxygenstruct:: fdt_t
    :members:

~~~~~~~~~
Functions
~~~~~~~~~

""""""""""""""
Initialization
""""""""""""""

.. doxygenfunction:: dt_init
.. doxygenfunction:: dt_reset

"""""""""""
Node Access
"""""""""""

.. doxygenfunction:: dt_get_node_by_path
.. doxygenfunction:: dt_get_node_in_subtree_by_path
.. doxygenfunction:: dt_get_node_child
.. doxygenfunction:: dt_get_node_sibling
.. doxygenfunction:: dt_get_node_name
.. doxygenfunction:: dt_get_node_name_ptr

"""""""""""""""
Property Access
"""""""""""""""

.. doxygenfunction:: dt_get_prop_by_name
.. doxygenfunction:: dt_get_first_prop
.. doxygenfunction:: dt_get_next_prop
.. doxygenfunction:: dt_get_prop_name
.. doxygenfunction:: dt_get_prop_name_ptr
.. doxygenfunction:: dt_get_prop_buffer

~~~~~~~~~~~~~~~~~~~~
Internal Definitions
~~~~~~~~~~~~~~~~~~~~

.. doxygenfile:: dt_defines.h