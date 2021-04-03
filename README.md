## UMKC CS461 - Simulated Annealing Project
This project is a learning project for COMP-SCI 461: Introduction to Artificial Intelligdence at the University of Missouri-Kansas City.

### Project Description
This program will generate an optimized class schedule using simulated annealing.
Provided only simple conditions and their respective weights measured in integer values, we were expected to research the technique ourselves and
implement it with whatever environment/language we were most comfortable with.

### Process Explained
Simulated Annealing is a machine learning technique based on the cooling of metal in blacksmithing.

The program begins with a completely randomized state, a maximum temperature (double), a minimum temperature (double), and a rate at which the temperature
will decrease (double). Each "state" of the schedule is composed of 12 different courses, each course containing an instructor, a room, a time, and the course's name. 
Starting with the randomized state, it calculates a fitness score based on various conditions such as:
- "+5 For each course thatdoes not have the same instructor teachign another course at the same time"
- "-5 If the state has an instructor teaching more than four courses (per course over 4)"
- "+5 If CS101A and CS101B are scheduled 3 hours apart or more"

It will then make a single, random change to this state. If the fitness score of the new state is higher OR is less than or equal to the acceptance
probability, it will take the new state and repeat the process from there. If it does not satisy either of these conditions, it will instead
keep the old state. The acceptance probabilty can be calculated by: "exp(-(oldScore - newScore) / T)" This means that the chances of exploring a state with 
a smaller score decreases as the temperature decreases.

The temperature will decrease by the specified rate if one of the following are satisfied:
- 4000 attempts are made
- 400 successful attempts are made

The program will terminate if 4000 attempts are made unsuccessfully OR the maximum temperature drops below the minimum temperature.

### Technologies & Languages Used
Visual Studio 2017, C++
