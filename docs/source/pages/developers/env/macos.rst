macOS
=====

Before you begin, ensure you have the `Homebrew <https://brew.sh/>`_ package manager installed on your system.

You will also need the Xcode Command Line Tools, which can be installed by running ``xcode-select --install`` in your terminal.

To install all necessary packages, run:

    .. code-block:: bash

        brew install llvm lld cmake qemu dosfstools e2fsprogs mtools include-what-you-use

To build the project, use one of the Clang presets, e.g.:

    .. code-block:: bash

        cmake --preset debug-clang
        cmake --build build
