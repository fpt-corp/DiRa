# DiRa Autonomous Driving #
## Introduction ##
DiRa Autonomous Driving library serves the purpose of solving self driving problems.
The library is used inside the scope of [Digital Racing](https://cuocduaso.fpt.com.vn/en) competition.
## Library Structure ##
The following structure is applied:

```
dira (wrapper)
|__ algorithms (wrapper)
|   |__ computer_vision
|   |	|__ lane
|   |	|__ obstacle
|   |	|__ sign
|   |	|__ other problems (TBD)
|   |__ other methods (TBD)	
|
|__ models
|__ utils
    |__ config
```
### Algorithms ### 
- Container for methods used to tackle autonomous control problems, such as computer vision, localizations, etc.
- All current and future algorithms is defined by the following folder structure: dira/algorithms/[\method\]\[[problem\]/\[algorithm name\]
- For the current version, the library provides solutions for the following problems:
	- Lane segmentation
	- Obstacle detection
	- Sign recognition
### Models ###
Container for all deep learning models used in this library
### Utils ###
Contains config file, all path related variables are defined hereser
