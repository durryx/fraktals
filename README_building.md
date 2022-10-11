## Build Instructions

A full build has different steps:
1) Install dependences
2) Configure the build
3) Build the project
4) Editing & git directives

### (1) Install dependences

* [clang](https://clang.llvm.org/)
<details>
	<summary>Install commands (Linux) </summary>


</details>

* [conan](https://conan.io/)
<details>
	<summary>Install commands (Linux) </summary>


</details>

* [cmake](https://cmake.org/)
<details>
	<summary>Install commands (Linux) </summary>


</details>

* [cppcheck](http://cppcheck.sourceforge.net/)
<details>
	<summary>Install commands (Linux) </summary>


</details>

* [glm](https://github.com/g-truc/glm)
<details>
	<summary>Install commands (Linux) </summary>


</details>

* [optional]()
<details>
	<summary>Install commands (Linux) </summary>


</details>

### (2) Configure the build

To configure the project, use `cmake` or `cmake-gui`.

#### (2.a) Configuring via cmake:
With Cmake directly:
```
    cmake -S . -B ./build
```
Cmake will automatically create the `./build` folder if it does not exist, and it wil configure the project.
#### (2.b) Configuring via cmake-gui:

Open the GUI in the root folder:

```
cmake-gui .
```
Set the build directory:

![build_dir](https://user-images.githubusercontent.com/16418197/82524586-fa48e380-9af4-11ea-8514-4e18a063d8eb.jpg)

Configure the generator (set the various flags to your needs):

![generate](https://user-images.githubusercontent.com/16418197/82781591-c97feb80-9e1f-11ea-86c8-f2748b96f516.png)

Generate:

![generate](https://user-images.githubusercontent.com/16418197/82781591-c97feb80-9e1f-11ea-86c8-f2748b96f516.png)


### (3) Build the project
Build the project for all targets:
```
    cmake --build ./build
```

### Editing and git directives
