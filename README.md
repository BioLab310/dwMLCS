# dwMLCS 

Release Date: 30th May, 2023

Author

	~Changyong Yu (Northeastern University in CHINA)
	~Dekuan Gao (Northeastern University in CHINA)

1.Introduction
--

dwMLCS is an algorithm that can calculate the longest common sub-sequence of any sequence set

2.Test Data
--

The data set required for dwMLCS can be downloaded from the widely known NCBI database (Available: www.ncbi.nlm.nih.gov), or you can customize your own character sets. But some requirements need to be met, and the first line of the data set needs to be the nonduplicate symbol set(For example, A,B,C,D,E,F,G,H,I, and so on, where the order of the symbol set can be arbitrary). From the second line to the last line is the set of sequences you want to compute, and each row can have a different length

We have uploaded a data set (seq.txt) that we run in our experiment, which can be customized according to the format of our data set

3.Building Notes
--

This code is written by c++, and compiled and run under linux system

This contains the CMakeLists.txt file, which can be built using the cmake command. When you build, you need to put all .h and .cpp files and CMakeLists.txt files in the same directory

```shell
cmake .
```

After the cmake build is complete, you need to compile the code using the make command

```shell
make
```

After the compilation is complete, an executable file is generated. By default, the dwMLCS executable is generated

We uploaded an executable file called dwMLCS compiled in release mode using CLion development tool under linux, which can be run directly under linux system

4.Usage Notes
--

This code uses the command line mode to pass parameters

1) -M
   * A: Method A represents the dwMLCS algorithm used in this paper, and finally finds all the longest common sub-sequences. For example, -M A
   * B: Method B is the automatic calculation of $LB(MLCS)$ by weight DAG in this paper. For example, -M B
   * C: Method C is to calculate $LB(MLCS)$ by weight DAG in this paper, but it needs to specify the -S strategy and the corresponding size of $\alpha$ and $\beta$ (The specific meanings are as defined in definition 9 and definition 10 in this paper)
   * D: Method D is the lower bound length calculated by Big-MLCS algorithm. For example, -M D
   * E: Method E is the lower bound length calculated by BEST-MLCS algorithm. For example, -M E
2) -D
   * Represents the number of sequences read in. For example, -D 10 indicates that 10 sequences are read sequentially from the input file as input sequences
3) -R
   * The file name that represents the input file. For example, -R seq.txt
4) -W
   * The name of the file that represents the output file. For example, -W mlcs.txt
5) -M C -S a -N b 
   * These three parameters need to be used together. You can specify which successor table to use to calculate $LB(MLCS)$ by weight DAG
   * Where a can only be a number of 1 and non-1, where 1 means use of Step successor table strategy, and non-1 means use of Section successor table strategy
   * b According to definition 9 or Definition 10 in the paper
   * For example, -M C -S 1 -N 5, which means I want to use the Step successor table policy, and $\alpha$ is set to 5, calculating $LB(MLCS)$

For example, a complete run command:

* ./dwMLCS -M A -D 40  -R seq.txt -W mlcs.txt        

  It means that I will use the dwMLCS algorithm proposed in this paper to calculate the longest common sub-sequence of 40 sequences, and output all the calculated longest common sub-sequences into the mlcs.txt file

* ./dwMLCS -M B -D 40 -R seq.txt   

  It means that I will use the weight DAG model proposed in this paper to calculate the $LB(MLCS)$ of 40 sequences

* ./dwMLCS -M C -D 10 -R seq.txt -S 1 -N 3   

  It means that I will use the weight DAG model proposed in this paper, and explicitly specify the use of Step successor table strategy and set the parameter $\alpha$=3 (see Definition 9), and finally calculate the $LB(MLCS)$ of 10 sequences

* ./dwMLCS -M D -D 30 -R seq.txt   

  That means I'm going to use Big-MLCS(The algorithm compared in Figure 9 of the paper) to compute $LB(MLCS)$

* ./dwMLCS -M E -D 30 -R seq.txt  

  That means I'm going to use the BEST-MLCS(The algorithm compared in Figure 9 of the paper) algorithm to compute $LB(MLCS)$

5.License
--

	See LICENSE

6.Contacts
--

	Please e-mail your feedback at cyyneu@126.com



