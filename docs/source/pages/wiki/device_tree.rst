Device Tree
===========

:Author: Kamil Zdancewicz
:Date: 06-03-2026

------------
Introduction
------------

A devicetree is a data structure used to describe the hardware configuration of a computer system in a way that can be understood and managed by the operating system. 
Instead of hard-coding information about every hardware component directly into the operating system kernel,
the system's hardware layout is represented in a structured form and provided to the kernel during the boot process.

In practice, a devicetree specifies the key hardware components of a machine, such as the processor or processors, memory, buses, and integrated peripherals.
This way the kernel can correctly initialize and manage these components without needing platform-specific code compiled directly into the kernel.
Such approach is especially important in embedded systems and architectures such as **RISC-V**,
where many devices share the same processor architecture but differ in their surrounding hardware configuration.

---------------------
Device Tree Structure
---------------------

A devicetree is a hierarchical tree data structure that represents the devices present in a system. Each node in the tree corresponds to a device or a component, 
and has **property/value pairs** that describe its characteristics. 
Every node has exactly one parent, except for the root node, which has no parent.

All devicetrees must have a **root** (`/`), **exactly one** `/cpus` node and **at least one** `/memory` node.

The structure of the tree resembles a directory hierarchy, with a root node at the top and child nodes branching below. For example *(with properties omitted)*:

.. code-block:: text

    /
    ├── cpus
    │   ├── cpu@0
    │   └── cpu@1
    ├── memory@0
    ├── uart@fe001000
    ├── chosen
    └── aliases

Devices represented in a devicetree may include:

* Physical hardware devices, such as UARTs or timers
* Components of a device, for example the random-number generator inside a TPM
* Virtual devices exposed through virtualization, such as I\ :sup:`2`\ C devices attached to a remote CPU

Each node contains properties that convey important configuration information. Some commonly used standard properties include:

*   `model` - The model property value is a <string> that specifies the manufacturer's model number of the device. *Value type: <string>*
*   `status` - The status property indicates the operational status of a device. *Value type: <string>*
*   `reg` - The reg property describes the address of the device's resources within the address space defined by its parent bus. *<prop-encoded-array> encoded as an arbitrary number of (address, length) pairs*

An example of a node without children:

.. code-block:: text

    memory@0 {
    device_type = "memory";
    reg = <0x000000000 0x00000000 0x00000000 0x80000000
    0x000000001 0x00000000 0x00000001 0x00000000>;
    };

-----------------------
Obtaining a Device Tree
-----------------------

While the kernel ultimately requires a binary, the data starts its life in various ways:

*   Source Files (`.dts` and `.dtsi`) - human-readable text files obtained externally. These must be compiled into a `.dtb` via before the bootloader can use them.
*   Precompiled Blobs (`.dtb`) - called "Flattened" Device Tree (FDT) binaries stored on the boot medium.
*   Dynamically Generated Descriptions: - in some systems the firmware generates a device description in RAM from scratch based on hardware it detects at power-on. This is then presented to the bootloader as a `.dtb`

Once the bootloader loads the `.dtb`` into system memory, it performs **Fixups** to reflect the real state of the machine. 
These include detecting the actual RAM size and overwriting the `memory` node, 
unique identifiers like Ethernet MAC addresses and Serial Numbers and optionally Clock and voltage tweaks.

Finally depending on the architecture, the bootloader hands the `.dtb` to the kernel in a specific register or memory location performing a handshake.
Then the kernel parses the device tree and initializes the hardware based on the information provided in the tree.

-------------------
Using a Device Tree
-------------------

Once the kernel receives the device tree, it usually looks for important nodes like `chosen` (so get the boot arguments) 
and `memory` (to determine how much RAM is available and where it is physically located) to initialize cricical subsystems.

The next (optional) step is an "Unflattening" process, where the kernel converts the flat binary format of the device tree into a more convenient in-memory representation.

Finally, the kernel matches drivers to devices based on the `compatible` property of the nodes. It may also export the device tree to userspace, 
allowing user applications (and drivers!) to query hardware information.

----------
References
----------

Devicetree.org
https://www.devicetree.org/

Device Tree Specification
https://devicetree-specification.readthedocs.io/en/stable/index.html
