#ifndef PID_H
#define PID_H


enum TWIDDLE_STATUS{
  STATUS_INIT,

  STATUS_INC,
  STATUS_DEC,
};

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;

	/*
  * Coefficients for Twiddle
  */
  TWIDDLE_STATUS  status;
  int p_id;
  double dKp;
  double dKi;
  double dKd;

  double best_Kp;
  double best_Ki;
  double best_Kd;
  double best_err;
  double error;
  int cnt;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * reset PID variables.
  */
  void resetError();

  /*
  * Calculate the total PID error.
  */
  double TotalError();

private:
  bool initialized;
  double prev_cte;
};


#endif /* PID_H */
