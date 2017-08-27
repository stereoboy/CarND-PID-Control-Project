#include "PID.h"
#include <iostream>

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;

  p_error = 0;
  i_error = 0;
  d_error = 0;

  initialized = false;
  prev_cte = 0;

	// Twiddle
  this->status = STATUS_INIT;
  this->p_id = 0;

	this->dKp = 0.1*Kp;
  this->dKi = 0.1*Ki;
  this->dKd = 0.1*Kd;

  this->best_err = 0;
  this->error = 0;
  this->cnt = 0;
}

void PID::UpdateError(double cte) {

  if (!initialized)
  {
    prev_cte = cte;
    initialized = true;
  }

  p_error = cte;
  d_error = cte - prev_cte;
  i_error += cte;
  prev_cte = cte;

  // Twiddle
  error += cte*cte;
}

void PID::resetError()
{
  p_error = 0;
  i_error = 0;
  d_error = 0;

  this->error = 0;
  this->cnt++;
}

double PID::TotalError() {

  return i_error;
}

