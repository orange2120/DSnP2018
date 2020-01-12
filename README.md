# 2018 DSnP Homework

2018 Fall, Data Structure and Programming  

For test cases, please refer to [DSnP Test Cases](https://github.com/Mckinsey666/dsnp_test_cases) by Brian Chao

## HW1 JSON File Parser

*** You need to hand in HW1 to get authorization code (授權碼)***  

Problem 1:
  Preapare for coding environment.

![Homework 1-P1](./hw1/p1/p1.png)

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

![Homework 2](./hw2/Homework_2.png)


## HW3 JSON File Database System 

A JSON database system with command line interface.  

![Homework 3](./hw3/Homework_3.png)


## HW4 Memory Manager

A program to practice memory management techniques.  

![Homework 4](./hw4/Homework_4.png)

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

![Homework 5](./hw5/Homework_5.png)

## HW6 Circuit graph

Parsing a circuit from a AIG description file (`.aag` file) 

![Homework 6](./hw6/Homework_6.png)

- Build the circuit netlist as a graph.
- Print a gate
- Perform DFS from fanin or fanout
- Write back gates in DFS order to the `.aag` file 

## HW7 Heap and hash

Implement a task manager using min-heap and hash table.

![Homework 7](./hw7/Homework_7.png)

## Final Project : Fraig

Functionally Reduced And-Inverter Graph (FRAIG)  

![Fraig](./fraig/Fraig.png)

<font color=red>WARNING: CirSim and CirFraig</font> remains unfinished yet, use with care!

### Useful tools (but NO WARRANTY)
`genTR.sh` : Automatically run Fraig and reference program with provided dofile.
If `[run diff]` is assigned, `diff` the result.

The script will create `td/` folder to store results if the folder doesn't exists.

```bash
$ ./genTR.sh <dofile filename> [run diff]
$ ./genTR.sh dosim5 1 // run diff after dofile
$ ./genTR.sh dosim5   // only run dofile
```
`packFraig.sh` : Package fraig project into the acceptable format and run `SelfCheck` for handing-in this homework.

```bash
$ ./packFraig.sh <student number>
$ ./packFraig.sh b066602037 // will generate b066602037_fraig.tgz
```
