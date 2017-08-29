# **PID Control**

## Rubric Points
### Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/824/view) individually and describe how I addressed each point in my implementation.  

---

### Describe the effect each of the P, I, D components had in your implementation.

* Student describes the effect of the P, I, D component of the PID algorithm in their implementation. Is it what you expected?
* Visual aids are encouraged, i.e. record of a small video of the car in the simulator and describe what each component is set to.

#### Apply P Only

The car is fluctuating between left and right. Finally the car leave off the lane in the corner.

[p_only.mp4](./p_only.mp4)

#### Apply P, D

The stability of driving is enhanced remarkably. The car can go through any corner.

[pd.mp4](./pd.mp4)

#### Apply P, I, D

The car trajectory in the experiment video looks similar as the previous one. But the sum of squraed CTE is smaller: 0.18805(PID) < 0.284644 (PD). It means that steering system has a little bias.

[pid.mp4](./pid.mp4)

### Describe how the final hyperparameters were chosen.

* Student discusses how they chose the final hyperparameters (P, I, D coefficients). This could be have been done through manual tuning, twiddle, SGD, or something else, or a combination!

At first I tune all parameters manually by following lecture note's parameters (Kp = 0.2, Kd = 3.0, Ki = 0.004). The result is not bad. After then I finetune all parameter by using TWIDDLE algorithm. after 20 iteration I fixed parameters: Kp = 0.22, Kd = 3.0, Ki = 0.00436

I think that SGD is more clear and powerful method for these kind of problems. But I want to test and implement TWIDDLE algorithm by myself.

[final_result.mp4](./final_result.mp4)

