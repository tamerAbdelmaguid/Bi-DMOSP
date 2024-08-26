# Bi-DMOSP
Welcome to the code repository of the algorithms used for solving the
bi-objective dynamic multiprocessor open shop scheduling problem (Bi-DMOSP). 
This repository contains the complete code used for implmeneting the three 
metaheuristic approaches (MOGWO, NSGA-II, and MOSS), along with the dataset 
of small-sized instances for which the epsilon-constraint method is used to 
find the optimal Pareto front. These correspond to the following publications:

1) Tamer F. Abdelmaguid. 2020. "Bi-Objective Dynamic Multiprocessor 
Open Shop Scheduling: An Exact Algorithm." Algorithms 13, no. 3: 74.
https://doi.org/10.3390/a13030074

3) Tamer F. Abdelmaguid. 2021. "Bi-objective dynamic multiprocessor 
open shop scheduling for maintenance and healthcare diagnostics." 
Expert Systems with Applications 186, no. : 115777. 
https://doi.org/10.1016/j.eswa.2021.115777 

4) Tamer F. Abdelmaguid. 2024. "Bi-Objective, Dynamic, Multiprocessor 
Open-Shop Scheduling: A Hybrid Scatter Search–Tabu Search Approach." 
Algorithms 17, no. 8: 371. https://doi.org/10.3390/a17080371

This code is copyrighted (2024) by the author of the above references,
Tamer F. Abdelmaguid, professor of industrial engineering at the
Faculty of Engineering, Cairo University, Egypt.
The code can be used without permission for research and teaching
purposes only under the GNU General Public License v3.0.

This code utilizes the graphical user interface library provided
by Embarcadero RAD studio C++ Builder 2010 and its 32-bit compiler.
It is implemented on Microsoft Windows 10.

Click on DMOSP.exe to start the program in MS Windows. The interface is very
simple. You start by opening a problem instance and using its
menus, you can run the implemented metaheuristics.

In addition to the metaheuristics, there are other supporting
tools for visulaizing the solutions. Visually see the effects
of applying different operations related to the metaheuristics,
as well as the sultion representation structures.

Part of the NSGA-II code is based on the code provided by 
Deb, K., Pratap, A., Agrawal, S., & Meyarivan, T. (2002).
A fast elitist non-dominated sorting genetic algorithm for multi-objective optimization:
NSGA-II. IEEE Transactions on Evolutionary Computation, 6, 182–97.

Three folders are included for the dataset used in the computational experiments:
* SolvedSet: contains small-sized Bi-DMOSP instances for which the optimal
  Pareto front solutions are obtained using the epsilon-constraint method.
  For each instance, the optimal Pareto front solutions are provided in
  separate files, which can be loaded and their Gantt charts can be drawn
  via the developed program.
* LargeSet-NSGAII-MOGWO: Contains the results for the large-sized test instances
  used in the computational comparison of MOGWO and NSGA-II as in publication #2. 
* LargeSet-NSGAII-MOSS: Contains the results for the large-sized test instances
  used in the computational comparison of MOSS and NSGA-II as in publication #3.
