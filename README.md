For build prereqs, see [the VM setup instructions](http://tomahawk.postech.ac.kr/csed353/assignments/vm/virtualbox).

## Sponge quickstart

Command to modify git origin:

    $ git remote remove origin
    $ git remote add origin https://github.com/POSTECH-HIS/sponge
    $ git fetch
    $ git merge origin/labn-startercode
    $ git remote remove origin
    $ git remote add origin git@github.com:jae1u/CSED353.git
    $ git fetch
    $ git push --set-upstream origin master

How to set up a VM:

    $ VBoxManage import ./cs144_vm.ova --vsys 0 --vmname csed353 --memory 4096 --cpus 4
    $ VBoxManage modifyvm csed353 --nat-pf1 delete ssh_forwarding_rule
    $ VBoxManage modifyvm csed353 --nat-pf1 "ssh_forwarding_rule, tcp, 0.0.0.0, 2222, , 22"
    $ VBoxManage startvm --type headless csed353

How to delete a VM:

    $ VBoxManage controlvm csed353 poweroff
    $ VBoxManage unregistervm csed353 --delete-all

Useful command:

    $ VBoxManage list vms
    $ VBoxManage list runningvms

To set up your build directory:

	$ mkdir -p <path/to/sponge>/build
	$ cd <path/to/sponge>/build
	$ cmake ..

**Note:** all further commands listed below should be run from the `build` dir.

To build:

    $ make

You can use the `-j` switch to build in parallel, e.g.,

    $ make -j$(nproc)

To test (after building; make sure you've got the [build prereqs](http://tomahawk.postech.ac.kr/csed353/assignments/vm/virtualbox) installed!)

    $ make check_labN *(replacing N with a checkpoint number)*

The first time you run `make check_lab...`, it will run `sudo` to configure two
[TUN](https://www.kernel.org/doc/Documentation/networking/tuntap.txt) devices for use during
testing.

### build options

You can specify a different compiler when you run cmake:

    $ CC=clang CXX=clang++ cmake ..

You can also specify `CLANG_TIDY=` or `CLANG_FORMAT=` (see "other useful targets", below).

Sponge's build system supports several different build targets. By default, cmake chooses the `Release`
target, which enables the usual optimizations. The `Debug` target enables debugging and reduces the
level of optimization. To choose the `Debug` target:

    $ cmake .. -DCMAKE_BUILD_TYPE=Debug

The following targets are supported:

- `Release` - optimizations
- `Debug` - debug symbols and `-Og`
- `RelASan` - release build with [ASan](https://en.wikipedia.org/wiki/AddressSanitizer) and
  [UBSan](https://developers.redhat.com/blog/2014/10/16/gcc-undefined-behavior-sanitizer-ubsan/)
- `RelTSan` - release build with
  [ThreadSan](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/Thread_Sanitizer)
- `DebugASan` - debug build with ASan and UBSan
- `DebugTSan` - debug build with ThreadSan

Of course, you can combine all of the above, e.g.,

    $ CLANG_TIDY=clang-tidy-6.0 CXX=clang++-6.0 .. -DCMAKE_BUILD_TYPE=Debug

**Note:** if you want to change `CC`, `CXX`, `CLANG_TIDY`, or `CLANG_FORMAT`, you need to remove
`build/CMakeCache.txt` and re-run cmake. (This isn't necessary for `CMAKE_BUILD_TYPE`.)

### other useful targets

To generate documentation (you'll need `doxygen`; output will be in `build/doc/`):

    $ make doc

To format (you'll need `clang-format`):

    $ make format

To see all available targets,

    $ make help
