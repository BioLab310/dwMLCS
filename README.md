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

The data set required for dwMLCS can be downloaded from the widely known NCBI database (Available: www.ncbi.nlm.nih.gov), or you can customize your own character sets. But some requirements need to be met, and the first line of the data set needs to be the nonduplicate symbol set(For example, A,B,C,D,E,F,G,H,I, and so on, where the order of the symbol set can be arbitrary, excepte symbol '$'). From the second line to the last line is the set of sequences you want to compute, and each row can have a different length

We have uploaded a data set (seq.txt), which can be customized according to the format of our data set

3.Building Notes
--

This code is written by c++, and compiled and run under linux system

cmake, ninja, and gcc need to be installed in advance. The compilation process is as follows:

1. First we need to create two folders called src and build. Then we need to add all the files from the src folder on github to the src folder we just created

2. We open the terminal in the build folder and execute the following command:

   ```shell
   cmake /path/src -B /path/build -G Ninja -DCMAKE_BUILD_TYPE=Release
   # Replace /path/src with the actual path to your src directory and /path/build with the actual path you want to set as your build directory
   ```

3. Execute the ninja command, compile, and a dwMLCS executable will be generated:

   ```shell
   ninja
   # Note that this command needs to be executed in the build directory you just created
   ```

We uploaded an executable file called dwMLCS that we compiled successfully with the above method

4.Usage Notes
--

This code uses the command line mode to pass parameters

1) -M
   * A: Method A represents the dwMLCS algorithm used in this paper, and finally finds all the longest common sub-sequences. For example, -M A
   * B: Method B is the automatic calculation of $LB(S)$ by weight DAG in this paper. For example, -M B
   * C: Method C is to calculate $LB(S)$ by weight DAG in this paper, but it needs to specify the -S strategy and the corresponding size of $\alpha$ and $\beta$ (The specific meanings are as defined in definition 9 and definition 10 in this paper)
2) -D
   * Represents the number of sequences read in. For example, -D 10 indicates that 10 sequences are read sequentially from the input file as input sequences
3) -R
   * The file name that represents the input file. For example, -R seq.txt
4) -W
   * The name of the file that represents the output file. For example, -W mlcs.txt
5) -M C -S a -N b 
   * These three parameters need to be used together. You can specify which successor table to use to calculate $LB(S)$ by weight DAG
   * Where a can only be a number of 1 and non-1, where 1 means use of Step successor table strategy, and non-1 means use of Section successor table strategy
   * b According to definition 9 or Definition 10 in the paper
   * For example, -M C -S 1 -N 5, which means I want to use the Step successor table policy, and $\alpha$ is set to 5, calculating $LB(S)$

For example, a complete run command:

* ./dwMLCS -M A -D 40  -R seq.txt -W mlcs.txt        

  It means that I will use the dwMLCS algorithm proposed in this paper to calculate the longest common sub-sequence of 40 sequences, and output all the calculated longest common sub-sequences into the mlcs.txt file

* ./dwMLCS -M B -D 40 -R seq.txt   

  It means that I will use the weight DAG model proposed in this paper to calculate the $LB(S)$ of 40 sequences

* ./dwMLCS -M C -D 10 -R seq.txt -S 1 -N 3   

  It means that I will use the weight DAG model proposed in this paper, and explicitly specify the use of Step successor table strategy and set the parameter $\alpha$=3 (see Definition 9), and finally calculate the $LB(S)$ of 10 sequences

5.License
--

	See LICENSE

6.Contacts
--

	Please e-mail your feedback at cyyneu@126.com
