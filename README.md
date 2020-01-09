# 2018 DSnP Homework

2018 Fall, Data Structure and Programming  

For test cases, please refer to [DSnP Test Cases](https://github.com/Mckinsey666/dsnp_test_cases) by Brian Chao

## HW1 JSON File Parser

*** You need to hand in HW1 to get authorization code (授權碼)***  

Problem 1:
  Preapare for coding environment.

![Homework 1-P1](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw1/p1/p1.png)

Problem 2:
  A JSON file parser.

* Features
    * Read
    * Print
    * Add
    * Sum
    * Ave
    * Max
    * Min

## HW2 Command Line Parser

A simple command line interface.  

![Homework 2](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw2/Homework_2.png)


## HW3 JSON File Database System 

A JSON database system with command line interface.  

![Homework 3](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw3/Homework_3.png)


## HW4 Memory Manager

A program to practice memory management techniques.  

![Homework 4](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw4/Homework_4.png)

## HW5 Abstract Data Type

Learning how to implement various abstract data types(ADTs) for sortinginput data.  

* Implement 3 abstract data types
  * Doubly Linked List 
  * Dynamic Array
  * Binary Search Tree
* Additional features
  * Doubly Linked List  
    * Using quick sort for better sorting performance
  * Binary Search Tree
    * Using dummy node 

![Homework 5](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw5/Homework_5.png)

## HW6 Circuit graph

Parsing a circuit from a description fileand(AAG file) create the netlist as a graph.  

![Homework 6](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw6/Homework_6.png)

## HW7 Heap and hash

Implement a task manager using heap and hash.

![Homework 7](https://raw.githubusercontent.com/orange2120/DSnP2018/master/hw7/Homework_7.png)

## Final Project : Fraig

Functionally Reduced And-Inverter Graph (FRAIG)  

![Fraig](https://raw.githubusercontent.com/orange2120/DSnP2018/master/fraig/Fraig.png)

### Useful tools (but NO WARRANTY)
`genTR.sh` : Automatic run Fraig and reference program with provided dofile.
If `[run diff]` is assigned, `diff` the result.

The script will create `td/` folder if not exists.

```bash
$ ./genTR.sh <dofile filename> [run diff]
$ ./genTR.sh dosim5 1
```
`packFraig.sh` : Packaging fraig project into the acceptable format and run `SelfCheck` for hand-in this homework.

```bash
$ ./packFraig.sh <student number>
$ ./packFraig.sh b066602037 // will generate b066602037_fraig.tgz
```
