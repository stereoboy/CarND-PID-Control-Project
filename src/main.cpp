#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid;
  // TODO: Initialize the pid variable.
	int i = 0;
//  double Kp = 0.2;
//  double Kd = 3.0;
//  double Ki = 0.004;
  double Kp = 0.22;
  double Kd = 3.0;
  double Ki = 0.00436;

  pid.Init(Kp, Ki, Kd);

  double *p[3];
  double *dp[3];
  p[0] = &pid.Kp;
  p[1] = &pid.Kd;
  p[2] = &pid.Ki;

  dp[0] = &pid.dKp;
  dp[1] = &pid.dKd;
  dp[2] = &pid.dKi;

  h.onMessage([&pid, &i, &p, &dp](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          //std::cout <<"[" << i << "]" << std::endl;
          pid.UpdateError(cte);

          steer_value = -pid.Kp*pid.p_error -pid.Kd*pid.d_error -pid.Ki*pid.i_error;

//					std::cout <<  pid.Kp << std::endl;
//          std::cout <<  pid.Kd << std::endl;
//          std::cout <<  pid.Ki << std::endl;
//
//          std::cout <<  pid.p_error << std::endl;
//          std::cout <<  pid.d_error << std::endl;
//          std::cout <<  pid.i_error << std::endl;
//
//          std::cout <<  pid.Kp*pid.p_error << std::endl;
//          std::cout <<  pid.Kd*pid.d_error << std::endl;
//          std::cout <<  pid.Ki*pid.i_error << std::endl;

          if (steer_value > 1.0)
            steer_value = 1.0;
          else  if (steer_value < -1.0)
            steer_value = -1.0;

					i++;

          //
          // update Kp, Kd, Ki using TWIDDLE logic
          //
				  if (i == 1500 /* 1 loop approximately */)
          {
            std::cout <<"[" << pid.cnt << "]#####################################" << std::endl;

            std::cout << "<best>=======" << std::endl;
            std::cout <<  "best_err" << pid.best_err << std::endl;
            std::cout <<  pid.best_Kp << std::endl;
            std::cout <<  pid.best_Kd << std::endl;
            std::cout <<  pid.best_Ki << std::endl;
            std::cout << "<current>=======" << std::endl;
            std::cout <<  pid.Kp << std::endl;
            std::cout <<  pid.Kd << std::endl;
            std::cout <<  pid.Ki << std::endl;
            std::cout << "---------------" << std::endl;
            std::cout <<  pid.dKp << std::endl;
            std::cout <<  pid.dKd << std::endl;
            std::cout <<  pid.dKi << std::endl;
            std::cout << "================" << std::endl;

            double error = pid.error/i;
            std::cout << "error=" << error << std::endl;

            switch(pid.status)
            {
              case STATUS_INIT: //
                std::cout<<"initialize"<< std::endl;
                pid.best_err = error;
                pid.best_Kp = pid.Kp;
                pid.best_Kd = pid.Kd;
                pid.best_Ki = pid.Ki;

                pid.status = STATUS_INC;
                *p[pid.p_id] += *dp[pid.p_id]; // try to increase Kp
                break;

              case STATUS_INC: // after increasing Kp
                if (error < pid.best_err)
                {
                  std::cout<<"cond INC" << pid.p_id << "- A"<< std::endl;
                  pid.best_err = error;
                  pid.best_Kp = pid.Kp;
                  pid.best_Kd = pid.Kd;
                  pid.best_Ki = pid.Ki;

                  *dp[pid.p_id] *= 1.1;
                  std::cout << "Increase " << pid.p_id<< std::endl;

                  pid.p_id = (pid.p_id + 1)%3;
                  pid.status = STATUS_INC;
                  *p[pid.p_id] += *dp[pid.p_id]; // try to increase Kx
                }
                else
                {
                  std::cout<<"cond INC" << pid.p_id << " - B"<< std::endl;
                  *p[pid.p_id] -= 2*(*dp[pid.p_id]); // try to decrease Kx
                  pid.status = STATUS_DEC;
                }
                break;

              case STATUS_DEC: // after decreasing Kx
                if (error < pid.best_err)
                {
                  std::cout<<"cond DEC" << pid.p_id << " - A"<< std::endl;
                  pid.best_err = error;
                  pid.best_Kp = pid.Kp;
                  pid.best_Kd = pid.Kd;
                  pid.best_Ki = pid.Ki;

                  *dp[pid.p_id] *=  1.1;
                  std::cout << "Decrease " << pid.p_id<< std::endl;
                }
                else
                {
                  std::cout<<"cond DEC" << pid.p_id << " - B"<< std::endl;
                  *p[pid.p_id] += *dp[pid.p_id]; // recover Kx
                  *dp[pid.p_id] *= 0.9;
                }

                pid.p_id = (pid.p_id + 1)%3;
                pid.status = STATUS_INC;
                *p[pid.p_id] += *dp[pid.p_id]; // try to increase Kx
                break;

            }
            pid.resetError();
            i = 0;
          }

          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
