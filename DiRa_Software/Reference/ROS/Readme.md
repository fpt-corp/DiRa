# ROS learning Instructions


The documents contained in this repository provide the necessary information required to understand and begin to install, apply and perform basic operations with the Robotic Operating System within the scope of the DiRa Platform


![alt text][note] Please contact us by opening an issue or emailing if you have any questions about the instructions, parts, or any part of the build process. My email: manhlinh.nguyendinh@gmail.com.

## Getting Started

There is no technical knowledge requirement for you to follow these instructions other than being able to operate the Linux Terminal, preferably Ubuntu. The mose basic of tutorials is as follow:

### [How to install ROS](http://wiki.ros.org/ROS/Tutorials/InstallingandConfiguringROSEnvironment)

### [Navigating the ROS file system](http://wiki.ros.org/ROS/Tutorials/NavigatingTheFilesystem)

### [How to properly create a package](http://wiki.ros.org/ROS/Tutorials/CreatingPackage)

### [How to build your package](http://wiki.ros.org/ROS/Tutorials/BuildingPackages)

### [What is a ROS node](http://wiki.ros.org/ROS/Tutorials/UnderstandingNodes)

### [What is a ROS topic](http://wiki.ros.org/ROS/Tutorials/UnderstandingTopics) 

### [Writing publisher and a subscriber(Python)](http://wiki.ros.org/ROS/Tutorials/WritingPublisherSubscriber%28python%29) 

### [Run ROS package](http://wiki.ros.org/ROS/Tutorials/ExaminingPublisherSubscriber)



## Incase you wish to know more:



A moderately sufficient reference set is provided for you under the form of Powerpoint files in this directory. If you'd like to meddle in more, creating services and messages of you own, please refer to these files

### [Instruction to ROS](https://github.com/fpt-corp/DiRa/blob/development/DiRa_Software/Reference/ROS/Introduction%20to%20ROS.pptx)



### [Working with ROS bags](https://github.com/fpt-corp/DiRa/blob/development/DiRa_Software/Reference/ROS/Working%20with%20ROS%20bags.pptx)



### [Working with ROS service](https://github.com/fpt-corp/DiRa/blob/development/DiRa_Software/Reference/ROS/Working%20with%20ROS%20service.pptx)



### [WOrking with ROS topic](https://github.com/fpt-corp/DiRa/blob/development/DiRa_Software/Reference/ROS/Working%20with%20ROS%20topic.pptx)

# Introduction to Github:

Github is a powerful version management and source code storing platform that DiRa uitilize. To keep up with the latest version of DiRa as well as checkout the earlier if you would like, knowing how to use git is important

## For terminal git:

To install git on your Linux machine (bot Jetson and the host pc):

```
sudo apt update

sudo apt upgrade

sudo apt install git
```

To download the latest version of a repository:

```
git clone <link to the repository>
```

Every repository is separated in to branches, which contains different versions of the source code and therefore different versions of the software itself. To checkout different branches of a repository:

```
git branch -a #to show all local and remote branches

git checkout <branch name>
```

To create a repository of your own code:

+ Follow [these](https://help.github.com/en/articles/create-a-repo) instruction to create a repo on the github website.
+ Copy the link to that repo
+ Open the terminal on your machine. Create an appropriate folder and navigate to that folder in the terminal and initialize git

```
mkdir <folder-name>
cd <folder-name>
git init
```
+ Add the remote repository. Enter any credentials required

```
git remote add origin <link-to-repo>
```
## For GitKraken:

If you feel familiar enough with git, chances are you will find it difficult to handle sometime. We recommend a great GUI version control called [GitKraken](https://www.gitkraken.com/). You can manage your Github projects there too. If you would like to swith to GitKraken, we recommend you checkout their [tutorial videos](https://www.gitkraken.com/learn-git)
